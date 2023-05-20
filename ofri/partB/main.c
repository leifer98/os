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
int server(char *argv[]) {}
int client(char *argv[]) {}
int main(int count, char *argv[])
{
    if (count < 5 || count > 7)
    {
        printf("USAGE: stnc -c|-s IP PORT\n"
               "CLIENT:    -c <IP> <PORT> -p <TYPE> <PROTOCOL>\n"
               "SERVER:    -s <PORT> -p -q \n");

        exit(0);
    }

    if (strcmp(argv[1], "-s") == 0)
    {
        server(argv);
    }
    else if (strcmp(argv[1], "-c") == 0)
    {
        char *type = argv[5];
        char *param = argv[6];
        if (strcmp(type, "ipv4") == 0)
        {
            if (strcmp(param, "tcp") == 0)
            {
                printf("You have chosen %s with %s.\n", type, param);
                tcp4_client(argv);
            }
            else if (strcmp(param, "udp") == 0)
            {
                printf("You have chosen %s with %s.\n", type, param);
                udp4_client(argv);
            }
        }
        else if (strcmp(type, "ipv6") == 0)
        {
            if (strcmp(param, "tcp") == 0)
            {
                printf("You have chosen %s with %s.\n", type, param);
                tcp6_client(argv);
            }
            else if (strcmp(param, "udp") == 0)
            {
                printf("You have chosen %s with %s.\n", type, param);
                udp6_client(argv);
            }
        }
        else if (strcmp(type, "uds") == 0)
        {
            if (strcmp(param, "stream") == 0)
            {
                printf("You have chosen %s with %s.\n", type, param);
                udss_client(argv);
            }
            else if (strcmp(param, "dgram") == 0)
            {
                printf("You have chosen %s with %s.\n", type, param);
                udsd_client(argv);
            }
        }
        else if (strcmp(type, "mmap") == 0)
        {
            printf("You have chosen %s with file name: %s.\n", type, param);
            mmap_client(argv);
        }
        else if (strcmp(type, "pipe") == 0)
        {
            printf("You have chosen %s with file name: %s.\n", type, param);
            pipe_client(argv);
        }
        else
        {
            printf("Incorrect type. Please choose from ipv4, ipv6, mmap, pipe, uds.\n");
            return 1;
        }
    }
    return 0;
}
