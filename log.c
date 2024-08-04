#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define LOG_FILE "server.log"

// Function to get current timestamp
void get_timestamp(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", t);
}

// Logging function with log level
void log_message(const char *level, const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Prepare the timestamp
    char timestamp[BUFFER_SIZE];
    get_timestamp(timestamp, sizeof(timestamp));

    // Prepare the message
    char message[BUFFER_SIZE * 2];
    vsnprintf(message, sizeof(message), format, args);
    
    // Print to console
    printf("%s [%s]: %s\n", timestamp, level, message);
    
    // Write to log file
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        fprintf(log_file, "%s [%s]: %s\n", timestamp, level, message);
        fclose(log_file);
    } else {
        perror("Unable to open log file");
    }

    va_end(args);
}

// Error logging function with perror
void log_error(const char *message) {
    char error_message[BUFFER_SIZE];
    snprintf(error_message, sizeof(error_message), "%s: %s", message, strerror(errno));
    perror(message);
    log_message("ERROR", error_message);
}
