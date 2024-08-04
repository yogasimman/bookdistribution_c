#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "socket.h"
#include "log.h"

// Function to create, bind, and listen on a socket
int create_and_bind_socket(int port) {
    // Creating socket file descriptor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        log_error("webserver (socket)");
        return -1;
    }
    log_message("INFO", "Socket created successfully");

    // Set socket options to allow address reuse
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        log_error("webserver (setsockopt)");
        close(sockfd);
        return -1;
    }

    // Creating the address to bind the socket to 
    struct sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(port);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) != 0) {
        log_error("webserver (bind)");
        close(sockfd);
        return -1;
    }
    log_message("INFO", "Socket successfully bound to address");

    // Listen for incoming connections
    if (listen(sockfd, SOMAXCONN) != 0) {
        log_error("webserver (listen)");
        close(sockfd);
        return -1;
    }
    log_message("INFO", "Server listening for connections");

    return sockfd; // Return the socket file descriptor
}
