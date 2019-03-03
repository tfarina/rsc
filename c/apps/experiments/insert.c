/*
 * Modified from http://zetcode.com/db/mysqlc/
 */

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

#include "db.h"

static MYSQL *conn = NULL;

static int db_connect(const char *host, int unsigned port, const char *user,
                      const char *password, const char *dbname)
{
  if ((conn = mysql_init(NULL)) == NULL) {
    fprintf(stderr, "mysql: unable to allocate memory for database connection.\n");
    return -1;
  }

  if (mysql_real_connect(conn, host, user, password, dbname, port,
                         NULL, 0) == NULL) {
    fprintf(stderr, "mysql: connection to database failed: %s\n", mysql_error(conn));
    mysql_close(conn);
    return -1;
  }

  return 0;
}

static int product_add(const char *name, int quantity, double price)
{
  char query[256];

  sprintf(query, "INSERT INTO products (`productName`, `productLine`, `quantityInStock`, `buyPrice`) VALUES ('%s', 'Motorcycles', %d, %f)",
          name, quantity, price);

  if (mysql_query(conn, query)) {
    fprintf(stderr, "mysql: sql insert failed: %s\n", mysql_error(conn));
    mysql_close(conn);
    return -1;
  }

  return 0;
}

int main(int argc, char **argv) {
  db_config_t config;

  db_config_init(&config);

  if (db_connect(config.host, config.port, config.user, config.password, config.dbname)) {
    return -1;
  }

  if (product_add("Ducati", 25, 8999.00) == -1) {
    return -1;
  }

  mysql_close(conn);

  return 0;
}
