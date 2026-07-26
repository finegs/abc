use serde::{Deserialize, Serialize};
use std::net::SocketAddr;

/// Identifies the transport mechanism used for message publishing.
#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub enum TransportMode {
    /// Direct UDP unicast to a specific node address.
    LanUnicast { target_addr: SocketAddr },
    /// UDP multicast to a multicast group (e.g. 239.255.0.1:8888).
    LanMulticast { group_addr: SocketAddr },
    /// UDP broadcast to local subnet on specified port (e.g. 255.255.255.255:8888).
    LanBroadcast { broadcast_port: u16 },
    /// TCP Cross-LAN cluster bridge forwarding.
    TcpCluster,
}

/// Pub/Sub message frame transmitted over UDP or TCP.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct Message {
    /// Unique message identifier (used for deduplication).
    pub id: String,
    /// Topic string (e.g., "sensors/temperature/room1").
    pub topic: String,
    /// Arbitrary message payload bytes.
    pub payload: Vec<u8>,
    /// Node identifier of the original publisher.
    pub sender_id: String,
    /// Transport mode requested or used for delivery.
    pub transport_mode: TransportMode,
    /// Hop count to prevent infinite cluster routing loops.
    pub hops: u8,
}

impl Message {
    /// Create a new message with default hop count 0.
    pub fn new(
        topic: impl Into<String>,
        payload: Vec<u8>,
        sender_id: impl Into<String>,
        transport_mode: TransportMode,
    ) -> Self {
        let sender = sender_id.into();
        Self {
            id: format!(
                "{}-{}",
                sender,
                std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .map(|d| d.as_nanos())
                    .unwrap_or(0)
            ),
            topic: topic.into(),
            payload,
            sender_id: sender,
            transport_mode,
            hops: 0,
        }
    }

    /// Serialize message to JSON bytes.
    pub fn to_bytes(&self) -> Result<Vec<u8>, serde_json::Error> {
        serde_json::to_vec(self)
    }

    /// Deserialize message from JSON bytes.
    pub fn from_bytes(bytes: &[u8]) -> Result<Self, serde_json::Error> {
        serde_json::from_slice(bytes)
    }
}
