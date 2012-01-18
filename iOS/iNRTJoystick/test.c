#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 61557
#define BUFLEN 6

int main(void)
{
  struct sockaddr_in si_other, si_me;
  int s, i, slen = sizeof(si_other);
  unsigned char buf[BUFLEN];

  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
    perror("Socket error");
    exit(-1);
  }

  memset((char*) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  if ( bind(s, &si_me, sizeof(si_me)) == -1 )
  {
    perror("Bind error");
    exit(-1);
  }

  while (1)
  {
    if (recvfrom(s, buf, BUFLEN, 0, &si_other, &slen) == -1)
    {
      perror("recvfrom()");
      exit(-1);
    }

    if (buf[0] == 's' && buf[5] == 'e')
    {
      if (buf[1] < 120)
        printf("Going left...\n");
      else if (buf[1] > 136)
        printf("Going right...\n");
    }
  }
  close(s);
  return 0;
}
