#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFFER_BYTES 1024
#define COMMAND_PREFIX "CMD:"
#define COMMAND_MAX_LEN (BUFFER_BYTES - 6 - 30)

static void send_response(struct lws *wsi, const char *response) {
    size_t response_len = strlen(response);
    unsigned char *buf = (unsigned char *)malloc(LWS_PRE + response_len + 1);
    if (!buf) {
        fprintf(stderr, "Failed to allocate memory for response\n");
        return;
    }
    memcpy(&buf[LWS_PRE], response, response_len);
    buf[LWS_PRE + response_len] = '\0';
    int write_result = lws_write(wsi, &buf[LWS_PRE], response_len, LWS_WRITE_TEXT);
    free(buf);
    if (write_result < 0) {
        fprintf(stderr, "Failed to send response: %d\n", write_result);
    }
}

static void handle_command(struct lws *wsi, const char *command, size_t len) {
    if (len >= COMMAND_MAX_LEN) {
        send_response(wsi, "Command too long\n");
        return;
    }

    char cmd_with_redirect[BUFFER_BYTES];
    int cmd_length = snprintf(cmd_with_redirect, sizeof(cmd_with_redirect),
                              "%.*s 2>&1; echo $? > /tmp/command_exit_code",
                              (int)len, command);

    if (cmd_length >= sizeof(cmd_with_redirect)) {
        send_response(wsi, "Command buffer overflow\n");
        return;
    }

    FILE *fp = popen(cmd_with_redirect, "r");
    if (!fp) {
        int err_code = errno;
        char response[BUFFER_BYTES];
        snprintf(response, sizeof(response), "Failed to execute command: %s (Error code: %d)\n",
                 strerror(err_code), err_code);
        send_response(wsi, response);
        return;
    }

    char output[BUFFER_BYTES];
    size_t total_len = 0;
    unsigned char *buf = (unsigned char *)malloc(LWS_PRE + BUFFER_BYTES);
    if (!buf) {
        fprintf(stderr, "Failed to allocate memory for response\n");
        pclose(fp);
        return;
    }

    while (fgets(output, sizeof(output), fp)) {
        size_t output_len = strlen(output);
        if (total_len + output_len >= BUFFER_BYTES) {
            unsigned char *new_buf = (unsigned char *)realloc(buf, LWS_PRE + total_len + output_len + 1);
            if (!new_buf) {
                fprintf(stderr, "Failed to reallocate memory for response\n");
                pclose(fp);
                free(buf);
                return;
            }
            buf = new_buf;
        }
        memcpy(&buf[LWS_PRE + total_len], output, output_len);
        total_len += output_len;
    }

    int exit_code = -1;
    FILE *code_fp = fopen("/tmp/command_exit_code", "r");
    if (code_fp) {
        if (fscanf(code_fp, "%d", &exit_code) != 1) {
            fprintf(stderr, "Failed to parse exit code\n");
        }
        fclose(code_fp);
    }

    char final_response[BUFFER_BYTES];
    snprintf(final_response, sizeof(final_response), "%sExit code: %d", buf + LWS_PRE, exit_code);
    send_response(wsi, final_response);

    free(buf);
    pclose(fp);
}

static int callback_websockets(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    const size_t command_prefix_len = strlen(COMMAND_PREFIX);

    switch (reason) {
    case LWS_CALLBACK_RECEIVE:
        {
            char *data = (char *)in;

            if (len > command_prefix_len && memcmp(data, COMMAND_PREFIX, command_prefix_len) == 0) {
                data += command_prefix_len;
                len -= command_prefix_len;
                char command[BUFFER_BYTES];
                memcpy(command, data, len);
                command[len] = '\0';

                printf("Received command: %s\n", command);
                handle_command(wsi, command, len);
            } else {
                printf("Received data: %.*s\n", (int)len, (char *)in);
                send_response(wsi, "Message received");
            }
        }
        break;

    case LWS_CALLBACK_ESTABLISHED:
        printf("Connection established\n");
        break;

    case LWS_CALLBACK_CLOSED:
        printf("Connection closed\n");
        break;

    default:
        break;
    }

    return 0;
}


static struct lws_protocols protocols[] = {
    {
        "test-protocol",
        callback_websockets,
        0,
        BUFFER_BYTES,
    },
    {NULL, NULL, 0, 0}
};

int main(int argc, char *argv[])
{
    struct lws_context_creation_info info = {0};

    info.port = 8080;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "lws_create_context failed\n");
        return -1;
    }

    printf("Starting server...\n");

    while (1)
    {
        lws_service(context, 1000);
    }

    lws_context_destroy(context);
    return 0;
}
