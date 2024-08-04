#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "read.h" // Include the header file
#include "log.h"

#define BUFFER_SIZE 1024

// Function to read file contents
struct file_content read_file(const char *path) {
    struct file_content result = {NULL, 0}; // Initialize the struct

    FILE *file = fopen(path, "rb");  // Open in binary mode to handle images
    if (file == NULL) {
        log_error("webserver (fopen)");
        return result;
    }

    fseek(file, 0, SEEK_END);
    result.length = ftell(file);
    fseek(file, 0, SEEK_SET);

    result.content = malloc(result.length);
    if (result.content == NULL) {
        log_error("webserver (malloc)");
        fclose(file);
        return result;
    }

    fread(result.content, 1, result.length, file);
    fclose(file);
    return result;
}

void serve_static_file(int newsockfd, const char *uri, const char *content_type) {
    char filepath[BUFFER_SIZE];
    struct file_content file;
    char resp[BUFFER_SIZE * 10];

    snprintf(filepath, sizeof(filepath), "html%s", uri);

    file = read_file(filepath);
    if (file.content == NULL) {
        snprintf(resp, sizeof(resp),
                "HTTP/1.0 404 NOT FOUND\r\n"
                "Content-Type: text/html\r\n\r\n"
                "<html><body><h1>404 Not Found</h1></body></html>");
        log_error("Failed to read static file");
    } else {
        snprintf(resp, sizeof(resp),
                "HTTP/1.0 200 OK\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %zu\r\n\r\n", content_type, file.length);
        write(newsockfd, resp, strlen(resp));
        write(newsockfd, file.content, file.length);
        free(file.content);
    }
}
