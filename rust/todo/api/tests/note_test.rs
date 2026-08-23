use sqlx::sqlite::SqlitePoolOptions;

#[tokio::test]
async fn test_note_crud_lifecycle() -> Result<(), Box<dyn std::error::Error>> {
    let pool = SqlitePoolOptions::new()
        .connect("sqlite::memory:")
        .await?;

    // Run migrations
    sqlx::migrate!().run(&pool).await?;

    let note_id = uuid::Uuid::new_v4().to_string();
    let title = format!("Test Note {}", note_id);
    let content = "Test Content";

    // 1. Create
    let insert_result = sqlx::query(
        "INSERT INTO notes (id, title, content, is_published) VALUES (?, ?, ?, ?)",
    )
    .bind(&note_id)
    .bind(&title)
    .bind(content)
    .bind(1i8)
    .execute(&pool)
    .await?;

    assert_eq!(insert_result.rows_affected(), 1);

    // 2. Read (Get Note)
    let (fetched_title, fetched_content): (String, String) =
        sqlx::query_as("SELECT title, content FROM notes WHERE id = ?")
            .bind(&note_id)
            .fetch_one(&pool)
            .await?;

    assert_eq!(fetched_title, title);
    assert_eq!(fetched_content, content);

    // 3. Update Note
    let updated_content = "Updated Test Content";
    let update_result = sqlx::query("UPDATE notes SET content = ? WHERE id = ?")
        .bind(updated_content)
        .bind(&note_id)
        .execute(&pool)
        .await?;

    assert_eq!(update_result.rows_affected(), 1);

    // 4. Delete Note
    let delete_result = sqlx::query("DELETE FROM notes WHERE id = ?")
        .bind(&note_id)
        .execute(&pool)
        .await?;

    assert_eq!(delete_result.rows_affected(), 1);

    Ok(())
}
