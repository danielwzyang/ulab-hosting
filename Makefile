CC := gcc
CCFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinclude -D_POSIX_C_SOURCE=200809L -MMD -MP
BUILD := build
TARGET := $(BUILD)/ulab-hosting
BIN := /usr/local/bin/ulab-hosting

SRC := $(wildcard src/*.c) # find all c files in src
OBJ := $(SRC:src/%.c=$(BUILD)/%.o) # substition to convert src list to obj list
DEP := $(OBJ:.o=.d)

.PHONY: run setuid install uninstall clean

run: $(TARGET)
	@./$(TARGET) $(ARGS)

$(TARGET): $(OBJ)
	@$(CC) $(OBJ) -o $@

$(BUILD)/%.o: src/%.c | $(BUILD) # build folder must exist but doesn't trigger rebuild
	@$(CC) $(CCFLAGS) -c $< -o $@

-include $(DEP)

$(BUILD):
	@mkdir -p $(BUILD)

setuid: $(TARGET)
	@sudo chown root:root $(TARGET)
	@sudo chmod 4755 $(TARGET)

install: $(TARGET)
	@sudo install -o root -g root -m 4755 $(TARGET) $(BIN)
	@echo "installed $(BIN)"

uninstall: $(TARGET)
	@sudo rm -f $(BIN)
	@echo "uninstalled $(BIN)"

clean:
	@rm -rf $(BUILD)
