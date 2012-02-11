#include "SendUDP.h"

struct hostent *he;
struct sockaddr_in their_addr; // connector's address information
int sockfd;		       // file descriptor for socket connection
int isopen;

//Initialize network connection
int SUDP_Init(const char* ipaddress, unsigned int port)
{
    if ((he=gethostbyname(ipaddress)) == NULL)  // get the host info
        return -1;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        return -1;
    
    their_addr.sin_family = AF_INET;     // host byte order
    their_addr.sin_port = htons(port); // short, network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
    isopen = 1;
	return 0;
}

//Send a UDP packet
int SUDP_SendMsg(const char * data, int length)
{
    int numbytes;
    if ((numbytes = sendto(sockfd, data, length, 0,
             (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) 
    {
        perror("sendto");
        return -1;
    }

    return numbytes;
}

int SUDP_RecvMsg(const char * data, int length)
{
    unsigned int yes = 1;
    int numbytes;
    if ( (numbytes = recvfrom(sockfd, data, length, 0, (struct sockaddr *)&their_addr, &yes)) )
    {
        perror("recvfrom");
        return -1;
    }
    return numbytes;
}

//Close the socket
int SUDP_Close(){
    close(sockfd);
    isopen = 0;
    return 0;
}

int SUDP_IsOpen()
{
    return isopen;
}


