use std::{fs, path::PathBuf};

use todo_api::history::{
    diff_merge::{HistoryDiffMergeEngine, MergeResolutionStrategy},
    model::HistoryEvent,
    retention::{RetentionConfig, RetentionManager},
};

#[test]
fn test_history_diff_computation() {
    let event1 = HistoryEvent {
        id: "evt-1".to_string(),
        event_type: "CREATED".to_string(),
        entity_type: "note".to_string(),
        entity_id: "note-100".to_string(),
        payload: Some("Title: First Note\nContent: Initial Version".to_string()),
        created_at: "2026-08-01T10:00:00Z".to_string(),
    };

    let event2 = HistoryEvent {
        id: "evt-2".to_string(),
        event_type: "UPDATED".to_string(),
        entity_type: "note".to_string(),
        entity_id: "note-100".to_string(),
        payload: Some("Title: First Note\nContent: Updated Version\nExtra Info".to_string()),
        created_at: "2026-08-02T10:00:00Z".to_string(),
    };

    let diff = HistoryDiffMergeEngine::compute_diff(&event1, &event2);

    assert_eq!(diff.event_id_1, "evt-1");
    assert_eq!(diff.event_id_2, "evt-2");
    assert_eq!(diff.field_diffs.len(), 1);
    assert_eq!(diff.field_diffs[0].field_name, "event_type");
    assert!(!diff.line_diffs.is_empty());
}

#[test]
fn test_history_merge_strategies() {
    let event1 = HistoryEvent {
        id: "evt-1".to_string(),
        event_type: "CREATED".to_string(),
        entity_type: "note".to_string(),
        entity_id: "note-100".to_string(),
        payload: Some("Content A".to_string()),
        created_at: "2026-08-01T10:00:00Z".to_string(),
    };

    let event2 = HistoryEvent {
        id: "evt-2".to_string(),
        event_type: "UPDATED".to_string(),
        entity_type: "note".to_string(),
        entity_id: "note-100".to_string(),
        payload: Some("Content B".to_string()),
        created_at: "2026-08-02T10:00:00Z".to_string(),
    };

    // Prefer Latest
    let merge_latest = HistoryDiffMergeEngine::merge_events(&event1, &event2, MergeResolutionStrategy::PreferLatest);
    assert_eq!(merge_latest.merged_content, "Content B");

    // Prefer Earliest
    let merge_earliest = HistoryDiffMergeEngine::merge_events(&event1, &event2, MergeResolutionStrategy::PreferEarliest);
    assert_eq!(merge_earliest.merged_content, "Content A");

    // Combine Text
    let merge_combine = HistoryDiffMergeEngine::merge_events(&event1, &event2, MergeResolutionStrategy::CombineText);
    assert!(merge_combine.merged_content.contains("Content A"));
    assert!(merge_combine.merged_content.contains("Content B"));
}

#[test]
fn test_retention_purge_engine() {
    let test_path = PathBuf::from("target/test_retention_db");
    let _ = fs::remove_dir_all(&test_path);
    let monthly_dir = test_path.join("monthly");
    let _ = fs::create_dir_all(&monthly_dir);

    let dummy_file = monthly_dir.join("history_2024_01.db");
    fs::write(&dummy_file, b"dummy data").unwrap();

    let config = RetentionConfig {
        history_retention_days: 0, // Purge immediately for test
        notes_retention_days: 90,
        stats_retention_days: 365,
        auto_purge_enabled: true,
    };

    let manager = RetentionManager::new(test_path.to_str().unwrap(), config);
    let summary = manager.purge_expired_partitions();

    assert_eq!(summary.total_partitions_checked, 1);
    assert_eq!(summary.total_purged_count, 1);
    assert!(!dummy_file.exists());

    let _ = fs::remove_dir_all(&test_path);
}
