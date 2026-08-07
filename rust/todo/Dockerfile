# ==========================================
# Stage 1: Build Phase
# ==========================================
FROM rust:1.80-slim-bookworm AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    pkg-config \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/app

# Pre-build dependencies for layer caching
COPY Cargo.toml Cargo.lock ./
RUN mkdir src && echo "fn main() {}" > src/main.rs
RUN cargo build --release
RUN rm -rf src

# Copy source code and migrations
COPY src ./src
COPY migrations ./migrations

# Touch main.rs to force rebuild of application code
RUN touch src/main.rs
RUN cargo build --release

# ==========================================
# Stage 2: Minimal Runtime Phase
# ==========================================
FROM debian:bookworm-slim AS runner

# Install runtime SSL certs and ca-certificates
RUN apt-get update && apt-get install -y \
    ca-certificates \
    curl \
    libssl3 \
    sqlite3 \
    && rm -rf /var/lib/apt/lists/*

# Create non-root system user
RUN groupadd -g 10001 appgroup && \
    useradd -u 10001 -g appgroup -s /bin/sh appuser

WORKDIR /app

# Create database folder with permissions for appuser
RUN mkdir -p /app/db && chown -R appuser:appgroup /app

# Copy binary, migrations, and default env from builder
COPY --from=builder --chown=appuser:appgroup /usr/src/app/target/release/todo /app/todo
COPY --from=builder --chown=appuser:appgroup /usr/src/app/migrations /app/migrations

# Switch to non-root user
USER appuser

# Set environment defaults
ENV DATABASE_URL="sqlite:///app/db/todo.db"
ENV RUST_LOG="info"
EXPOSE 8080

# Container healthcheck
HEALTHCHECK --interval=10s --timeout=3s --start-period=5s --retries=3 \
  CMD curl -f http://localhost:8080/api/healthcheck || exit 1

ENTRYPOINT ["/app/todo"]
