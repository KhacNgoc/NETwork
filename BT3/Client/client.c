// Ho ten: Nguyen Khac Ngoc
// MSSV: 17020931
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define BLOCK_SIZE 1000

int main( int argc, const char* argv[]) {
    int sockfd;

    FILE * fp;
    char url[50] = "../Client/";
    char fileName[30] = "test.txt";
    long lSize;
    size_t result;
    int blockSize;
    int nbytes;
    char buf[BLOCK_SIZE+6];

    //tao socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    //connect voi server
    char ipserver[50];
    printf("nhap ip server: ");
    fgets(ipserver, 50, stdin);
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(ipserver);
    if(connect(sockfd, (struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0) {
        perror("connect");
        exit(1);
    }

    //yêu cầu file từ server
    if(write(sockfd, fileName, sizeof(fileName))<0) {
        perror("write");
        exit(1);
    }

    strcat(url, fileName);
    fp = fopen(url, "wb");
    if(fp==NULL) {
        printf("error open file");
    }

    int size = 0;
    int fileServerSize = 0;
    //doc message rep
    while(1) {
        if((nbytes = read(sockfd, buf, sizeof(buf))) < 0) {
            perror("read");
            exit(1);
        }
        else 
        {
            if(size == 0) {
                char *str = strtok(buf, "@");
                fileServerSize = atoi(str);
                str = strtok(NULL,"@");
                fwrite(str, 1, strlen(str), fp);
                fseek(fp, 0, SEEK_END);
                size += strlen(str);
            }
            else {
                fwrite(buf, 1, strlen(buf), fp);
                fseek(fp, 0, SEEK_END);
                size = ftell(fp);
                if(size >= fileServerSize) {
                    break;
                }
            }
        }  
    }
    close(sockfd);
}
