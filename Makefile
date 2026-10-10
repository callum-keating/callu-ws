BUILD := build
OBJ_DIR := $(BUILD)/objs
EXECUTABLE := $(BUILD)/callu-ws

SRCFILES := $(shell find src -name "*.c")
OBJFILES := $(patsubst src/%.c, $(OBJ_DIR)/%.o,$(SRCFILES))

CC := clang
CFLAGS := -std=gnu23 -g

all: $(EXECUTABLE)


$(EXECUTABLE): $(OBJFILES)
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $^ -o $@

run: all
	$(EXECUTABLE)

clean:
	rm -rf $(BUILD)
