CC       = cc
CFLAGS   = -Wall -Wextra -Werror -std=c99 -g
LDFLAGS  =
TARGET   = vasuki

SRCS     = game/main.c      \
           game/game.c      \
           math/math.c      \
           string/string.c  \
           memory/memory.c  \
           screen/screen.c  \
           keyboard/keyboard.c \
           timer/timer.c    \
           random/random.c  \
           fileio/fileio.c  \
           audio/audio.c

OBJS     = game/main.o      \
           game/game.o      \
           math/math.o      \
           string/string.o  \
           memory/memory.o  \
           screen/screen.o  \
           keyboard/keyboard.o \
           timer/timer.o    \
           random/random.o  \
           fileio/fileio.o  \
           audio/audio.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Dependencies
game/main.o:     game/main.c game/game.h math/math.h string/string.h \
                 memory/memory.h screen/screen.h keyboard/keyboard.h \
                 timer/timer.h random/random.h fileio/fileio.h audio/audio.h
game/game.o:     game/game.c game/game.h math/math.h string/string.h \
                 memory/memory.h screen/screen.h keyboard/keyboard.h \
                 timer/timer.h random/random.h audio/audio.h
math/math.o:     math/math.c math/math.h
string/string.o: string/string.c string/string.h
memory/memory.o: memory/memory.c memory/memory.h
screen/screen.o: screen/screen.c screen/screen.h
keyboard/keyboard.o: keyboard/keyboard.c keyboard/keyboard.h
timer/timer.o:   timer/timer.c timer/timer.h
random/random.o: random/random.c random/random.h
fileio/fileio.o: fileio/fileio.c fileio/fileio.h string/string.h
audio/audio.o:   audio/audio.c audio/audio.h

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

# Build WebAssembly frontend
wasm:
	emcc frontend/wasm/wasm_bridge.c game/game.c math/math.c string/string.c \
		memory/memory.c frontend/wasm/timer_wasm.c frontend/wasm/random_wasm.c \
		frontend/wasm/fileio_wasm.c frontend/wasm/audio_wasm.c \
		frontend/wasm/screen_wasm.c frontend/wasm/keyboard_wasm.c \
		-o frontend/vasuki.js \
		-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
		-s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 \
		-s EXPORT_NAME='VasukiModule' -s ENVIRONMENT='web' \
		-std=c99 -O2

# Serve frontend locally
serve: wasm
	cd frontend && python3 -m http.server 8080

.PHONY: all clean run wasm serve
