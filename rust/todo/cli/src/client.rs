#![allow(dead_code)]

use crate::models::{CreateNotePayload, NoteItem, NoteListResponse};
use reqwest::Client;

#[derive(Clone, Debug)]
pub struct ApiClient {
    base_url: String,
    client: Client,
}

impl ApiClient {
    pub fn new(base_url: impl Into<String>) -> Self {
        Self {
            base_url: base_url.into(),
            client: Client::new(),
        }
    }

    pub async fn healthcheck(&self) -> Result<bool, reqwest::Error> {
        let url = format!("{}/api/healthcheck", self.base_url);
        let res = self.client.get(&url).send().await?;
        Ok(res.status().is_success())
    }

    pub async fn fetch_notes(&self) -> Result<Vec<NoteItem>, reqwest::Error> {
        let url = format!("{}/api/notes", self.base_url);
        let res = self.client.get(&url).send().await?;
        let data: NoteListResponse = res.json().await?;
        Ok(data.notes)
    }

    pub async fn create_note(
        &self,
        title: &str,
        content: &str,
        is_published: bool,
    ) -> Result<bool, reqwest::Error> {
        let url = format!("{}/api/notes", self.base_url);
        let payload = CreateNotePayload {
            title: title.to_string(),
            content: content.to_string(),
            is_published: Some(is_published),
        };
        let res = self.client.post(&url).json(&payload).send().await?;
        Ok(res.status().is_success())
    }

    pub async fn delete_note(&self, id: &str) -> Result<bool, reqwest::Error> {
        let url = format!("{}/api/notes/{}", self.base_url, id);
        let res = self.client.delete(&url).send().await?;
        Ok(res.status().is_success())
    }
}
