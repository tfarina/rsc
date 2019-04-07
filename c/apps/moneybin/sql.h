#ifndef MB_SQL_H_
#define MB_SQL_H_

typedef struct dba_conn_s {
  void *data;
} dba_conn_t;

typedef struct dba_driver_s {
  char *name;

  int (*alloc)(dba_conn_t *conn);
  int (*free)(dba_conn_t *conn);

  int (*connect)(dba_conn_t *conn);
  int (*disconnect)(dba_conn_t *conn);
} dba_driver_t;

int dba_conn_alloc(dba_conn_t *conn);
int dba_conn_free(dba_conn_t *conn);

int dba_conn_connect(dba_conn_t *conn);
int dba_conn_disconnect(dba_conn_t *conn);

#endif  /* MB_SQL_H_ */