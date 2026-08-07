#![allow(dead_code)]

use crate::models::NoteItem;
use ratatui::{
    layout::{Constraint, Direction, Layout, Rect},
    style::{Color, Modifier, Style},
    text::{Line, Span},
    widgets::{Block, Borders, Clear, List, ListItem, ListState, Paragraph},
    Frame,
};

#[derive(Debug, PartialEq, Clone)]
pub enum AppMode {
    Normal,
    AddTitle,
    AddContent,
}

pub struct App {
    pub items: Vec<NoteItem>,
    pub list_state: ListState,
    pub mode: AppMode,
    pub status_message: String,
    pub new_title: String,
    pub new_content: String,
}

impl App {
    pub fn new(items: Vec<NoteItem>) -> Self {
        let mut list_state = ListState::default();
        if !items.is_empty() {
            list_state.select(Some(0));
        }
        Self {
            items,
            list_state,
            mode: AppMode::Normal,
            status_message: "Ready | [q] Quit | [j/k] Nav | [a] Add | [d] Del | [r] Refresh".to_string(),
            new_title: String::new(),
            new_content: String::new(),
        }
    }

    pub fn next(&mut self) {
        if self.items.is_empty() {
            return;
        }
        let i = match self.list_state.selected() {
            Some(i) => {
                if i >= self.items.len() - 1 {
                    0
                } else {
                    i + 1
                }
            }
            None => 0,
        };
        self.list_state.select(Some(i));
    }

    pub fn previous(&mut self) {
        if self.items.is_empty() {
            return;
        }
        let i = match self.list_state.selected() {
            Some(i) => {
                if i == 0 {
                    self.items.len() - 1
                } else {
                    i - 1
                }
            }
            None => 0,
        };
        self.list_state.select(Some(i));
    }

    pub fn selected_item(&self) -> Option<&NoteItem> {
        self.list_state.selected().and_then(|i| self.items.get(i))
    }

    pub fn render(&mut self, f: &mut Frame) {
        let chunks = Layout::default()
            .direction(Direction::Vertical)
            .margin(1)
            .constraints(
                [
                    Constraint::Length(3),
                    Constraint::Min(8),
                    Constraint::Length(3),
                ]
                .as_ref(),
            )
            .split(f.area());

        // Header Block
        let header = Paragraph::new("📝 Rust Axum Todo CLI TUI Manager")
            .style(
                Style::default()
                    .fg(Color::Cyan)
                    .add_modifier(Modifier::BOLD),
            )
            .block(Block::default().borders(Borders::ALL));
        f.render_widget(header, chunks[0]);

        // Main Layout (Left: List, Right: Details)
        let main_chunks = Layout::default()
            .direction(Direction::Horizontal)
            .constraints([Constraint::Percentage(40), Constraint::Percentage(60)].as_ref())
            .split(chunks[1]);

        // Note List Widget
        let list_items: Vec<ListItem> = self
            .items
            .iter()
            .map(|note| {
                let status_symbol = if note.is_published != 0 {
                    " [Published]"
                } else {
                    " [Draft]"
                };
                let content = format!("• {}{}", note.title, status_symbol);
                ListItem::new(Line::from(Span::styled(
                    content,
                    Style::default().fg(Color::White),
                )))
            })
            .collect();

        let list = List::new(list_items)
            .block(Block::default().borders(Borders::ALL).title("Notes List"))
            .highlight_style(
                Style::default()
                    .bg(Color::Blue)
                    .fg(Color::Yellow)
                    .add_modifier(Modifier::BOLD),
            )
            .highlight_symbol("> ");
        f.render_stateful_widget(list, main_chunks[0], &mut self.list_state);

        // Details Widget
        let details_text = match self.selected_item() {
            Some(note) => format!(
                "ID: {}\nTitle: {}\nStatus: {}\n\nContent:\n{}",
                note.id,
                note.title,
                if note.is_published != 0 { "Published" } else { "Draft" },
                note.content
            ),
            None => "No note selected.".to_string(),
        };

        let details = Paragraph::new(details_text)
            .block(Block::default().borders(Borders::ALL).title("Note Details"))
            .style(Style::default().fg(Color::Green));
        f.render_widget(details, main_chunks[1]);

        // Footer / Status Bar Widget
        let footer = Paragraph::new(self.status_message.as_str())
            .style(Style::default().fg(Color::Yellow))
            .block(Block::default().borders(Borders::ALL));
        f.render_widget(footer, chunks[2]);

        // Floating Modal Popup Dialog for Add Note
        if self.mode == AppMode::AddTitle || self.mode == AppMode::AddContent {
            let area = centered_rect(70, 50, f.area());
            let title_active = if self.mode == AppMode::AddTitle { " ▶" } else { "" };
            let content_active = if self.mode == AppMode::AddContent { " ▶" } else { "" };

            let popup_text = format!(
                "Title{}:\n  {}\n\nContent{}:\n  {}\n\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n[Enter] Next/Submit | [Esc] Cancel",
                title_active,
                if self.new_title.is_empty() { "<Type note title...>" } else { &self.new_title },
                content_active,
                if self.new_content.is_empty() { "<Type note content...>" } else { &self.new_content }
            );

            let popup = Paragraph::new(popup_text)
                .block(
                    Block::default()
                        .borders(Borders::ALL)
                        .title("➕ Create New Note Dialog")
                        .style(Style::default().fg(Color::Yellow)),
                )
                .style(Style::default().fg(Color::White).bg(Color::Reset));

            f.render_widget(Clear, area);
            f.render_widget(popup, area);
        }
    }
}

fn centered_rect(percent_x: u16, percent_y: u16, r: Rect) -> Rect {
    let popup_layout = Layout::default()
        .direction(Direction::Vertical)
        .constraints([
            Constraint::Percentage((100 - percent_y) / 2),
            Constraint::Percentage(percent_y),
            Constraint::Percentage((100 - percent_y) / 2),
        ])
        .split(r);

    Layout::default()
        .direction(Direction::Horizontal)
        .constraints([
            Constraint::Percentage((100 - percent_x) / 2),
            Constraint::Percentage(percent_x),
            Constraint::Percentage((100 - percent_x) / 2),
        ])
        .split(popup_layout[1])[1]
}
