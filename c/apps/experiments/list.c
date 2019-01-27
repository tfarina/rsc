/*
 * Modified from http://zetcode.com/db/mysqlc/
 */

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>

#include "iniparser.h"

#define USERCONFFILE ".experimentsrc"
#define PATH_SEP '/'

typedef struct {
  const char *host;
  const char *user;
  const char *password;
  const char *dbname;
} config_t;

/**
 * Gets the current user's home directory.
 */
static const char *get_home_dir(void)
{
  return getenv("HOME");
}

/* Code from msmtp:src/tools.c:get_filename */
static char *build_filename(const char *directory, const char *name)
{
  char *path;
  size_t dirlen;

  dirlen = strlen(directory);
  path = malloc((dirlen + strlen(name) + 2) * sizeof(char));
  strcpy(path, directory);
  if (dirlen == 0 || path[dirlen - 1] != PATH_SEP) {
    path[dirlen++] = PATH_SEP;
  }
  strcpy(path + dirlen, name);
  return path;
}

static MYSQL *db_connect(const char *host, const char *user,
                         const char *password, const char *dbname)
{
  MYSQL *conn;
  unsigned int port = 0;

  if ((conn = mysql_init(NULL)) == NULL) {
    fprintf(stderr, "mysql: unable to allocate memory for database connection.\n");
    return NULL;
  }

  if (mysql_real_connect(conn, host, user, password, dbname, port,
                         NULL, 0) == NULL) {
    fprintf(stderr, "mysql: connection to database failed: %s\n", mysql_error(conn));
    mysql_close(conn);
    return NULL;
  }

  return conn;
}

/*
 * http://www.kitebird.com/mysql-book/ch06-3ed.pdf
 */
static void print_dashes(MYSQL_RES *res)
{
  MYSQL_FIELD *field;
  unsigned int i, j;
  unsigned int num_fields;

  mysql_field_seek(res, 0);

  fputc('+', stdout);

  num_fields = mysql_num_fields(res);

  for (i = 0; i < num_fields; i++) {
    field = mysql_fetch_field(res);
    for (j = 0; j < field->max_length +2; j++) {
      fputc('-', stdout);
    }
    fputc('+', stdout);
  }

  fputc('\n', stdout);
}

static int product_list(MYSQL *conn)
{
  char query[256];
  MYSQL_RES *res = NULL;
  unsigned int num_fields;
  unsigned long col_len;
  MYSQL_FIELD *field;
  MYSQL_ROW row;
  int i;

  sprintf(query, "SELECT productCode, productName, productLine FROM products");

  if (mysql_query(conn, query)) {
    fprintf(stderr, "mysql: sql select failed: %s\n", mysql_error(conn));
    mysql_close(conn);
    return -1;
  }

  if ((res = mysql_store_result(conn)) == NULL) {
    fprintf(stderr, "mysql: sql result retrieval failed: %s\n", mysql_error(conn));
    mysql_close(conn);
    return -1;
  }

  num_fields = mysql_num_fields(res);

  /*
   * Determine column display widths -- requires result set to be
   * generated with mysql_store_result(), not mysql_use_result().
   */
  mysql_field_seek (res, 0);
  for (i = 0; i < num_fields; i++) {
    field = mysql_fetch_field (res);
    col_len = strlen (field->name);
    if (col_len < field->max_length)
      col_len = field->max_length;
    if (col_len < 4 && !IS_NOT_NULL (field->flags))
      col_len = 4;    /* 4 = length of the word "NULL" */
    field->max_length = col_len;    /* reset column info */
  }

  print_dashes(res);
  fputc('|', stdout);
  mysql_field_seek(res, 0);
  for(i = 0; i < num_fields; i++) {
    field = mysql_fetch_field(res);
    printf(" %-*s |", (int)field->max_length, field->name);
  }
  fputc('\n', stdout);
  print_dashes(res);

  while ((row = mysql_fetch_row(res))) {
    mysql_field_seek(res, 0);
    fputc('|', stdout);
    for (i = 0; i < num_fields; i++) {
      field = mysql_fetch_field(res);
      if (row[i] == NULL) {
        printf(" %-*s |", (int)field->max_length, "NULL");
      } else if(IS_NUM(field->type)) {
        printf(" %*s |", (int)field->max_length, row[i]);
      } else {
        printf(" %-*s |", (int)field->max_length, row[i]);
      }
    }
    fputc('\n', stdout);
  }

  print_dashes(res);

  mysql_free_result(res);

  return 0;
}

static void config_init(config_t *config) {
  const char *homedir;
  char *userconffile;
  dictionary *ini;

  homedir = get_home_dir();
  userconffile = build_filename(homedir, USERCONFFILE);

  ini = iniparser_load(userconffile);

  config->host = strdup(iniparser_getstring(ini, "mysql:host", NULL));
  config->user = strdup(iniparser_getstring(ini, "mysql:user", NULL));
  config->password = strdup(iniparser_getstring(ini, "mysql:password", NULL));
  config->dbname = strdup(iniparser_getstring(ini, "mysql:dbname", NULL));

  iniparser_freedict(ini);
}

int main(int argc, char **argv) {
  config_t config;
  MYSQL *conn = NULL;

  config_init(&config);

  if ((conn = db_connect(config.host, config.user, config.password, config.dbname)) == NULL) {
    return -1;
  }

  if (product_list(conn) == -1) {
    return -1;
  }

  mysql_close(conn);

  return 0;
}
