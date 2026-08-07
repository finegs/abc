mod client;
mod models;
mod tui;

use clap::{Parser, Subcommand};
use client::ApiClient;
use crossterm::{
    event::{self, Event, KeyCode},
    execute,
    terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen},
};
use ratatui::{backend::CrosstermBackend, Terminal};
use std::{error::Error, io};
use tui::{App, AppMode};

#[derive(Parser, Debug)]
#[command(name = "todo-cli")]
#[command(about = "Rust Axum Todo TUI & CLI Manager", long_about = None)]
struct CliArgs {
    /// Base URL of the REST API server
    #[arg(short, long, default_value = "http://127.0.0.1:8080")]
    api_url: String,

    #[command(subcommand)]
    command: Option<Commands>,
}

#[derive(Subcommand, Debug)]
enum Commands {
    /// Health check status of the REST API server
    Health,
    /// List all notes
    List,
    /// Create a new note
    Add {
        #[arg(short, long)]
        title: String,
        #[arg(short, long)]
        content: String,
        #[arg(short, long, default_value_t = true)]
        published: bool,
    },
    /// Delete a note by ID
    Delete {
        #[arg(short, long)]
        id: String,
    },
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let args = CliArgs::parse();
    let client = ApiClient::new(&args.api_url);

    match args.command {
        Some(Commands::Health) => {
            let ok = client.healthcheck().await?;
            if ok {
                println!("✅ API Server at {} is healthy!", args.api_url);
            } else {
                println!("❌ API Server at {} returned error!", args.api_url);
            }
        }
        Some(Commands::List) => {
            let notes = client.fetch_notes().await?;
            println!("📋 Total Notes: {}", notes.len());
            for note in notes {
                println!(
                    "- [{}] {} ({})",
                    note.id,
                    note.title,
                    if note.is_published != 0 {
                        "Published"
                    } else {
                        "Draft"
                    }
                );
            }
        }
        Some(Commands::Add {
            title,
            content,
            published,
        }) => {
            let ok = client.create_note(&title, &content, published).await?;
            if ok {
                println!("✅ Note '{}' created successfully!", title);
            } else {
                println!("❌ Failed to create note!");
            }
        }
        Some(Commands::Delete { id }) => {
            let ok = client.delete_note(&id).await?;
            if ok {
                println!("✅ Note '{}' deleted successfully!", id);
            } else {
                println!("❌ Failed to delete note!");
            }
        }
        None => {
            // Default to interactive TUI mode
            run_tui_app(client).await?;
        }
    }

    Ok(())
}

async fn run_tui_app(client: ApiClient) -> Result<(), Box<dyn Error>> {
    enable_raw_mode()?;
    let mut stdout = io::stdout();
    execute!(stdout, EnterAlternateScreen)?;
    let backend = CrosstermBackend::new(stdout);
    let mut terminal = Terminal::new(backend)?;

    let initial_notes = client.fetch_notes().await.unwrap_or_default();
    let mut app = App::new(initial_notes);

    loop {
        terminal.draw(|f| app.render(f))?;

        if event::poll(std::time::Duration::from_millis(100))? {
            if let Event::Key(key) = event::read()? {
                match app.mode {
                    AppMode::Normal => match key.code {
                        KeyCode::Char('q') => break,
                        KeyCode::Char('j') | KeyCode::Down => app.next(),
                        KeyCode::Char('k') | KeyCode::Up => app.previous(),
                        KeyCode::Char('a') => {
                            app.mode = AppMode::AddTitle;
                            app.new_title.clear();
                            app.new_content.clear();
                            app.status_message =
                                "MODE: Add Note (Title) | Type title -> press [Enter]".to_string();
                        }
                        KeyCode::Char('e') | KeyCode::Enter => {
                            if let Some(note) = app.selected_item().cloned() {
                                app.mode = AppMode::EditTitle;
                                app.editing_id = Some(note.id);
                                app.edit_title = note.title;
                                app.edit_content = note.content;
                                app.status_message =
                                    "MODE: Edit Note (Title) | Edit title -> press [Enter]".to_string();
                            }
                        }
                        KeyCode::Char('d') => {
                            if let Some(note) = app.selected_item().cloned() {
                                app.status_message = format!("Deleting note '{}'...", note.title);
                                terminal.draw(|f| app.render(f))?;
                                if client.delete_note(&note.id).await.unwrap_or(false) {
                                    app.status_message = format!("✅ Note '{}' deleted!", note.title);
                                    if let Ok(updated) = client.fetch_notes().await {
                                        app.items = updated;
                                        if !app.items.is_empty() {
                                            app.list_state.select(Some(0));
                                        } else {
                                            app.list_state.select(None);
                                        }
                                    }
                                } else {
                                    app.status_message = "❌ Failed to delete note!".to_string();
                                }
                            }
                        }
                        KeyCode::Char('r') => {
                            if let Ok(updated) = client.fetch_notes().await {
                                app.items = updated;
                                app.status_message = "🔄 Refreshed notes list!".to_string();
                            }
                        }
                        KeyCode::Esc => break,
                        _ => {}
                    },
                    AppMode::AddTitle => match key.code {
                        KeyCode::Esc => {
                            app.mode = AppMode::Normal;
                            app.status_message = "Cancelled adding note.".to_string();
                        }
                        KeyCode::Enter => {
                            if !app.new_title.trim().is_empty() {
                                app.mode = AppMode::AddContent;
                                app.status_message =
                                    "MODE: Add Note (Content) | Type content -> press [Enter] to submit"
                                        .to_string();
                            }
                        }
                        KeyCode::Backspace => {
                            app.new_title.pop();
                        }
                        KeyCode::Char(c) => {
                            app.new_title.push(c);
                        }
                        _ => {}
                    },
                    AppMode::AddContent => match key.code {
                        KeyCode::Esc => {
                            app.mode = AppMode::Normal;
                            app.status_message = "Cancelled adding note.".to_string();
                        }
                        KeyCode::Enter => {
                            let title = app.new_title.trim().to_string();
                            let content = app.new_content.trim().to_string();
                            if !title.is_empty() {
                                app.status_message = format!("Creating note '{}'...", title);
                                terminal.draw(|f| app.render(f))?;
                                if client.create_note(&title, &content, true).await.unwrap_or(false) {
                                    app.status_message =
                                        format!("✅ Note '{}' created successfully!", title);
                                    if let Ok(updated) = client.fetch_notes().await {
                                        app.items = updated;
                                        app.list_state.select(Some(app.items.len().saturating_sub(1)));
                                    }
                                } else {
                                    app.status_message = "❌ Failed to create note!".to_string();
                                }
                            }
                            app.mode = AppMode::Normal;
                        }
                        KeyCode::Backspace => {
                            app.new_content.pop();
                        }
                        KeyCode::Char(c) => {
                            app.new_content.push(c);
                        }
                        _ => {}
                    },
                    AppMode::EditTitle => match key.code {
                        KeyCode::Esc => {
                            app.mode = AppMode::Normal;
                            app.status_message = "Cancelled editing note.".to_string();
                        }
                        KeyCode::Enter => {
                            app.mode = AppMode::EditContent;
                            app.status_message =
                                "MODE: Edit Note (Content) | Edit content -> press [Enter] to save"
                                    .to_string();
                        }
                        KeyCode::Backspace => {
                            app.edit_title.pop();
                        }
                        KeyCode::Char(c) => {
                            app.edit_title.push(c);
                        }
                        _ => {}
                    },
                    AppMode::EditContent => match key.code {
                        KeyCode::Esc => {
                            app.mode = AppMode::Normal;
                            app.status_message = "Cancelled editing note.".to_string();
                        }
                        KeyCode::Enter => {
                            if let Some(id) = app.editing_id.clone() {
                                let title = app.edit_title.trim().to_string();
                                let content = app.edit_content.trim().to_string();
                                app.status_message = format!("Saving updates for note '{}'...", title);
                                terminal.draw(|f| app.render(f))?;
                                if client
                                    .edit_note(&id, Some(&title), Some(&content), None)
                                    .await
                                    .unwrap_or(false)
                                {
                                    app.status_message =
                                        format!("✅ Note '{}' updated successfully!", title);
                                    if let Ok(updated) = client.fetch_notes().await {
                                        app.items = updated;
                                    }
                                } else {
                                    app.status_message = "❌ Failed to update note!".to_string();
                                }
                            }
                            app.mode = AppMode::Normal;
                        }
                        KeyCode::Backspace => {
                            app.edit_content.pop();
                        }
                        KeyCode::Char(c) => {
                            app.edit_content.push(c);
                        }
                        _ => {}
                    },
                }
            }
        }
    }

    disable_raw_mode()?;
    execute!(terminal.backend_mut(), LeaveAlternateScreen)?;
    terminal.show_cursor()?;

    Ok(())
}
