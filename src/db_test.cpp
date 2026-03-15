#include <iostream>
#include <libpq-fe.h>

using namespace std;

int main() {
    // Connect to PostgreSQL
    PGconn* conn = PQconnectdb("user=postgres password=yourpassword dbname=grocery_db hostaddr=127.0.0.1 port=5432");

    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Connection failed: " << PQerrorMessage(conn) << endl;
        PQfinish(conn);
        return 1;
    }

    cout << "Connected to database successfully!\n";

    // Simple query
    PGresult* res = PQexec(conn, "SELECT * FROM products");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Query failed: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        PQfinish(conn);
        return 1;
    }

    int rows = PQntuples(res);
    cout << "Products in DB: " << rows << "\n";

    for (int i = 0; i < rows; i++) {
        cout << PQgetvalue(res, i, 0) << " | "    // id
             << PQgetvalue(res, i, 1) << " | "    // name
             << PQgetvalue(res, i, 2) << " | "    // price
             << PQgetvalue(res, i, 3) << "\n";   // stock
    }

    PQclear(res);
    PQfinish(conn);
    return 0;
}