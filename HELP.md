# WebSocket Client-Server Application!

This project implements a WebSocket client-server application in C, utilizing Google's Protocol Buffers (`protobuf-c`) and `libwebsockets` for WebSocket communication.


# WebSocket Client (`ws_client`)

The WebSocket client sends protocol buffer messages over WebSocket to a server.

# Requirements
-   gcc compiler
-   Libraries: `libcurl`, `libprotobuf-c`, `libwebsockets`

## Usage

ws_client [GET|POST]  obj_path=/path&param=parameter&value=somevalue&required=1||0
./ws_client  GET  obj_path=/devices/1&param=temperature&value=25&required=1||0

## Parameters
-  GET|POST:  Specify  the  HTTP  method (GET or  POST) to send parameters.

-  obj_path:  Path  to  the  object.

-  param:  Parameter  name.

-  value:  Value  of  the  parameter.

-  required:  Integer  flag (0 or  1) indicating if  the  parameter  is  required.

## Example
./ws_client  GET  obj_path=/devices/1&param=temperature&value=25&required=1

# WebSocket Server (`ws_server`)

The  WebSocket  server  listens  for  WebSocket  connections  and  handles  incoming  messages.


## Usage

ws_client PortNumber
./ws_client  ./ws_server PortNumber

## Parameters
-  PortNumber:  Specify  the  port dedicated to server.

## Example
./ws_server 8080

# Makefile  Commands
To  compile  both  ws_client  and  ws_server:
- make


## Installation
To  install  ws_client  as  a  CGI  script  and  copy  index.html  to  a  web  directory:
- sudo make  install
