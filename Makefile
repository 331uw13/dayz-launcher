CC = gcc
CC_FLAGS = -O2 -Wall -Wextra 
LD_FLAGS = 

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

#%.o: %.c
#	@$(CC) $(FLAGS) -I/usr/include/freetype2 -I/usr/include/libpng16 \
#		-c $< -o $@ && (echo -e "\033[32m[Compiled]\033[0m $<") || (echo -e "\033[31m[Failed]\033[0m $<"; exit 1) 
#
#$(TARGET_NAME): $(OBJS)
#	@echo -e "\033[90mLinking...\033[0m"
#	@$(CC) $(OBJS) $(LIBS) -o $@ && (echo -e "\033[36mDone.\033[0m"; ls -lh $(TARGET_NAME))
#
#clean:
#	@rm -v $(OBJS) $(TARGET_NAME)

.PHONY: all clean

