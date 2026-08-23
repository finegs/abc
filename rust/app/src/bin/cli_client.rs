use app_util::pubsub::{
    generate_self_signed_tls_config, PubSubBroker, SymmetricKey, TransportMode,
};
use chrono::{DateTime, Local, NaiveDateTime, TimeZone};
use std::env;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Arc;
use std::time::Duration;
use tokio::time::interval;

#[derive(Debug)]
struct CliConfig {
    topic: String,
    payload_skeleton: String,
    period_val: u64,
    period_unit: String,
    end_type: String,
    end_val: String,
    transport: String,
    target_addr: String,
}

impl Default for CliConfig {
    fn default() -> Self {
        Self {
            topic: "t#1".to_string(),
            payload_skeleton: "m#".to_string(),
            period_val: 10,
            period_unit: "ms".to_string(),
            end_type: "none".to_string(),
            end_val: "".to_string(),
            transport: "unicast".to_string(),
            target_addr: "127.0.0.1:18801".to_string(),
        }
    }
}

fn parse_args() -> CliConfig {
    let args: Vec<String> = env::args().collect();
    let mut config = CliConfig::default();

    let mut i = 1;
    while i < args.len() {
        match args[i].as_str() {
            "--topic" | "-t" => {
                if i + 1 < args.len() { config.topic = args[i + 1].clone(); i += 1; }
            }
            "--payload" | "-p" => {
                if i + 1 < args.len() { config.payload_skeleton = args[i + 1].clone(); i += 1; }
            }
            "--period" => {
                if i + 1 < args.len() {
                    if let Ok(val) = args[i + 1].parse() { config.period_val = val; }
                    i += 1;
                }
            }
            "--unit" => {
                if i + 1 < args.len() { config.period_unit = args[i + 1].clone(); i += 1; }
            }
            "--end-type" => {
                if i + 1 < args.len() { config.end_type = args[i + 1].to_lowercase(); i += 1; }
            }
            "--end-val" => {
                if i + 1 < args.len() { config.end_val = args[i + 1].clone(); i += 1; }
            }
            "--transport" => {
                if i + 1 < args.len() { config.transport = args[i + 1].to_lowercase(); i += 1; }
            }
            "--target-addr" => {
                if i + 1 < args.len() { config.target_addr = args[i + 1].clone(); i += 1; }
            }
            "--help" | "-h" => {
                print_help();
                std::process::exit(0);
            }
            _ => {}
        }
        i += 1;
    }

    config
}

fn print_help() {
    println!("📡 Pub/Sub CLI Stream Publisher Client");
    println!("Usage:");
    println!("  cargo run --bin cli_client -- [OPTIONS]\n");
    println!("Options:");
    println!("  --topic, -t <NAME>        Topic name (default: 't#1')");
    println!("  --payload, -p <SKELETON>  Payload template with '#' for seq number (default: 'm#')");
    println!("  --period <NUM>            Send period number (default: 10)");
    println!("  --unit <ms|sec>           Send period unit (default: 'ms')");
    println!("  --end-type <TYPE>         End condition: 'none', 'count', 'keyword', 'datetime'");
    println!("  --end-val <VAL>           End condition target value (e.g. '2026-07-26T15:57:51+09:00' or '2026-07-26T15:57:51')");
    println!("  --transport <MODE>        Transport: 'unicast', 'multicast', 'broadcast', 'tcp_cluster'");
    println!("  --target-addr <IP:PORT>   Target address for unicast (default: '127.0.0.1:18801')");
}

fn parse_target_datetime(val: &str) -> Option<DateTime<Local>> {
    if let Ok(dt) = DateTime::parse_from_rfc3339(val) {
        return Some(dt.with_timezone(&Local));
    }
    if let Ok(ndt) = NaiveDateTime::parse_from_str(val, "%Y-%m-%dT%H:%M:%S") {
        if let Some(dt) = Local.from_local_datetime(&ndt).single() {
            return Some(dt);
        }
    }
    if let Ok(ndt) = NaiveDateTime::parse_from_str(val, "%Y-%m-%d %H:%M:%S") {
        if let Some(dt) = Local.from_local_datetime(&ndt).single() {
            return Some(dt);
        }
    }
    None
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let config = parse_args();

    println!("⚡ Starting Pub/Sub Configurable CLI Client...");
    println!(" 📋 Topic: '{}'", config.topic);
    println!(" 📝 Payload Template: '{}'", config.payload_skeleton);
    println!(" ⏱️ Period: {}{}", config.period_val, config.period_unit);
    println!(" ⏹️ End Condition: {} ({})", config.end_type, if config.end_val.is_empty() { "None" } else { &config.end_val });
    println!(" 🚚 Transport: {}", config.transport);

    let symmetric_key = SymmetricKey::generate();
    let tls_config = generate_self_signed_tls_config("localhost")?;

    let client_broker = PubSubBroker::builder()
        .node_id("cli-configurable-client")
        .bind_udp("0.0.0.0:18897".parse()?)
        .enable_broadcast(true)
        .udp_symmetric_key(symmetric_key.0)
        .bind_tcp_cluster("0.0.0.0:19997".parse()?)
        .tls_config(tls_config)
        .build()
        .await?;

    // Subscribe for background verification
    let mut sub = client_broker.subscribe(&config.topic).await;
    let recv_counter = Arc::new(AtomicU64::new(0));
    let counter_clone = recv_counter.clone();

    tokio::spawn(async move {
        while let Some(msg) = sub.rx.recv().await {
            let count = counter_clone.fetch_add(1, Ordering::Relaxed) + 1;
            let payload_str = String::from_utf8_lossy(&msg.payload);
            if count <= 5 || count % 20 == 0 {
                println!(" 📥 [CLI Recv #{}] Topic: '{}' | Message: '{}'", count, msg.topic, payload_str);
            }
        }
    });

    let interval_duration = if config.period_unit == "sec" {
        Duration::from_secs(config.period_val)
    } else {
        Duration::from_millis(config.period_val)
    };

    let transport_mode = match config.transport.as_str() {
        "multicast" => TransportMode::LanMulticast { group_addr: "239.255.0.1:18803".parse()? },
        "broadcast" => TransportMode::LanBroadcast { broadcast_port: 18805 },
        "tcp_cluster" => TransportMode::TcpCluster,
        _ => TransportMode::LanUnicast { target_addr: config.target_addr.parse()? },
    };

    let mut ticker = interval(interval_duration);
    let mut seq: u64 = 1;

    println!("\n🚀 Stream active. Press Ctrl+C to terminate early.\n");

    loop {
        ticker.tick().await;

        let payload_str = if config.payload_skeleton.contains('#') {
            config.payload_skeleton.replace('#', &seq.to_string())
        } else {
            format!("{}-{}", config.payload_skeleton, seq)
        };

        if let Err(e) = client_broker
            .publish(&config.topic, payload_str.as_bytes().to_vec(), transport_mode.clone())
            .await
        {
            eprintln!(" ❌ Publish error on seq {}: {}", seq, e);
        } else {
            if seq <= 5 || seq % 20 == 0 {
                println!(" 📤 [CLI Send #{}] Topic: '{}' | Payload: '{}'", seq, config.topic, payload_str);
            }
        }

        // Check End Conditions
        if config.end_type == "count" {
            if let Ok(target_count) = config.end_val.parse::<u64>() {
                if seq >= target_count {
                    println!(" ⏹️ Target count reached ({}/{}). Stopping CLI stream.", seq, target_count);
                    break;
                }
            }
        } else if config.end_type == "keyword" {
            if payload_str.contains(&config.end_val) || config.topic.contains(&config.end_val) {
                println!(" ⏹️ Matched keyword '{}'. Stopping CLI stream.", config.end_val);
                break;
            }
        } else if config.end_type == "datetime" {
            if let Some(target_dt) = parse_target_datetime(&config.end_val) {
                if Local::now() >= target_dt {
                    println!(" ⏹️ Reached target date & time ({}). Stopping CLI stream.", config.end_val);
                    break;
                }
            }
        }

        seq += 1;
    }

    tokio::time::sleep(Duration::from_millis(200)).await;
    println!("✨ CLI Stream session completed! Total Sent: {}", seq);
    Ok(())
}
