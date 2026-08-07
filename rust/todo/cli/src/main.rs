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
use tui::{App, AppMode, FormEditorMode, FormFocus};

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

                let is_enter_code = key.code == KeyCode::Enter
                    || key.code == KeyCode::Char('\r')
                    || key.code == KeyCode::Char('\n');

                // Universal Save shortcut: Ctrl+S or Ctrl+Enter (or Alt+Enter)
                let is_save_shortcut = (is_ctrl
                    && (key.code == KeyCode::Char('s')
                        || key.code == KeyCode::Char('S')
                        || key.code == KeyCode::Enter
                        || key.code == KeyCode::Char('\r')
                        || key.code == KeyCode::Char('\n')
                        || key.code == KeyCode::Char('j')
                        || key.code == KeyCode::Char('m')))
                    || (is_alt && is_enter_code);

                match app.mode {
                    AppMode::Normal => match key.code {
                        KeyCode::Char('q') => break,
                        KeyCode::Char('j') | KeyCode::Down => app.next(),
                        KeyCode::Char('k') | KeyCode::Up => app.previous(),
                        KeyCode::Char('g') | KeyCode::Home => app.jump_top(),
                        KeyCode::Char('G') | KeyCode::End => app.jump_bottom(),
                        KeyCode::Char('a') => app.open_add_form(),
                        KeyCode::Char('e') | KeyCode::Enter => app.open_edit_form(),
                        KeyCode::Char('d') | KeyCode::Char('x') => {
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
                        KeyCode::Char('?') | KeyCode::Char('h') => {
                            app.mode = AppMode::Help;
                        }
                        KeyCode::Esc => break,
                        _ => {}
                    },
                    AppMode::Help => match key.code {
                        KeyCode::Char('?') | KeyCode::Char('h') | KeyCode::Esc | KeyCode::Char('q') => {
                            app.mode = AppMode::Normal;
                        }
                        _ => {}
                    },
                    AppMode::AddForm | AppMode::EditForm => {
                        if is_save_shortcut {
                            submit_form(&mut app, &client, &mut terminal).await?;
                        } else {
                            match app.form_editor_mode {
                                FormEditorMode::VimNormal => match key.code {
                                    KeyCode::Char('i') | KeyCode::Char('a') => {
                                        if app.form_focus == FormFocus::SaveButton || app.form_focus == FormFocus::CancelButton {
                                            app.form_focus = FormFocus::Content;
                                        }
                                        app.form_editor_mode = FormEditorMode::VimInsert;
                                        app.status_message = "-- VIM INSERT -- (Press Esc for Normal mode)".to_string();
                                    }
                                    KeyCode::Char('o') => {
                                        app.form_focus = FormFocus::Content;
                                        app.content_insert_char('\n');
                                        app.form_editor_mode = FormEditorMode::VimInsert;
                                        app.status_message = "-- VIM INSERT -- (Opened new line)".to_string();
                                    }
                                    KeyCode::Char('j') => {
                                        app.form_focus = app.form_focus.next();
                                    }
                                    KeyCode::Char('k') => {
                                        app.form_focus = app.form_focus.previous();
                                    }
                                    KeyCode::Char('h') | KeyCode::Left => match app.form_focus {
                                        FormFocus::Title => {
                                            if app.title_cursor > 0 {
                                                app.title_cursor -= 1;
                                            }
                                        }
                                        FormFocus::Content => {
                                            if app.content_cursor > 0 {
                                                app.content_cursor -= 1;
                                            }
                                        }
                                        FormFocus::CancelButton => {
                                            app.form_focus = FormFocus::SaveButton;
                                        }
                                        FormFocus::SaveButton => {}
                                    },
                                    KeyCode::Char('l') | KeyCode::Right => match app.form_focus {
                                        FormFocus::Title => {
                                            if app.title_cursor < app.form_title.len() {
                                                app.title_cursor += 1;
                                            }
                                        }
                                        FormFocus::Content => {
                                            if app.content_cursor < app.form_content.len() {
                                                app.content_cursor += 1;
                                            }
                                        }
                                        FormFocus::SaveButton => {
                                            app.form_focus = FormFocus::CancelButton;
                                        }
                                        FormFocus::CancelButton => {}
                                    },
                                    KeyCode::Char('0') => match app.form_focus {
                                        FormFocus::Title => app.title_cursor = 0,
                                        FormFocus::Content => app.content_cursor = 0,
                                        _ => {}
                                    },
                                    KeyCode::Char('$') => match app.form_focus {
                                        FormFocus::Title => app.title_cursor = app.form_title.len(),
                                        FormFocus::Content => app.content_cursor = app.form_content.len(),
                                        _ => {}
                                    },
                                    KeyCode::Char('x') => match app.form_focus {
                                        FormFocus::Title => app.title_delete_char(),
                                        FormFocus::Content => app.content_delete_char(),
                                        _ => {}
                                    },
                                    KeyCode::Char(':') => {
                                        app.form_editor_mode = FormEditorMode::CommandInput;
                                        app.vim_cmd_buffer.clear();
                                    }
                                    KeyCode::Tab => {
                                        app.form_focus = app.form_focus.next();
                                    }
                                    KeyCode::BackTab => {
                                        app.form_focus = app.form_focus.previous();
                                    }
                                    KeyCode::Esc => {
                                        app.mode = AppMode::Normal;
                                        app.status_message = "Cancelled form editor.".to_string();
                                    }
                                    KeyCode::Enter => match app.form_focus {
                                        FormFocus::SaveButton => {
                                            submit_form(&mut app, &client, &mut terminal).await?;
                                        }
                                        FormFocus::CancelButton => {
                                            app.mode = AppMode::Normal;
                                            app.status_message = "Cancelled form editor.".to_string();
                                        }
                                        _ => {}
                                    },
                                    _ => {}
                                },
                                FormEditorMode::VimInsert => match key.code {
                                    KeyCode::Esc => {
                                        app.form_editor_mode = FormEditorMode::VimNormal;
                                        app.status_message = "-- VIM NORMAL -- [i=Insert | j/k=Move Field | h/l=Cursor | :w=Save]".to_string();
                                    }
                                    KeyCode::Tab => {
                                        app.form_focus = app.form_focus.next();
                                    }
                                    KeyCode::BackTab => {
                                        app.form_focus = app.form_focus.previous();
                                    }
                                    KeyCode::Enter | KeyCode::Char('\r') | KeyCode::Char('\n') => {
                                        match app.form_focus {
                                            FormFocus::Title => {
                                                app.form_focus = FormFocus::Content;
                                            }
                                            FormFocus::Content => {
                                                app.content_insert_char('\n');
                                            }
                                            FormFocus::SaveButton => {
                                                submit_form(&mut app, &client, &mut terminal).await?;
                                            }
                                            FormFocus::CancelButton => {
                                                app.mode = AppMode::Normal;
                                                app.status_message = "Cancelled form editor.".to_string();
                                            }
                                        }
                                    }
                                    KeyCode::Backspace => match app.form_focus {
                                        FormFocus::Title => app.title_backspace(),
                                        FormFocus::Content => app.content_backspace(),
                                        _ => {}
                                    },
                                    KeyCode::Char(c) => match app.form_focus {
                                        FormFocus::Title => app.title_insert_char(c),
                                        FormFocus::Content => app.content_insert_char(c),
                                        FormFocus::SaveButton | FormFocus::CancelButton => {}
                                    },
                                    _ => {}
                                },
                                FormEditorMode::CommandInput => match key.code {
                                    KeyCode::Esc => {
                                        app.form_editor_mode = FormEditorMode::VimNormal;
                                        app.vim_cmd_buffer.clear();
                                    }
                                    KeyCode::Enter => {
                                        let cmd = app.vim_cmd_buffer.trim();
                                        match cmd {
                                            "w" | "wq" => {
                                                submit_form(&mut app, &client, &mut terminal).await?;
                                            }
                                            "q" | "q!" => {
                                                app.mode = AppMode::Normal;
                                                app.status_message = "Cancelled form editor.".to_string();
                                            }
                                            _ => {
                                                app.status_message = format!("Unknown command: :{}", cmd);
                                                app.form_editor_mode = FormEditorMode::VimNormal;
                                            }
                                        }
                                        app.vim_cmd_buffer.clear();
                                    }
                                    KeyCode::Backspace => {
                                        app.vim_cmd_buffer.pop();
                                        if app.vim_cmd_buffer.is_empty() {
                                            app.form_editor_mode = FormEditorMode::VimNormal;
                                        }
                                    }
                                    KeyCode::Char(c) => {
                                        app.vim_cmd_buffer.push(c);
                                    }
                                    _ => {}
                                },
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
        _ => {}
    }

    app.mode = AppMode::Normal;
    Ok(())
}
