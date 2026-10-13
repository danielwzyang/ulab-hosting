CC := gcc
CCFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinclude -D_POSIX_C_SOURCE=200809L -MMD -MP
BUILD := build
TARGET := $(BUILD)/ulab-hosting

SRC := $(wildcard src/*.c) # find all c files in src
OBJ := $(SRC:src/%.c=$(BUILD)/%.o) # substition to convert src list to obj list
DEP := $(OBJ:.o=.d)

.PHONY: run setuid clean

run: $(TARGET)
	@./$(TARGET) $(ARGS)

$(TARGET): $(OBJ)
	@$(CC) $(OBJ) -o $@

$(BUILD)/%.o: src/%.c | $(BUILD) # build folder must exist but doesn't trigger rebuild
	@$(CC) $(CCFLAGS) -c $< -o $@

-include $(DEP)

$(BUILD):
	@mkdir -p $(BUILD)

setuid:
	@sudo chown root:root build/ulab-hosting 
	@sudo chmod u+s build/ulab-hosting 

clean:
	@rm -rf $(BUILD)
