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
#include "log.h"
#include "json.h" // Include the JSON handling header

#define PORT 8080 
#define BUFFER_SIZE 1024
#define LOG_FILE "server.log"

// Function to serve content based on URI
void serve_content(int newsockfd, const char *uri) {
    if (strstr(uri, ".css") != NULL) {
        serve_static_file(newsockfd, uri, "text/css");
    } else if (strstr(uri, ".js") != NULL) {
        serve_static_file(newsockfd, uri, "application/javascript");
    } else if (strstr(uri, ".png") != NULL) {
        serve_static_file(newsockfd, uri, "image/png");
    } else if (strstr(uri, ".jpg") != NULL) {
        serve_static_file(newsockfd, uri, "image/jpeg");
    } else if (strstr(uri, ".jpeg") != NULL) {
        serve_static_file(newsockfd, uri, "image/jpeg");
    } else if (strstr(uri, ".gif") != NULL) {
        serve_static_file(newsockfd, uri, "image/gif");
    } else {
        char filepath[BUFFER_SIZE];
        struct file_content file;
        char resp[BUFFER_SIZE * 10];

        // Determine response based on the requested URI
        if (strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0) {
            snprintf(filepath, sizeof(filepath), "html/index.html");
        } else if (strcmp(uri, "/login") == 0) {
            snprintf(filepath, sizeof(filepath), "html/login.html");
        } else {
            snprintf(filepath, sizeof(filepath), "html/404.html");
        }

        file = read_file(filepath);
        if (file.content == NULL) {
            snprintf(resp, sizeof(resp),
                "HTTP/1.0 500 INTERNAL SERVER ERROR\r\n"
                "Content-Type: text/html\r\n\r\n"
                "<html><body><h1>500 Internal Server Error</h1></body></html>");
            log_error("Failed to read file");
        } else {
            snprintf(resp, sizeof(resp),
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %zu\r\n\r\n"
                "%s", file.length, file.content);
            free(file.content);
        }

        // Write the response
        write(newsockfd, resp, strlen(resp));
    }
}

// Function to handle login POST request
void handle_login(int newsockfd, const char *body) {
    char resp[BUFFER_SIZE * 10];
    size_t num_pairs;
    struct json_kv *kv_pairs = parse_json(body, &num_pairs);

    if (kv_pairs == NULL) {
        snprintf(resp, sizeof(resp),
            "HTTP/1.0 400 BAD REQUEST\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
        write(newsockfd, resp, strlen(resp));
        return;
    }

    const char *username = get_json_value(kv_pairs, num_pairs, "username");
    const char *password = get_json_value(kv_pairs, num_pairs, "password");

    if (username && password) {
        // For now, just log the username and password
        log_message("INFO", "Login attempt with username: %s, password: %s", username, password);

        // Redirect to the home page on successful login
        snprintf(resp, sizeof(resp),
            "HTTP/1.0 302 FOUND\r\n"
            "Location: /\r\n\r\n");
    } else {
        snprintf(resp, sizeof(resp),
            "HTTP/1.0 400 BAD REQUEST\r\n"
            "Content-Type: application/json\r\n\r\n"
            "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
    }
    free_json_kv(kv_pairs, num_pairs); // Free the JSON key-value pairs

    // Write the response
    write(newsockfd, resp, strlen(resp));
}


int main() {
    char buffer[BUFFER_SIZE];
    // Create, bind, and listen on the socket
    int sockfd = create_and_bind_socket(PORT);
    if (sockfd == -1) {
        log_error("Failed to create and bind socket");
        return 1; // Exit if socket creation/binding failed
    }

    for (;;) {
        // Define client address structure
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);

        // Accepting incoming connections
        int newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);

        if (newsockfd < 0) {
            log_error("webserver (accept)");
            continue;
        }
        log_message("INFO", "Connection accepted");

        // Read from the sockets
        int valread = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (valread < 0) {
            log_error("webserver (read)");
            close(newsockfd);
            continue;
        }
        buffer[valread] = '\0'; // Null-terminate the buffer

        // Read the request
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        log_message("INFO", "[%s:%u] %s %s %s", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);

        // Check if the request is a POST request to /login
        if (strcmp(method, "POST") == 0 && strcmp(uri, "/login") == 0) {
            // Extract the body of the POST request
            char *body = strstr(buffer, "\r\n\r\n");
            if (body != NULL) {
                body += 4; // Move past the "\r\n\r\n"
                handle_login(newsockfd, body);
            } else {
                log_error("Invalid POST request (no body found)");
                char resp[] = "HTTP/1.0 400 BAD REQUEST\r\n\r\n";
                write(newsockfd, resp, sizeof(resp) - 1);
            }
        } else {
            // Serve content based on the requested URI
            serve_content(newsockfd, uri);
        }
        close(newsockfd);
    }

    return 0;
}
