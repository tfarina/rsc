#include <stdio.h>

#include <sqlite3.h>

// http://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm

static int callback(void* data, int argc, char** argv, char** column_name) {
  int i;
  fprintf(stdout, "%s: \n", (const char*)data);
  for (i = 0; i < argc; ++i) {
    printf("%-12s %s\n", column_name[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

int main(int argc, char* argv[]) {
  sqlite3* db;
  int rc;

  rc = sqlite3_open("users.db", &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return rc;
  }

  const char* sql = "SELECT * FROM user";
  const char* data = "Callback function called";

  rc = sqlite3_exec(db, sql, callback, (void*)data, NULL);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);

  return 0;
}
