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
#include <pthread.h>

pthread_mutex_t mutex;
#define BUFLEN 255
#define SOCKET_COUNT 256

typedef struct thread_data
{
    int socketId;
    FILE *log_file;
} thread_data_t;

void *threadListener(void *args)
{
    pthread_detach(pthread_self());

    thread_data_t *data = (thread_data_t *)args;
    int sockClient = data->socketId;
    FILE *log_file = data->log_file;
    char buf[BUFLEN] = {'\0'};

    for (;;)
    {
        memset((char *)&buf, 0, sizeof(buf));
        int msgLength = recv(sockClient, buf, BUFLEN, 0);

        if (msgLength < 0)
        {
            perror("Плохое получение дочерним процессом.");
            pthread_exit(NULL);
        }
        if (msgLength == 0)
        {
            printf("[SERVER]: клиент с номером %d разорвал соединение с сервером\n\n", sockClient);
            close(sockClient);
            pthread_exit(NULL);
        }

        printf("[SERVER]: сокет для клиента: %d\n", sockClient);
        printf("\tДлина сообщения: %d\n", msgLength);
        printf("\tСообщение: %s\n\n", buf);

        pthread_mutex_lock(&mutex);
        //printf("%d:%s\n", sockClient, buf);
        fprintf(log_file, "%d:%s\n", sockClient, buf);
        pthread_mutex_unlock(&mutex);
        fflush(log_file);
    }
}

int main()
{
    pthread_mutex_init(&mutex, NULL);
    int sockMain = 0;
    int length = 0;
    struct sockaddr_in servAddr;
    FILE *log_file = fopen("logfile.log", "w+");

    if(log_file == NULL)
    {
        printf("[ERROR] - при создании файла\n");
    }

    if ((sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Сервер не может открыть главный сокет.");
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
    if (getsockname(sockMain, (struct sockaddr *)&servAddr, (socklen_t *restrict)&length))
    {
        perror("Getsockname завершился с ошибкой.");
        exit(1);
    }

    printf("[SERVER] - номер порта %d\n", ntohs(servAddr.sin_port));
    char string_buff[BUFLEN] = {'\0'};
    inet_ntop(AF_INET, &servAddr.sin_addr, string_buff, BUFLEN);
    printf("[SERVER] - IP - %s\n", string_buff);

    listen(sockMain, 5);

    for (;;)
    {
        int socketClient = 0;
        if ((socketClient = accept(sockMain, 0, 0)) < 0)
        {
            perror("Неверный сокет для клиента.");
            exit(1);
        }

        thread_data_t data = {socketClient, log_file};

        pthread_t new_thread = 0;
        if (pthread_create(&new_thread, NULL, threadListener, (void *)&data))
        {
            exit(0);
        }
    }
    pthread_mutex_destroy(&mutex);
    fclose(log_file);
    return 0;
}
