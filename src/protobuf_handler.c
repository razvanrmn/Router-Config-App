#include "protobuf_handler.h"
#include <usp-msg-1-3.pb-c.h>
#include <stdio.h>
#include <string.h>

int create_protobuf_message(
    uint8_t **buffer,
    size_t *size,
    const char *obj_path,
    const char *param,
    const char *value,
    const int required) {

    Usp__Set set_message = USP__SET__INIT;
    Usp__Set__UpdateObject update_object = USP__SET__UPDATE_OBJECT__INIT;

    if (!obj_path || !param || !value) {
        fprintf(stderr, "Error: NULL pointer detected in create_protobuf_message\n");
        goto error_set_message;
    }

    update_object.obj_path = strdup(obj_path);
    if (!update_object.obj_path) {
        fprintf(stderr,
                "Error: Memory allocation failed"
                "for obj_path in create_protobuf_message\n");
        goto error_set_message;
    }

    Usp__Set__UpdateParamSetting param_setting = USP__SET__UPDATE_PARAM_SETTING__INIT;
    param_setting.param = strdup(param);
    if (!param_setting.param) {
        fprintf(stderr, "Error: Memory allocation failed for param in create_protobuf_message\n");
        goto cleanup_update_obj_path;
    }

    param_setting.value = strdup(value);
    if (!param_setting.value) {
        fprintf(stderr, "Error: Memory allocation failed for value in create_protobuf_message\n");
        goto cleanup_param_setting_param;
    }
    param_setting.required = required;

    update_object.param_settings = malloc(sizeof(Usp__Set__UpdateParamSetting *));
    if (!update_object.param_settings) {
        fprintf(stderr, "Error: Memory allocation failed"
        "for param_settings in create_protobuf_message\n");
        goto cleanup_param_settings_value;
    }
    update_object.param_settings[0] = &param_setting;
    update_object.n_param_settings = 1;

    set_message.update_objs = malloc(sizeof(Usp__Set__UpdateObject *));
    if (!set_message.update_objs) {
        fprintf(stderr, "Error: Memory allocation failed"
        "for update_objs in create_protobuf_message\n");
        goto cleanup_update_obj_param_settings;
    }
    set_message.update_objs[0] = &update_object;
    set_message.n_update_objs = 1;

    *size = usp__set__get_packed_size(&set_message);
    *buffer = malloc(*size);
    if (!*buffer) {
        fprintf(stderr, "Error: Memory allocation failed for buffer in create_protobuf_message\n");
        goto cleanup_set_message_update_objs;
    }
    usp__set__pack(&set_message, *buffer);

    return 0;

cleanup_set_message_update_objs:
    free(set_message.update_objs);

cleanup_update_obj_param_settings:
    free(update_object.param_settings);

cleanup_param_settings_value:
    free(param_setting.value);

cleanup_param_setting_param:
    free(param_setting.param);

cleanup_update_obj_path:
    free(update_object.obj_path);

error_set_message:
    return 1;
}