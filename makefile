CC = gcc
OPT = -O1
CFLAGS = -Wall -Wextra -std=c99 $(OPT) -I include/

ifeq ($(OS), Windows_NT)
	MKDIR = @mkdir
	RM = @rmdir /s /q
	LIBS = -L lib/ -lraylib_win -lgdi32 -lwinmm
	BIN = out.exe
else # Linux
	MKDIR = @mkdir -p
	RM = @rm -r
	LIBS = -L lib/ -lraylib_linux -lGL -lm -lpthread -ldl -lrt -lX11
	BIN = out
endif

BIN_DIR = bin
OBJ_DIR = obj

OBJS = main.o vm.o opcode.o renderer.o

all: create_dir $(BIN_DIR)/$(BIN)

create_dir:
	$(MKDIR) bin
	$(MKDIR) obj

# Create executable.
$(BIN_DIR)/$(BIN): $(addprefix $(OBJ_DIR)/,$(OBJS))
	$(CC) $(LIBS) -o $@ $^ $(LIBS)

# Create object files.
$(addprefix $(OBJ_DIR),/%.o):src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM) bin
	$(RM) obj

