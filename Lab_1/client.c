#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 1024

// Driver code
int main(int argc, char* argv[]) {
    int sockfd;
    char buffer[MAXLINE];
    char out[MAXLINE];

    struct sockaddr_in     servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n, n2;
    socklen_t len = sizeof(servaddr);  
    for (int i = 2; i < 6; i++) {
        
        sendto(sockfd, (const char*)argv[1], strlen(argv[1]), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));
        printf("Message sent. Delay[%dsec]\n",i);
        n2 = recvfrom(sockfd, (char*)out, MAXLINE, MSG_WAITALL, (struct sockaddr*)&servaddr, &len);
        out[n2] = '\0';
        printf("ANS SERVER:  %s\n",  out);
        //sleep(atoi(argv[1]));
        sleep(i);
    }
    close(sockfd);
    return 0;
}