pub mod broker;
pub mod cross_lan_tcp;
pub mod lan_udp;
pub mod patterns;
pub mod protocol;
pub mod routing_rules;
pub mod security;
pub mod stats;
pub mod topic_matcher;
pub mod web_manager;

#[cfg(test)]
mod tests;

pub use broker::{PubSubBroker, PubSubBrokerBuilder, Subscription};
pub use patterns::{
    DeduplicationFilter, HopLimitFilter, MessageFactory, MessageFilter, MessagePipeline,
    ObserverRegistry, TransportContext, TransportStrategy,
};
pub use protocol::{Message, TransportMode};
pub use routing_rules::{RuleAction, RuleEngine, RoutingRule};
pub use security::{
    generate_self_signed_tls_config, MessageDeduplicator, SymmetricKey, TlsConfig, AES_KEY_SIZE,
};
pub use stats::{ConnectivityStatus, MetricsCollector, SystemStatsSummary, TopicStats};
pub use web_manager::start_web_manager;
