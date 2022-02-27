#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT     8080
#define MAXLINE 1024

// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char* hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = 0;

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr*)&servaddr,
        sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    socklen_t length = sizeof(servaddr);
    if (getsockname(sockfd, (struct sockaddr *)&servaddr, &length))
    {
        perror("Вызов getsockname неудачен.");
        exit(1);
    }
    printf("Server: port number: % d\n", ntohs(servaddr.sin_port));

    int  n;
    socklen_t len = sizeof(cliaddr);  

    for (;;) {
        n = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*)&cliaddr, &len);
        buffer[n] = '\0';
        printf("Client: IP [%s] port[%d] %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), buffer);
        sendto(sockfd, (const char*)buffer, n, 0, (const struct sockaddr*)&cliaddr, sizeof(cliaddr));
    }

    return 0;
}