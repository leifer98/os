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

#define BUFFER_SIZE 63200
#define FILE_SIZE 96000

int client()
{
    int fd = shm_open("new", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("shm_open");
        exit(1);
    }

    struct stat st;
    if (stat("send.txt", &st) == -1)
    {
        perror("stat");
        exit(1);
    }

    if (ftruncate(fd, st.st_size) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    char *shared_msg = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_msg == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    FILE *file = fopen("send.txt", "r");
    if (file == NULL)
    {
        perror("fopen");
        exit(1);
    }

    size_t nread;
    while ((nread = fread(shared_msg, 1, BUFFER_SIZE, file)) > 0)
    {
        shared_msg += nread;
    }

    if (munmap(shared_msg - st.st_size, st.st_size) == -1)
    {
        perror("munmap");
        exit(1);
    }

    return 0;
}

int server(char *argv[])
{
    int fd = shm_open("new", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("shm_open");
        exit(1);
    }

    struct stat st;
    if (stat("send.txt", &st) == -1)
    {
        perror("stat");
        exit(1);
    }

    if (ftruncate(fd, st.st_size) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    char *shared_msg = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_msg == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    size_t nread = 0;
    while (nread < st.st_size)
    {
        size_t bytes_to_read = st.st_size - nread;
        if (bytes_to_read > BUFFER_SIZE)
        {
            bytes_to_read = BUFFER_SIZE;
        }

        printf("Server received %zu bytes\n", bytes_to_read);
        fwrite(shared_msg, 1, bytes_to_read, stdout);
        shared_msg += bytes_to_read;
        nread += bytes_to_read;
    }

    if (munmap(shared_msg - st.st_size, st.st_size) == -1)
    {
        perror("munmap");
        exit(1);
    }

    if (shm_unlink("new") == -1)
    {
        perror("shm_unlink");
        exit(1);
    }

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