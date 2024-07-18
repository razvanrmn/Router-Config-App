CC = gcc
CFLAGS = -I/usr/local/include -Isrc/include
LDFLAGS = -lcurl -lprotobuf-c -lwebsockets
SRC_DIR = src
CLIENT_SRC = $(SRC_DIR)/ws_client_main.c $(SRC_DIR)/protobuf_handler.c $(SRC_DIR)/wss_handler.c $(SRC_DIR)/include/usp-msg-1-3.pb-c.c
SERVER_SRC = $(SRC_DIR)/ws_server_main.c

BUILD_DIR = build
CLIENT_OUT = $(BUILD_DIR)/ws_client
SERVER_OUT = $(BUILD_DIR)/ws_server

CGI_BIN_DIR = /var/www/html/cgi-bin
CGI_BIN_CLIENT = $(CGI_BIN_DIR)/ws_client.cgi

HTML_DIR = /var/www/html

all: $(CLIENT_OUT) $(SERVER_OUT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLIENT_OUT): $(CLIENT_SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(CLIENT_OUT) $(CLIENT_SRC) $(LDFLAGS)

$(SERVER_OUT): $(SERVER_SRC) $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(SERVER_OUT) $(SERVER_SRC) $(LDFLAGS)

install: $(CLIENT_OUT) $(SERVER_OUT)
	mkdir -p $(CGI_BIN_DIR)
	cp $(CLIENT_OUT) $(CGI_BIN_CLIENT)
	mkdir -p $(HTML_DIR)
	cp html/index.html $(HTML_DIR)

clean:
	rm -f $(CLIENT_OUT) $(SERVER_OUT)
	rm -rf $(BUILD_DIR)

.PHONY: all install clean
