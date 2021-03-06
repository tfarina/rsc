#ifndef DB_H_
#define DB_H_

typedef struct {
  char const *host;
  char const *user;
  char const *password;
  char const *dbname;
  int unsigned port;
} db_config_t;

void db_config_init(db_config_t *config);

#endif  /* DB_H_ */
