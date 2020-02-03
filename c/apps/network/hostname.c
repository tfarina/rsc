#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

static char *get_hostname(void) {
  static char hostname[MAXHOSTNAMELEN + 1];

  if (gethostname(hostname, sizeof(hostname)) < 0) {
    fprintf(stderr, "gethostname() failed: %s\n", strerror(errno));
    return (char *) 0;
  }

  hostname[MAXHOSTNAMELEN] = '\0'; /* Null terminate, just to be safe. */

  return hostname;
}

int main(void) {
  char *hostname;

  hostname = get_hostname();

  printf("%s\n", hostname);

  return EXIT_SUCCESS;
}
