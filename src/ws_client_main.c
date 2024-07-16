#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "protobuf_handler.h"
#include "wss_handler.h"
#include "usp-msg-1-3.pb-c.h"

#define MAX_BUFFER_SIZE 256

CURL *curl = NULL;
uint8_t *send_payload_buffer = NULL;
size_t send_payload_size = 0;
char obj_path[MAX_BUFFER_SIZE] = {0};
char param[MAX_BUFFER_SIZE] = {0};
char value[MAX_BUFFER_SIZE] = {0};
int required = 0;

void parse_query_string(char *query,
                        char *obj_path,
                        char *param,
                        char *value,
                        int *required) {
    sscanf(query,
    "obj_path=%[^&]&param=%[^&]&value=%[^&]&required=%d", obj_path, param, value, required);
}

int init() {
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error initializing curl\n");
        return 1;
    }
    send_payload_buffer = (uint8_t *)malloc(MAX_BUFFER_SIZE * sizeof(uint8_t));
    if (!send_payload_buffer) {
        fprintf(stderr, "Error allocating memory for send payload buffer\n");
        curl_easy_cleanup(curl);
        return 1;
    }
    return 0;
}

void uninit() {
    if (curl) {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }
    if (send_payload_buffer) {
        free(send_payload_buffer);
        send_payload_buffer = NULL;
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr,
                "Usage: %s [GET|POST]"
                "obj_path=/path&param=parameter&value=somevalue&required=1\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "GET") != 0 && strcmp(argv[1], "POST") != 0) {
        fprintf(stderr, "Unsupported request method: %s\n", argv[1]);
        fprintf(stderr, "Supported methods are: GET, POST\n");
        return 1;
    }

    if (argc < 3) {
        fprintf(stderr, "%s request requires a query string or data string argument\n", argv[1]);
        return 1;
    }

    parse_query_string(argv[2], obj_path, param, value, &required);

    if (create_protobuf_message(
        &send_payload_buffer, &send_payload_size, obj_path, param, value, required)) {
        fprintf(stderr, "Error creating protobuf message\n");
        free(send_payload_buffer);
        return 1;
    }

    if (init() != 0) {
        fprintf(stderr, "Failed to initialize resources\n");
        free(send_payload_buffer);
        return 1;
    }

    if (curl) {
        perform_ws_operations(curl, send_payload_buffer, send_payload_size);
    }

    uninit();
    free(send_payload_buffer);

    return 0;
}
