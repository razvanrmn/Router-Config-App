# WebSocket Client-Server Application!

This project implements a WebSocket client-server application in C, utilizing Google's Protocol Buffers (`protobuf-c`) and `libwebsockets` for WebSocket communication.


# WebSocket Client (`ws_client`)

The WebSocket client sends HTTP GET or POST requests containing parameters formatted in a query string. These requests are sent to a server using WebSocket communication.

# Requirements
-   gcc compiler
-   Libraries: `libcurl`, `libprotobuf-c`, `libwebsockets`

## Usage

ws_client [GET|POST] obj_path=/path&param=parameter&value=somevalue&required=1|0

## Parameters
-  GET|POST:  Specify  the  HTTP  method (GET or  POST) to send parameters.

-  obj_path:  Path  to  the  object.

-  param:  Parameter  name.

-  value:  Value  of  the  parameter.

-  required:  Integer  flag (0 or  1) indicating if  the  parameter  is  required.

## Example
./ws_client  GET  obj_path=/devices/1&param=temperature&value=25&required=1

# WebSocket Server (`ws_server`)

The WebSocket server listens for WebSocket connections on a specified port and handles incoming messages.

## Usage
ws_server PortNumber

## Parameters
-  PortNumber:  Specify  the  port dedicated to server.

## Example
./ws_server 8080

# Makefile  Commands

-   `make`: Compiles both `ws_client` and `ws_server`.
-   `make install`: Installs `ws_client` as a CGI script and copies `index.html` to a web directory.
