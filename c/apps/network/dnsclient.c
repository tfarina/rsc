#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

struct dnsheader {
  uint16_t id;
  uint16_t flags;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
};

struct dnsquestion {
  char *qname;
  size_t qnamelen;
  uint16_t qtype;
  uint16_t qclass;
};

static const uint16_t kFlagRD = 0x100;

static const uint16_t kQTypeA = 1;
static const uint16_t kQClassIN = 1;

static const uint16_t kMessageMaxLen = 512;
static const uint8_t kMaxNameLength = 255;
static const uint8_t kDefaultPort = 53;

int main(int argc, char **argv) {
  struct dnsheader* header;
  struct dnsquestion* question;
  char hostname[kMaxNameLength];
  uint8_t *packet;
  struct sockaddr_in to;
  socklen_t tolen = sizeof(to);
  int sockfd;
  uint8_t response[kMessageMaxLen];
  struct sockaddr_storage addr;
  socklen_t addrlen2 = sizeof(addr);
  int rlen;

  if (argc != 2) {
    fprintf(stderr, "usage: ./dnsclient hostname\n");
    exit(EXIT_FAILURE);
  }

  strcpy(hostname, argv[1]);

  if (strlen(hostname) > kMaxNameLength) {
    exit(EXIT_FAILURE);
  }

  header = malloc(sizeof(*header));
  memset(header, 0, sizeof(*header));

  header->id = rand();
  header->flags = htons(kFlagRD); //htons(1 << 8) htons(0x0100);
  header->qdcount = htons(1);

  question = malloc(sizeof(*question) + strlen(hostname) + 2);
  memset(question, 0, sizeof(*question));

  question->qnamelen = strlen(hostname) + 2;
  question->qname = malloc(question->qnamelen);

  // dns_domain_fromdot
  char *label = NULL;
  char *hostnamedup = strdup(hostname);
  int len, total = 0;
  while ((label = strsep(&hostnamedup, ".")) != NULL) {
    len = strlen(label);
    if (len > kMaxNameLength) {
      fprintf(stderr, "Label is too long");
      free(hostnamedup);
      return -1;
    }
    question->qname[total++] = len;
    strcat(&question->qname[total], label);
    total += len;
  }
  free(hostnamedup);

  question->qtype = htons(kQTypeA);
  question->qclass = htons(kQClassIN);

  // Build the packet (header + question) we are going to send to the DNS
  // server.
  size_t questionsize =
      question->qnamelen + sizeof(question->qtype) + sizeof(question->qclass);
  size_t packet_length = sizeof(*header) + questionsize;

  packet = malloc(packet_length);

  int offset = 0;

  // HEADER
  memcpy(packet + offset, header, sizeof(*header));
  offset += sizeof(*header);

  // QUESTION: QNAME + QTYPE + QCLASS

  // QNAME
  memcpy(packet + offset, question->qname, question->qnamelen);
  offset += question->qnamelen;

  // QTYPE
  memcpy(packet + offset, &question->qtype, sizeof(question->qtype));
  offset += sizeof(question->qtype);

  // QCLASS
  memcpy(packet + offset, &question->qclass, sizeof(question->qclass));

  // Connect to the DNS Server that we will send the udp query to.
  memset(&to, 0, tolen);
  to.sin_family = AF_INET;
  to.sin_port = htons(kDefaultPort);
  inet_pton(AF_INET, "8.8.8.8", &to.sin_addr);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    printf("cannon create socket\n");
    exit(EXIT_FAILURE);
  }

  // Send the query.
  if (sendto(sockfd, packet, packet_length, 0, (struct sockaddr *)&to,
             tolen) == -1) {
    fprintf(stderr, "sendto failed\n");
    exit(EXIT_FAILURE);
  }

  // Receive the response.
  if ((rlen = recvfrom(sockfd, response, sizeof(response), 0,
                       (struct sockaddr *)&addr, &addrlen2)) == -1) {
    printf("recvfrom failed\n");
    exit(EXIT_FAILURE);
  }

  printf("%d\n", rlen);

  // Parse the response.
  struct dnsheader *response_header = malloc(sizeof(struct dnsheader));
  memcpy(response_header, response, sizeof(struct dnsheader));

  printf("QUERY: %d\n", ntohs(response_header->qdcount));
  printf("ANSWER: %d\n", ntohs(response_header->ancount));

  return 0;
}
