// wss_handler.h
#ifndef WSS_HANDLER_H
#define WSS_HANDLER_H

#include <curl/curl.h>
#include <stdint.h>

int init_curl(CURL **curl);
void uninit_curl(CURL *curl);
int perform_ws_operations(CURL *curl, const uint8_t *send_payload_buffer, size_t send_payload_size);
int send_command_over_websocket(CURL *curl, const char *command);
#endif // WSS_HANDLER_H
