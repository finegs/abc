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
use tui::App;

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
            // Default to TUI mode
            let notes = match client.fetch_notes().await {
                Ok(n) => n,
                Err(_) => vec![],
            };
            run_tui_app(notes)?;
        }
    }

    Ok(())
}

fn run_tui_app(items: Vec<models::NoteItem>) -> Result<(), Box<dyn Error>> {
    enable_raw_mode()?;
    let mut stdout = io::stdout();
    execute!(stdout, EnterAlternateScreen)?;
    let backend = CrosstermBackend::new(stdout);
    let mut terminal = Terminal::new(backend)?;

    let mut app = App::new(items);

    loop {
        terminal.draw(|f| app.render(f))?;

        if event::poll(std::time::Duration::from_millis(100))? {
            if let Event::Key(key) = event::read()? {
                match key.code {
                    KeyCode::Char('q') | KeyCode::Esc => break,
                    KeyCode::Char('j') | KeyCode::Down => app.next(),
                    KeyCode::Char('k') | KeyCode::Up => app.previous(),
                    _ => {}
                }
            }
        }
    }

    disable_raw_mode()?;
    execute!(terminal.backend_mut(), LeaveAlternateScreen)?;
    terminal.show_cursor()?;

    Ok(())
}
