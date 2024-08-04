#ifndef READ_H
#define READ_H

struct file_content {
    char *content;
    size_t length;
};

struct file_content read_file(const char *path);
void serve_static_file(int newsockfd, const char *uri, const char *content_type);

#endif // READ_H
