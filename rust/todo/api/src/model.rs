use serde::{Deserialize, Serialize};


#[derive(Debug, Deserialize, Serialize, sqlx::FromRow)]
#[allow(non_snake_case)]
pub struct NoteModel {
    pub id: String,
    pub title: String,
    pub content: String,
    pub is_published: i8,
    pub created_at: Option<chrono::DateTime<chrono::Utc>>,
    pub updated_at: Option<chrono::DateTime<chrono::Utc>>,
}

// For json response
#[derive(Debug, Deserialize, Serialize, PartialEq)]
#[allow(non_snake_case)]
pub struct NoteModelResponse {
    pub id: String,
    pub title: String,
    pub content: String,
    pub is_published: i8,
    pub created_at: Option<chrono::DateTime<chrono::Utc>>,
    pub updated_at: Option<chrono::DateTime<chrono::Utc>>,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_note_model_response_serialization() {
        let note_response = NoteModelResponse {
            id: "uuid-1234".to_string(),
            title: "Test Title".to_string(),
            content: "Test Content".to_string(),
            is_published: 1,
            created_at: None,
            updated_at: None,
        };

        let json = serde_json::to_string(&note_response).unwrap();
        assert!(json.contains("uuid-1234"));
        assert!(json.contains("Test Title"));

        let deserialized: NoteModelResponse = serde_json::from_str(&json).unwrap();
        assert_eq!(deserialized, note_response);
    }
}
