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
#include <sys/mman.h>
#include <openssl/sha.h>

#define SHM_NAME "/my_shm"
#define SIZE 96000
struct stat st;

int client(char *argv[])
{
    // open the shared memory object
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1)
    {
        perror("shm open");
        return -1;
    }

    // Configure the size of the shared memory object
    if (ftruncate(shm_fd, st.st_size) == -1)
    {
        printf("\nShared memory sizing error \n");
        close(shm_fd);
        return -1;
    }

    // memory map the shared memory object
    int *addr = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,  shm_fd, 0);
    if (addr == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }
    // time
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    // Open file
    int fd_send = open("send.txt", O_RDONLY);
    if (fd_send == -1)
    {
        printf("\nFile opening error \n");
        return -1;
    }
    // Get file size
    if (fstat(fd_send, &st) == -1)
    {
        printf("\nFile stat error \n");
        close(fd_send);
        return -1;
    }
    // memory map the shared memory object
    int *ptr = mmap(0, SIZE, PROT_READ, MAP_PRIVATE, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }
    // Read the file and write its content to the shared memory object
    char *buffer = (char *)malloc(st.st_size);
    if (read(fd_send, buffer, st.st_size) != st.st_size)
    {
        perror("File reading error \n");
        free(buffer);
        close(fd_send);
        close(shm_fd);
        return -1;
    }

    memcpy(addr, buffer, st.st_size);

    // Clean up
    free(buffer);
    close(fd_send);
    close(shm_fd);

    return 0;
}
int server(char *argv[])
{
    int fd_recv = open("recv.txt", O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd_recv < 0)
    {
        perror("Error in reading file.");
        exit(1);
    }
    // open the shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm open");
        return -1;
    }

    // Configure the size of the shared memory object
    if (ftruncate(shm_fd, st.st_size) == -1)
    {
        printf("\nShared memory sizing error \n");
        close(shm_fd);
        return -1;
    }

    // memory map the shared memory object
    void *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        return -1;
    }

    // Allocate a buffer to hold the file data
    char *buffer = (char *)malloc(st.st_size);
    if (buffer == NULL)
    {
        perror("malloc");
        close(shm_fd);
        return -1;
    }

    // Copy the data from shared memory to the buffer
    memcpy(buffer, ptr, st.st_size);

    // Print the data
    printf("Read data: %s\n", buffer);
    if (shm_unlink(SHM_NAME) == -1)
    {
        perror("Error unlinking shared memory");
        return -1;
    }

    // Clean up
    free(buffer);
    close(shm_fd);

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