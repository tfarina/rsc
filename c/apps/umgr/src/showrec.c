#include "user.h"

int main(int argc, char **argv) {
  int rc;

  ab_init();

  rc = ab_print_user_records();

  ab_close();

  return rc;
}
