#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#define PORT "9034"     // Port we're listening on
#define MAXDATASIZE 100 // max number of bytes we can get at once

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void)
{
    int listener; // Listening socket descriptor
    int yes = 1;  // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0)
    {
        std::cerr << "selectserver: " << gai_strerror(rv) << std::endl;
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
        {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(listener);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL)
    {
        return -1;
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1)
    {
        return -1;
    }

    return listener;
}

// Add a new file descriptor to the set
void add_to_pfds(std::vector<pollfd> &pfds, int newfd)
{
    pollfd newpfd;
    newpfd.fd = newfd;
    newpfd.events = POLLIN; // Check ready-to-read

    pfds.push_back(newpfd);
}

// Remove an index from the set
void del_from_pfds(std::vector<pollfd> &pfds, int index)
{
    pfds.erase(pfds.begin() + index);
}

// Main
int main(void)
{
    int listener; // Listening socket descriptor

    int newfd;                          // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char buf[MAXDATASIZE]; // Buffer for client data

    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with room for 5 connections
    std::vector<pollfd> pfds(5);
    int fd_count = 0;

    // Set up and get a listening socket
    listener = get_listener_socket();

    if (listener == -1)
    {
        std::cerr << "error getting listening socket" << std::endl;
        exit(1);
    }

    // Add the listener to set
    pfds[0].fd = listener;
    pfds[0].events = POLLIN; // Report ready-to-read on incoming connection
    fd_count++;

    std::cout << "pollserver: waiting for connections..." << std::endl;

    // Main loop
    while (1)
    {
        int poll_count = poll(&pfds[0], fd_count, -1);

        if (poll_count == -1)
        {
            std::cerr << "poll error" << std::endl;
            break;
        }

        for (int i = 0; i < fd_count; i++)
        {
            // Check if someone's got something to say
            if (pfds[i].revents & POLLIN)
            {
                if (pfds[i].revents & POLLHUP)
                {
                    std::cout << "pollserver: socket " << pfds[i].fd << " hung up" << std::endl;
                    close(pfds[i].fd);
                    del_from_pfds(pfds, i);
                    fd_count--;
                    i--;
                    continue;
                }

                if (pfds[i].fd == listener)
                {
                    // New connection
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1)
                    {
                        std::cerr << "accept error" << std::endl;
                    }
                    else
                    {
                        // Add to the pollfd structure
                        add_to_pfds(pfds, newfd);
                        std::cout << "pollserver: new connection from "
                                  << inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN)
                                  << " on socket " << newfd << std::endl;
                    }
                }
                else
                {
                    // Handle data from a client
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    if (nbytes <= 0)
                    {
                        // Got error or connection closed by client
                        if (nbytes == 0)
                        {
                            // Connection closed
                            std::cout << "pollserver: socket " << pfds[i].fd << " hung up" << std::endl;
                        }
                        else
                        {
                            std::cerr << "recv error" << std::endl;
                        }

                        // Close the connection
                        close(pfds[i].fd);
                        del_from_pfds(pfds, i);
                        fd_count--;
                        i--; // Decrement i to correctly process the next socket in the pfds vector
                    }
                    else
                    {
                        // We got some data from a client
                        buf[nbytes - 1] = '\0';
                        std::cout << "pollserver: received message from socket " << pfds[i].fd << ": " << buf << std::endl;
                        send(pfds[i].fd, buf, nbytes, 0);
                    }
                }
            }
        }
    }

    // Close all connections
    for (int i = 0; i < fd_count; i++)
    {
        close(pfds[i].fd);
    }

    return 0;
}
