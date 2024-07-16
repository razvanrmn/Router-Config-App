#include "wss_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define MAX_SIZE 256

void init_curl(CURL **curl) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    *curl = curl_easy_init();
    if (!*curl) {
        fprintf(stderr, "Error: curl_easy_init() failed\n");
        curl_global_cleanup();
    }
}

static void cleanup_and_exit(CURL *curl, uint8_t *send_payload_buffer) {
    if (curl) curl_easy_cleanup(curl);
    curl_global_cleanup();
    if (send_payload_buffer) free(send_payload_buffer);
}

static void check_curl_result(CURLcode res, CURL *curl, uint8_t *send_payload_buffer, const char *error_msg) {
    if (res != CURLE_OK) {
        fprintf(stderr, "%s: %s\n", error_msg, curl_easy_strerror(res));
        cleanup_and_exit(curl, send_payload_buffer);
    }
}

void perform_ws_operations(CURL *curl, uint8_t *send_payload_buffer, size_t send_payload_size) {
    CURLcode res;
    size_t sent;
    size_t rlen;
    const struct curl_ws_frame *meta;
    char buffer[MAX_SIZE];

    curl_easy_setopt(curl, CURLOPT_URL, "ws://localhost:8080/");
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 2L);

    res = curl_easy_perform(curl);
    check_curl_result(res, curl, send_payload_buffer, "curl_easy_perform() failed");

    res = curl_ws_send(curl, (const char *)send_payload_buffer, send_payload_size, &sent, 0, CURLWS_BINARY);
    check_curl_result(res, curl, send_payload_buffer, "curl_ws_send() failed");
    printf("Sent %zu bytes.\n", sent);

    do {
        res = curl_ws_recv(curl, buffer, sizeof(buffer), &rlen, &meta);
    } while (res == CURLE_AGAIN);

    check_curl_result(res, curl, send_payload_buffer, "curl_ws_recv() failed");

    buffer[rlen] = '\0';
    printf("Received %zu bytes: %s\n", rlen, buffer);
}