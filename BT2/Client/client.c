#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "transfer.h"
#define PORT 8080

ssize_t total = 0;
void sendfile(FILE *fp, int sockfd);
int main(int argc, char const *argv[])
{
    int a=0;
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client.";
    char hello1[999] = {0};
    char buffer[1024] = {0};
    char add[225];
    int _continue = 1;
    printf("Nhap dia chi server: ");
    gets(add);
    //tao socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, add, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    // connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    while (_continue == 1)
    {
        
        if(a==0)
        {
            char mess_from_client[225] = "Hello server!";

            hello = &mess_from_client;

            printf("\n");
            send(sock, hello, strlen(hello), 0);
            a++;
        }
        valread = read(sock, buffer, 1024);
        // printf("Hello message sent\n");
        if (strcmp(buffer, "Hello client!") == 0)
        {
            printf("%s\n", buffer);
            char *confirm = "Are you ready?";
            send(sock, confirm, strlen(confirm), 0);
            continue;
        }
        else if (strcmp(buffer, "Yes! Ten file muon upload: ") == 0)
        {
            printf("%s\n", buffer);
            char *filename[99];
            gets(filename);
            fflush(stdin);
            if (filename == NULL)
            {
                perror("Can't get filename");
                exit(1);
            }

            char buff[BUFFSIZE] = {0};
            strncpy(buff, filename, strlen(filename));
            if (send(sock, buff, BUFFSIZE, 0) == -1)
            {
                perror("Can't send filename");
                exit(1);
            }

            FILE *fp = fopen(filename, "rb");
            if (fp == NULL)
            {
                perror("Can't open file");
                exit(1);
            }

            sendfile(fp, sock);
            //puts("Send Success");
            printf("Gui thanh cong, NumBytes = %ld\n", total);
            fclose(fp);
            close(sock);
            return;
        }
        fflush(stdin);
    }
    close(sock);
    return 0;
}
void sendfile(FILE *fp, int sockfd)
{
    int n;
    char sendline[MAX_LINE] = {0};
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0)
    {
        total += n;
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }

        if (send(sockfd, sendline, n, 0) == -1)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
}
