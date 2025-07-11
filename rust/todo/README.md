# Setup for Todo Svc

## 1. Run Docker Compose & Detach

from  [Todo](https://medium.com/@raditzlawliet/build-crud-rest-api-with-rust-and-mysql-using-axum-sqlx-d7e50b3cd130)
from  [Git](https://github.com/raditzlawliet/rust-notes-rest-axum-sqlx)

``` sh
sudo  docker-compose up -d
```

## 2. Stopping MySQL Docker

``` sh
sudo  docker-compose stop
```

## 3. Database migration

### 3.1 CLI For migration

``` sh
```

cargo install sqlx-cli

sqlx migrate add -r create_notes_table

```

### 3.2 ( Database migration up/revert )

### Perform migration up

``` sh
```

sqlx migrate run

```
```

## (Bonus!, perform migrate down/revert)

``` sh

sqlx migrate revert
```

```

