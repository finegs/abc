use app_util::pubsub::{PubSubBroker, TransportMode};
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Arc;
use std::time::Duration;
use tokio::time::interval;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("⚡ Initializing High-Frequency Sequence Client (10ms Period, Max 1,000,000)...");

    let client_broker = PubSubBroker::builder()
        .node_id("seq-client-node")
        .bind_udp("0.0.0.0:18898".parse()?)
        .enable_broadcast(true)
        .build()
        .await?;

    let topic = "t#1";
    let target_addr = "127.0.0.1:18801".parse()?;

    // 1. Subscribe to topic "t#1"
    let mut sub = client_broker.subscribe(topic).await;
    let recv_counter = Arc::new(AtomicU64::new(0));

    // Spawn Background Receiver Task
    let counter_clone = recv_counter.clone();
    tokio::spawn(async move {
        while let Some(msg) = sub.rx.recv().await {
            let count = counter_clone.fetch_add(1, Ordering::Relaxed) + 1;
            let payload_str = String::from_utf8_lossy(&msg.payload);
            if count <= 5 || count % 50 == 0 {
                println!(
                    " 📥 [Recv #{}] Topic: '{}' | Message: '{}' | Sender: '{}'",
                    count, msg.topic, payload_str, msg.sender_id
                );
            }
        }
    });

    // 2. Periodic Sequence Publisher Loop (10ms interval = 100 msg/sec)
    let total_target: u64 = 1_000_000;
    let mut ticker = interval(Duration::from_millis(10));

    println!(" 🚀 Starting Publisher Stream on Topic '{}' (10ms period)...", topic);

    for seq in 1..=total_target {
        ticker.tick().await;
        let msg_str = format!("message:#{}", seq);

        if let Err(e) = client_broker
            .publish(
                topic,
                msg_str.as_bytes().to_vec(),
                TransportMode::LanUnicast { target_addr },
            )
            .await
        {
            eprintln!(" ❌ Publish error on seq {}: {}", seq, e);
        }

        if seq <= 5 || seq % 50 == 0 {
            println!(" 📤 [Send #{}] Topic: '{}' | Payload: '{}'", seq, topic, msg_str);
        }

        // Print status milestone every 200 messages
        if seq % 200 == 0 {
            let recvd = recv_counter.load(Ordering::Relaxed);
            println!(
                " 📊 Milestone: Sent {} / {} messages | Received {} messages",
                seq, total_target, recvd
            );
        }
    }

    // Allow pending receivers to finish
    tokio::time::sleep(Duration::from_millis(500)).await;
    println!("\n✨ Sequence Stream Completed! Sent {} messages.", total_target);
    Ok(())
}
