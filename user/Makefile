CLIENT = mclient
SERVER = echo

SRC_DIR = src
OBJ_DIR = build

SRC = $(SRC_DIR)/eth.c $(SRC_DIR)/stats.c #/$(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CFLAGS += -Iinclude
CFLAGS += -Wall -std=gnu99 -O3
#LDFLAGS += -Llib
#LDLIBS += -lm

.PHONY: all clean module clean-module

all: $(CLIENT) $(SERVER)

$(CLIENT): $(OBJ) $(OBJ_DIR)/$(CLIENT).o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $(OBJ_DIR)/$@

$(SERVER): $(OBJ) $(OBJ_DIR)/$(SERVER).o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $(OBJ_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)/*.o $(OBJ_DIR)/$(CLIENT) $(OBJ_DIR)/$(SERVER)

