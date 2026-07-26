use app_util::pubsub::{
    generate_self_signed_tls_config, start_web_manager, PubSubBroker, RoutingRule, RuleAction,
    RuleEngine, SymmetricKey, TransportMode,
};
use std::sync::atomic::AtomicU64;
use std::sync::Arc;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 Starting Network Pub/Sub Broker & Web Manager Server...");

    // 1. Generate Security Credentials
    let symmetric_key = SymmetricKey::generate();
    let tls_config = generate_self_signed_tls_config("localhost")?;

    // 2. Build PubSub Broker using Builder Pattern
    let broker = PubSubBroker::builder()
        .node_id("production-node-1")
        .bind_udp("0.0.0.0:18801".parse()?)
        .udp_multicast("239.255.0.1:18803".parse()?)
        .enable_broadcast(true)
        .udp_symmetric_key(symmetric_key.0)
        .bind_tcp_cluster("0.0.0.0:19901".parse()?)
        .tls_config(tls_config)
        .build()
        .await?;

    println!(" ✅ LAN UDP Transport bound to 0.0.0.0:18801 (Multicast: 239.255.0.1:18803, Broadcast: Enabled)");
    println!(" ✅ Intra-LAN Payload Encryption: AES-256-GCM AEAD Enabled");
    println!(" ✅ Cross-LAN TCP Cluster bound to 0.0.0.0:19901 (TLS 1.3 Enabled)");

    // 3. Initialize Rule Engine & Sample Routing Rule
    let rule_engine = RuleEngine::new();
    rule_engine
        .add_rule(RoutingRule {
            id: "rule-auto-1".to_string(),
            name: "Route Critical Temperature Alerts".to_string(),
            source_topic_pattern: "sensors/+/temperature".to_string(),
            action: RuleAction::TransformTopic {
                new_topic: "alerts/critical_temp".to_string(),
            },
            target_transport: TransportMode::LanBroadcast {
                broadcast_port: 18805,
            },
            enabled: true,
            priority: 10,
        })
        .await;

    println!(" ✅ Initialized Rule Engine with default routing rules");

    // 4. Start Web Manager Backend & Serve UI Dashboard
    let message_counter = Arc::new(AtomicU64::new(0));
    let web_addr = "0.0.0.0:28888".parse()?;

    start_web_manager(web_addr, broker, rule_engine, message_counter).await?;

    println!("=======================================================");
    println!("🌐 WEB MANAGER DASHBOARD RUNNING AT:");
    println!("   👉 http://localhost:28888");
    println!("   👉 http://127.0.0.1:28888");
    println!("=======================================================");

    // Keep server running
    tokio::signal::ctrl_c().await?;
    println!("\nShutting down server gracefully.");
    Ok(())
}
