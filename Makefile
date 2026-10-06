CC = clang
CFLAGS = -Wall -Wextra -O2 -Isrc/ -MMD -MP

BUILD_DIR = out
TARGET = $(BUILD_DIR)/toypoll
OBJ_DIR = $(BUILD_DIR)/obj

SRCS = src/main.c src/reactor.c src/server.c
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(OBJS:.o=.d)

-include $(DEPS)

$(TARGET): $(OBJS)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $(OBJS)

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

compile_commands.json: $(SRCS)
	bear -- make

format:
	clang-format -i $(SRCS) src/*.h

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean compile_commands.json
