CC = gcc
OPT = -O1
CFLAGS = -Wall -Wextra -std=c99 $(OPT) -I include/

ifeq ($(OS), Windows_NT)
	RM_BIN = @del /S /Q bin
	RM_OBJ = @del /S /Q obj
	LIBS = -L lib/ -lraylib_win -lgdi32 -lwinmm
	BIN = out.exe
else
	RM_BIN = @rm -r bin/*
	RM_OBJ = @rm -r obj/*
	LIBS = -L lib/ -lraylib_linux -lGL -lm -lpthread -ldl -lrt -lX11
	BIN = out
endif

OUT_DIR = bin
OBJ_DIR = obj

OBJS = main.o vm.o opcode.o renderer.o

all: $(OUT_DIR)/$(BIN)

# Create executable.
$(OUT_DIR)/$(BIN): $(addprefix $(OBJ_DIR)/,$(OBJS))
	$(CC) $(LIBS) -o $@ $^ $(LIBS)

# Create object files.
$(addprefix $(OBJ_DIR),/%.o):src/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	$(RM_BIN)
	$(RM_OBJ)

