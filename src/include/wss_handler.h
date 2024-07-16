#ifndef WSS_HANDLER_H
#define WSS_HANDLER_H

#include <curl/curl.h>
#include <stdint.h>

void init_curl(CURL **curl);
void perform_ws_operations(CURL *curl, uint8_t *send_payload_buffer, size_t send_payload_size);

#endif // WSS_HANDLER_H
