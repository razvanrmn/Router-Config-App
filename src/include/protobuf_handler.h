#ifndef PROTOBUF_HANDLER_H
#define PROTOBUF_HANDLER_H

#include <stdint.h>
#include <stdlib.h>

int pack_protobuf_message(
    uint8_t **buffer,
    size_t *size,
    const char *obj_path,
    const char *param,
    const char *value,
    const int required);

int unpack_protobuf_message(
    const uint8_t *buffer,
    size_t size,
    char **obj_path,
    char **param,
    char **value,
    int *required);

#endif /* PROTOBUF_HANDLER_H */
