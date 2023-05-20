#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define BUFFER_SIZE 8192
#define MAX_EVENTS 2

int client(char *argv[])
{
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Could not create socket: %d\n", errno);
        return -1;
    }

    struct sockaddr_un serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, argv[2], sizeof(serverAddress.sun_path) - 1);

    int connectResult = connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectResult == -1)
    {
        printf("connect() failed with error code: %d\n", errno);
        close(sock);
        return -1;
    }

    printf("Connected to server\n");

    struct pollfd fds[MAX_EVENTS];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = sock;
    fds[1].events = POLLIN;

    char buffer[1024];
    int keepRunning = 1;
    int timeoutMs = -1; // wait indefinitely
    while (keepRunning)
    {
        int pollResult = poll(fds, MAX_EVENTS, timeoutMs);
        if (pollResult == -1)
        {
            printf("poll() failed with error code: %d\n", errno);
            break;
        }

        // Check for events on stdin
        if (fds[0].revents & POLLIN)
        {
            fgets(buffer, sizeof(buffer), stdin);
            int sendResult = send(sock, buffer, strlen(buffer), 0);
            if (sendResult == -1)
            {
                printf("send() failed with error code: %d\n", errno);
                break;
            }
        }

        // Check for events on the socket
        if (fds[1].revents & POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            int recvResult = recv(sock, buffer, sizeof(buffer), 0);
            if (recvResult <= 0)
            {
                printf("Connection closed by server\n");
                keepRunning = 0;
            }
            else
            {
                printf("%s", buffer);
            }
        }
    }

    close(sock);
    return 0;
}

int server(char *argv[])
{
    int listeningSocket = -1;
    listeningSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listeningSocket == -1)
    {
        printf("Could not create listening socket: %d\n", errno);
        return 1;
    }

    struct sockaddr_un serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sun_family = AF_UNIX;

    printf("%s\n", argv[2]);

    strncpy(serverAddress.sun_path, argv[2], sizeof(serverAddress.sun_path) - 1);

    // Check if the socket file already exists and remove it
    if (access(serverAddress.sun_path, F_OK) == 0)
    {
        if (unlink(serverAddress.sun_path) == -1)
        {
            printf("Could not remove existing socket file: %d\n", errno);
            close(listeningSocket);
            return 1;
        }
    }

    int optval = 1;
    setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    int bindResult = bind(listeningSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1)
    {
        printf("Bind failed with error code: %d\n", errno);
        close(listeningSocket);
        return 1;
    }

    int listenResult = listen(listeningSocket, 3);
    if (listenResult == -1)
    {
        printf("listen() failed with error code: %d\n", errno);
        close(listeningSocket);
        return 1;
    }

    printf("Waiting for incoming UDS connections...\n");

    // Accept incoming connections
    struct sockaddr_un clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddressLen = sizeof(clientAddress);
    int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (clientSocket == -1)
    {
        printf("listen failed with error code: %d\n", errno);
        close(listeningSocket);
        return 1;
    }

    printf("A new client connection accepted\n");

    char buffer[1024];
    struct pollfd fds[2];
    fds[0].fd = clientSocket;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    while (1)
    {
        int pollResult = poll(fds, 2, -1);
        if (pollResult == -1)
        {
            printf("poll() failed with error code: %d\n", errno);
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            int recvResult = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (recvResult <= 0)
            {
                printf("Connection closed by client\n");
                break;
            }

            printf("%s", buffer);
        }

        if (fds[1].revents & POLLIN)
        {
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer), stdin);
            int sendResult = send(clientSocket, buffer, strlen(buffer), 0);
            if (sendResult == -1)
            {
                printf("send() failed with error code: %d\n", errno);
                break;
            }
        }
    }

    close(clientSocket);
    close(listeningSocket);
    return 0;
}

int main(int count, char *argv[])
{
    if (count <= 2 || count > 4)
    {
        printf("USAGE: stnc [-c IP PORT | -s PORT]\n"
               "    -c IP PORT    Connect to a server at IP and PORT\n"
               "    -s PORT       Listen for incoming connections on PORT\n");

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