#ifndef READ_H
#define READ_H

char* read_file(const char *path);
void serve_static_file(int newsockfd, const char *uri, const char *content_type);

#endif // READ_H
