#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <thread>

#define PORT "9034"

int create_client_socket() {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo("localhost", PORT, &hints, &res) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        return -1;
    }

    freeaddrinfo(res);

    return sockfd;
}

int main() {
    int sockfd = create_client_socket();
    if (sockfd == -1) {
        std::cerr << "Failed to create client socket\n";
        return 1;
    }

    std::thread receive_thread([sockfd]() {
        char buf[256];

        while (true) {
            ssize_t nbytes = recv(sockfd, buf, sizeof(buf) - 1, 0);
            if (nbytes <= 0) {
                if (nbytes == 0) {
                    std::cout << "Server closed the connection\n";
                } else {
                    perror("recv");
                }

                close(sockfd);
                break;
            }

            buf[nbytes] = '\0';
            std::cout << "Received: " << buf << '\n';
        }
    });

    std::thread send_thread([sockfd]() {
        char buf[256];

        while (true) {
            std::cin.getline(buf, sizeof(buf));

            if (send(sockfd, buf, strlen(buf), 0) == -1) {
                perror("send");
                close(sockfd);
                break;
            }
        }
    });

    receive_thread.join();
    send_thread.join();

    return 0;
}