#include <errno.h>// for system error using perror() function
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#define LOG_FILE "server.log"

void log_message(const char *format, ...) {
    FILE *log_file = fopen(LOG_FILE, "a"); // Open log file in append mode
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }

    // Get the current time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    // Write the timestamp and the message to the log file
    fprintf(log_file, "%s: ", timestamp);

    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args); // Use vfprintf to handle the variable arguments
    va_end(args);

    fprintf(log_file, "\n"); // Newline at the end of the log entry
    fclose(log_file);
}