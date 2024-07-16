#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define EXAMPLE_RX_BUFFER_BYTES (256)


static int callback_websockets(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_RECEIVE:
    {

        printf("Received data: %.*s\n", (int)len, (char *)in);

        const char *response = "message received";
        size_t response_len = strlen(response);

        unsigned char *buf = (unsigned char *)malloc(LWS_PRE + response_len);
        if (!buf) {
            fprintf(stderr, "Failed to allocate memory for response\n");
            return 1;
        }

        memcpy(&buf[LWS_PRE], response, response_len);

        lws_write(wsi, &buf[LWS_PRE], response_len, LWS_WRITE_TEXT);

        free(buf);
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
        EXAMPLE_RX_BUFFER_BYTES,
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
