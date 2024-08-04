#include <errno.h>// for system error using perror() function
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080 
#define BUFFER_SIZE 1024

int main(){
    char buffer[BUFFER_SIZE];
    char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>hello, world <html>\r\n";

    //creating socket file descriptor
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1){
        perror("webserver (socket)");
        return 1;
    }
    printf("socket created successfully\n");

    //creating the address to bind the socket to 
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Creating for client address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    //bind the socket to the address
    if(bind(sockfd, (struct sockaddr*)&host_addr, host_addrlen) != 0) {
        perror("webserver (bind)");
        return 1;
    }
    printf("socket sucessfully bound to address\n");

    //listen for incoming connections
    if(listen(sockfd,SOMAXCONN) != 0){
        //SOMAXCONN is constant defined by our system
        //used to set backlog argument
        perror("webserver (listen)");
        return 1;
    }
    printf("server listening for connections\n");

    for(;;){
        //Accepting incoming connections
        int newsockfd = accept(sockfd,(struct sockaddr *)&host_addr,(socklen_t *)&host_addrlen);

        if(newsockfd < 0){
            perror("webserver (accept)");
            continue;
        }
        printf("connection accepted\n");

        //Get client address
        int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr,(socklen_t *)&client_addrlen);

        if(sockn < 0){
            perror("webserver (getsocketname)");
            continue;
        }
        //Read from the socket
        int valread = read(newsockfd,buffer,BUFFER_SIZE);
        if(valread < 0){
            perror("webserver (read)");
            continue;
        }

        // Read the request
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s",method,uri,version);
        printf("[%s:%u] %s %s %s\n",inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port),method,version,uri);

        // Write to the socket
        int valwrite = write(newsockfd, resp,strlen(resp));
        if(valread < 0){
            perror("webserver (write)");
            continue;
        }
        close(newsockfd);
    }
    
    return 0;
}