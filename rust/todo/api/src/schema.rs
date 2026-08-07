use serde::{Deserialize, Serialize};

// List
#[derive(Deserialize, Debug, Default)]
pub struct FilterOptions {
    pub page: Option<usize>,
    pub limit: Option<usize>,
}

// Create
#[derive(Serialize, Deserialize, Debug)]
pub struct CreateNoteSchema {
    pub title: String,
    pub content: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub is_published: Option<bool>,
}

// Update
#[derive(Serialize, Deserialize, Debug)]
pub struct UpdateNoteSchema {
    pub title: Option<String>,
    pub content: Option<String>,
    pub is_published: Option<bool>,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_create_note_schema_deserialization() {
        let json = r#"{"title": "Rust Guide", "content": "Axum + SQLx"}"#;
        let schema: CreateNoteSchema = serde_json::from_str(json).unwrap();
        assert_eq!(schema.title, "Rust Guide");
        assert_eq!(schema.content, "Axum + SQLx");
        assert_eq!(schema.is_published, None);
    }

    #[test]
    fn test_update_note_schema_deserialization() {
        let json = r#"{"title": "Updated Title", "is_published": true}"#;
        let schema: UpdateNoteSchema = serde_json::from_str(json).unwrap();
        assert_eq!(schema.title, Some("Updated Title".to_string()));
        assert_eq!(schema.content, None);
        assert_eq!(schema.is_published, Some(true));
    }

    #[test]
    fn test_filter_options_deserialization() {
        let json = r#"{"page": 2, "limit": 20}"#;
        let opts: FilterOptions = serde_json::from_str(json).unwrap();
        assert_eq!(opts.page, Some(2));
        assert_eq!(opts.limit, Some(20));
    }
}
