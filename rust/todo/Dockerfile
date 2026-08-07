# ==========================================
# Stage 1: Build Phase (Cargo Workspace)
# ==========================================
FROM rust:1.80-slim-bookworm AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    pkg-config \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/app

# Pre-build dependencies for workspace layer caching
COPY Cargo.toml Cargo.lock ./
COPY api/Cargo.toml ./api/
COPY cli/Cargo.toml ./cli/

RUN mkdir -p api/src cli/src && \
    echo "fn main() {}" > api/src/main.rs && \
    echo "fn main() {}" > cli/src/main.rs

RUN cargo build --release --workspace
RUN rm -rf api/src cli/src

# Copy source code and migrations
COPY api/src ./api/src
COPY api/migrations ./api/migrations
COPY cli/src ./cli/src

# Force rebuild of workspace binaries
RUN touch api/src/main.rs cli/src/main.rs
RUN cargo build --release --workspace

# ==========================================
# Stage 2: Minimal Runtime Phase
# ==========================================
FROM debian:bookworm-slim AS runner

# Install runtime dependencies
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

# Copy binaries and migrations from builder
COPY --from=builder --chown=appuser:appgroup /usr/src/app/target/release/todo-api /app/todo-api
COPY --from=builder --chown=appuser:appgroup /usr/src/app/target/release/todo-cli /app/todo-cli
COPY --from=builder --chown=appuser:appgroup /usr/src/app/api/migrations /app/migrations

# Switch to non-root user
USER appuser

# Set environment defaults
ENV DATABASE_URL="sqlite:///app/db/todo.db"
ENV RUST_LOG="info"
EXPOSE 8080

# Container healthcheck
HEALTHCHECK --interval=10s --timeout=3s --start-period=5s --retries=3 \
  CMD curl -f http://localhost:8080/api/healthcheck || exit 1

ENTRYPOINT ["/app/todo-api"]
