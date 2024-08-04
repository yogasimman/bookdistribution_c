#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "read.h" // Include the header file

#define BUFFER_SIZE 1024

// Function to read file contents
char* read_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("webserver (fopen)");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = malloc(length + 1);
    if (content == NULL) {
        perror("webserver (malloc)");
        fclose(file);
        return NULL;
    }

    fread(content, 1, length, file);
    content[length] = '\0';

    fclose(file);
    return content;
}

void serve_static_file(int newsockfd, const char *uri, const char *content_type) {
    char filepath[BUFFER_SIZE];
    char *content;
    char resp[BUFFER_SIZE * 10];

    snprintf(filepath, sizeof(filepath), "html%s", uri);

    content = read_file(filepath);
    if (content == NULL) {
        snprintf(resp, sizeof(resp),
                "HTTP/1.0 404 NOT FOUND\r\n"
                "Content-Type: text/html\r\n\r\n"
                "<html><body><h1>404 Not Found</h1></body></html>");
    } else {
        snprintf(resp, sizeof(resp),
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: %s\r\n\r\n"
                "%s", content_type, content); // Corrected here
        free(content);
    }
    write(newsockfd, resp, strlen(resp));
}
