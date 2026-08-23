use std::{
    fs,
    path::{Path, PathBuf},
    str::FromStr,
    sync::{Arc, Mutex},
};

use chrono::Utc;
use sqlx::{
    sqlite::{SqliteConnectOptions, SqlitePoolOptions},
    SqlitePool,
};

use super::{
    model::{HistoryEvent, HistoryQueryFilter, PartitionConfig, PartitionStat},
    state::{PartitionGranularity, PartitionState},
};

#[derive(Clone)]
pub struct SqlitePartitionStrategy {
    base_dir: PathBuf,
    config: PartitionConfig,
    state: Arc<Mutex<PartitionState>>,
    active_row_count: Arc<Mutex<u64>>,
}

impl SqlitePartitionStrategy {
    pub async fn new(base_path: &str, config: PartitionConfig) -> Self {
        let base_dir = PathBuf::from(base_path);
        let _ = fs::create_dir_all(base_dir.join("monthly"));
        let _ = fs::create_dir_all(base_dir.join("weekly"));
        let _ = fs::create_dir_all(base_dir.join("daily"));

        Self {
            base_dir,
            config,
            state: Arc::new(Mutex::new(PartitionState::default())),
            active_row_count: Arc::new(Mutex::new(0)),
        }
    }

    fn current_partition_path(&self, now: &chrono::DateTime<Utc>) -> (PathBuf, String, String) {
        let state = self.state.lock().unwrap().clone();
        match state.current_granularity {
            PartitionGranularity::Monthly => {
                let key = now.format("%Y_%m").to_string();
                let file = format!("history_{}.db", key);
                (self.base_dir.join("monthly").join(&file), key, "Monthly".to_string())
            }
            PartitionGranularity::Weekly => {
                let key = now.format("%Y_W%V").to_string();
                let file = format!("history_{}.db", key);
                (self.base_dir.join("weekly").join(&file), key, "Weekly".to_string())
            }
            PartitionGranularity::Daily => {
                let key = now.format("%Y_%m_%d").to_string();
                let file = format!("history_{}.db", key);
                (self.base_dir.join("daily").join(&file), key, "Daily".to_string())
            }
        }
    }

    async fn get_pool_for_file(&self, path: &Path) -> Result<SqlitePool, String> {
        if let Some(parent) = path.parent() {
            let _ = fs::create_dir_all(parent);
        }
        let db_url = format!("sqlite://{}", path.to_str().unwrap_or_default());
        let connect_opts = SqliteConnectOptions::from_str(&db_url)
            .map_err(|e| e.to_string())?
            .create_if_missing(true);

        let pool = SqlitePoolOptions::new()
            .max_connections(5)
            .connect_with(connect_opts)
            .await
            .map_err(|e| e.to_string())?;

        sqlx::query(
            r#"
            CREATE TABLE IF NOT EXISTS history_events (
                id VARCHAR(36) PRIMARY KEY NOT NULL,
                event_type VARCHAR(50) NOT NULL,
                entity_type VARCHAR(50) NOT NULL,
                entity_id VARCHAR(36) NOT NULL,
                payload TEXT,
                created_at VARCHAR(50) NOT NULL
            );
            "#,
        )
        .execute(&pool)
        .await
        .map_err(|e| e.to_string())?;

        Ok(pool)
    }

    pub async fn record_event(&self, event: &HistoryEvent) -> Result<(), String> {
        let now = Utc::now();
        let (db_path, _key, _gran) = self.current_partition_path(&now);
        let pool = self.get_pool_for_file(&db_path).await?;

        sqlx::query(
            "INSERT INTO history_events (id, event_type, entity_type, entity_id, payload, created_at) VALUES (?, ?, ?, ?, ?, ?)"
        )
        .bind(&event.id)
        .bind(&event.event_type)
        .bind(&event.entity_type)
        .bind(&event.entity_id)
        .bind(&event.payload)
        .bind(&event.created_at)
        .execute(&pool)
        .await
        .map_err(|e| e.to_string())?;

        let mut count = self.active_row_count.lock().unwrap();
        *count += 1;
        let current_count = *count;

        let mut state = self.state.lock().unwrap();
        if state.evaluate_escalation(current_count, self.config.max_rows_per_partition) {
            *count = 0;
        }

        Ok(())
    }

    pub async fn query_events(&self, filter: &HistoryQueryFilter) -> Result<Vec<HistoryEvent>, String> {
        let mut results = Vec::new();
        let dirs = vec!["monthly", "weekly", "daily"];

        for sub in dirs {
            let dir_path = self.base_dir.join(sub);
            if let Ok(entries) = fs::read_dir(dir_path) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.extension().and_then(|s| s.to_str()) == Some("db") {
                        if let Ok(pool) = self.get_pool_for_file(&path).await {
                            let mut query_str = "SELECT id, event_type, entity_type, entity_id, payload, created_at FROM history_events WHERE 1=1".to_string();

                            if filter.entity_type.is_some() {
                                query_str.push_str(" AND entity_type = ?");
                            }
                            if filter.entity_id.is_some() {
                                query_str.push_str(" AND entity_id = ?");
                            }
                            if filter.event_type.is_some() {
                                query_str.push_str(" AND event_type = ?");
                            }

                            query_str.push_str(" ORDER BY created_at DESC LIMIT ?");

                            let limit = filter.limit.unwrap_or(50) as i64;
                            let mut query = sqlx::query_as::<_, HistoryEvent>(&query_str);

                            if let Some(ref et) = filter.entity_type {
                                query = query.bind(et);
                            }
                            if let Some(ref ei) = filter.entity_id {
                                query = query.bind(ei);
                            }
                            if let Some(ref evt) = filter.event_type {
                                query = query.bind(evt);
                            }

                            query = query.bind(limit);

                            if let Ok(events) = query.fetch_all(&pool).await {
                                results.extend(events);
                            }
                        }
                    }
                }
            }
        }

        results.sort_by(|a, b| b.created_at.cmp(&a.created_at));
        results.truncate(filter.limit.unwrap_or(50));
        Ok(results)
    }

    pub async fn get_partition_stats(&self) -> Result<Vec<PartitionStat>, String> {
        let mut stats = Vec::new();
        let dirs = vec![
            ("monthly", "Monthly"),
            ("weekly", "Weekly"),
            ("daily", "Daily"),
        ];

        for (sub, gran_label) in dirs {
            let dir_path = self.base_dir.join(sub);
            if let Ok(entries) = fs::read_dir(dir_path) {
                for entry in entries.flatten() {
                    let path = entry.path();
                    if path.extension().and_then(|s| s.to_str()) == Some("db") {
                        if let Ok(pool) = self.get_pool_for_file(&path).await {
                            let count: (i64,) = sqlx::query_as("SELECT COUNT(*) FROM history_events")
                                .fetch_one(&pool)
                                .await
                                .unwrap_or((0,));

                            let file_name = path
                                .file_stem()
                                .and_then(|s| s.to_str())
                                .unwrap_or("unknown")
                                .to_string();

                            stats.push(PartitionStat {
                                partition_key: file_name,
                                granularity: gran_label.to_string(),
                                file_path: path.to_string_lossy().to_string(),
                                row_count: count.0 as u64,
                            });
                        }
                    }
                }
            }
        }

        Ok(stats)
    }

    pub fn get_current_granularity(&self) -> PartitionGranularity {
        self.state.lock().unwrap().current_granularity
    }
}
