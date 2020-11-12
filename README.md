## Disclaimer:

1. Please do not run server and client in the same directory as the copied file has also the same name as the original.
We will be reading and writing in the same file which can lead to error or stop the system (Reader-Writer's problem).

2. Please do not ctrlC or ctrlZ in client program when both server and client are running. It will make server go into and infinite loop and result in system hang.

## Instructions to run:

- Two terminal windows are needed.
- Run the following command in terminal windows:

    ```bash
    gcc server.c -o server
    ./server

    gcc client.c -o client
    ./client
    ```

## Features Implemented
All features are implemented in client program.

1. `get <filename1> <...>`  
It will copy the files residing in server's directory with filename1, ... (if exist) into client's directory. It will give `Invalid file` error if file with given filename doesn't exist.

2. `exit`   
Closes the connection between server and client.