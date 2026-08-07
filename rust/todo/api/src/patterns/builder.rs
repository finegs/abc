#![allow(dead_code)]

use crate::model::NoteModel;
use crate::schema::CreateNoteSchema;

/// 🔹 2. BUILDER PATTERN (For NoteModel)
/// Enables step-by-step fluid construction of NoteModel objects.
pub struct NoteBuilder {
    id: String,
    title: String,
    content: String,
    is_published: i8,
}

impl NoteBuilder {
    pub fn new(title: impl Into<String>, content: impl Into<String>) -> Self {
        Self {
            id: uuid::Uuid::new_v4().to_string(),
            title: title.into(),
            content: content.into(),
            is_published: 0,
        }
    }

    pub fn id(mut self, id: impl Into<String>) -> Self {
        self.id = id.into();
        self
    }

    pub fn is_published(mut self, published: bool) -> Self {
        self.is_published = if published { 1 } else { 0 };
        self
    }

    pub fn build(self) -> NoteModel {
        NoteModel {
            id: self.id,
            title: self.title,
            content: self.content,
            is_published: self.is_published,
            created_at: Some(chrono::Utc::now()),
            updated_at: Some(chrono::Utc::now()),
        }
    }
}

/// 🔹 BUILDER PATTERN (For CreateNoteSchema)
pub struct CreateNoteSchemaBuilder {
    title: String,
    content: String,
    is_published: Option<bool>,
}

impl CreateNoteSchemaBuilder {
    pub fn new(title: impl Into<String>, content: impl Into<String>) -> Self {
        Self {
            title: title.into(),
            content: content.into(),
            is_published: None,
        }
    }

    pub fn is_published(mut self, published: bool) -> Self {
        self.is_published = Some(published);
        self
    }

    pub fn build(self) -> CreateNoteSchema {
        CreateNoteSchema {
            title: self.title,
            content: self.content,
            is_published: self.is_published,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_note_builder() {
        let note = NoteBuilder::new("Builder Test", "Testing builder pattern")
            .id("fixed-uuid")
            .is_published(true)
            .build();

        assert_eq!(note.id, "fixed-uuid");
        assert_eq!(note.title, "Builder Test");
        assert_eq!(note.content, "Testing builder pattern");
        assert_eq!(note.is_published, 1);
    }

    #[test]
    fn test_create_note_schema_builder() {
        let schema = CreateNoteSchemaBuilder::new("Schema Title", "Schema Content")
            .is_published(true)
            .build();

        assert_eq!(schema.title, "Schema Title");
        assert_eq!(schema.content, "Schema Content");
        assert_eq!(schema.is_published, Some(true));
    }
}
