#include <errno.h>// for system error using perror() function
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// Function to create, bind, and listen on a socket
int create_and_bind_socket(int port) {
    // Creating socket file descriptor
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("webserver (socket)");
        return -1;
    }
    printf("Socket created successfully\n");

    // Creating the address to bind the socket to 
    struct sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(port);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) != 0) {
        perror("webserver (bind)");
        close(sockfd);
        return -1;
    }
    printf("Socket successfully bound to address\n");

    // Listen for incoming connections
    if (listen(sockfd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        close(sockfd);
        return -1;
    }
    printf("Server listening for connections\n");

    return sockfd; // Return the socket file descriptor
}