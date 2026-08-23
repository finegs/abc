#[path = "../src/models.rs"]
mod models;
#[path = "../src/tui.rs"]
mod tui;

use models::{CreateNotePayload, NoteItem, NoteListResponse};
use tui::{App, AppMode};

#[test]
fn test_note_item_deserialization() {
    let json = r#"{
        "id": "test-uuid-1",
        "title": "TUI Unit Test",
        "content": "Testing models in CLI",
        "is_published": 1,
        "created_at": "2026-08-07T12:00:00Z",
        "updated_at": "2026-08-07T12:00:00Z"
    }"#;

    let item: NoteItem = serde_json::from_str(json).unwrap();
    assert_eq!(item.id, "test-uuid-1");
    assert_eq!(item.title, "TUI Unit Test");
    assert_eq!(item.is_published, 1);
}

#[test]
fn test_note_list_response_deserialization() {
    let json = r#"{
        "status": "ok",
        "count": 1,
        "notes": [
            {
                "id": "uuid-1",
                "title": "Note 1",
                "content": "Content 1",
                "is_published": 0,
                "created_at": null,
                "updated_at": null
            }
        ]
    }"#;

    let res: NoteListResponse = serde_json::from_str(json).unwrap();
    assert_eq!(res.status, "ok");
    assert_eq!(res.count, 1);
    assert_eq!(res.notes[0].title, "Note 1");
}

#[test]
fn test_create_note_payload_serialization() {
    let payload = CreateNotePayload {
        title: "New Note".to_string(),
        content: "New Content".to_string(),
        is_published: Some(true),
    };

    let json = serde_json::to_string(&payload).unwrap();
    assert!(json.contains("New Note"));
    assert!(json.contains("true"));
}

#[test]
fn test_update_note_payload_serialization() {
    use models::UpdateNotePayload;
    let payload = UpdateNotePayload {
        title: Some("Updated Title".to_string()),
        content: Some("Updated Content".to_string()),
        is_published: None,
    };

    let json = serde_json::to_string(&payload).unwrap();
    assert!(json.contains("Updated Title"));
    assert!(json.contains("Updated Content"));
}

#[test]
fn test_tui_app_navigation() {
    let items = vec![
        NoteItem {
            id: "1".to_string(),
            title: "First Note".to_string(),
            content: "First Content".to_string(),
            is_published: 1,
            created_at: None,
            updated_at: None,
        },
        NoteItem {
            id: "2".to_string(),
            title: "Second Note".to_string(),
            content: "Second Content".to_string(),
            is_published: 0,
            created_at: None,
            updated_at: None,
        },
    ];

    let mut app = App::new(items);
    assert_eq!(app.mode, AppMode::Normal);
    assert_eq!(app.list_state.selected(), Some(0));
    assert_eq!(app.selected_item().unwrap().title, "First Note");

    // Navigate next
    app.next();
    assert_eq!(app.list_state.selected(), Some(1));
    assert_eq!(app.selected_item().unwrap().title, "Second Note");

    // Navigate next (wraps around)
    app.next();
    assert_eq!(app.list_state.selected(), Some(0));

    // Navigate previous (wraps around backwards)
    app.previous();
    assert_eq!(app.list_state.selected(), Some(1));
}

#[test]
fn test_form_focus_navigation() {
    use tui::FormFocus;
    let mut focus = FormFocus::Title;
    focus = focus.next();
    assert_eq!(focus, FormFocus::Content);
    focus = focus.next();
    assert_eq!(focus, FormFocus::SaveButton);
    focus = focus.next();
    assert_eq!(focus, FormFocus::CancelButton);
    focus = focus.next();
    assert_eq!(focus, FormFocus::Title);

    focus = focus.previous();
    assert_eq!(focus, FormFocus::CancelButton);
}
