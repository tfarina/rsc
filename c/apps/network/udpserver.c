#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "die.h"

#define BUFLEN 512
#define PORT 5300

int main(void) {
  struct sockaddr_in myaddr, remoteaddr;
  socklen_t addrlen;
  int sockfd;
  int recvlen;
  char buf[BUFLEN];

  memset(&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(PORT);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    die("cannot create socket");

  if (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1)
    die("bind failed");

  for (;;) {
    recvlen = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&remoteaddr, &addrlen);
    if (recvlen <= 0) {
     continue;
    }
    buf[recvlen] = '\0';
    printf("received message: %s\n", buf);
  }

  return 0;
}
