#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <openssl/evp.h>

#define SOCK_PATH "/tmp/uds_socket"
#define BUFFER_SIZE 1024
#define FILE_SIZE 96000

int client(char *argv[])
{
    int sock, read_size, valread;
    char buffer[65536] = {0};
    EVP_MD_CTX *mdctx;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    // Open file
    int fd = open("send.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("\nFile opening error \n");
        return -1;
    }
    // Create socket
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("\n Socket creation error \n");
        return -1;
    }
    struct sockaddr_un serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCK_PATH, sizeof(serverAddress.sun_path) - 1);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("connect");
        return -1;
    }
    // Initialize OpenSSL
    OpenSSL_add_all_digests();
    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);
    // Send the file to the server
    int read_bytes;
    while ((read_bytes = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (EVP_DigestUpdate(mdctx, buffer, read_bytes) != 1)
        {
            perror("EVP_DigestUpdate");
            return -1;
        }
        if (send(sock, buffer, read_bytes, 0) == 1)
        {
            printf("\nError while sending data to server \n");
            close(fd);
            close(sock);
            return -1;
        }
    }
    printf("File sent\n");
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);

    // Send the checksum to the server
    if (send(sock, md_value, md_len, 0) == -1)
    {
        printf("\nError while sending checksum to server \n");
        close(fd);
        close(sock);
        return -1;
    }

    printf("Checksum sent\n");

    close(fd);
    close(sock);
}

int server(char *argv[])
{
    int sock, conn, read_size, valread;
    unlink(SOCK_PATH);
    unsigned char buffer[EVP_MAX_MD_SIZE] = {0};
    // Create socket
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return -1;
    }
    struct sockaddr_un serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, SOCK_PATH, sizeof(serverAddress.sun_path) - 1);

    // Bind
    if (bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("bind");
        return -1;
    }

    // Listen
    if (listen(sock, 5) == -1)
    {
        perror("listen");
        return -1;
    }

    printf("Listening for connections...\n");

    struct sockaddr_un clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);
    memset(&clientAddress, 0, sizeof(clientAddress));
    int addrlen = sizeof(clientAddress);
    // Accept connection
    if ((conn = accept(sock, (struct sockaddr *)&clientAddress, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        return -1;
    }
    // Read incoming file data from the client in chunks of 65536 bytes
    printf("Read incoming file\n");

    int total_read = 0;
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    // Read the incoming checksum from the client
    printf("Read incoming checksum\n");
    while (total_read < 100662273)
    { // loop until 100 MB of data is read
        valread = recv(conn, buffer, EVP_MAX_MD_SIZE, 0);
        if (valread < 0)
        {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        total_read += valread;
        printf("Read incoming file %d\n", total_read);
    }
    printf("File received successfully\n");
    gettimeofday(&end_time, NULL);
     // Print the checksum
    printf("Received checksum: ");
    for (int i = 0; i < valread; i++)
    {
        printf("%02x", buffer[i]);
    }
    printf("\n");

    // Calculate and print the elapsed time
    long int elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 +
                            (end_time.tv_usec - start_time.tv_usec) / 1000;
    printf("uds_stream, %ld\n", elapsed_time);

    // Close the file and socket
    close(conn);
    close(sock);
    printf("Checksum received successfully\n");
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