#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "socket.h"
#include "read.h"
#define PORT 8080 
#define BUFFER_SIZE 1024

// Function to serve content based on URI
void serve_content(int newsockfd, const char *uri) {
    if (strstr(uri, ".css") != NULL) {
        serve_static_file(newsockfd, uri, "text/css");
    } else if (strstr(uri, ".js") != NULL) {
        serve_static_file(newsockfd, uri, "application/javascript");
    } else {
        char filepath[BUFFER_SIZE];
        char *content;
        char resp[BUFFER_SIZE * 10];

        // Determine response based on the requested URI
        if (strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0) {
            snprintf(filepath, sizeof(filepath), "html/index.html");
        } else {
            snprintf(filepath, sizeof(filepath), "html/404.html");
        }

        content = read_file(filepath);
        if (content == NULL) {
            snprintf(resp, sizeof(resp),
                "HTTP/1.0 500 INTERNAL SERVER ERROR\r\n"
                "Content-Type: text/html\r\n\r\n"
                "<html><body><h1>500 Internal Server Error</h1></body></html>");
        } else {
            snprintf(resp, sizeof(resp),
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: text/html\r\n\r\n"
                "%s", content); // Corrected here
            free(content);
        }

        // Write the response
        write(newsockfd, resp, strlen(resp));
    }
}

int main() {
    char buffer[BUFFER_SIZE];
    // Create, bind, and listen on the socket
    int sockfd = create_and_bind_socket(PORT);
    if (sockfd == -1) {
        return 1; // Exit if socket creation/binding failed
    }

    for (;;) {
        // Define client address structure
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);

        // Accepting incoming connections
        int newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);

        if (newsockfd < 0) {
            perror("webserver (accept)");
            continue;
        }
        printf("Connection accepted\n");

        // Read from the sockets
        int valread = read(newsockfd, buffer, BUFFER_SIZE);
        if (valread < 0) {
            perror("webserver (read)");
            close(newsockfd);
            continue;
        }

        // Read the request
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

        // Serve content based on the requested URI
        serve_content(newsockfd, uri);
        close(newsockfd);
    }
    
    return 0;
}
