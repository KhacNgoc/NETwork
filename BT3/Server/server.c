// Ho ten: Nguyen Khac Ngoc
// MSSV: 17020931
#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>


#define PORT 8080

int main(int argc, char const *argv[]) {


    int sockfd;
    struct sockaddr_in sockaddr;
    struct sockaddr_in cliaddr;
    int newfd;
    int cliaddr_len = sizeof(cliaddr);
    int nbytes;

    char url[50] = "../Server/";
    char fileName[30];
    FILE * fp;
    long lSize;
    size_t result;
    int blockSize;
    int BLOCK_SIZE;
    printf("nhap kich thuoc buffer: ");
    scanf("%d", &BLOCK_SIZE);
    char buf[BLOCK_SIZE+1];



    //tao socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //tuy bien
    int option = 1;
    if(setsockopt(sockfd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
        printf("setsockopt failed\n");
        close(sockfd);
        exit(1);
    }

    //gan dia chi cho socket
    bzero(&sockaddr, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(PORT);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(sockfd, (struct sockaddr*) &sockaddr, sizeof(sockaddr)) < 0) {
        perror("bind");
        exit(1);
    }

    //chi dinh socket lang nghe ket noi
    if(listen(sockfd, 5) <0) {
        perror("listen");
        exit(1);
    }

    //cho chap nhan ket noi den
    newfd = accept(sockfd, (struct sockaddr*) &cliaddr, &cliaddr_len);
    if(newfd < 0) {
        perror("accept");
        exit(1);
    }
    else {
        printf("connected with ip: %s and port: %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    }

    //đọc tên file
    if(nbytes = read(newfd, fileName, sizeof(fileName)) < 0) {
        perror("read");
        exit(1);
    }
    else printf("client want to download file %s\n", fileName);

    strcat(url, fileName);
    fp = fopen(url, "rb");
    if(fp == NULL) {
        fputs("File error\n", stderr);
    }
    else {
        fseek(fp, 0, SEEK_END); //di chuyển con trỏ xuống cuối file
        lSize = ftell(fp); //trả về  vị trí của con trỏ file để lấy kích thước file
        rewind(fp); //di chuyển con trỏ file về đầu

        int sizeToRead = lSize;
        int sizefile = lSize;
        while(sizeToRead > 0) {
            if(sizeToRead >= BLOCK_SIZE) {
                blockSize = BLOCK_SIZE;
            }
            else {
                blockSize = sizeToRead;
            }
            result = fread (buf, 1, blockSize, fp); //đọc file với kích thước = 1.
            buf[result] = '\0'; //gán phần tử cuối bằng null
            
            //gắn size file vào message
            if(sizeToRead == lSize) {
                //chuyển int thành string
                char filesize[BLOCK_SIZE+6];
                sprintf(filesize, "%ld@", lSize);

                //ghi size vào message đầu tiên và truyền đi
                strcat(filesize, buf);
                if(write(newfd, filesize, sizeof(filesize)) < 0) {
                    perror("write");
                    exit(1);
                }
            } else {
                if(write(newfd, buf, sizeof(buf)) < 0) {
                    perror("write");
                    exit(1);
                }
            }
            sizeToRead -= blockSize;
            sleep(1);
        }
        printf("completed!\n");
        fclose(fp);
    }
   close(newfd);
}
