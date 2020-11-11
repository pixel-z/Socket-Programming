#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8000
#include<unistd.h>
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

int main(int argc, char *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Binding done\n");

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        no_commands = 0;
        char *input = malloc(sizeof(char)*1024);
        valread = read(new_socket , input, 1024);  // read infromation received into the input

        if (strcmp(input,"exit\n") == 0)
            return 1;

        char **tokenized_command = space_tokenize(input);
        if (no_commands==0)
            continue;
        if (strcmp(tokenized_command[0],"get") == 0)
        {
            for (int i = 1; i < no_commands; i++)
            {
                
            }
            
        }

    }
    
    return 0;
}
