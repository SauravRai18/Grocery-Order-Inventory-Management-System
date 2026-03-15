# Grocery Order & Inventory Management System

A **Command-Line Interface (CLI) application** built using **C++ and PostgreSQL** to manage grocery products, inventory, and orders.  

This project demonstrates **database integration, modular C++ design, and ACID-compliant transactions**, suitable for real-world backend systems.

---

## Features

- **View Products**: List all products with ID, name, price, and stock.
- **Add Product**: Add new products to the database with price and stock.
- **Place Orders**: Place an order for a product, automatically updating stock and ensuring data integrity using **transactions**.
- **ACID Compliance**: Ensures atomicity, consistency, isolation, and durability for orders.

---

## Technologies Used

- **C++** (STL, modular design)
- **PostgreSQL** (Database management)
- **libpq** (PostgreSQL C API)
- **WSL** (Windows Subsystem for Linux) / VS Code
- **Git & GitHub** (Version control)

---

## Database Schema

### `products`
| Column | Type    | Description           |
|--------|---------|---------------------|
| id     | SERIAL  | Primary key          |
| name   | TEXT    | Product name         |
| price  | REAL    | Price per unit       |
| stock  | INT     | Available stock      |

### `orders`
| Column | Type   | Description        |
|--------|--------|------------------|
| id     | SERIAL | Primary key       |

### `order_items`
| Column     | Type | Description               |
|------------|------|---------------------------|
| id         | SERIAL | Primary key             |
| order_id   | INT    | Foreign key to `orders` |
| product_id | INT    | Foreign key to `products` |
| quantity   | INT    | Quantity ordered          |

---

## Installation & Setup

1. **Clone the repository**

```bash
git clone https://github.com/SauravRai18/Grocery-Order-Inventory-Management-System.git
cd Grocery-Order-Inventory-Management-System
