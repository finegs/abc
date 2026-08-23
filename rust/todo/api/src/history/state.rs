use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum PartitionGranularity {
    Monthly,
    Weekly,
    Daily,
}

impl PartitionGranularity {
    pub fn as_str(&self) -> &'static str {
        match self {
            PartitionGranularity::Monthly => "Monthly",
            PartitionGranularity::Weekly => "Weekly",
            PartitionGranularity::Daily => "Daily",
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PartitionState {
    pub current_granularity: PartitionGranularity,
}

impl Default for PartitionState {
    fn default() -> Self {
        Self {
            current_granularity: PartitionGranularity::Monthly,
        }
    }
}

impl PartitionState {
    pub fn new(granularity: PartitionGranularity) -> Self {
        Self {
            current_granularity: granularity,
        }
    }

    pub fn evaluate_escalation(&mut self, current_row_count: u64, threshold: u64) -> bool {
        if current_row_count >= threshold {
            match self.current_granularity {
                PartitionGranularity::Monthly => {
                    self.current_granularity = PartitionGranularity::Weekly;
                    println!("🚀 Auto-Repartitioning Escalated: Monthly -> Weekly");
                    true
                }
                PartitionGranularity::Weekly => {
                    self.current_granularity = PartitionGranularity::Daily;
                    println!("🚀 Auto-Repartitioning Escalated: Weekly -> Daily");
                    true
                }
                PartitionGranularity::Daily => false,
            }
        } else {
            false
        }
    }
}
