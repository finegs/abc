use crate::pubsub::protocol::{Message, TransportMode};
use crate::pubsub::security::{decrypt_payload, encrypt_payload, AES_KEY_SIZE};
use socket2::{Domain, Protocol, Socket, Type};
use std::io;
use std::net::{Ipv4Addr, SocketAddr, SocketAddrV4};
use std::sync::Arc;
use tokio::net::UdpSocket;
use tokio::sync::mpsc;

/// Configuration for LAN UDP transport.
#[derive(Debug, Clone)]
pub struct LanUdpConfig {
    pub bind_addr: SocketAddr,
    pub multicast_group: Option<SocketAddrV4>,
    pub enable_broadcast: bool,
    pub symmetric_key: Option<[u8; AES_KEY_SIZE]>,
}

impl Default for LanUdpConfig {
    fn default() -> Self {
        Self {
            bind_addr: "0.0.0.0:0".parse().unwrap(),
            multicast_group: None,
            enable_broadcast: true,
            symmetric_key: None,
        }
    }
}

/// UDP Socket Manager supporting Unicast, Multicast, and Broadcast modes.
pub struct LanUdpTransport {
    socket: Arc<UdpSocket>,
    symmetric_key: Option<[u8; AES_KEY_SIZE]>,
}

impl LanUdpTransport {
    /// Constructs a UDP socket configured for unicast, multicast, and broadcast as requested.
    pub fn new(config: &LanUdpConfig) -> io::Result<Self> {
        let domain = match config.bind_addr {
            SocketAddr::V4(_) => Domain::IPV4,
            SocketAddr::V6(_) => Domain::IPV6,
        };

        let socket = Socket::new(domain, Type::DGRAM, Some(Protocol::UDP))?;
        socket.set_reuse_address(true)?;

        if config.enable_broadcast {
            socket.set_broadcast(true)?;
        }

        socket.bind(&config.bind_addr.into())?;

        let std_socket: std::net::UdpSocket = socket.into();
        std_socket.set_nonblocking(true)?;

        if let Some(mcast_addr) = config.multicast_group {
            let multi_addr = mcast_addr.ip();
            let interface = Ipv4Addr::UNSPECIFIED;
            std_socket.join_multicast_v4(multi_addr, &interface)?;
            std_socket.set_multicast_loop_v4(true)?;
        }

        let tokio_socket = UdpSocket::from_std(std_socket)?;
        Ok(Self {
            socket: Arc::new(tokio_socket),
            symmetric_key: config.symmetric_key,
        })
    }

    /// Returns a cloneable handle to the underlying UDP socket.
    pub fn socket(&self) -> Arc<UdpSocket> {
        self.socket.clone()
    }

    /// Send a message frame over UDP depending on its TransportMode (with optional payload encryption).
    pub async fn send_message(&self, msg: &Message) -> io::Result<()> {
        let raw_bytes = msg.to_bytes().map_err(|e| io::Error::new(io::ErrorKind::InvalidData, e))?;

        let payload_bytes = if let Some(ref key) = self.symmetric_key {
            encrypt_payload(key, &raw_bytes)
                .map_err(|e| io::Error::new(io::ErrorKind::InvalidData, e))?
        } else {
            raw_bytes
        };

        match &msg.transport_mode {
            TransportMode::LanUnicast { target_addr } => {
                self.socket.send_to(&payload_bytes, target_addr).await?;
            }
            TransportMode::LanMulticast { group_addr } => {
                self.socket.send_to(&payload_bytes, group_addr).await?;
            }
            TransportMode::LanBroadcast { broadcast_port } => {
                let target_addr: SocketAddr = format!("255.255.255.255:{}", broadcast_port)
                    .parse()
                    .map_err(|e| io::Error::new(io::ErrorKind::InvalidInput, e))?;
                self.socket.send_to(&payload_bytes, target_addr).await?;
            }
            TransportMode::TcpCluster => {
                return Err(io::Error::new(
                    io::ErrorKind::InvalidInput,
                    "TcpCluster mode cannot be sent directly via UDP",
                ));
            }
        }
        Ok(())
    }

    /// Spawn background loop receiving UDP messages and pushing them to a channel (with optional payload decryption).
    pub fn start_receive_loop(
        socket: Arc<UdpSocket>,
        tx: mpsc::Sender<Message>,
        symmetric_key: Option<[u8; AES_KEY_SIZE]>,
    ) {
        tokio::spawn(async move {
            let mut buf = vec![0u8; 65535];
            loop {
                match socket.recv_from(&mut buf).await {
                    Ok((len, _src)) => {
                        let data = &buf[..len];
                        let raw_bytes_res = if let Some(ref key) = symmetric_key {
                            decrypt_payload(key, data)
                                .map_err(|e| io::Error::new(io::ErrorKind::InvalidData, e))
                        } else {
                            Ok(data.to_vec())
                        };

                        if let Ok(raw_bytes) = raw_bytes_res {
                            if let Ok(msg) = Message::from_bytes(&raw_bytes) {
                                if tx.send(msg).await.is_err() {
                                    break; // Channel closed
                                }
                            }
                        }
                    }
                    Err(e) => {
                        eprintln!("[LAN UDP] Recv error: {}", e);
                        tokio::time::sleep(tokio::time::Duration::from_millis(10)).await;
                    }
                }
            }
        });
    }
}
