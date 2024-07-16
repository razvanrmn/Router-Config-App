#ifndef PROTOBUF_HANDLER_H
#define PROTOBUF_HANDLER_H

#include <stdint.h>
#include <stdlib.h>

int create_protobuf_message(
    uint8_t **buffer,   // Changed to a pointer to a pointer
    size_t *size,       // Changed to a pointer to size_t
    const char *obj_path,
    const char *param,
    const char *value,
    int required);

#endif /* PROTOBUF_HANDLER_H */
