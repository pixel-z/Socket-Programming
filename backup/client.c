// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<fcntl.h>
#include<unistd.h>
#define PORT 8000
#define SIZE 100000000

int no_commands = 0;

char **space_tokenize(char *input_str)
{
    char **tokens;
    char *command;
    tokens=malloc(1024*sizeof(char *));
    command=strtok(input_str," \n\t\a\r");

    while (command!=NULL)
    {
        tokens[no_commands++]=command;
        command=strtok(NULL," \n\t\a\r");
    }
    tokens[no_commands]=NULL;
    return tokens;
}

void downloadFile(int sockfd)
{
    char check[2];
    recv(sockfd,check,sizeof(check),0);
    if (strcmp(check,"N")==0)
    {
        // file doesn't exist
        return;
    }

    int fd = open(,);
    
    while(1)
    {
        char *buffer=malloc(SIZE);
        
        if(recv(sockfd, buffer, SIZE, 0)<=0)
            break;  // done downloading

        // send acknowledgement that message received
        send(socket, "done", 5, 0);

    }
}

int main(int argc, char *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;
    // char *hello = "msg from client";
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected to server\n");

    while (1)
    {
        no_commands = 0; 
        char *input=malloc(sizeof(char)*1024);
        size_t len = 0;

        printf("> ");
        getline(&input,&len,stdin);
        send(sock,input,len,0);   // sending command "get <file1> <file2>", server handles the other

        if (strcmp(input,"exit\n") == 0)
        {
            sleep(1); // so that server first exits (if client 1st exits then garbage is printed)
            return -1;
        }
        char **tokenized_command = space_tokenize(input);

        
    }
    
    return 0;
}
