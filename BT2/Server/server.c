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
void writefile(int sockfd, FILE *fp);
int main(int argc, char const *argv[])
{
    int a = 0;
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char mess_from_client[225];
    char buffer[1024] = {0};
    char *hello = "Hello from server.";
    int continu = 1;
    //tao socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    //gan dia chi cho socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); //gan cong la 8080
    // bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    //listen, chi dinh socket lang nghe ket noi
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    //accept, chap nhan ket noi
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    while (continu == 1)
    {
        char str_cli_ip[INET_ADDRSTRLEN];
        struct sockaddr_in *ip_client = (struct sockaddr_in *)&address;
        inet_ntop(AF_INET, &ip_client->sin_addr, str_cli_ip, INET_ADDRSTRLEN);
        if (a == 0)
        {
            printf("ipclient: %s\n", str_cli_ip);
            char str_cli_port[INET_ADDRSTRLEN];
            printf("port: %d\n", ntohs(ip_client->sin_port));
            printf("\n");
            a++;
        }
        valread = read(new_socket, buffer, 1024);
        if (strcmp(buffer, "Hello server!")==0)
        {
            printf("%s\n", buffer);
            char *reHello = "Hello client!";
            send(new_socket, reHello, strlen(reHello), 0);
            continue;
        }
        else if (strcmp(buffer, "Are you ready?")==0)
        {
            printf("%s\n", buffer);
            char *resend = "Yes! Ten file muon upload: ";
            send(new_socket, resend, strlen(resend), 0);
            continue;
        }
        else
        {
            char filename[BUFFSIZE] = {0};
            if (recv(new_socket, filename, BUFFSIZE, 0) == -1)
            {
                perror("Can't receive filename");
                exit(1);
            }

            FILE *fp = fopen("filetransfer.txt", "wb");
            if (fp == NULL)
            {
                perror("Can't open file");
                exit(1);
            }

            char addr[INET_ADDRSTRLEN];
            printf("Start receive file: %s from %s\n", filename, inet_ntop(AF_INET, &address.sin_addr, addr, INET_ADDRSTRLEN));
            writefile(new_socket, fp);
            printf("Receive Success, NumBytes = %ld\n", total);

            fclose(fp);
            close(new_socket);
            return 0;
        }
    }
    close(new_socket);
    return 0;
}
void writefile(int sockfd, FILE *fp)
{
    ssize_t n;
    char buff[MAX_LINE] = {0};
    while ((n = recv(sockfd, buff, MAX_LINE, 0)) > 0)
    {
        total += n;
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }

        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, MAX_LINE);
    }
}