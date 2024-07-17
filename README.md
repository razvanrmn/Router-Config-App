# WebSocket Client and Server with Protobuf Integration

This program demonstrates a WebSocket client-server communication setup using C programming language. The client sends messages encoded with Protocol Buffers to a WebSocket server, which then processes and responds to these messages.

## Folder Structure

- `src/`: Contains all the source files for the WebSocket client and server.
  - `include/`: Header files, including `protobuf_handler.h`, `wss_handler.h`, and `usp-msg-1-3.pb-c.h`.
  - `protobuf_handler.c`: Implements functions for creating Protocol Buffers messages.
  - `wss_handler.c`: Handles WebSocket operations using libcurl and libwebsockets.
  - `ws_client_main.c`: Main program for the WebSocket client.
  - `ws_server_main.c`: Main program for the WebSocket server.
  - `usp-msg-1-3.pb-c.c`: Implements Protocol Buffers message handling.
- `build/`: Directory where the executable binaries (`ws_client` and `ws_server`) are generated.
- `html/`: Contains `index.html` for a basic web interface to interact with the WebSocket client.

## Installation and Running

### Building the Project

1. Clone the repository:

   ```bash
   git clone <repository-url>
   cd <repository-directory>
Build the WebSocket Client and Server:

    make

2. Running the WebSocket Server

  Start the WebSocket server:

    cd /build
    ./ws_server 8080

  This starts the WebSocket server on port 8080.

3. Deploying and Using the WebSocket Client

After building, install the WebSocket client to a CGI directory for web deployment:

    sudo make install

This command copies the compiled client (ws_client.cgi) to /var/www/html/cgi-bin.

Place the HTML interface (index.html) in your web server's document root (/var/www/html).

Help and Running Parameters

4. To run the WebSocket client, use the following command format:
   
        ./build/ws_client [GET|POST] obj_path=/path&param=parameter&value=somevalue&required=1

Parameters:
- param1: Specifies the HTTP method (GET or POST) for sending data.
- param2: Defines the query parameters (obj_path, param, value, required) to be sent to the server.

Parameters Explained:
- lobj_path: Path to the object in the server.
- param: Parameter to be passed to the server.
- value: Value associated with the parameter.
- required: Indicates whether the parameter is required (1 for true, 0 for false).

Troubleshooting
- Ensure all dependencies (libcurl, libprotobuf-c, libwebsockets) are installed and accessible to the build environment.
- Check permissions if encountering issues with file operations or web server integration
