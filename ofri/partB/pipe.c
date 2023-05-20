#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <openssl/evp.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define FIFO_NAME "new.txt"
#define BUFFER_SIZE 1024

#define BUFFER_SIZE 1024
#define FILE_SIZE 96000

int client(char *argv[]) {
    char message[100];
    int fifo_fd;

    // Open the FIFO in write-only mode
    fifo_fd = open(FIFO_NAME, O_WRONLY);

    // Get the message from the user
    printf("Enter a message to send to the server: ");
    fgets(message, sizeof(message), stdin);

    // Write the message to the FIFO
    write(fifo_fd, message, strlen(message)+1);

    // Close the FIFO
    close(fifo_fd);

    return 0;

}


int server(char *argv[])
{
    char message[100];
    int fifo_fd;

    // Create the FIFO (named pipe) if it doesn't exist
    mkfifo(FIFO_NAME, 0666);

    // Open the FIFO in read-only mode
    fifo_fd = open(FIFO_NAME, O_RDONLY);

    // Read the message from the client
    read(fifo_fd, message, sizeof(message));

    // Print the message received from the client
    printf("Server received message: %s\n", message);

    // Close the FIFO
    close(fifo_fd);
    unlink(FIFO_NAME);

    return 0;
}

int main(int count, char *argv[])
{
    if (count < 5 || count > 7)
    {
        printf("USAGE: stnc -c|-s IP PORT TYPE PROTOCOL\n"
               "CLIENT:    -c <IP> <PORT> -p <TYPE> <PROTOCOL> Connect to a server at IP and PORT using PROTOCOL and TYPE\n"
               "SERVER:    -s <PORT> -p -q  Listen for incoming connections on PORT using PROTOCOL and TYPE\n");

        exit(0);
    }

    if (strcmp(argv[1], "-s") == 0)
    {
        server(argv);
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
        client(argv);
    }

    return 0;
}