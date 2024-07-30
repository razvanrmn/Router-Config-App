#include "protobuf_handler.h"
#include <usp-msg-1-3.pb-c.h>
#include <stdio.h>
#include <string.h>

int pack_protobuf_message(
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
        goto error_update_obj_path;
    }

    param_setting.value = strdup(value);
    if (!param_setting.value) {
        fprintf(stderr, "Error: Memory allocation failed for value in create_protobuf_message\n");
        goto error_param_setting_param;
    }
    param_setting.required = required;

    update_object.param_settings = malloc(sizeof(Usp__Set__UpdateParamSetting *));
    if (!update_object.param_settings) {
        fprintf(stderr, "Error: Memory allocation failed"
                "for param_settings in create_protobuf_message\n");
        goto error_param_settings_value;
    }
    update_object.param_settings[0] = &param_setting;
    update_object.n_param_settings = 1;

    set_message.update_objs = malloc(sizeof(Usp__Set__UpdateObject *));
    if (!set_message.update_objs) {
        fprintf(stderr, "Error: Memory allocation failed"
        "for update_objs in create_protobuf_message\n");
        goto error_update_obj_param_settings;
    }
    set_message.update_objs[0] = &update_object;
    set_message.n_update_objs = 1;

    *size = usp__set__get_packed_size(&set_message);
    *buffer = malloc(*size);
    if (!*buffer) {
        fprintf(stderr, "Error: Memory allocation failed for buffer in create_protobuf_message\n");
        goto error_set_message_update_objs;
    }
    usp__set__pack(&set_message, *buffer);
    return 0;

error_set_message_update_objs:
    free(set_message.update_objs);

error_update_obj_param_settings:
    free(update_object.param_settings);

error_param_settings_value:
    free(param_setting.value);

error_param_setting_param:
    free(param_setting.param);

error_update_obj_path:
    free(update_object.obj_path);

error_set_message:
    return 1;
}

int unpack_protobuf_message(
    const uint8_t *buffer,
    size_t size,
    char **obj_path,
    char **param,
    char **value,
    int *required) {

    Usp__Set *set_message = NULL;
    Usp__Set__UpdateObject *update_object = NULL;
    Usp__Set__UpdateParamSetting *param_setting = NULL;

    set_message = usp__set__unpack(NULL, size, buffer);
    if (!set_message) {
        fprintf(stderr, "Error: Failed to unpack protobuf message\n");
        return 1;
    }

    if (set_message->n_update_objs == 0 || set_message->update_objs[0]->n_param_settings == 0) {
        fprintf(stderr, "Error: No update objects or param settings in unpacked message\n");
        usp__set__free_unpacked(set_message, NULL);
        return 1;
    }

    update_object = set_message->update_objs[0];
    param_setting = update_object->param_settings[0];

    *obj_path = strdup(update_object->obj_path);
    if (!*obj_path) {
        fprintf(stderr,
                "Error: Memory allocation failed for obj_path in unpack_protobuf_message\n");
        goto error_free_unpacked;
    }

    *param = strdup(param_setting->param);
    if (!*param) {
        fprintf(stderr, "Error: Memory allocation failed for param in unpack_protobuf_message\n");
        goto error_free_obj_path;
    }

    *value = strdup(param_setting->value);
    if (!*value) {
        fprintf(stderr, "Error: Memory allocation failed for value in unpack_protobuf_message\n");
        goto error_free_param;
    }

    *required = param_setting->required;

    usp__set__free_unpacked(set_message, NULL);

    return 0;

error_free_param:
    free(*param);

error_free_obj_path:
    free(*obj_path);

error_free_unpacked:
    usp__set__free_unpacked(set_message, NULL);

    return 1;
}

