CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -Iinclude -Iexternal/raylib/src
LDFLAGS = external/raylib/src/libraylib.a -lm -lpthread -ldl -lrt -lX11

SRC = src/main.c \
      src/game.c \
      src/ui_cyberpunk.c \
      src/audio_synth.c \
      src/sudoku_core.c \
      src/sudoku_generator.c \
      src/storage.c

OBJ = $(SRC:.c=.o)
TARGET = naga_sudoku

all: raylib $(TARGET)

raylib:
	@if [ ! -d external/raylib ]; then \
		echo "Cloning Raylib repository..."; \
		mkdir -p external && git clone --depth 1 https://github.com/raysan5/raylib.git external/raylib; \
	fi
	@if [ ! -f external/raylib/src/libraylib.a ]; then \
		echo "Compiling Raylib static library..."; \
		make -C external/raylib/src PLATFORM=PLATFORM_DESKTOP -j4; \
	fi

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)
	@echo "Build successful: ./$(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test:
	$(CC) -Wall -Wextra -std=c99 -Iinclude src/sudoku_core.c src/sudoku_generator.c tests/test_core.c -o tests/test_core
	./tests/test_core

clean:
	rm -f src/*.o tests/test_core $(TARGET)

.PHONY: all raylib test clean
