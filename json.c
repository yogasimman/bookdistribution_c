#include "json.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Helper function to trim whitespace
static char* trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';

    return str;
}

// Function to parse a JSON string into key-value pairs
struct json_kv* parse_json(const char *json_str, size_t *num_pairs) {
    *num_pairs = 0;

    char *str = strdup(json_str);
    char *token = strtok(str, "{},:\"");

    size_t capacity = 10;
    struct json_kv *kv_pairs = malloc(capacity * sizeof(struct json_kv));

    while (token) {
        if (*num_pairs >= capacity) {
            capacity *= 2;
            kv_pairs = realloc(kv_pairs, capacity * sizeof(struct json_kv));
        }

        kv_pairs[*num_pairs].key = trim_whitespace(strdup(token));
        token = strtok(NULL, "{},:\"");

        if (token) {
            kv_pairs[*num_pairs].value = trim_whitespace(strdup(token));
            token = strtok(NULL, "{},:\"");
        } else {
            kv_pairs[*num_pairs].value = strdup("");
        }

        (*num_pairs)++;
    }

    free(str);
    return kv_pairs;
}

// Function to free the allocated memory for JSON key-value pairs
void free_json_kv(struct json_kv *kv_pairs, size_t num_pairs) {
    for (size_t i = 0; i < num_pairs; i++) {
        free(kv_pairs[i].key);
        free(kv_pairs[i].value);
    }
    free(kv_pairs);
}

// Utility function to get the value for a specific key
const char* get_json_value(const struct json_kv *kv_pairs, size_t num_pairs, const char *key) {
    for (size_t i = 0; i < num_pairs; i++) {
        if (strcmp(kv_pairs[i].key, key) == 0) {
            return kv_pairs[i].value;
        }
    }
    return NULL;
}
