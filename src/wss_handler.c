#include "wss_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define MAX_SIZE 256

int init_curl(CURL **curl)
{
    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: curl_global_init() failed: %s\n", curl_easy_strerror(res));
        return -1;
    }

    *curl = curl_easy_init();
    if (!*curl) {
        fprintf(stderr, "Error: curl_easy_init() failed\n");
        curl_global_cleanup();
        return -1;
    }
    return 0;
}

void uninit_curl(CURL *curl)
{
    if (curl)
        curl_easy_cleanup(curl);

    curl_global_cleanup();
}

static int check_curl_result(CURLcode res, CURL *curl, const char *error_msg)
{
    if (res != CURLE_OK) {
        fprintf(stderr, "%s: %s\n", error_msg, curl_easy_strerror(res));
        if (curl) {
            uninit_curl(curl);
        }
        return -1;
    }
    return 0;
}

int perform_ws_operations(CURL *curl, const uint8_t *send_payload_buffer, size_t send_payload_size)
{
    if (!send_payload_buffer || send_payload_size == 0) {
        fprintf(stderr, "Invalid payload buffer or size\n");
        return -1;
    }

    CURLcode res;
    size_t sent = 0;
    size_t rlen = 0;
    const struct curl_ws_frame *meta = NULL;
    char buffer[MAX_SIZE] = {0};

    curl_easy_setopt(curl, CURLOPT_URL, "ws://localhost:8080/");
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 2L);

    res = curl_easy_perform(curl);

    if (check_curl_result(res, curl, "curl_easy_perform() failed") != 0)
        return -1;

    res = curl_ws_send(curl,
                       (const char *)send_payload_buffer,
                       send_payload_size,
                       &sent,
                       0,
                       CURLWS_BINARY);

    if (check_curl_result(res, curl, "curl_ws_send() failed") != 0)
        return -1;
    printf("Sent %zu bytes.\n", sent);

    do {
        res = curl_ws_recv(curl, buffer, sizeof(buffer), &rlen, &meta);
        if (res == CURLE_AGAIN) {
            fprintf(stderr, "curl_ws_recv() returned CURLE_AGAIN, retrying...\n");
        }
    } while (res == CURLE_AGAIN);

    if (check_curl_result(res, curl, "curl_ws_recv() failed") != 0)
        return -1;

    if (rlen < sizeof(buffer)) {
        buffer[rlen] = '\0';
        printf("Received %zu bytes: %s\n", rlen, buffer);
    } else {
        fprintf(stderr, "Received data exceeds buffer size\n");
        return -1;
    }

    uninit_curl(curl);
    return 0;
}