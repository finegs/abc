#![allow(dead_code)]

use crate::models::NoteItem;
use ratatui::{
    layout::{Constraint, Direction, Layout, Rect},
    style::{Color, Modifier, Style},
    text::{Line, Span, Text},
    widgets::{Block, Borders, Clear, List, ListItem, ListState, Paragraph, Wrap},
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

#[derive(Debug, PartialEq, Clone, Copy)]
pub enum FormEditorMode {
    VimNormal,
    VimInsert,
    CommandInput,
}

#[derive(Debug, PartialEq, Clone)]
pub enum AppMode {
    Normal,
    AddForm,
    EditForm,
    Help,
}

pub struct App {
    pub items: Vec<NoteItem>,
    pub list_state: ListState,
    pub mode: AppMode,
    pub form_editor_mode: FormEditorMode,
    pub form_focus: FormFocus,
    pub status_message: String,
    pub form_id: Option<String>,
    pub form_title: String,
    pub form_content: String,
    pub title_cursor: usize,
    pub content_cursor: usize,
    pub vim_cmd_buffer: String,
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
            form_editor_mode: FormEditorMode::VimNormal,
            form_focus: FormFocus::Title,
            status_message: "Vim Keymaps Active | [j/k] Nav | [a] Add | [e] Edit | [d/x] Del | [?] Help".to_string(),
            form_id: None,
            form_title: String::new(),
            form_content: String::new(),
            title_cursor: 0,
            content_cursor: 0,
            vim_cmd_buffer: String::new(),
        }
    }

    pub fn open_add_form(&mut self) {
        self.mode = AppMode::AddForm;
        self.form_editor_mode = FormEditorMode::VimInsert;
        self.form_focus = FormFocus::Title;
        self.form_id = None;
        self.form_title.clear();
        self.form_content.clear();
        self.title_cursor = 0;
        self.content_cursor = 0;
        self.vim_cmd_buffer.clear();
        self.status_message = "-- VIM INSERT -- Type title, press Esc for Vim Normal mode".to_string();
    }

    pub fn open_edit_form(&mut self) {
        if let Some(note) = self.selected_item().cloned() {
            self.mode = AppMode::EditForm;
            self.form_editor_mode = FormEditorMode::VimNormal;
            self.form_focus = FormFocus::Title;
            self.form_id = Some(note.id);
            self.form_title = note.title;
            self.form_content = note.content;
            self.title_cursor = self.form_title.len();
            self.content_cursor = self.form_content.len();
            self.vim_cmd_buffer.clear();
            self.status_message = "-- VIM NORMAL -- [i] Insert mode | [j/k] Focus Field | [h/l] Cursor | [:w] Save".to_string();
        }
    }

    pub fn title_insert_char(&mut self, c: char) {
        if self.title_cursor > self.form_title.len() {
            self.title_cursor = self.form_title.len();
        }
        self.form_title.insert(self.title_cursor, c);
        self.title_cursor += 1;
    }

    pub fn title_backspace(&mut self) {
        if self.title_cursor > 0 && !self.form_title.is_empty() {
            self.title_cursor -= 1;
            self.form_title.remove(self.title_cursor);
        }
    }

    pub fn title_delete_char(&mut self) {
        if self.title_cursor < self.form_title.len() {
            self.form_title.remove(self.title_cursor);
        }
    }

    pub fn content_insert_char(&mut self, c: char) {
        if self.content_cursor > self.form_content.len() {
            self.content_cursor = self.form_content.len();
        }
        self.form_content.insert(self.content_cursor, c);
        self.content_cursor += 1;
    }

    pub fn content_backspace(&mut self) {
        if self.content_cursor > 0 && !self.form_content.is_empty() {
            self.content_cursor -= 1;
            self.form_content.remove(self.content_cursor);
        }
    }

    pub fn content_delete_char(&mut self) {
        if self.content_cursor < self.form_content.len() {
            self.form_content.remove(self.content_cursor);
        }
    }

    pub fn content_line_start(&mut self) {
        if self.content_cursor == 0 || self.form_content.is_empty() {
            return;
        }
        let safe_cursor = self.content_cursor.min(self.form_content.len());
        let slice = &self.form_content[..safe_cursor];
        if let Some(pos) = slice.rfind('\n') {
            self.content_cursor = pos + 1;
        } else {
            self.content_cursor = 0;
        }
    }

    pub fn content_line_end(&mut self) {
        let safe_cursor = self.content_cursor.min(self.form_content.len());
        let slice = &self.form_content[safe_cursor..];
        if let Some(pos) = slice.find('\n') {
            self.content_cursor = safe_cursor + pos;
        } else {
            self.content_cursor = self.form_content.len();
        }
    }

    pub fn content_move_up(&mut self) -> bool {
        let safe = self.content_cursor.min(self.form_content.len());
        let current_start = match self.form_content[..safe].rfind('\n') {
            Some(pos) => pos + 1,
            None => 0,
        };
        if current_start == 0 {
            return false;
        }
        let col = safe - current_start;
        let prev_slice = &self.form_content[..current_start - 1];
        let prev_start = match prev_slice.rfind('\n') {
            Some(pos) => pos + 1,
            None => 0,
        };
        let prev_line_len = (current_start - 1) - prev_start;
        self.content_cursor = prev_start + col.min(prev_line_len);
        true
    }

    pub fn content_move_down(&mut self) -> bool {
        let safe = self.content_cursor.min(self.form_content.len());
        let current_start = match self.form_content[..safe].rfind('\n') {
            Some(pos) => pos + 1,
            None => 0,
        };
        let col = safe - current_start;
        let next_start = match self.form_content[safe..].find('\n') {
            Some(pos) => safe + pos + 1,
            None => return false,
        };
        let next_end = match self.form_content[next_start..].find('\n') {
            Some(pos) => next_start + pos,
            None => self.form_content.len(),
        };
        let next_line_len = next_end - next_start;
        self.content_cursor = next_start + col.min(next_line_len);
        true
    }

    pub fn content_delete_line(&mut self) {
        if self.form_content.is_empty() {
            return;
        }
        let safe = self.content_cursor.min(self.form_content.len());
        let start = match self.form_content[..safe].rfind('\n') {
            Some(pos) => pos + 1,
            None => 0,
        };
        let end = match self.form_content[safe..].find('\n') {
            Some(pos) => safe + pos + 1,
            None => self.form_content.len(),
        };
        self.form_content.drain(start..end);
        self.content_cursor = start.min(self.form_content.len());
    }

    pub fn content_delete_to_end(&mut self) {
        if self.form_content.is_empty() {
            return;
        }
        let safe = self.content_cursor.min(self.form_content.len());
        let end = match self.form_content[safe..].find('\n') {
            Some(pos) => safe + pos,
            None => self.form_content.len(),
        };
        if safe < end {
            self.form_content.drain(safe..end);
        }
    }

    pub fn content_open_below(&mut self) {
        self.content_line_end();
        self.content_insert_char('\n');
        self.form_editor_mode = FormEditorMode::VimInsert;
    }

    pub fn content_open_above(&mut self) {
        self.content_line_start();
        let pos = self.content_cursor;
        self.form_content.insert(pos, '\n');
        self.content_cursor = pos;
        self.form_editor_mode = FormEditorMode::VimInsert;
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

    pub fn jump_top(&mut self) {
        if !self.items.is_empty() {
            self.list_state.select(Some(0));
        }
    }

    pub fn jump_bottom(&mut self) {
        if !self.items.is_empty() {
            self.list_state.select(Some(self.items.len() - 1));
        }
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
        let header = Paragraph::new("📝 Rust Axum Todo TUI (Vim Modal Editing Active)")
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
            .block(Block::default().borders(Borders::ALL).title("Notes List [j/k/g/G]"))
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
            .style(Style::default().fg(Color::Green))
            .wrap(Wrap { trim: false });
        f.render_widget(details, main_chunks[1]);

        // Footer / Status Bar Widget
        let footer = Paragraph::new(self.status_message.as_str())
            .style(Style::default().fg(Color::Yellow))
            .block(Block::default().borders(Borders::ALL));
        f.render_widget(footer, chunks[2]);

        // Floating Modal Form Dialog (Add / Edit Note)
        if self.mode == AppMode::AddForm || self.mode == AppMode::EditForm {
            let area = centered_rect(75, 75, f.area());
            f.render_widget(Clear, area);

            let dialog_title = if self.mode == AppMode::AddForm {
                " ➕ PROMPT: Create New Note ".to_string()
            } else {
                format!(
                    " ✏️ PROMPT: Edit Note [ID: {}] ",
                    self.form_id.as_deref().unwrap_or("N/A")
                )
            };

            let outer_block = Block::default()
                .borders(Borders::ALL)
                .title(Span::styled(
                    dialog_title,
                    Style::default().fg(Color::Yellow).add_modifier(Modifier::BOLD),
                ))
                .style(Style::default().fg(Color::Cyan).bg(Color::Reset));
            f.render_widget(outer_block, area);

            // Sub-layout inside popup modal
            let popup_chunks = Layout::default()
                .direction(Direction::Vertical)
                .margin(1)
                .constraints(
                    [
                        Constraint::Length(1), // Vim Mode Banner
                        Constraint::Length(3), // Title Field
                        Constraint::Min(6),    // Content Field (Multiline)
                        Constraint::Length(3), // Buttons Area
                        Constraint::Length(1), // Command / Status Hints Footer
                    ]
                    .as_ref(),
                )
                .split(area);

            // 0. Vim Mode Banner
            let vim_banner_str = match self.form_editor_mode {
                FormEditorMode::VimNormal => "-- VIM NORMAL MODE -- (Press 'i' for Insert, 'j/k' to move field, ':w' to save, 'Esc' for normal)".to_string(),
                FormEditorMode::VimInsert => "-- VIM INSERT MODE -- (Press 'Esc' for Normal mode, 'Enter' for newline in Content)".to_string(),
                FormEditorMode::CommandInput => format!("-- VIM COMMAND MODE -- :{}", self.vim_cmd_buffer),
            };
            let vim_banner_style = match self.form_editor_mode {
                FormEditorMode::VimNormal => Style::default().fg(Color::Cyan).add_modifier(Modifier::BOLD),
                FormEditorMode::VimInsert => Style::default().fg(Color::Green).add_modifier(Modifier::BOLD),
                FormEditorMode::CommandInput => Style::default().fg(Color::Yellow).add_modifier(Modifier::BOLD),
            };
            let vim_banner = Paragraph::new(vim_banner_str)
                .alignment(ratatui::layout::Alignment::Center)
                .style(vim_banner_style);
            f.render_widget(vim_banner, popup_chunks[0]);

            // 1. Title Input Block
            let is_title_focused = self.form_focus == FormFocus::Title;
            let title_style = if is_title_focused {
                Style::default().fg(Color::Yellow).add_modifier(Modifier::BOLD)
            } else {
                Style::default().fg(Color::Gray)
            };
            let title_block = Block::default()
                .borders(Borders::ALL)
                .title(if is_title_focused { "📌 Title (Focused)" } else { "📌 Title" })
                .style(title_style);

            let formatted_title = format_text_with_cursor(
                &self.form_title,
                self.title_cursor,
                is_title_focused,
                self.form_editor_mode == FormEditorMode::VimInsert,
            );

            let title_para = Paragraph::new(if self.form_title.is_empty() && !is_title_focused {
                Text::styled("<Enter title...>", Style::default().fg(Color::DarkGray))
            } else {
                Text::from(formatted_title)
            })
            .block(title_block);
            f.render_widget(title_para, popup_chunks[1]);

            // 2. Multiline Content Input Block
            let is_content_focused = self.form_focus == FormFocus::Content;
            let content_style = if is_content_focused {
                Style::default().fg(Color::Yellow).add_modifier(Modifier::BOLD)
            } else {
                Style::default().fg(Color::Gray)
            };
            let content_block = Block::default()
                .borders(Borders::ALL)
                .title(if is_content_focused { "📝 Content (Focused - Multiline: Enter=Newline)" } else { "📝 Content" })
                .style(content_style);

            let formatted_content = format_text_with_cursor(
                &self.form_content,
                self.content_cursor,
                is_content_focused,
                self.form_editor_mode == FormEditorMode::VimInsert,
            );

            let content_para = Paragraph::new(if self.form_content.is_empty() && !is_content_focused {
                Text::styled("<Enter content (multiline supported)...>", Style::default().fg(Color::DarkGray))
            } else {
                Text::from(formatted_content)
            })
            .block(content_block)
            .wrap(Wrap { trim: false });
            f.render_widget(content_para, popup_chunks[2]);

            // 3. Buttons Area (Save & Cancel)
            let button_chunks = Layout::default()
                .direction(Direction::Horizontal)
                .constraints([Constraint::Percentage(50), Constraint::Percentage(50)].as_ref())
                .split(popup_chunks[3]);

            // Save Button
            let save_style = if self.form_focus == FormFocus::SaveButton {
                Style::default().fg(Color::Black).bg(Color::Green).add_modifier(Modifier::BOLD)
            } else {
                Style::default().fg(Color::Green)
            };
            let save_btn = Paragraph::new(" [ Save (:w / Ctrl+S / Ctrl+Enter) ] ")
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
            let cancel_btn = Paragraph::new(" [ Cancel (:q / Esc) ] ")
                .alignment(ratatui::layout::Alignment::Center)
                .block(Block::default().borders(Borders::ALL))
                .style(cancel_style);
            f.render_widget(cancel_btn, button_chunks[1]);

            // 4. Command / Status Hints Footer
            let hint_str = if self.form_editor_mode == FormEditorMode::CommandInput {
                format!(":{}", self.vim_cmd_buffer)
            } else {
                "Vim Mode: [i] Insert | [Esc] Normal | [j/k] Switch Field | [h/l] Cursor | [:w] Save | [:q] Quit".to_string()
            };
            let hint = Paragraph::new(hint_str)
                .style(Style::default().fg(Color::Yellow))
                .alignment(ratatui::layout::Alignment::Center);
            f.render_widget(hint, popup_chunks[4]);
        }

        // Vim Keybindings Help Sheet Overlay
        if self.mode == AppMode::Help {
            let area = centered_rect(65, 75, f.area());
            f.render_widget(Clear, area);

            let help_text = r#"
🎮 Vim Keybindings Quick Reference

Navigation (Normal Mode):
  j / ↓         Move down in note list
  k / ↑         Move up in note list
  g / Home      Jump to top of list
  G / End       Jump to bottom of list

Actions (Normal Mode):
  a             Add new note (Open Prompt)
  e / Enter     Edit selected note (Open Prompt)
  d / x         Delete selected note
  r             Refresh notes list from API
  ? / h         Toggle this Vim Help sheet
  q / Esc       Quit TUI

Vim Modal Note Editor (Inside Prompt):
  i / a         Enter Vim Insert mode
  o             Open new line in Content field (Insert mode)
  Esc           Return to Vim Normal mode
  j / k         Move field focus (Title <-> Content <-> Save)
  h / l         Move cursor Left / Right
  x / dd        Delete char / line
  :w / :wq      Save note & exit prompt
  :q / :q!      Cancel & quit prompt
  Ctrl+S /      Save & submit immediately
  Ctrl+Enter

Press [?] or [Esc] to return to application.
"#;

            let help_para = Paragraph::new(help_text)
                .block(
                    Block::default()
                        .borders(Borders::ALL)
                        .title(" 💡 Vim Keybindings Help Sheet ")
                        .style(Style::default().fg(Color::Yellow)),
                )
                .style(Style::default().fg(Color::White).bg(Color::Reset));

            f.render_widget(help_para, area);
        }
    }
}

fn format_text_with_cursor(text: &str, cursor: usize, active: bool, is_insert: bool) -> String {
    if !active {
        return text.to_string();
    }
    let safe_cursor = cursor.min(text.len());
    let (left, right) = text.split_at(safe_cursor);
    if is_insert {
        format!("{}█{}", left, right)
    } else {
        if right.is_empty() {
            format!("{}█", left)
        } else {
            let mut chars = right.chars();
            let _c = chars.next().unwrap();
            let rest = chars.as_str();
            format!("{}▒{}", left, rest)
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
