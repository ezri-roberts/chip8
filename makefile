CC = gcc
OPT = -O
CFLAGS = -Wall -Wextra -std=c99 $(OPT)
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

OUT_DIR = bin
OBJ_DIR = obj

OBJS = main.o vm.o opcode.o renderer.o

ifeq ($(OS), Windows_NT)
	RM = @deltree
else
	RM = @rm -r
endif

all: $(OUT_DIR)/out

# Create executable.
$(OUT_DIR)/out: $(addprefix $(OBJ_DIR)/,$(OBJS))
	$(CC) $(LIBS) -o $@ $^ $(LIBS)

# Create object files.
$(addprefix $(OBJ_DIR),/%.o):src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) $(OUT_DIR)/out $(addprefix $(OBJ_DIR)/,$(OBJS))

