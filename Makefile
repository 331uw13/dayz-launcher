CC = gcc
CC_FLAGS = -ggdb -Wall -Wextra 
LD_FLAGS = -lraylib -lGL -lX11 -lpthread -lm

TARGET_NAME = dayz-launcher

SRC_FILES = $(shell find ./src -type f -name '*.c')
OBJ_FILES = $(patsubst ./src/%.c, ./obj/%.o, $(SRC_FILES))

all: $(TARGET_NAME)


$(OBJ_FILES): ./obj/%.o: ./src/%.c
	@mkdir -p $(shell dirname $@)
	@echo "$<"
	@$(CC) $(CC_FLAGS) -c $< -o $@

$(TARGET_NAME): $(OBJ_FILES)
	$(CC) $(LD_FLAGS) $(OBJ_FILES) -o $@


clean:
	@rm -v $(OBJ_FILES) $(TARGET_NAME)

.PHONY: all clean

