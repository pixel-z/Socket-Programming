#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define PORT 8000
#define SIZE 32768

int no_commands = 0;
// int counter=0;

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
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
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
    address.sin_family = AF_INET;         // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons(PORT);       // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
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
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        no_commands = 0;
        char *input = malloc(sizeof(char) * 1024);
        valread = read(new_socket, input, 1024); // read infromation received into the input

        if (strcmp(input, "exit\n") == 0)
            return 1;

        char **tokenized_command = space_tokenize(input);
        if (no_commands == 0)
            continue;
        if (strcmp(tokenized_command[0], "get") == 0)
        {
            for (int i = 1; i < no_commands; i++)
            {
                // tokenized_command[i] == filename
                int fd = open(tokenized_command[i], O_RDONLY);

                if (fd < 0)
                {
                    printf("Invalid file\n");
                    char *check = "error";
                    send(new_socket, check, strlen(check), 0);
                    continue;
                }
                else
                {
                    printf("File %s found, uploading...\n", tokenized_command[i]);
                    char *check = "success";
                    send(new_socket, check, strlen(check), 0);
                    sleep(1); // because the below send can get mixed up

                    /* Finding file size */
                    struct stat st;
                    stat(tokenized_command[i], &st);
                    long long file_size = st.st_size;

                    char file_size_str[100] = {0};
                    sprintf(file_size_str, "%lld", file_size);
                    send(new_socket, file_size_str, strlen(file_size_str), 0); // sending file_size to client for progress bar

                    /* Acknowledge that file_size received (for sync) */
                    char ack[4]={0};
                    read(new_socket, ack, 3);
                    // printf("%s\n", ack);

                    char *tyu = (char *)malloc(100);
                    if (strcmp(ack, "ack") == 0)
                    {
                        /* Actual upload */
                        long long done = 0;
                        char *chunk = malloc(SIZE + 2);
                        while (1)
                        {
                            bzero(chunk, strlen(chunk));
                            bzero(tyu, strlen(tyu));

                            // long long len_to_upload = min(SIZE, file_size - done);
                            read(fd, chunk, SIZE); // reading from file

                            done += strlen(chunk);
                            send(new_socket, chunk, strlen(chunk), 0); // sending the read part to client

                            /* Acknowledgement for chunk */
                            read(new_socket, tyu, 4);
                            // counter++;

                            if (done >= file_size)
                            {
                                printf("File %s uploaded\n", tokenized_command[i]);
                                char *comp = "comp";
                                send(new_socket, comp, strlen(comp), 0);
                                break;
                            }
                        }
                        // printf("counter=%d\n", counter);
                    }
                }
                sleep(2);
                close(fd);
            }
        }
    }

    return 0;
}
