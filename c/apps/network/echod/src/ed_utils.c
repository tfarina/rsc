#include "ed_utils.h"

#include <stdint.h>
#include <string.h>

#define ED_DIR_SEP '/'

char *ed_get_progname(char *argv0) {
  char *name;

  name = strrchr(argv0, ED_DIR_SEP);
  if (name == NULL) {
    name = argv0;
  } else {
    name++;
  }

  return name;
}

int ed_valid_port(int port) {
  if (port < 1 || port > UINT16_MAX) {
    return 0;
  }

  return 1;
}
