#include <iostream>
#include <libpq-fe.h>
#include <string>

using namespace std;

// Connect to PostgreSQL
PGconn* connectDB() {
    PGconn* conn = PQconnectdb("user=postgres password=yourpassword dbname=grocery_db hostaddr=127.0.0.1 port=5432");
    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Connection failed: " << PQerrorMessage(conn) << endl;
        PQfinish(conn);
        return nullptr;
    }
    return conn;
}

// View all products
void viewProducts(PGconn* conn) {
    PGresult* res = PQexec(conn, "SELECT * FROM products");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Query failed: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);
    cout << "\nProducts in DB: " << rows << "\n";
    cout << "ID | Name | Price | Stock\n";
    for (int i = 0; i < rows; i++) {
        cout << PQgetvalue(res, i, 0) << " | "
             << PQgetvalue(res, i, 1) << " | "
             << PQgetvalue(res, i, 2) << " | "
             << PQgetvalue(res, i, 3) << "\n";
    }
    PQclear(res);
}

// Add new product
void addProduct(PGconn* conn) {
    string name;
    double price;
    int stock;

    cout << "\nEnter product name: ";
    cin.ignore(); // Clear newline
    getline(cin, name);

    cout << "Enter product price: ";
    cin >> price;

    cout << "Enter stock quantity: ";
    cin >> stock;

    string query = "INSERT INTO products (name, price, stock) VALUES ('" + name + "', " + to_string(price) + ", " + to_string(stock) + ")";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Insert failed: " << PQerrorMessage(conn) << endl;
    } else {
        cout << "Product added successfully!\n";
    }

    PQclear(res);
}

// Place order with transaction
void placeOrder(PGconn* conn) {
    int productId, quantity;

    cout << "\nEnter Product ID to order: ";
    cin >> productId;
    cout << "Enter Quantity: ";
    cin >> quantity;

    // Begin transaction
    PGresult* res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Failed to begin transaction: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        return;
    }
    PQclear(res);

    // Check stock
    string checkQuery = "SELECT stock FROM products WHERE id=" + to_string(productId);
    res = PQexec(conn, checkQuery.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        cerr << "Product not found or query failed.\n";
        PQclear(res);
        PQexec(conn, "ROLLBACK");
        return;
    }

    int stock = stoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    if (stock < quantity) {
        cout << "Not enough stock! Available: " << stock << endl;
        PQexec(conn, "ROLLBACK");
        return;
    }

    // Insert into orders
    res = PQexec(conn, "INSERT INTO orders DEFAULT VALUES RETURNING id");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Failed to create order: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        PQexec(conn, "ROLLBACK");
        return;
    }
    int orderId = stoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    // Insert into order_items
    string insertItem = "INSERT INTO order_items (order_id, product_id, quantity) VALUES (" +
                        to_string(orderId) + ", " + to_string(productId) + ", " + to_string(quantity) + ")";
    res = PQexec(conn, insertItem.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Failed to add order item: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        PQexec(conn, "ROLLBACK");
        return;
    }
    PQclear(res);

    // Update stock
    string updateStock = "UPDATE products SET stock = stock - " + to_string(quantity) + " WHERE id=" + to_string(productId);
    res = PQexec(conn, updateStock.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Failed to update stock: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        PQexec(conn, "ROLLBACK");
        return;
    }
    PQclear(res);

    // Commit transaction
    res = PQexec(conn, "COMMIT");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        cerr << "Failed to commit transaction: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        PQexec(conn, "ROLLBACK");
        return;
    }
    PQclear(res);

    cout << "Order placed successfully! Order ID: " << orderId << "\n";
}

// Main CLI
int main() {
    cout << "Welcome to Grocery Order & Inventory Management System!\n";

    PGconn* conn = connectDB();
    if (!conn) return 1;  // Exit if DB connection failed

    int choice;
    do {
        cout << "\nMenu:\n";
        cout << "1. View Products\n";
        cout << "2. Add Product\n";
        cout << "3. Place Order\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 1:
                viewProducts(conn);
                break;
            case 2:
                addProduct(conn);
                break;
            case 3:
                placeOrder(conn);
                break;
            case 4:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }

    } while(choice != 4);

    PQfinish(conn);
    return 0;
}