use std::{fs, path::PathBuf, sync::Arc};
use tokio::time::{sleep, Duration};
use todo_api::history::{
    HistoryFacade, HistoryQueryFilter, HistoryStoreFactory, PartitionConfig, PartitionGranularity,
};

#[tokio::test]
async fn test_5_design_patterns_and_adaptive_repartitioning() {
    let test_dir = PathBuf::from("target/test_history_db");
    let _ = fs::remove_dir_all(&test_dir);
    let _ = fs::create_dir_all(&test_dir);

    // 1. Factory Pattern & Config: Set threshold to 2 events to trigger auto-escalation
    let config = PartitionConfig {
        max_rows_per_partition: 2,
    };
    let backend = HistoryStoreFactory::create_backend(test_dir.to_str().unwrap(), config).await;

    // 2. Facade Pattern
    let facade = Arc::new(HistoryFacade::new(backend));

    // Verify initial State Pattern granularity is Monthly
    assert_eq!(facade.current_granularity(), PartitionGranularity::Monthly);

    // 3. Observer Pattern: Publish 1st event
    facade.record_action("CREATED", "note", "note-1", Some("payload-1".to_string()));
    sleep(Duration::from_millis(150)).await;

    // Still Monthly after 1 event
    assert_eq!(facade.current_granularity(), PartitionGranularity::Monthly);

    // Publish 2nd event -> Escalates Monthly -> Weekly (threshold = 2)
    facade.record_action("UPDATED", "note", "note-1", Some("payload-2".to_string()));
    sleep(Duration::from_millis(150)).await;

    assert_eq!(facade.current_granularity(), PartitionGranularity::Weekly);

    // Publish 3rd event
    facade.record_action("CREATED", "note", "note-2", Some("payload-3".to_string()));
    sleep(Duration::from_millis(150)).await;

    // Publish 4th event -> Escalates Weekly -> Daily (threshold = 2)
    facade.record_action("DELETED", "note", "note-1", None);
    sleep(Duration::from_millis(150)).await;

    assert_eq!(facade.current_granularity(), PartitionGranularity::Daily);

    // 4. Facade query execution across partitions
    let filter = HistoryQueryFilter::default();
    let events = facade.query_trail(filter).await.expect("Failed to query trail");

    assert_eq!(events.len(), 4);

    // 5. Partition statistics check
    let stats = facade.get_stats().await.expect("Failed to get stats");
    assert!(!stats.is_empty());

    let _ = fs::remove_dir_all(&test_dir);
}
