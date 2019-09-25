#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080 
   
int main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char *hello = "Hello from client";
    char hello1[999]={0};
    char buffer[1024] = {0}; 
    char add[225];
    int continu = 1;
    printf("Nhap dia chi server\n");
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
    if(inet_pton(AF_INET, add, &serv_addr.sin_addr) <= 0)  
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
    while(continu == 1){
        char mess_from_client[225]="Hello server!";
        
        hello = &mess_from_client;

        printf("\n");
        send(sock , hello , strlen(hello) , 0 ); 
        // printf("Hello message sent\n"); 
        valread = read( sock , buffer, 1024); 
        printf("%s\n",buffer );
        if(strcmp(buffer, "Hello client!\nWhat is your name?") == 0){
		char name_client[99];
		printf("Enter your name: ");
		gets(name_client);
		
        	fflush(stdin);
		strcpy(hello1, "My name is ");
		strcat(hello1, name_client);
		strcat(hello1, " ! Nice to meet you!\n");
		printf("%s", hello1);
		
		send(sock , hello1 , strlen(hello1) , 0 );
		close(sock);
		return 0; 
		
	}
        if (strcmp(mess_from_client, "bye") == 0)
        {
            continu = 0;
        }
        fflush(stdin);
    }
    close(sock);
    return 0; 
}
