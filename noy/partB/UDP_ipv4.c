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

// #define PORT 8080
#define BUFFER_SIZE 1024
#define FILE_SIZE 96000

int client(char *argv[])
{
    int sock = 0, valread;
    char buffer[65507] = {0};

    // Open file
    int fd = open("send.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("\nFile opening error \n");
        return -1;
    }

    // Create a socket file descriptor
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[3]));

    // Convert IPv4 and IPv6 serverAddresses from text to binary form
    if (inet_pton(AF_INET, argv[2], &serverAddress.sin_addr) <= 0)
    {
        printf("\nInvalid serverAddress/ serverAddress not supported \n");
        return -1;
    }
    printf("Ready to communicate with server\n");

    // Send the file to the server
    int read_bytes;
    while ((read_bytes = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (sendto(sock, buffer, read_bytes, 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            perror("\nError while sending data to server ");;
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
    int server_fd, valread;
    int opt = 1;
    char buffer[65507] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == 0)
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
    struct sockaddr_in serverAddress;
    // int addrlen = sizeof(serverAddress);
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(argv[2]));

    // Bind the socket to the specified serverAddress
    if (bind(server_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset(&clientAddress, 0, sizeof(clientAddress));
    // Read incoming file data from the client in chunks of 65536 bytes
    printf("Read incoming file\n");

    int total_read = 0;
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    while (total_read < 100662273)
    { // loop until 100 MB of data is read
        valread = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &clientAddressLen);
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
    printf("IPV4 UDP milliseconds ??? : %ld\n", elapsed_time);

    // Close the file and socket
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