# Vasuki - Terminal Snake Game

A real-time Snake game built entirely in C from scratch, using **custom-written standard libraries** — no `<string.h>`, `<math.h>`, or default `malloc()`/`free()`.

Built as the **Operating Systems Final Capstone Project**.

---

## Team Members

| Name               | Enrollment No. |
|--------------------|----------------|
| Raman Luhach       | 230107         |
| Rachit Kumar       | 230128         |
| Harshal Nerpagar   | 230076         |

---

## Project Structure

```
vasuki/
├── Makefile                 # Build system
├── math/
│   ├── math.h               # Custom math library (header)
│   └── math.c               # Multiply, divide, modulo, abs, bounds checking
├── string/
│   ├── string.h             # Custom string library (header)
│   └── string.c             # Length, copy, compare, split, int-to-string
├── memory/
│   ├── memory.h             # Custom memory allocator (header)
│   └── memory.c             # 64KB virtual RAM pool, alloc(), free()
├── screen/
│   ├── screen.h             # Terminal rendering library (header)
│   └── screen.c             # ANSI escape codes, cursor, colors, draw box
├── keyboard/
│   ├── keyboard.h           # Keyboard input library (header)
│   └── keyboard.c           # Non-blocking input, arrow key parsing (termios)
└── game/
    ├── game.h               # Snake game logic (header)
    ├── game.c               # Init, input, update, collision, rendering
    └── main.c               # Entry point, welcome screen, game loop
```

---

## How to Build and Run

**Requirements:** macOS or Linux with a C compiler (gcc/clang).

```bash
# Build the game
make

# Build and run
make run

# Clean build files
make clean
```

---

## Controls

| Key              | Action          |
|------------------|-----------------|
| `W` / `Up Arrow` | Move Up        |
| `A` / `Left Arrow` | Move Left    |
| `S` / `Down Arrow` | Move Down    |
| `D` / `Right Arrow` | Move Right  |
| `Q` / `ESC`      | Quit Game       |

---

## Custom Libraries

### 1. math.c
Custom arithmetic functions: `custom_multiply()`, `custom_divide()`, `custom_modulo()`, `custom_abs()`, `is_in_bounds()`, `clamp()`. All include safe division-by-zero handling.

### 2. string.c
String manipulation without `<string.h>`: `str_length()`, `str_copy()`, `str_compare()`, `str_split()`, `int_to_string()`, `string_to_int()`.

### 3. memory.c
A custom memory allocator using a **64KB static array** as virtual RAM. Implements **first-fit allocation** with block splitting and **free-block coalescing** to prevent fragmentation.

### 4. screen.c
Terminal rendering via **ANSI escape codes**: clear screen, move cursor, hide/show cursor, draw characters/strings with 16 colors, draw box outlines.

### 5. keyboard.c
Non-blocking keyboard input using **POSIX termios**. Handles raw terminal mode, arrow key escape sequence parsing (`ESC [ A/B/C/D`), and safe terminal restoration on exit.

---

## Library Integration Pipeline

```
keyboard_pressed()  -->  game_process_input()  -->  game_update()  -->  game_render()
     |                         |                        |                    |
  keyboard.c               game.c                  math.c              screen.c
                                                 (is_in_bounds)       string.c
                                                 (custom_modulo)    (int_to_string)
                                                   memory.c
                                                (mem_alloc/free)
```

All five libraries work together as a pipeline every frame.

---

## Rules Followed

1. **No standard C libraries for core logic** — `<string.h>`, `<math.h>`, and default `malloc()`/`free()` are not used.
2. **No hard-coded values** — all logic is computed dynamically using custom libraries.
3. **Hardware Abstraction Exception** — only `<stdio.h>` (terminal I/O) and `<stdlib.h>` (process exit) are used from standard C. `<termios.h>` and `<unistd.h>` are POSIX system headers for terminal control.

---

## Current Status

- **Phase 1 (Complete):** All five custom libraries implemented and integrated. Basic interactive game loop running. Snake moves with keyboard input, food is rendered, collisions are detected.
- **Phase 2 (Upcoming):** Dynamic memory management for snake segments (linked list with `mem_alloc`/`mem_free`), difficulty scaling, high score tracking, full game polish.
