#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "protobuf_handler.h"
#include "wss_handler.h"
#include "usp-msg-1-3.pb-c.h"

#define MAX_BUFFER_SIZE 256
#define ERROR_CURL_INIT 1
#define ERROR_MEMORY_ALLOCATION 2
#define ERROR_UNSUPPORTED_METHOD 3
#define ERROR_MISSING_ARGUMENTS 4
#define ERROR_INVALID_METHOD 5
#define ERROR_CREATING_PROTOBUF 6

CURL *curl = NULL;
uint8_t *send_payload_buffer = NULL;
size_t send_payload_size = 0;
char obj_path[MAX_BUFFER_SIZE] = {0};
char param[MAX_BUFFER_SIZE] = {0};
char value[MAX_BUFFER_SIZE] = {0};
int required = 0;

void parse_query_string(char *query, char *obj_path, char *param, char *value, int *required) {
    sscanf(query, "obj_path=%[^&]&param=%[^&]&value=%[^&]&required=%d", obj_path, param, value, required);
}

int init() {
    printf("Content-type: text/html\n\n");

    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: curl_global_init() failed: %s\n", curl_easy_strerror(res));
        return ERROR_CURL_INIT;
    }

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error: curl_easy_init() failed\n");
        curl_global_cleanup();
        return ERROR_CURL_INIT;
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

int handle_environment_input() {
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
        fprintf(stderr, "Unsupported request method, the supported request methods are POST / GET\n");
        return ERROR_UNSUPPORTED_METHOD;
    }
    return 0;
}

int handle_argument_input(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr,
                "Usage: %s [GET|POST] obj_path=/path&param=parameter&value=somevalue&required=1\n",
                argv[0]);
        return ERROR_MISSING_ARGUMENTS;
    }

    if (strcmp(argv[1], "GET") != 0 && strcmp(argv[1], "POST") != 0) {
        fprintf(stderr, "Unsupported request the supported methods are: GET, POST: %s\n", argv[1]);
        return ERROR_INVALID_METHOD;
    }

    parse_query_string(argv[2], obj_path, param, value, &required);
    return 0;
}

int main(int argc, char *argv[]) {
    int result = 0;

    if (getenv("REQUEST_METHOD")) {
        result = handle_environment_input();
    } else {
        result = handle_argument_input(argc, argv);
    }

    if (result != 0) {
        return result;
    }

    if (create_protobuf_message(&send_payload_buffer, &send_payload_size, obj_path, param, value, required)) {
        fprintf(stderr, "Error creating protobuf message\n");
        return ERROR_CREATING_PROTOBUF;
    }

    if (init() != 0) {
        fprintf(stderr, "Failed to initialize resources\n");
        return ERROR_CURL_INIT;
    }

    if (curl) {
        result = perform_ws_operations(curl, send_payload_buffer, send_payload_size);
        if (result != 0) {
            fprintf(stderr, "Failed to perform WebSocket operations\n");
            uninit();
            return result;
        }

        const char *command = "CMD:ls";
        char response_buffer[MAX_BUFFER_SIZE] = {0};
        size_t response_buffer_size = sizeof(response_buffer);

        result = send_command_over_websocket("ws://localhost:8080/", (const uint8_t *)command, strlen(command), response_buffer, response_buffer_size);
        if (result != 0) {
            fprintf(stderr, "Failed to send command over WebSocket\n");
            uninit();
            return result;
        }
    }

    uninit();

    return 0;
}
