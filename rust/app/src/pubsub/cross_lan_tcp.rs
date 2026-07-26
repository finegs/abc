use crate::pubsub::protocol::Message;
use crate::pubsub::security::TlsConfig;
use rustls_pki_types::ServerName;
use std::io;
use std::net::SocketAddr;
use std::sync::Arc;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::{TcpListener, TcpStream};
use tokio::sync::mpsc;
use tokio_rustls::{TlsAcceptor, TlsConnector};

/// TCP Framing Helper: writes a length-prefixed message frame to an async stream (TcpStream or TlsStream).
pub async fn write_framed_message<S>(stream: &mut S, msg: &Message) -> io::Result<()>
where
    S: AsyncWriteExt + Unpin,
{
    let bytes = msg.to_bytes().map_err(|e| io::Error::new(io::ErrorKind::InvalidData, e))?;
    let len = bytes.len() as u32;
    stream.write_all(&len.to_be_bytes()).await?;
    stream.write_all(&bytes).await?;
    stream.flush().await?;
    Ok(())
}

/// TCP Framing Helper: reads a length-prefixed message frame from an async stream (TcpStream or TlsStream).
pub async fn read_framed_message<S>(stream: &mut S) -> io::Result<Message>
where
    S: AsyncReadExt + Unpin,
{
    let mut len_buf = [0u8; 4];
    stream.read_exact(&mut len_buf).await?;
    let len = u32::from_be_bytes(len_buf) as usize;

    let mut payload_buf = vec![0u8; len];
    stream.read_exact(&mut payload_buf).await?;

    Message::from_bytes(&payload_buf).map_err(|e| io::Error::new(io::ErrorKind::InvalidData, e))
}

/// Cross-LAN TCP Cluster Server & Peer Manager supporting TLS 1.3 stream encryption.
pub struct CrossLanTcpCluster {
    listen_addr: SocketAddr,
    peers: Vec<SocketAddr>,
    tls_config: Option<TlsConfig>,
    broadcast_tx: mpsc::Sender<Message>,
}

impl CrossLanTcpCluster {
    pub fn new(
        listen_addr: SocketAddr,
        peers: Vec<SocketAddr>,
        tls_config: Option<TlsConfig>,
        broadcast_tx: mpsc::Sender<Message>,
    ) -> Self {
        Self {
            listen_addr,
            peers,
            tls_config,
            broadcast_tx,
        }
    }

    /// Start listening for incoming TCP/TLS cluster connections and accepting connections.
    pub async fn start(self: Arc<Self>) -> io::Result<()> {
        let listener = TcpListener::bind(self.listen_addr).await?;

        let acceptor = self
            .tls_config
            .as_ref()
            .and_then(|c| c.server_config.clone())
            .map(TlsAcceptor::from);

        // Task to accept incoming TCP gateway connections
        let cluster = self.clone();
        tokio::spawn(async move {
            loop {
                match listener.accept().await {
                    Ok((stream, _addr)) => {
                        let cluster_inner = cluster.clone();
                        let acceptor_inner = acceptor.clone();
                        tokio::spawn(async move {
                            if let Some(acc) = acceptor_inner {
                                match acc.accept(stream).await {
                                    Ok(tls_stream) => {
                                        let _ = cluster_inner.handle_connection(tls_stream).await;
                                    }
                                    Err(e) => {
                                        eprintln!("[TCP TLS Cluster] Handshake error: {}", e);
                                    }
                                }
                            } else {
                                let _ = cluster_inner.handle_connection(stream).await;
                            }
                        });
                    }
                    Err(e) => {
                        eprintln!("[TCP Cluster] Accept error: {}", e);
                        tokio::time::sleep(tokio::time::Duration::from_millis(100)).await;
                    }
                }
            }
        });

        Ok(())
    }

    /// Forward a message to all configured cross-LAN peer cluster gateways over TCP / TLS.
    pub async fn forward_to_peers(&self, mut msg: Message) {
        if msg.hops >= 5 {
            return;
        }
        msg.hops += 1;

        let tls_connector = self
            .tls_config
            .as_ref()
            .and_then(|c| c.client_config.clone())
            .map(TlsConnector::from);

        let server_name_str = self
            .tls_config
            .as_ref()
            .map(|c| c.server_name.clone())
            .unwrap_or_else(|| "localhost".to_string());

        for peer_addr in &self.peers {
            let msg_clone = msg.clone();
            let peer_addr = *peer_addr;
            let tls_connector = tls_connector.clone();
            let server_name_str = server_name_str.clone();

            tokio::spawn(async move {
                if let Ok(tcp_stream) = TcpStream::connect(peer_addr).await {
                    if let Some(connector) = tls_connector {
                        if let Ok(server_name) = ServerName::try_from(server_name_str.as_str()) {
                            if let Ok(mut tls_stream) =
                                connector.connect(server_name.to_owned(), tcp_stream).await
                            {
                                let _ = write_framed_message(&mut tls_stream, &msg_clone).await;
                            }
                        }
                    } else {
                        let mut plain_stream = tcp_stream;
                        let _ = write_framed_message(&mut plain_stream, &msg_clone).await;
                    }
                }
            });
        }
    }

    /// Handle framed messages on an established TCP or TLS stream.
    async fn handle_connection<S>(&self, mut stream: S) -> io::Result<()>
    where
        S: AsyncReadExt + Unpin,
    {
        loop {
            match read_framed_message(&mut stream).await {
                Ok(msg) => {
                    if self.broadcast_tx.send(msg).await.is_err() {
                        break; // Channel closed
                    }
                }
                Err(e) => {
                    if e.kind() == io::ErrorKind::UnexpectedEof {
                        break; // Connection closed normally
                    }
                    return Err(e);
                }
            }
        }
        Ok(())
    }
}
