#include <stdio.h>
#include <stdlib.h>

#include "db.h"
#include "user.h"

// http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm
static void close_user_db(sqlite3* db) {
  if (sqlite3_close(db) != SQLITE_OK) {
    fprintf(stderr, "failed to close the user db: %s\n", sqlite3_errmsg(db));
  }
}

/* Lists records from the user table. */
static int list_user_records(sqlite3* db) {
  sqlite3_stmt* stmt;
  const char* sql = sqlite3_mprintf("SELECT * FROM user");

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  // http://www.csl.mtu.edu/cs1141/www/examples/sqlite/sqlite_select.c
  int col_width[] = {12, 9, 5, 19, 0};

  printf(" |     uid   |    login  |     pw    |         email     |\n");
  printf(" |-----------|-----------|-----------|-------------------|\n");

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    for (int i = 0; i < sqlite3_column_count(stmt); i++) {
      printf(" | %*s", col_width[1], sqlite3_column_text(stmt, i));
    }
    printf("\n");
  }

  sqlite3_finalize(stmt);

  return 0;
}

int main(int argc, char **argv) {
  sqlite3* db;
  int rv;

  db = db_open("users.db");
  if (!db) {
    return -1;
  }

  if (db_user_create_table(db)) {
    close_user_db(db);
    return -1;
  }

  rv = list_user_records(db);

  close_user_db(db);

  return rv;
}
