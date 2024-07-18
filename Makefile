CC = gcc

CFLAGS = -I/usr/local/include -Isrc/include
LDFLAGS = -lcurl -lprotobuf-c -lwebsockets

SRC_DIR = src
BUILD_DIR = build
CGI_BIN_DIR = /var/www/html/cgi-bin
HTML_DIR = /var/www/html

CLIENT_SRCS = $(SRC_DIR)/ws_client_main.c \
              $(SRC_DIR)/protobuf_handler.c \
              $(SRC_DIR)/wss_handler.c \
              $(SRC_DIR)/include/usp-msg-1-3.pb-c.c

SERVER_SRCS = $(SRC_DIR)/ws_server_main.c

CLIENT_OBJS = $(CLIENT_SRCS:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)
SERVER_OBJS = $(SERVER_SRCS:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)


CLIENT_OUT = $(BUILD_DIR)/ws_client
SERVER_OUT = $(BUILD_DIR)/ws_server
CGI_BIN_CLIENT = $(CGI_BIN_DIR)/ws_client.cgi

all: $(CLIENT_OUT) $(SERVER_OUT)

$(CLIENT_OUT): $(CLIENT_OBJS) | $(BUILD_DIR)
	$(CC) -o $(CLIENT_OUT) $(CLIENT_OBJS) $(LDFLAGS)

$(SERVER_OUT): $(SERVER_OBJS) | $(BUILD_DIR)
	$(CC) -o $(SERVER_OUT) $(SERVER_OBJS) $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c | $(SRC_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(CLIENT_OUT) $(SERVER_OUT)
	mkdir -p $(CGI_BIN_DIR)
	cp $(CLIENT_OUT) $(CGI_BIN_CLIENT)
	mkdir -p $(HTML_DIR)
	cp html/index.html $(HTML_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(SRC_DIR)/*.o

.PHONY: all install clean
