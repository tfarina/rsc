/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/* https://vcansimplify.wordpress.com/2013/03/14/c-socket-tutorial-echo-server/ */

#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <grp.h>
#include <netdb.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ed_log.h"
#include "ed_net.h"

#define ED_USER "_echod"

#define ED_INTERFACE NULL
#define ED_TCP_PORT 7
#define ED_BACKLOG 1024

#define BUFSIZE 8129

#define CRLF "\x0d\x0a"

static const char *progname;

static int running = 0;
static unsigned int forked = 0; /* Number of child processes. */

static char short_options[] =
    "h"  /* help */
    "d"  /* daemon mode */
    "p:" /* tcp port number to listen on */
    ;

static struct option long_options[] = {
    { "help",      no_argument,       NULL, 'h' }, /* help */
    { "daemonize", no_argument,       NULL, 'd' }, /* daemon mode */
    { "port",      required_argument, NULL, 'p' }, /* tcp port number to listen on */
    { NULL,    0,                 NULL,  0  }
};

static char *get_progname(char *argv0) {
  char *name;

  name = strrchr(argv0, '/');
  if (name == NULL) {
    name = argv0;
  } else {
    name++;
  }

  return name;
}

static void ed_show_usage(void) {
  fprintf(stderr,
	  "usage: %s [-hd] [-p port]" CRLF CRLF,
	  progname);
  fprintf(stderr,
	  "options:" CRLF
          "  -h, --help       show usage, options and exit" CRLF
	  "  -d, --daemonize  run as a daemon" CRLF
          "  -p, --port=N     set the tcp port to listen on (default: %d)" CRLF
	  "",
          ED_TCP_PORT
	  );
}

static int ed_valid_port(int port) {
  if (port < 1 || port > UINT16_MAX) {
    return 0;
  }

  return 1;
}

static void drop_privileges(uid_t uid, gid_t gid) {
  if (setgroups(1, &gid) != 0) {
    fprintf(stderr, "setgroups failed\n");
    exit(EXIT_FAILURE);
  }
  if (setresgid(gid, gid, gid) != 0) {
    fprintf(stderr, "setresgid failed\n");
    exit(EXIT_FAILURE);
  }
  if (setresuid(uid, uid, uid) != 0) {
    fprintf(stderr, "setresuid failed\n");
    exit(EXIT_FAILURE);
  }
}

static void print_num_child_forked(void) {
  log_info("num child forked: %d", forked);
}

static void sigchld_handler(int sig) {
  pid_t pid;
  int status;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    --forked;
    log_info("pid %d status %d", pid, status);
    print_num_child_forked();
  }
  signal(SIGCHLD, sigchld_handler);
}

static void handle_signal(int sig) {
  char *type;

  switch (sig) {
  case SIGINT:
    type = "SIGINT";
    break;
  case SIGTERM:
    type = "SIGTERM";
    break;
  }

  if (sig == SIGTERM || sig == SIGINT) {
    running = 0;
  }

  log_info("signal %s received, shutting down...", type);
}

static void echo_stream(int fd) {
  char buf[BUFSIZE];
  int ret;

  while ((ret = read(fd, buf, sizeof(buf))) > 0 && write(fd, buf, ret) > 0);

  sleep(1);  /* allow socket to drain before signalling the socket is closed */
  close(fd);

  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  int ch;
  int daemonize = 0;
  int value;
  struct passwd *pw;
  char *host = ED_INTERFACE;
  int port = ED_TCP_PORT;
  int tcpfd;
  fd_set rfds_in;
  /* We need to have a copy of the fd set as it's not safe to reuse FD sets
   * after select(). */
  fd_set rfds_out;
  int clientfd;
  char clientip[46];
  int clientport;
  pid_t pid;

  progname = get_progname(argv[0]);

  while ((ch = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (ch) {
    case 'd':
      daemonize = 1;
      break;

    case 'p':
      value = atoi(optarg);
      if (value <= 0) {
	fprintf(stderr, "%s: option -p requires a non zero number\n", progname);
	return EXIT_FAILURE;
      }
      if (!ed_valid_port(value)) {
	fprintf(stderr, "%s: option -s value %d is not a valid port\n", progname, value);
	return EXIT_FAILURE;
      }

      port = value;
      break;

    case 'h':
      ed_show_usage();
      return EXIT_SUCCESS;

    case '?':
      ed_show_usage();
      return EXIT_SUCCESS;

    default:
      ed_show_usage();
      exit(EXIT_FAILURE);
      /* NOTREACHED */
    }
  }
  argc -= optind;
  argv += optind;

  if (argc > 0) {
    ed_show_usage();
    exit(EXIT_FAILURE);
  }

  if (geteuid() != 0) {
    fprintf(stderr, "%s: need root privileges\n", progname);
    exit(EXIT_FAILURE);
  }

  if ((pw = getpwnam(ED_USER)) == NULL) {
    fprintf(stderr, "unknown user %s\n", ED_USER);
    exit(EXIT_FAILURE);
  }

  if (daemonize) {
    if (daemon(0, 0) == -1) {
      fprintf(stderr, "%s: unable to daemonize\n", progname);
      exit(EXIT_FAILURE);
    }
  }

  log_init(progname, !daemonize);

  tcpfd = fnet_tcp_socket_listen(host, port, ED_BACKLOG);
  if (tcpfd == FNET_ERR) {
    return EXIT_FAILURE;
  }

  log_info("%s started (%ld), port %d", progname, (long)getpid(), port);

  drop_privileges(pw->pw_uid, pw->pw_gid);

  /* Setup signals. */
  signal(SIGCHLD, sigchld_handler);
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  print_num_child_forked();

  FD_ZERO(&rfds_in);

  FD_SET(tcpfd, &rfds_in);

  running = 1;

  while (running == 1) {
    memcpy(&rfds_out, &rfds_in, sizeof(fd_set));

    if (select(tcpfd + 1, &rfds_out, NULL, NULL, NULL) > 0) {
      if (FD_ISSET(tcpfd, &rfds_out)) {
        clientfd = fnet_tcp_socket_accept(tcpfd, clientip, sizeof(clientip), &clientport);
	if (clientfd == FNET_ERR) {
	  return EXIT_FAILURE;
	}

	log_info("Accepted connection from %s:%d", clientip, clientport);

        ++forked;
        print_num_child_forked();

        pid = fork();
        switch (pid) {
        case -1:
          close(clientfd);
          --forked;
          print_num_child_forked();
          break;

        case 0:
          close(tcpfd);
          echo_stream(clientfd);
          break;

        default:
          close(clientfd); /* we are the parent so look for another connection. */
         log_info("pid: %d", pid);
        }
      }
    }
  }

  closelog();
  log_info("Stopped");

  return EXIT_SUCCESS;
}
