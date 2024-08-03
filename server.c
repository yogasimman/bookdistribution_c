#include <errno.h>// for system error using perror() function
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "socket.c"

#define PORT 8080 
#define BUFFER_SIZE 1024

//
void serve_content(int newsockfd, const char *uri) {
    char resp[BUFFER_SIZE * 10];

    // Determine response based on the requested URI
    if (strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0) {
        snprintf(resp, sizeof(resp),
                 "HTTP/1.0 200 OK\r\n"
                 "Content-type: text/html\r\n\r\n"
                 "<html><body><h1>Welcome to the Home Page</h1></body></html>");
    } else if (strcmp(uri, "/info") == 0) {
        snprintf(resp, sizeof(resp),
                 "HTTP/1.0 200 OK\r\n"
                 "Content-type: text/html\r\n\r\n"
                 "<html><body><h1>Information Page</h1></body></html>");
    } else if (strcmp(uri, "/contact") == 0) {
        snprintf(resp, sizeof(resp),
                 "HTTP/1.0 200 OK\r\n"
                 "Content-type: text/html\r\n\r\n"
                 "<html><body><h1>Contact Page</h1></body></html>");
    } else {
        snprintf(resp, sizeof(resp),
                 "HTTP/1.0 404 NOT FOUND\r\n"
                 "Content-type: text/html\r\n\r\n"
                 "<html><body><h1>404 Not Found</h1></body></html>");
    }

    // Write the response
    write(newsockfd, resp, strlen(resp));
}

int main(){
    char buffer[BUFFER_SIZE];
     // Create, bind, and listen on the socket
    int sockfd = create_and_bind_socket(PORT);
    if (sockfd == -1){
        return 1; // Exit if socket creation/binding failed
    }

    for(;;){
        //Define client address structure
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);

        //Accepting incoming connections
        int newsockfd = accept(sockfd,NULL,NULL);

        if(newsockfd < 0){
            perror("webserver (accept)");
            continue;
        }
        printf("Connection accepted\n");

        //Read from the sockets
        int valread = read(newsockfd, buffer,BUFFER_SIZE);
        if(valread < 0){
            perror("webserver (read)");
            close(newsockfd);
            continue;
        }


        //Read the request
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];

        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),method,version,uri);

        // Serve content based on the requested URI
        serve_content(newsockfd, uri);
        close(newsockfd);
    }
    
    return 0;
}