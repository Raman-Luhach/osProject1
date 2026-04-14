# ============================================================================
#                        VASUKI - Terminal Snake Game
#                              Build System
# ============================================================================
#
#  Project Structure:
#    math/       Custom math library
#    string/     Custom string library
#    memory/     Custom memory allocator
#    screen/     Terminal rendering library
#    keyboard/   Keyboard input library
#    game/       Snake game logic + main entry point
#
#  Usage:
#    make          Build the snake executable
#    make run      Build and run the game
#    make clean    Remove all compiled files
#
# ============================================================================

CC       = cc
CFLAGS   = -Wall -Wextra -Werror -std=c99 -g
LDFLAGS  =
TARGET   = vasuki

# ---- Source Files (organized by folder) ----
SRCS     = game/main.c    \
           game/game.c    \
           math/math.c    \
           string/string.c \
           memory/memory.c \
           screen/screen.c \
           keyboard/keyboard.c

# ---- Object Files (built in each folder next to the source) ----
OBJS     = game/main.o    \
           game/game.o    \
           math/math.o    \
           string/string.o \
           memory/memory.o \
           screen/screen.o \
           keyboard/keyboard.o

# ---- Default Target ----
all: $(TARGET)

# ---- Link all object files into the final executable ----
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# ---- Compile each .c file into a .o file ----
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# ---- Dependency Rules ----
# If a header changes, the right .o files get rebuilt.

game/main.o:     game/main.c game/game.h math/math.h string/string.h \
                 memory/memory.h screen/screen.h keyboard/keyboard.h
game/game.o:     game/game.c game/game.h math/math.h string/string.h \
                 memory/memory.h screen/screen.h keyboard/keyboard.h
math/math.o:     math/math.c math/math.h
string/string.o: string/string.c string/string.h
memory/memory.o: memory/memory.c memory/memory.h
screen/screen.o: screen/screen.c screen/screen.h string/string.h
keyboard/keyboard.o: keyboard/keyboard.c keyboard/keyboard.h

# ---- Build and run ----
run: $(TARGET)
	./$(TARGET)

# ---- Remove all build artifacts ----
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean run
