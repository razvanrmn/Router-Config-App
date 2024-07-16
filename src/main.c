#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include "protobuf_handler.h"
#include "wss_handler.h"
#include "usp-msg-1-3.pb-c.h"

#define MAX_BUFFER_SIZE 256

void parse_query_string(char *query, char *obj_path, char *param, char *value, int *required) {
    sscanf(query,
    "obj_path=%[^&]&param=%[^&]&value=%[^&]&required=%d", obj_path, param, value, required);
}

int main(int argc, char *argv[])
{
    CURL *curl = NULL;
    uint8_t *send_payload_buffer = NULL;
    size_t send_payload_size = 0;
    char obj_path[MAX_BUFFER_SIZE] = {0};
    char param[MAX_BUFFER_SIZE] = {0};
    char value[MAX_BUFFER_SIZE] = {0};
    int required = 0;

    printf("Content-Type: text/plain\n\n");

    char *method = getenv("REQUEST_METHOD");
    if (method && strcmp(method, "GET") == 0) {
        char *query = getenv("QUERY_STRING");
        if (query) {
            parse_query_string(query, obj_path, param, value, &required);
        }
    } else if (method && strcmp(method, "POST") == 0) {
        int content_length = atoi(getenv("CONTENT_LENGTH"));
        if (content_length > 0) {
            char *post_data = malloc(content_length + 1);
            fread(post_data, 1, content_length, stdin);
            post_data[content_length] = '\0';
            parse_query_string(post_data, obj_path, param, value, &required);
            free(post_data);
        }
    } else {
        fprintf(stderr, "Unsupported request method\n");
        return 1;
    }

    if (create_protobuf_message(
        &send_payload_buffer, &send_payload_size, obj_path, param, value, required)) {
        printf("Error creating protobuf message\n");
        goto err_create_proto_msg;
    }

    init_curl(&curl);
    if (curl) {
        perform_ws_operations(curl, send_payload_buffer, send_payload_size);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    free(send_payload_buffer);
    return 0;

err_create_proto_msg:
    free(send_payload_buffer);
    return 1;
}