#ifndef JSON_H
#define JSON_H

#include <stddef.h>

// Structure to hold JSON key-value pairs
struct json_kv {
    char *key;
    char *value;
};

// Function to parse a JSON string into key-value pairs
struct json_kv* parse_json(const char *json_str, size_t *num_pairs);

// Function to free the allocated memory for JSON key-value pairs
void free_json_kv(struct json_kv *kv_pairs, size_t num_pairs);

// Utility function to get the value for a specific key
const char* get_json_value(const struct json_kv *kv_pairs, size_t num_pairs, const char *key);

#endif
