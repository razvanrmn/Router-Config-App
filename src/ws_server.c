#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define EXAMPLE_RX_BUFFER_BYTES (256)

// WebSocket callback function
static int callback_websockets(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_RECEIVE:
    {
        // When data is received from the client
        printf("Received data: %.*s\n", (int)len, (char *)in);

        // Define the response message
        const char *response = "message received";
        size_t response_len = strlen(response);

        // Allocate memory for the response buffer
        unsigned char *buf = (unsigned char *)malloc(LWS_PRE + response_len);
        if (!buf)
        {
            fprintf(stderr, "Failed to allocate memory for response\n");
            return 1; // Return an error if memory allocation fails
        }

        // Copy the response message into the buffer
        memcpy(&buf[LWS_PRE], response, response_len);

        // Write the response back to the client
        lws_write(wsi, &buf[LWS_PRE], response_len, LWS_WRITE_TEXT);

        // Free the allocated memory for the response buffer
        free(buf);
        break;
    }

    case LWS_CALLBACK_ESTABLISHED:
        // When a new WebSocket connection is established
        printf("Connection established\n");
        break;

    case LWS_CALLBACK_CLOSED:
        // When an established WebSocket connection is closed
        printf("Connection closed\n");
        break;

    default:
        break;
    }
    return 0; // Return 0 to indicate that processing of the callback is complete
}

// Define the WebSocket protocol and its callback function
static struct lws_protocols protocols[] = {
    {
        "test-protocol",         // Protocol name
        callback_websockets,     // Callback function for handling WebSocket events
        0,                       // Per-connection user data size (not used in this example)
        EXAMPLE_RX_BUFFER_BYTES, // Receive buffer size (maximum size of received messages)
    },
    {NULL, NULL, 0, 0} // Terminator for the protocol list
};

// Main function
int main(void)
{
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);

    // Set up the context creation info
    info.port = 8080;           // Port number to listen on
    info.protocols = protocols; // Array of protocols supported by the server
    info.gid = -1;              // Group ID (not used in this example)
    info.uid = -1;              // User ID (not used in this example)

    // Create the libwebsockets context
    struct lws_context *context = lws_create_context(&info);
    if (context == NULL)
    {
        fprintf(stderr, "lws_create_context failed\n");
        return -1; // Return an error if context creation fails
    }

    printf("Starting server...\n");

    // Main server loop
    while (1)
    {
        lws_service(context, 1000); // Process WebSocket events with a timeout of 1000 ms
    }

    // Destroy the libwebsockets context when done
    lws_context_destroy(context);

    return 0;
}
