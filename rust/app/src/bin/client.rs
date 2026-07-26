use app_util::pubsub::{PubSubBroker, TransportMode};
use std::time::Duration;
use tokio::time::timeout;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("📡 Initializing Pub/Sub Client...");

    // Create client broker instance
    let client_broker = PubSubBroker::builder()
        .node_id("pubsub-client-node-1")
        .bind_udp("0.0.0.0:18899".parse()?)
        .enable_broadcast(true)
        .build()
        .await?;

    let topic = "t#1";
    let message_payload = "m#1";

    // 1. Subscribe to topic "t#1" (Observer Pattern)
    let mut sub = client_broker.subscribe(topic).await;
    println!(" ✅ Client Subscribed to topic: '{}'", topic);

    tokio::time::sleep(Duration::from_millis(50)).await;

    // 2. Send / Publish message "m#1" to topic "t#1"
    println!(" 📤 [Client Send] Publishing to Topic: '{}' | Message: '{}'", topic, message_payload);

    client_broker
        .publish(
            topic,
            message_payload.as_bytes().to_vec(),
            TransportMode::LanUnicast {
                target_addr: "127.0.0.1:18801".parse()?,
            },
        )
        .await?;

    // 3. Receive published message
    match timeout(Duration::from_secs(3), sub.rx.recv()).await {
        Ok(Some(msg)) => {
            let recv_text = String::from_utf8_lossy(&msg.payload);
            println!(" 📥 [Client Recv] Received Topic: '{}' | Message: '{}' | Sender: '{}'", msg.topic, recv_text, msg.sender_id);
            println!("\n✨ Send & Recv Handshake Successfully Completed!");
        }
        Ok(None) => {
            println!(" ⚠️ Subscription channel closed.");
        }
        Err(_) => {
            println!(" ⚠️ Timeout waiting for message.");
        }
    }

    Ok(())
}
