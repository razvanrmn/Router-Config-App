
# WebSocket Client-Server Application!

This project implements a WebSocket client-server application in C, utilizing Google's Protocol Buffers (`protobuf-c`) and `libwebsockets` for WebSocket communication.

## Requirements

- GCC compiler
- Libraries: `libcurl`, `libprotobuf-c`, `libwebsockets`

# WebSocket Client (`ws_client`)

The WebSocket client sends HTTP GET or POST requests containing parameters formatted in a query string. These requests are sent to a server using WebSocket communication.

## Usage

```sh
./ws_client [GET|POST] obj_path=/path&param=parameter&value=somevalue&required=1|0
```

## Parameters

- `GET|POST`: Specify the HTTP method (GET or POST) to send parameters.
- `obj_path`: Path to the object.
- `param`: Parameter name.
- `value`: Value of the parameter.
- `required`: Integer flag (0 or 1) indicating if the parameter is required.

## Example

```sh
./ws_client GET obj_path=/devices/1&param=temperature&value=25&required=1
```

# WebSocket Server (`ws_server`)

The WebSocket server listens for WebSocket connections on a specified port and handles incoming messages.

## Usage

```sh
./ws_server PortNumber
```

## Parameters

- `PortNumber`: Specify the port on which the server will listen.

## Example

```sh
./ws_server 8080
```

# Makefile Commands

- `make`: Compiles both `ws_client` and `ws_server`.
- `make install`: Installs `ws_client` as a CGI script and copies `index.html` to a web directory.

# Code Summary

## WebSocket Client (`ws_client.c`)

- Initializes CURL for HTTP operations.
- Parses query string parameters from GET or POST requests.
- Creates a Protobuf message based on the provided parameters.
- Establishes a WebSocket connection to send the Protobuf message.
- Handles responses from the WebSocket server.

## WebSocket Server (`ws_server.c`)

- Uses `libwebsockets` to manage WebSocket connections.
- Listens for incoming WebSocket messages on a specified port.
- Parses and processes received commands.
- Executes shell commands and sends the output back to the client.
- Uses Protobuf to decode and handle incoming messages.

# Example Files

## `index.html`

A simple HTML file to interact with the WebSocket server through a web interface.

# Installation and Execution

1. **Compile the project**:

    ```sh
    make
    ```

2. **Install the client and server**:

    ```sh
    make install
    ```

3. **Run the WebSocket server**:

    ```sh
    ./ws_server 8080
    ```

4. **Run the WebSocket client**:

    ```sh
    ./ws_client GET obj_path=/devices/1&param=temperature&value=25&required=1
    ```

This setup provides a complete WebSocket client-server application that can communicate using Protobuf messages. The client sends HTTP requests, which are parsed and converted into Protobuf messages before being sent over a WebSocket connection to the server. The server handles these messages, executes corresponding commands, and returns the results to the client.
