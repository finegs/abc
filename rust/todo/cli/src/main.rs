mod client;
mod models;
mod tui;

use clap::{Parser, Subcommand};
use client::ApiClient;
use crossterm::{
    event::{
        self, Event, KeyCode, KeyModifiers, KeyboardEnhancementFlags,
        PopKeyboardEnhancementFlags, PushKeyboardEnhancementFlags,
    },
    execute,
    terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen},
};
use ratatui::{backend::CrosstermBackend, Terminal};
use std::{error::Error, io};
use tui::{App, AppMode, FormFocus};

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
    let _ = execute!(
        stdout,
        EnterAlternateScreen,
        PushKeyboardEnhancementFlags(KeyboardEnhancementFlags::DISAMBIGUATE_ESCAPE_CODES)
    );
    let backend = CrosstermBackend::new(stdout);
    let mut terminal = Terminal::new(backend)?;

    let initial_notes = client.fetch_notes().await.unwrap_or_default();
    let mut app = App::new(initial_notes);

    loop {
        terminal.draw(|f| app.render(f))?;

        if event::poll(std::time::Duration::from_millis(100))? {
            if let Event::Key(key) = event::read()? {
                let is_ctrl = key.modifiers.contains(KeyModifiers::CONTROL);
                let is_alt = key.modifiers.contains(KeyModifiers::ALT);

                // Save shortcut: Ctrl+S or Ctrl+Enter (or Alt+Enter)
                let is_save_shortcut = (is_ctrl
                    && (key.code == KeyCode::Char('s')
                        || key.code == KeyCode::Char('S')
                        || key.code == KeyCode::Enter
                        || key.code == KeyCode::Char('\r')
                        || key.code == KeyCode::Char('\n')
                        || key.code == KeyCode::Char('j')
                        || key.code == KeyCode::Char('m')))
                    || (is_alt
                        && (key.code == KeyCode::Enter
                            || key.code == KeyCode::Char('\r')
                            || key.code == KeyCode::Char('\n')));

                match app.mode {
                    AppMode::Normal => match key.code {
                        KeyCode::Char('q') | KeyCode::Esc => break,
                        KeyCode::Char('j') | KeyCode::Down => app.next(),
                        KeyCode::Char('k') | KeyCode::Up => app.previous(),
                        KeyCode::Char('a') => app.open_add_form(),
                        KeyCode::Char('e') | KeyCode::Enter => app.open_edit_form(),
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
                        _ => {}
                    },
                    AppMode::AddForm | AppMode::EditForm => {
                        if is_save_shortcut {
                            submit_form(&mut app, &client, &mut terminal).await?;
                        } else {
                            match key.code {
                                KeyCode::Esc => {
                                    app.mode = AppMode::Normal;
                                    app.status_message = "Cancelled form.".to_string();
                                }
                                KeyCode::Tab | KeyCode::Down => {
                                    app.form_focus = app.form_focus.next();
                                }
                                KeyCode::BackTab | KeyCode::Up => {
                                    app.form_focus = app.form_focus.previous();
                                }
                                KeyCode::Enter | KeyCode::Char('\r') | KeyCode::Char('\n') => {
                                    match app.form_focus {
                                        FormFocus::Title => {
                                            app.form_focus = FormFocus::Content;
                                        }
                                        FormFocus::Content => {
                                            app.form_content.push('\n');
                                        }
                                        FormFocus::SaveButton => {
                                            submit_form(&mut app, &client, &mut terminal).await?;
                                        }
                                        FormFocus::CancelButton => {
                                            app.mode = AppMode::Normal;
                                            app.status_message = "Cancelled form.".to_string();
                                        }
                                    }
                                }
                                KeyCode::Char(' ') => match app.form_focus {
                                    FormFocus::SaveButton => {
                                        submit_form(&mut app, &client, &mut terminal).await?;
                                    }
                                    FormFocus::CancelButton => {
                                        app.mode = AppMode::Normal;
                                        app.status_message = "Cancelled form.".to_string();
                                    }
                                    FormFocus::Title => app.form_title.push(' '),
                                    FormFocus::Content => app.form_content.push(' '),
                                },
                                KeyCode::Backspace => match app.form_focus {
                                    FormFocus::Title => {
                                        app.form_title.pop();
                                    }
                                    FormFocus::Content => {
                                        app.form_content.pop();
                                    }
                                    _ => {}
                                },
                                KeyCode::Char(c) => {
                                    if c != '\r' && c != '\n' {
                                        match app.form_focus {
                                            FormFocus::Title => {
                                                app.form_title.push(c);
                                            }
                                            FormFocus::Content => {
                                                app.form_content.push(c);
                                            }
                                            _ => {}
                                        }
                                    }
                                }
                                _ => {}
                            }
                        }
                    }
                }
            }
        }
    }

    disable_raw_mode()?;
    let _ = execute!(
        terminal.backend_mut(),
        PopKeyboardEnhancementFlags,
        LeaveAlternateScreen
    );
    terminal.show_cursor()?;

    Ok(())
}

async fn submit_form(
    app: &mut App,
    client: &ApiClient,
    terminal: &mut Terminal<CrosstermBackend<io::Stdout>>,
) -> Result<(), Box<dyn Error>> {
    let title = app.form_title.trim().to_string();
    let content = app.form_content.trim().to_string();

    if title.is_empty() {
        app.status_message = "❌ Title cannot be empty!".to_string();
        return Ok(());
    }

    match app.mode {
        AppMode::AddForm => {
            app.status_message = format!("Creating note '{}'...", title);
            terminal.draw(|f| app.render(f))?;
            if client.create_note(&title, &content, true).await.unwrap_or(false) {
                app.status_message = format!("✅ Note '{}' created successfully!", title);
                if let Ok(updated) = client.fetch_notes().await {
                    app.items = updated;
                    app.list_state.select(Some(app.items.len().saturating_sub(1)));
                }
            } else {
                app.status_message = "❌ Failed to create note!".to_string();
            }
        }
        AppMode::EditForm => {
            if let Some(id) = app.form_id.clone() {
                app.status_message = format!("Saving updates for note '{}'...", title);
                terminal.draw(|f| app.render(f))?;
                if client
                    .edit_note(&id, Some(&title), Some(&content), None)
                    .await
                    .unwrap_or(false)
                {
                    app.status_message = format!("✅ Note '{}' updated successfully!", title);
                    if let Ok(updated) = client.fetch_notes().await {
                        app.items = updated;
                    }
                } else {
                    app.status_message = "❌ Failed to update note!".to_string();
                }
            }
        }
        AppMode::Normal => {}
    }

    app.mode = AppMode::Normal;
    Ok(())
}
