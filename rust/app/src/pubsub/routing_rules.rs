use crate::pubsub::protocol::{Message, TransportMode};
use crate::pubsub::topic_matcher::matches_topic;
use serde::{Deserialize, Serialize};
use std::sync::Arc;
use tokio::sync::RwLock;

/// Action to apply when a message matches a routing rule.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub enum RuleAction {
    Forward,
    TransformTopic { new_topic: String },
}

/// A configurable message routing rule.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RoutingRule {
    pub id: String,
    pub name: String,
    pub source_topic_pattern: String,
    pub action: RuleAction,
    pub target_transport: TransportMode,
    pub enabled: bool,
    pub priority: u32,
}

/// Thread-safe engine managing dynamic topic routing rules.
#[derive(Clone, Default)]
pub struct RuleEngine {
    rules: Arc<RwLock<Vec<RoutingRule>>>,
}

impl RuleEngine {
    pub fn new() -> Self {
        Self {
            rules: Arc::new(RwLock::new(Vec::new())),
        }
    }

    /// Add a new routing rule.
    pub async fn add_rule(&self, rule: RoutingRule) {
        let mut rules = self.rules.write().await;
        rules.push(rule);
        rules.sort_by(|a, b| b.priority.cmp(&a.priority));
    }

    /// Update an existing routing rule by ID.
    pub async fn update_rule(&self, rule: RoutingRule) -> bool {
        let mut rules = self.rules.write().await;
        if let Some(idx) = rules.iter().position(|r| r.id == rule.id) {
            rules[idx] = rule;
            rules.sort_by(|a, b| b.priority.cmp(&a.priority));
            true
        } else {
            false
        }
    }

    /// Remove a rule by ID.
    pub async fn remove_rule(&self, id: &str) -> bool {
        let mut rules = self.rules.write().await;
        let initial_len = rules.len();
        rules.retain(|r| r.id != id);
        rules.len() < initial_len
    }

    /// Toggle rule enabled status.
    pub async fn toggle_rule(&self, id: &str) -> Option<bool> {
        let mut rules = self.rules.write().await;
        if let Some(rule) = rules.iter_mut().find(|r| r.id == id) {
            rule.enabled = !rule.enabled;
            Some(rule.enabled)
        } else {
            None
        }
    }

    /// Get a snapshot of all current rules.
    pub async fn get_rules(&self) -> Vec<RoutingRule> {
        self.rules.read().await.clone()
    }

    /// Evaluate incoming message against active rules. Returns list of routed messages to dispatch.
    pub async fn evaluate_and_route(&self, msg: &Message) -> Vec<(Message, TransportMode)> {
        let rules = self.rules.read().await;
        let mut routed_outputs = Vec::new();

        for rule in rules.iter() {
            if !rule.enabled {
                continue;
            }

            if matches_topic(&rule.source_topic_pattern, &msg.topic) {
                let mut routed_msg = msg.clone();
                match &rule.action {
                    RuleAction::Forward => {}
                    RuleAction::TransformTopic { new_topic } => {
                        routed_msg.topic = new_topic.clone();
                    }
                }
                routed_outputs.push((routed_msg, rule.target_transport.clone()));
            }
        }

        routed_outputs
    }
}
