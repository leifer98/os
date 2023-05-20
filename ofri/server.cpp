#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "reactor.cpp"

#define PORT "9034"

int create_server_socket()
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0)
    {
        perror("getaddrinfo");
        return -1;
    }

    int listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listener == -1)
    {
        perror("socket");
        return -1;
    }

    int yes = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        return -1;
    }

    if (bind(listener, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("bind");
        return -1;
    }

    freeaddrinfo(res);

    if (listen(listener, 10) == -1)
    {
        perror("listen");
        return -1;
    }

    return listener;
}

int main()
{
    // Create the reactor.
    Reactor reactor;
    reactor.createReactor();

    // Create the server socket.
    int server_fd = create_server_socket();
    if (server_fd == -1)
    {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    // Add the server socket to the reactor.
    reactor.addFD(server_fd, [server_fd, &reactor]() {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd == -1)
        {
            perror("accept");
            return;
        }

        // Add the client socket to the reactor.
        reactor.addFD(client_fd, [client_fd, &reactor, server_fd]() {
            char buf[256];
            ssize_t nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);

            if (nbytes <= 0)
            {
                if (nbytes == 0)
                {
                    std::cout << "socket " << client_fd << " hung up\n";
                }
                else
                {
                    perror("recv");
                }

                close(client_fd);
                // Here you'd also want to remove the fd from the reactor.
                return;
            }

            buf[nbytes] = '\0';
            std::cout << buf << '\n';

            // Send the received message to all clients except the originating client.
            for (const auto &p : reactor.getFDHandlers())
            {
                int other_client_fd = p.first;
                if (other_client_fd != server_fd && other_client_fd != client_fd)
                {
                    if (send(other_client_fd, buf, nbytes, 0) == -1)
                    {
                        perror("send");
                        close(other_client_fd);
                        // Here you'd also want to remove the fd from the reactor.
                    }
                }
            }
        });
    });

    // Start the reactor.
    reactor.startReactor();

    // Do other stuff here (or just sleep, as we do here).
    // The reactor is running in a separate thread and will
    // handle incoming connections and data from clients.
    while (true)
    {
        sleep(1);
    }

    return 0;
}
