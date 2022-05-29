#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <signal.h>

#define BUFLEN 255

int main()
{
    signal(SIGCHLD, SIG_IGN);

    int sockMain;
    socklen_t length;
    struct sockaddr_in servAddr, clientAddr;
    socklen_t lenClient = sizeof(clientAddr);

    if ((sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Сервер не может открыть главный socket.");
        exit(1);
    }

    memset((char *)&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = 0;

    if (bind(sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr)))
    {
        perror("Связывание сервера неудачно.");
        exit(1);
    }

    length = sizeof(servAddr);
    if (getsockname(sockMain, (struct sockaddr *)&servAddr, &length))
    {
        perror("Вызов getsockname неудачен.");
        exit(1);
    }

    printf("[SERVER] - номер порта %d\n", ntohs(servAddr.sin_port));
    char string_buff[BUFLEN] = {'\0'};
    inet_ntop(AF_INET, &servAddr.sin_addr, string_buff, BUFLEN);
    printf("[SERVER] - IP - %s\n", string_buff);

    listen(sockMain, 5);
    
    
    for(;;)
    {
        int socketClient = 0;
        if ((socketClient = accept(sockMain, (struct sockaddr*)&clientAddr, &lenClient)) < 0)
        {
            perror("Неверный socket для клиента.");
            exit(1);
        }

        if (fork() == 0)
        {
            for(;;)
            {
                char buf[BUFLEN];
                int msgLength = 0;
                memset((char *)&buf, 0, sizeof(buf));

                if ((msgLength = recv(socketClient, buf, BUFLEN, 0)) < 0)
                {
                    perror("Плохое получение дочерним процессом.");
                    exit(1);
                }

                if (msgLength == 0)
                {
                    printf("Сервер: клиент с номером %d разорвал соединение с сервером\n\n", socketClient);
                    close(socketClient);
                    exit(0);
                }

                printf("Сервер: Socket для клиента: %d\n", socketClient);
                printf("\tДлина сообщения: %d\n", msgLength);
                printf("\tСообщение: %s\n\n", buf);
            }
        }
    }

    return 0;
}
