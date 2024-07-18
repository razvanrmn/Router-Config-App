#if 0
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <usp-msg-1-3.pb-c.h>

void parse_query_string(char *query, char *obj_path, char *param, char *value, int *required) {
    sscanf(query, "obj_path=%[^&]&param=%[^&]&value=%[^&]&required=%d", obj_path, param, value, required);
}

void cleanup_and_exit(CURL *curl, uint8_t *send_payload_buffer) {
    if (curl) curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (send_payload_buffer)
        free(send_payload_buffer);
}

void create_protobuf_message(
    uint8_t **buffer,
    size_t *size,
    const char *obj_path,
    const char *param,
    const char *value,
    int required) {

    Usp__Set set_message = USP__SET__INIT;
    Usp__Set__UpdateObject update_object = USP__SET__UPDATE_OBJECT__INIT;

    if (!obj_path || !param || !value) {
        fprintf(stderr, "Error: NULL pointer detected in create_protobuf_message\n");
        exit(1);
    }

    update_object.obj_path = strdup(obj_path);

    Usp__Set__UpdateParamSetting param_setting = USP__SET__UPDATE_PARAM_SETTING__INIT;
    param_setting.param = strdup(param);
    param_setting.value = strdup(value);
    param_setting.required = required;

    update_object.param_settings = malloc(sizeof(Usp__Set__UpdateParamSetting *));
    if (!update_object.param_settings) {
        fprintf(stderr, "Error: Memory allocation failed in create_protobuf_message\n");
        exit(1);
    }
    update_object.param_settings[0] = &param_setting;
    update_object.n_param_settings = 1;

    set_message.update_objs = malloc(sizeof(Usp__Set__UpdateObject *));
    if (!set_message.update_objs) {
        fprintf(stderr, "Error: Memory allocation failed in create_protobuf_message\n");
        exit(1);
    }
    set_message.update_objs[0] = &update_object;
    set_message.n_update_objs = 1;

    *size = usp__set__get_packed_size(&set_message);
    *buffer = malloc(*size);
    if (!*buffer) {
        fprintf(stderr, "Error: Memory allocation failed in create_protobuf_message\n");
        exit(1);
    }
    usp__set__pack(&set_message, *buffer);

    free(update_object.param_settings);
    free(set_message.update_objs);
    free((void *)update_object.obj_path);
    free((void *)param_setting.param);
    free((void *)param_setting.value);
}

int main(void) {
    CURL *curl;
    CURLcode res;
    size_t sent;
    size_t rlen;
    const struct curl_ws_frame *meta;
    uint8_t *send_payload_buffer = NULL;
    size_t send_payload_size = 0;
    char buffer[256];

    char obj_path[256] = {0};
    char param[256] = {0};
    char value[256] = {0};
    int required = 0;
    
    // Set Content-Type for CGI response
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
        exit(1);
    }

    create_protobuf_message(&send_payload_buffer, &send_payload_size, obj_path, param, value, required);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "ws://localhost:8080/");
        curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 2L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            cleanup_and_exit(curl, send_payload_buffer);
        }

        res = curl_ws_send(curl, (const char *)send_payload_buffer, send_payload_size, &sent, 0, CURLWS_BINARY);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_ws_send() failed: %s\n", curl_easy_strerror(res));
            cleanup_and_exit(curl, send_payload_buffer);
        }
        printf("Sent %zu bytes.\n", sent);

        do {
            res = curl_ws_recv(curl, buffer, sizeof(buffer), &rlen, &meta);
        } while (res == CURLE_AGAIN);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_ws_recv() failed: %s\n", curl_easy_strerror(res));
            cleanup_and_exit(curl, send_payload_buffer);
        }
        buffer[rlen] = '\0';
        printf("Received %zu bytes: %s\n", rlen, buffer);

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    free(send_payload_buffer);
    return 0;
}
#endif