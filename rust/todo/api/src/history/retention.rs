use std::{
    fs,
    path::PathBuf,
    time::{Duration, SystemTime},
};

use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, PartialEq, Eq, Serialize, Deserialize)]
pub enum UserRole {
    Admin,
    User,
}

impl UserRole {
    pub fn is_admin(&self) -> bool {
        matches!(self, UserRole::Admin)
    }

    pub fn from_header(role_str: Option<&str>) -> Self {
        match role_str {
            Some(s) if s.eq_ignore_ascii_case("admin") => UserRole::Admin,
            _ => UserRole::User,
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RetentionConfig {
    pub history_retention_days: u32,
    pub notes_retention_days: u32,
    pub stats_retention_days: u32,
    pub auto_purge_enabled: bool,
}

impl Default for RetentionConfig {
    fn default() -> Self {
        Self {
            history_retention_days: 365, // 1 Year default history retention
            notes_retention_days: 90,
            stats_retention_days: 365,
            auto_purge_enabled: true,
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct UpdateRetentionConfigSchema {
    pub history_retention_days: Option<u32>,
    pub notes_retention_days: Option<u32>,
    pub stats_retention_days: Option<u32>,
    pub auto_purge_enabled: Option<bool>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct RetentionSummary {
    pub total_partitions_checked: usize,
    pub purged_files: Vec<String>,
    pub total_purged_count: usize,
    pub retention_days: u32,
}

pub struct RetentionManager {
    base_dir: PathBuf,
    config: RetentionConfig,
}

impl RetentionManager {
    pub fn new(base_path: &str, config: RetentionConfig) -> Self {
        Self {
            base_dir: PathBuf::from(base_path),
            config,
        }
    }

    pub fn purge_expired_partitions(&self) -> RetentionSummary {
        let mut purged_files = Vec::new();
        let mut checked_count = 0;
        let retention_secs = (self.config.history_retention_days as u64) * 86_400;
        let cutoff_time = SystemTime::now()
            .checked_sub(Duration::from_secs(retention_secs))
            .unwrap_or_else(SystemTime::now);

        let sub_dirs = vec!["monthly", "weekly", "daily"];

        for sub in sub_dirs {
            let dir_path = self.base_dir.join(sub);
            if let Ok(entries) = fs::read_dir(dir_path) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.extension().and_then(|s| s.to_str()) == Some("db") {
                        checked_count += 1;
                        if let Ok(metadata) = entry.metadata() {
                            if let Ok(modified) = metadata.modified() {
                                if modified < cutoff_time {
                                    let file_name = path
                                        .file_name()
                                        .and_then(|s| s.to_str())
                                        .unwrap_or("unknown")
                                        .to_string();

                                    if fs::remove_file(&path).is_ok() {
                                        purged_files.push(file_name);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        let purged_count = purged_files.len();

        RetentionSummary {
            total_partitions_checked: checked_count,
            purged_files,
            total_purged_count: purged_count,
            retention_days: self.config.history_retention_days,
        }
    }

    pub fn get_summary(&self) -> RetentionSummary {
        let sub_dirs = vec!["monthly", "weekly", "daily"];
        let mut checked_count = 0;

        for sub in sub_dirs {
            let dir_path = self.base_dir.join(sub);
            if let Ok(entries) = fs::read_dir(dir_path) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.extension().and_then(|s| s.to_str()) == Some("db") {
                        checked_count += 1;
                    }
                }
            }
        }

        RetentionSummary {
            total_partitions_checked: checked_count,
            purged_files: Vec::new(),
            total_purged_count: 0,
            retention_days: self.config.history_retention_days,
        }
    }
}
