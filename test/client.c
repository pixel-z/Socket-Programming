// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#define PORT 8000
#define SIZE 1000000

int no_commands = 0;

long long min(long long a, long long b)
{
    return (a < b) ? a : b;
}

char **space_tokenize(char *input_str)
{
    char **tokens;
    char *command;
    tokens = malloc(1024 * sizeof(char *));
    command = strtok(input_str, " \n\t\a\r");

    while (command != NULL)
    {
        tokens[no_commands++] = command;
        command = strtok(NULL, " \n\t\a\r");
    }
    tokens[no_commands] = NULL;
    return tokens;
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
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connected to server\n");

    while (1)
    {
        no_commands = 0;
        char *input = NULL;
        size_t len = 0;

        printf("> ");
        getline(&input, &len, stdin);
        send(sock, input, strlen(input), 0); // send the whole command to server

        if (strcmp(input, "exit\n") == 0)
        {
            sleep(1); // so that server first exits (if client 1st exits then garbage is printed)
            return 1;
        }

        char **tokenized_command = space_tokenize(input);
        if (no_commands == 0)
            continue;
        if (strcmp(tokenized_command[0], "get") == 0)
        {
            // printf("%d\n", no_commands);
            for (int i = 1; i < no_commands; i++)
            {
                char *check = malloc(sizeof(char) * 1024);
                read(sock, check, 1024);
                if (strcmp(check, "error") == 0)
                {
                    printf("Invalid file\n");
                    continue;
                }
                else if (strcmp(check, "success") == 0)
                {
                    printf("File %s downloading...\n", tokenized_command[i]);

                    char file_size_str[100] = {0};
                    read(sock, file_size_str, 100);
                    long long file_size = atol(file_size_str);

                    /* Acknowledge that file_size received (for sync) */
                    char *ack = "ack";
                    send(sock, ack, strlen(ack), 0);

                    /* Actual download */
                    int fd = open(tokenized_command[i], O_CREAT | O_WRONLY | O_TRUNC, 0600);
                    char *chunk = (char *)malloc(SIZE + 2);
                    long long done = 0;
                    while (1)
                    {
                        bzero(chunk, strlen(chunk));

                        //long long len_to_upload = min(SIZE, file_size - done);
                        //printf("%ld\n", strlen(chunk));
                        read(sock, chunk, SIZE);

                        write(fd, chunk, strlen(chunk));
                        done += strlen(chunk);

                        /* Acknowledgement for chunk */
                        send(sock, "wait", 4, 0);

                        if (done >= file_size)
                        {
                            char comp[10] = {0};
                            read(sock, comp, 10);
                            if (strcmp(comp, "comp") == 0)
                            {
                                printf("File %s downloaded\n", tokenized_command[i]);
                                break;
                            }
                        }
                    }
                }
                bzero(buffer, sizeof(buffer)); // strlen?
            }
        }
    }

    return 0;
}
