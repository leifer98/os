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
#include <poll.h>
#include <fcntl.h>

// #define PORT 8080
#define BUFFER_SIZE 1024
#define FILE_SIZE 96000

int client(char *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in6 serv_addr6;
    char buffer[65536] = {0};

    // Open file
    int fd = open("send.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("\nFile opening error \n");
        return -1;
    }

    // Create a socket file descriptor
    if ((sock = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr6, '0', sizeof(serv_addr6));

    serv_addr6.sin6_family = AF_INET6;
    serv_addr6.sin6_port = htons(atoi(argv[3]));

    // Convert IPv4 and IPv6 address6es from text to binary form
    if (inet_pton(AF_INET6, argv[2], &serv_addr6.sin6_addr) <= 0)
    {
        printf("\nInvalid address6/ address6 not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr6, sizeof(serv_addr6)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Send the file to the server
    int read_bytes;
    while ((read_bytes = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (send(sock, buffer, read_bytes, 0) == -1)
        {
            printf("\nError while sending data to server \n");
            close(fd);
            close(sock);
            return -1;
        }
    }
        printf("File sent\n");


    close(fd);
    close(sock);
    return 0;
}


int server(char *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in6 address6;
    int opt = 1;
    int addrlen = sizeof(address6);
    char buffer[65536] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET6, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address6.sin6_family = AF_INET6;
    address6.sin6_addr = in6addr_any;
    address6.sin6_port = htons(atoi(argv[2]));

    // Bind the socket to the specified address6
    if (bind(server_fd, (struct sockaddr *)&address6, sizeof(address6)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept a new incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address6, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Read incoming file data from the client in chunks of 65536 bytes
    printf("Read incoming file\n");

    int total_read = 0;
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    while (total_read < 100662273)
    { // loop until 100 MB of data is read
        valread = read(new_socket, buffer, 65536);
        if (valread < 0)
        {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        total_read += valread;
        printf("Read incoming file %d\n", total_read);
    }
    gettimeofday(&end_time, NULL);

    // Calculate and print the elapsed time
    long int elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 +
                            (end_time.tv_usec - start_time.tv_usec) / 1000;
    printf("IPV4 TCP milliseconds ??? : %ld\n", elapsed_time);

    // Close the file and socket
    close(new_socket);
    close(server_fd);

    printf("File received successfully\n");
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