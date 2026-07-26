use aes_gcm::{
    aead::{Aead, KeyInit, OsRng},
    Aes256Gcm, Nonce,
};
use rcgen::generate_simple_self_signed;
use rustls_pki_types::{CertificateDer, PrivateKeyDer};
use std::io;
use std::sync::Arc;
use tokio_rustls::rustls::{ClientConfig, RootCertStore, ServerConfig};

/// Symmetric key size for AES-256-GCM (32 bytes).
pub const AES_KEY_SIZE: usize = 32;
/// Nonce size for AES-256-GCM (12 bytes).
pub const NONCE_SIZE: usize = 12;

/// Symmetric key wrapper for Intra-LAN UDP payload encryption.
#[derive(Clone)]
pub struct SymmetricKey(pub [u8; AES_KEY_SIZE]);

impl SymmetricKey {
    /// Generate a cryptographically secure random 256-bit key.
    pub fn generate() -> Self {
        use aes_gcm::aead::rand_core::RngCore;
        let mut key = [0u8; AES_KEY_SIZE];
        OsRng.fill_bytes(&mut key);
        Self(key)
    }

    /// Construct key from raw 32-byte slice.
    pub fn from_bytes(bytes: &[u8; AES_KEY_SIZE]) -> Self {
        Self(*bytes)
    }
}

/// Encrypt plaintext using AES-256-GCM. Returns `[Nonce (12 bytes) || Ciphertext+Tag]`.
pub fn encrypt_payload(key: &[u8; AES_KEY_SIZE], plaintext: &[u8]) -> Result<Vec<u8>, String> {
    use aes_gcm::aead::rand_core::RngCore;
    let cipher = Aes256Gcm::new_from_slice(key).map_err(|e| e.to_string())?;

    let mut nonce_bytes = [0u8; NONCE_SIZE];
    OsRng.fill_bytes(&mut nonce_bytes);
    let nonce = Nonce::from_slice(&nonce_bytes);

    let ciphertext = cipher
        .encrypt(nonce, plaintext)
        .map_err(|e| format!("Encryption error: {}", e))?;

    let mut result = Vec::with_capacity(NONCE_SIZE + ciphertext.len());
    result.extend_from_slice(&nonce_bytes);
    result.extend_from_slice(&ciphertext);
    Ok(result)
}

/// Decrypt payload `[Nonce (12 bytes) || Ciphertext+Tag]` using AES-256-GCM.
pub fn decrypt_payload(key: &[u8; AES_KEY_SIZE], encrypted: &[u8]) -> Result<Vec<u8>, String> {
    if encrypted.len() < NONCE_SIZE {
        return Err("Encrypted payload too short".to_string());
    }

    let cipher = Aes256Gcm::new_from_slice(key).map_err(|e| e.to_string())?;
    let (nonce_bytes, ciphertext) = encrypted.split_at(NONCE_SIZE);
    let nonce = Nonce::from_slice(nonce_bytes);

    cipher
        .decrypt(nonce, ciphertext)
        .map_err(|e| format!("Decryption error: {}", e))
}

/// TLS Configuration container for Inter-LAN TCP Cluster Gateways.
#[derive(Clone)]
pub struct TlsConfig {
    pub server_config: Option<Arc<ServerConfig>>,
    pub client_config: Option<Arc<ClientConfig>>,
    pub server_name: String,
}

/// Helper to generate self-signed TLS configurations for testing / cluster nodes.
pub fn generate_self_signed_tls_config(domain: &str) -> io::Result<TlsConfig> {
    let _ = tokio_rustls::rustls::crypto::ring::default_provider().install_default();

    let cert = generate_simple_self_signed(vec![domain.to_string()])
        .map_err(|e| io::Error::new(io::ErrorKind::Other, e.to_string()))?;

    let cert_der = cert.cert.der().to_vec();
    let key_der = cert.key_pair.serialize_der();

    let cert_pki = CertificateDer::from(cert_der);
    let key_pki = PrivateKeyDer::Pkcs8(key_der.into());

    let server_config = ServerConfig::builder()
        .with_no_client_auth()
        .with_single_cert(vec![cert_pki.clone()], key_pki)
        .map_err(|e| io::Error::new(io::ErrorKind::Other, e.to_string()))?;

    let mut root_store = RootCertStore::empty();
    root_store
        .add(cert_pki)
        .map_err(|e| io::Error::new(io::ErrorKind::Other, e.to_string()))?;

    let client_config = ClientConfig::builder()
        .with_root_certificates(root_store)
        .with_no_client_auth();

    Ok(TlsConfig {
        server_config: Some(Arc::new(server_config)),
        client_config: Some(Arc::new(client_config)),
        server_name: domain.to_string(),
    })
}

/// Deduplication cache to prevent message loops across cluster peers and subnets.
#[derive(Default, Clone)]
pub struct MessageDeduplicator {
    seen_ids: Arc<tokio::sync::Mutex<std::collections::HashSet<String>>>,
}

impl MessageDeduplicator {
    pub fn new() -> Self {
        Self {
            seen_ids: Arc::new(tokio::sync::Mutex::new(std::collections::HashSet::new())),
        }
    }

    /// Insert a message ID. Returns `true` if it's new (not seen before), `false` if duplicated.
    pub async fn check_and_insert(&self, id: &str) -> bool {
        let mut set = self.seen_ids.lock().await;
        if set.contains(id) {
            false
        } else {
            if set.len() > 10_000 {
                set.clear();
            }
            set.insert(id.to_string());
            true
        }
    }
}
