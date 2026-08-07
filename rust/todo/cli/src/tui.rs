#![allow(dead_code)]

use crate::models::NoteItem;
use ratatui::{
    layout::{Constraint, Direction, Layout, Rect},
    style::{Color, Modifier, Style},
    text::{Line, Span},
    widgets::{Block, Borders, Clear, List, ListItem, ListState, Paragraph},
    Frame,
};

#[derive(Debug, PartialEq, Clone, Copy)]
pub enum FormFocus {
    Title,
    Content,
    SaveButton,
    CancelButton,
}

impl FormFocus {
    pub fn next(&self) -> Self {
        match self {
            FormFocus::Title => FormFocus::Content,
            FormFocus::Content => FormFocus::SaveButton,
            FormFocus::SaveButton => FormFocus::CancelButton,
            FormFocus::CancelButton => FormFocus::Title,
        }
    }

    pub fn previous(&self) -> Self {
        match self {
            FormFocus::Title => FormFocus::CancelButton,
            FormFocus::Content => FormFocus::Title,
            FormFocus::SaveButton => FormFocus::Content,
            FormFocus::CancelButton => FormFocus::SaveButton,
        }
    }
}

#[derive(Debug, PartialEq, Clone)]
pub enum AppMode {
    Normal,
    AddForm,
    EditForm,
}

pub struct App {
    pub items: Vec<NoteItem>,
    pub list_state: ListState,
    pub mode: AppMode,
    pub form_focus: FormFocus,
    pub status_message: String,
    pub form_id: Option<String>,
    pub form_title: String,
    pub form_content: String,
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
            form_focus: FormFocus::Title,
            status_message: "Ready | [q] Quit | [j/k] Nav | [e/Enter] Edit | [a] Add | [d] Del | [r] Refresh".to_string(),
            form_id: None,
            form_title: String::new(),
            form_content: String::new(),
        }
    }

    pub fn open_add_form(&mut self) {
        self.mode = AppMode::AddForm;
        self.form_focus = FormFocus::Title;
        self.form_id = None;
        self.form_title.clear();
        self.form_content.clear();
        self.status_message = "MODE: Add Note | [Tab/↑↓] Focus Field/Button | [Ctrl+Enter] Save".to_string();
    }

    pub fn open_edit_form(&mut self) {
        if let Some(note) = self.selected_item().cloned() {
            self.mode = AppMode::EditForm;
            self.form_focus = FormFocus::Title;
            self.form_id = Some(note.id);
            self.form_title = note.title;
            self.form_content = note.content;
            self.status_message = "MODE: Edit Note | [Tab/↑↓] Focus Field/Button | [Ctrl+Enter] Save".to_string();
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

        // Floating Modal Form Dialog (Add / Edit Note)
        if self.mode == AppMode::AddForm || self.mode == AppMode::EditForm {
            let area = centered_rect(75, 65, f.area());
            f.render_widget(Clear, area);

            let dialog_title = if self.mode == AppMode::AddForm {
                "➕ Create New Note"
            } else {
                "✏️ Edit Note"
            };

            let outer_block = Block::default()
                .borders(Borders::ALL)
                .title(dialog_title)
                .style(Style::default().fg(Color::Cyan).bg(Color::Reset));
            f.render_widget(outer_block, area);

            // Sub-layout inside popup modal
            let popup_chunks = Layout::default()
                .direction(Direction::Vertical)
                .margin(1)
                .constraints(
                    [
                        Constraint::Length(3), // Title Field
                        Constraint::Min(5),    // Content Field (Multiline)
                        Constraint::Length(3), // Buttons Area
                        Constraint::Length(1), // Shortcut Hints
                    ]
                    .as_ref(),
                )
                .split(area);

            // 1. Title Input Block
            let title_style = if self.form_focus == FormFocus::Title {
                Style::default().fg(Color::Yellow).add_modifier(Modifier::BOLD)
            } else {
                Style::default().fg(Color::Gray)
            };
            let title_block = Block::default()
                .borders(Borders::ALL)
                .title(if self.form_focus == FormFocus::Title { "📌 Title (Focused)" } else { "📌 Title" })
                .style(title_style);
            let title_para = Paragraph::new(if self.form_title.is_empty() {
                Span::styled("<Enter title...>", Style::default().fg(Color::DarkGray))
            } else {
                Span::styled(&self.form_title, Style::default().fg(Color::White))
            })
            .block(title_block);
            f.render_widget(title_para, popup_chunks[0]);

            // 2. Multiline Content Input Block
            let content_style = if self.form_focus == FormFocus::Content {
                Style::default().fg(Color::Yellow).add_modifier(Modifier::BOLD)
            } else {
                Style::default().fg(Color::Gray)
            };
            let content_block = Block::default()
                .borders(Borders::ALL)
                .title(if self.form_focus == FormFocus::Content { "📝 Content (Focused - Multiline: Enter=Newline)" } else { "📝 Content" })
                .style(content_style);
            let content_para = Paragraph::new(if self.form_content.is_empty() {
                Span::styled("<Enter content (multiline supported)...>", Style::default().fg(Color::DarkGray))
            } else {
                Span::styled(&self.form_content, Style::default().fg(Color::White))
            })
            .block(content_block);
            f.render_widget(content_para, popup_chunks[1]);

            // 3. Buttons Area (Save & Cancel)
            let button_chunks = Layout::default()
                .direction(Direction::Horizontal)
                .constraints([Constraint::Percentage(50), Constraint::Percentage(50)].as_ref())
                .split(popup_chunks[2]);

            // Save Button
            let save_style = if self.form_focus == FormFocus::SaveButton {
                Style::default().fg(Color::Black).bg(Color::Green).add_modifier(Modifier::BOLD)
            } else {
                Style::default().fg(Color::Green)
            };
            let save_btn = Paragraph::new(" [ Save (Ctrl+Enter) ] ")
                .alignment(ratatui::layout::Alignment::Center)
                .block(Block::default().borders(Borders::ALL))
                .style(save_style);
            f.render_widget(save_btn, button_chunks[0]);

            // Cancel Button
            let cancel_style = if self.form_focus == FormFocus::CancelButton {
                Style::default().fg(Color::White).bg(Color::Red).add_modifier(Modifier::BOLD)
            } else {
                Style::default().fg(Color::Red)
            };
            let cancel_btn = Paragraph::new(" [ Cancel (Esc) ] ")
                .alignment(ratatui::layout::Alignment::Center)
                .block(Block::default().borders(Borders::ALL))
                .style(cancel_style);
            f.render_widget(cancel_btn, button_chunks[1]);

            // 4. Hints Footer
            let hint = Paragraph::new("Nav: [Tab/↑↓] Switch Focus | Multiline: [Enter] Newline in Content | Save: [Ctrl+Enter]")
                .style(Style::default().fg(Color::DarkGray))
                .alignment(ratatui::layout::Alignment::Center);
            f.render_widget(hint, popup_chunks[3]);
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
