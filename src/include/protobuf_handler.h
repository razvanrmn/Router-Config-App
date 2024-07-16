#ifndef PROTOBUF_HANDLER_H
#define PROTOBUF_HANDLER_H

#include <stdint.h>
#include <stdlib.h>

int create_protobuf_message(
    uint8_t **buffer,
    size_t *size,
    const char *obj_path,
    const char *param,
    const char *value,
    const int required);

#endif /* PROTOBUF_HANDLER_H */
