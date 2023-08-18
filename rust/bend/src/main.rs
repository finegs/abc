extern crate mysql;

use mysql::prelude::*;
use mysql::Pool;

// Define a struct to represent your data
#[derive(Debug, PartialEq, Eq, Queryable, Insertable)]
struct Item {
    id: Option<i32>,
    name: String,
    description: String,
}

fn main() {
    // Create a MySQL connection pool
    let pool = Pool::new("mysql://username:password@localhost/db_name").unwrap();

    // Create a new item
    let new_item = Item {
        id: None,
        name: "Example Item".to_string(),
        description: "This is an example item.".to_string(),
    };

    // Insert the item into the database
    let insert_result = mysql::query_as::<_, Item>(
        "INSERT INTO items (name, description) VALUES (:name, :description)",
        new_item,
    )
    .execute(&pool)
    .expect("Failed to insert item");

    println!("Inserted: {:?}", insert_result);

    // Fetch and display all items
    let items: Vec<Item> = mysql::query_as("SELECT id, name, description FROM items")
        .fetch(&pool)
        .expect("Failed to fetch items");

    println!("Items: {:?}", items);

    // Update an item
    let updated_item = Item {
        id: Some(1), // Assuming you want to update an item with ID 1
        name: "Updated Item".to_string(),
        description: "This item has been updated.".to_string(),
    };

    let update_result = mysql::query_as::<_, Item>(
        "UPDATE items SET name = :name, description = :description WHERE id = :id",
        updated_item,
    )
    .execute(&pool)
    .expect("Failed to update item");

    println!("Updated: {:?}", update_result);

    // Delete an item
    let delete_id = 1; // Assuming you want to delete an item with ID 1
    let delete_result = mysql::query_as::<_, Item>(
        "DELETE FROM items WHERE id = :id",
        params! {
            "id" => delete_id,
        },
    )
    .execute(&pool)
    .expect("Failed to delete item");

    println!("Deleted: {:?}", delete_result);
}

