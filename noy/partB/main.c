#include <stdio.h>
// Linux and other UNIXes
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "partA.c"
#include "mmap.c"
#include "pipe.c"
#include "TCP_ipv4.c"
#include "TCP_ipv6.c"
#include "UDP_ipv4.c"
#include "UDP_ipv6.c"
#include "UDS_dgram.c"
#include "UDS_stream.c"
#include "createFile.c"

#define BUFFER_SIZE 8192
#define PORT 8080
#define LOCALHOST "127.0.0.1"

int client(char *argv[])
{
    char *ip = argv[2];
    char *port = argv[3];
    char *type = argv[5];
    char *param = argv[6];
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    // Create a socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, LOCALHOST, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Make message to server with arguments
    strcpy(buffer, ip);
    strcat(buffer, " ");
    strcat(buffer, port);
    strcat(buffer, " ");
    strcat(buffer, type);
    strcat(buffer, " ");
    strcat(buffer, param);

    // send to server
    if (send(sock, buffer, strlen(buffer), 0) < 0)
    {
        perror("send");
        return -1;
    }

    close(sock);
    return 0;
}

int server(char *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[65536] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
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
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(LOCALHOST));

    // Bind the socket to the specified address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
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
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    if (recv(new_socket, buffer, BUFFER_SIZE - 1, 0) < 0)
    {
        perror("recv");
        return -1;
    }
    char *ip = strtok(buffer, " ");
    char *port = strtok(NULL, " ");
    char *type = strtok(NULL, " ");
    char *param = strtok(NULL, " ");

    if (strcmp(type, "ipv4") == 0)
    {
        if (strcmp(param, "tcp") == 0)
        {
            printf("You have chosen %s with %s.\n", type, param);
            tcp4_server(argv);
            sleep(2);
            tcp4_client(argv);
        }
        else if (strcmp(param, "udp") == 0)
        {
            printf("You have chosen %s with %s.\n", type, param);
            udp4_server(argv);
            sleep(2);
            udp4_client(argv);
        }
    }
    else if (strcmp(type, "ipv6") == 0)
    {
        if (strcmp(param, "tcp") == 0)
        {
            printf("You have chosen %s with %s.\n", type, param);
            tcp6_server(argv);
            sleep(2);
            tcp6_client(argv);
        }
        else if (strcmp(param, "udp") == 0)
        {
            printf("You have chosen %s with %s.\n", type, param);
            udp6_server(argv);
            sleep(2);
            udp6_client(argv);
        }
    }
    else if (strcmp(type, "uds") == 0)
    {
        if (strcmp(param, "stream") == 0)
        {
            printf("You have chosen %s with %s.\n", type, param);
            udss_server(argv);
            sleep(2);
            udss_client(argv);
        }
        else if (strcmp(param, "dgram") == 0)
        {
            printf("You have chosen %s with %s.\n", type, param);
            udsd_server(argv);
            sleep(2);
            udsd_client(argv);
        }
    }
    else if (strcmp(type, "mmap") == 0)
    {
        printf("You have chosen %s with file name: %s.\n", type, param);
        mmap_server(argv);
        sleep(2);
        mmap_client(argv);
    }
    else if (strcmp(type, "pipe") == 0)
    {
        printf("You have chosen %s with file name: %s.\n", type, param);
        pipe_server(argv);
        sleep(2);
        pipe_client(argv);
    }
    else
    {
        printf("Incorrect type. Please choose from ipv4, ipv6, mmap, pipe, uds.\n");
        return 1;
    }

    // Close the file and socket
    close(new_socket);
    close(server_fd);

    return 0;
}

int main(int count, char *argv[])
{
    if (count <= 2)
    {
        printf("USAGE: stnc [-c IP PORT | -s PORT]\n"
               "    -c IP PORT    Connect to a server at IP and PORT\n"
               "    -s PORT       Listen for incoming connections on PORT\n");

        exit(0);
    }
    if (strcmp(argv[1], "-s") == 0)
    {
        if (count == 5)
        {
            server(argv);
        }
        else
        {
            serverA(argv);
        }
    }
    if (strcmp(argv[1], "-c") == 0)
    {
        if (count == 7)
        {
            client(argv);
        }
        else
        {
            clientA(argv);
        }
    }
    else
    {
        printf("Not a valid command");
    }
    return 0;
}

// int main(int count, char *argv[])
// {
//     if (count < 5 || count > 7)
//     {
//         printf("USAGE: stnc -c|-s IP PORT\n"
//                "CLIENT:    -c <IP> <PORT> -p <TYPE> <PROTOCOL>\n"
//                "SERVER:    -s <PORT> -p -q \n");

//         exit(0);
//     }

//     if (strcmp(argv[1], "-s") == 0)
//     {
//         server(argv);
//     }
//     else if (strcmp(argv[1], "-c") == 0)
//     {
//         char *type = argv[5];
//         char *param = argv[6];
//         if (strcmp(type, "ipv4") == 0)
//         {
//             if (strcmp(param, "tcp") == 0)
//             {
//                 printf("You have chosen %s with %s.\n", type, param);
//                 tcp4_client(argv);
//             }
//             else if (strcmp(param, "udp") == 0)
//             {
//                 printf("You have chosen %s with %s.\n", type, param);
//                 udp4_client(argv);
//             }
//         }
//         else if (strcmp(type, "ipv6") == 0)
//         {
//             if (strcmp(param, "tcp") == 0)
//             {
//                 printf("You have chosen %s with %s.\n", type, param);
//                 tcp6_client(argv);
//             }
//             else if (strcmp(param, "udp") == 0)
//             {
//                 printf("You have chosen %s with %s.\n", type, param);
//                 udp6_client(argv);
//             }
//         }
//         else if (strcmp(type, "uds") == 0)
//         {
//             if (strcmp(param, "stream") == 0)
//             {
//                 printf("You have chosen %s with %s.\n", type, param);
//                 udss_client(argv);
//             }
//             else if (strcmp(param, "dgram") == 0)
//             {
//                 printf("You have chosen %s with %s.\n", type, param);
//                 udsd_client(argv);
//             }
//         }
//         else if (strcmp(type, "mmap") == 0)
//         {
//             printf("You have chosen %s with file name: %s.\n", type, param);
//             mmap_client(argv);
//         }
//         else if (strcmp(type, "pipe") == 0)
//         {
//             printf("You have chosen %s with file name: %s.\n", type, param);
//             pipe_client(argv);
//         }
//         else
//         {
//             printf("Incorrect type. Please choose from ipv4, ipv6, mmap, pipe, uds.\n");
//             return 1;
//         }
//     }
//     return 0;
// }
