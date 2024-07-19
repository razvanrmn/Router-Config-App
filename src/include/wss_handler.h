// wss_handler.h
#ifndef WSS_HANDLER_H
#define WSS_HANDLER_H

#include <curl/curl.h>
#include <stdint.h>

int init_curl(CURL **curl);
void uninit_curl(CURL *curl);
int perform_ws_operations(CURL *curl, const uint8_t *send_payload_buffer, size_t send_payload_size);
int send_command_over_websocket(
    const char *url,
    const uint8_t *command,
    size_t command_size,
    char *response_buffer,
    size_t response_buffer_size);
int check_curl_result(CURLcode res, CURL *curl, const char *msg);

#endif // WSS_HANDLER_H
