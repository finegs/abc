use serde::{Deserialize, Serialize};
use super::model::HistoryEvent;

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub enum LineType {
    Added,
    Removed,
    Unchanged,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DiffLine {
    pub line_type: LineType,
    pub content: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FieldDiff {
    pub field_name: String,
    pub old_value: Option<String>,
    pub new_value: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct HistoryDiffRequest {
    pub event_id_1: String,
    pub event_id_2: String,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct HistoryDiffResult {
    pub event_id_1: String,
    pub event_id_2: String,
    pub field_diffs: Vec<FieldDiff>,
    pub line_diffs: Vec<DiffLine>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum MergeResolutionStrategy {
    PreferLatest,
    PreferEarliest,
    CombineText,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct HistoryMergeRequest {
    pub event_id_1: String,
    pub event_id_2: String,
    pub strategy: Option<MergeResolutionStrategy>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct HistoryMergeResult {
    pub event_id_1: String,
    pub event_id_2: String,
    pub merged_title: String,
    pub merged_content: String,
    pub merged_is_published: bool,
    pub resolved_conflicts: Vec<String>,
}

pub struct HistoryDiffMergeEngine;

impl HistoryDiffMergeEngine {
    pub fn compute_diff(event1: &HistoryEvent, event2: &HistoryEvent) -> HistoryDiffResult {
        let mut field_diffs = Vec::new();
        let mut line_diffs = Vec::new();

        if event1.event_type != event2.event_type {
            field_diffs.push(FieldDiff {
                field_name: "event_type".to_string(),
                old_value: Some(event1.event_type.clone()),
                new_value: Some(event2.event_type.clone()),
            });
        }

        let p1 = event1.payload.as_deref().unwrap_or("");
        let p2 = event2.payload.as_deref().unwrap_or("");

        let lines1: Vec<&str> = p1.lines().collect();
        let lines2: Vec<&str> = p2.lines().collect();

        for line in &lines1 {
            if !lines2.contains(line) {
                line_diffs.push(DiffLine {
                    line_type: LineType::Removed,
                    content: line.to_string(),
                });
            } else {
                line_diffs.push(DiffLine {
                    line_type: LineType::Unchanged,
                    content: line.to_string(),
                });
            }
        }

        for line in &lines2 {
            if !lines1.contains(line) {
                line_diffs.push(DiffLine {
                    line_type: LineType::Added,
                    content: line.to_string(),
                });
            }
        }

        HistoryDiffResult {
            event_id_1: event1.id.clone(),
            event_id_2: event2.id.clone(),
            field_diffs,
            line_diffs,
        }
    }

    pub fn merge_events(
        event1: &HistoryEvent,
        event2: &HistoryEvent,
        strategy: MergeResolutionStrategy,
    ) -> HistoryMergeResult {
        let p1 = event1.payload.as_deref().unwrap_or("");
        let p2 = event2.payload.as_deref().unwrap_or("");

        let (merged_title, merged_content) = match strategy {
            MergeResolutionStrategy::PreferLatest => {
                let latest_payload = if event2.created_at >= event1.created_at { p2 } else { p1 };
                ("Merged Note".to_string(), latest_payload.to_string())
            }
            MergeResolutionStrategy::PreferEarliest => {
                let earliest_payload = if event1.created_at <= event2.created_at { p1 } else { p2 };
                ("Merged Note".to_string(), earliest_payload.to_string())
            }
            MergeResolutionStrategy::CombineText => {
                let combined = format!("--- Revision 1 ({}) ---\n{}\n\n--- Revision 2 ({}) ---\n{}", event1.id, p1, event2.id, p2);
                ("Combined Merged Note".to_string(), combined)
            }
        };

        HistoryMergeResult {
            event_id_1: event1.id.clone(),
            event_id_2: event2.id.clone(),
            merged_title,
            merged_content,
            merged_is_published: true,
            resolved_conflicts: vec!["payload_conflict_resolved".to_string()],
        }
    }
}
