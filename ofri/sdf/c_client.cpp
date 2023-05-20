/*
** client.cpp -- a stream socket client demo
*/

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT "9034"      // the port client will be connecting to
#define MAXDATASIZE 100  // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        std::cerr << "usage: client hostname" << std::endl;
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != nullptr; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == nullptr)
    {
        std::cerr << "client: failed to connect" << std::endl;
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    std::cout << "client: connecting to " << s << std::endl;

    freeaddrinfo(servinfo); // all done with this structure

    struct pollfd pfds[2];
    pfds[0].fd = 0;      // Stdin
    pfds[0].events = POLLIN;
    pfds[1].fd = sockfd;
    pfds[1].events = POLLIN;

    for (;;)
    {
        poll(pfds, 2, -1);

        if (pfds[0].revents & POLLIN)
        {
            numbytes = read(0, buf, MAXDATASIZE - 1);
            buf[numbytes - 1] = '\0'; // Remove newline character from input

            send(sockfd, buf, numbytes, 0);
            std::cout << "Message sent: " << buf << std::endl;
        }
        else
        {
            numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
            if (numbytes <= 0)
            {
                if (numbytes == 0)
                {
                    // Connection closed
                    std::cout << "Server hung up" << std::endl;
                }
                else
                {
                    perror("recv");
                }
                break;
            }
            buf[numbytes - 1] = '\0';
            std::cout << buf << std::endl;
            // std::cout << "Received message from server: " << buf << std::endl;
        }
    }

    close(sockfd);

    return 0;
}
