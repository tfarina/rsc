/*
 * Heavily modified from https://github.com/reagent/http
 */

#include <netdb.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define RECVSIZE 1024

typedef struct {
	char *data;
	size_t len;
	size_t cap;
} sbuf;

#if defined(__GNUC__)
#define NORETURN __attribute__((__noreturn__))
#else
#define NORETURN
#endif

static void NORETURN die(const char *msg, ...)
{
        va_list args;

        va_start(args, msg);

        fprintf(stderr, "fatal: ");
        vfprintf(stderr, msg, args);
        fprintf(stderr, "\n");

        va_end(args);

        exit(EXIT_FAILURE);
}

static void *xrealloc(void *p, size_t n)
{
	p = realloc(p, n);
        if (p == NULL)
                die("out of memory: %lu", n);
	return p;
}

static void sbuf_setlen(sbuf *b, size_t len)
{
        b->len = len;
        b->data[len] = '\0';
}

static void sbuf_init(sbuf *b)
{
	b->data = NULL;
	b->len = 0;
	b->cap = 0;
}

static void sbuf_free(sbuf *b)
{
	free(b->data);
	sbuf_init(b);
}

static void sbuf_grow(sbuf *b, size_t extra)
{
	size_t want;

        want = b->len + extra;
        if (want > b->cap) {
                b->cap = 2 * want;
                if (b->cap < 64)
                        b->cap = 64;
                b->data = xrealloc(b->data, b->cap);
        }
}

static void sbuf_append(sbuf *b, const void *data, size_t len)
{
	sbuf_grow(b, len);
	memcpy(b->data + b->len, data, len);
        sbuf_setlen(b, b->len + len);
}

int main(int argc, char **argv) {
  struct addrinfo hints, *res;
  int sockfd;
  char request[] = "GET / HTTP/1.0\r\nHost: google.com\r\nConnection: close\r\n\r\n";
  size_t bytes_sent = 0;
  size_t total_bytes_sent = 0;
  size_t bytes_to_send = strlen(request);
  ssize_t bytes_received;
  char data[RECVSIZE];
  sbuf response;

  sbuf_init(&response);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo("google.com", "80", &hints, &res);

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  connect(sockfd, res->ai_addr, res->ai_addrlen);

  printf("bytes to send: %ld\n", bytes_to_send);

  for (;;) {
    bytes_sent = send(sockfd, request, strlen(request), 0);
    total_bytes_sent += bytes_sent;

    printf("bytes sent: %ld\n", bytes_sent);

    if (total_bytes_sent >= bytes_to_send) {
      break;
    }
  }

  printf("receiving data...");

  for (;;) {
    bytes_received = recv(sockfd, data, RECVSIZE, 0);

    if (bytes_received == -1) {
      return -1;
    } else if (bytes_received == 0) {
      break; /*return 0;*/
    }

    if (bytes_received > 0) {
      sbuf_append(&response, data, bytes_received);
    }
  }

  printf("finished receiving data.\n");

  printf("%s\n", response.data);

  sbuf_free(&response);

  return 0;
}
