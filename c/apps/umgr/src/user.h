#ifndef USER_H_
#define USER_H_

#include "alpm_list.h"

typedef struct user_s
{
  int id;
  const char *fname;
  const char *lname;
  const char *email;
} user_t;

user_t *user_alloc(void);

int user_init(void);

int user_add(user_t *user);

int user_change(user_t *user);

int user_delete(user_t *user);

alpm_list_t *ab_get_user_list(void);

user_t *ab_get_user_by_id(alpm_list_t *users, int id);

/**
 * Prints to standard output all the records found in the table.
 */
int ab_print_user_records(void);

#endif  /* USER_H_ */
