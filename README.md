# Vasuki - Terminal Snake Game

A real-time Snake game built entirely in C from scratch, using **custom-written libraries** — no `<string.h>`, `<math.h>`, or default `malloc()`/`free()`.

**Operating Systems Final Capstone Project.**

---

## Team

| Name             | Enrollment No. |
|------------------|----------------|
| Raman Luhach     | 230107         |
| Rachit Kumar     | 230128         |
| Harshal Nerpagar | 230076         |

---

## Build and Run

```bash
make          # Build terminal game
make run      # Build and run
make wasm     # Build web frontend (requires emscripten)
make serve    # Build WASM and start local server
make clean    # Remove build artifacts
```

---

## Project Structure

```
vasuki/
├── config/       Config constants (scoring, speed, levels)
├── math/         Bit manipulation arithmetic (shift-and-add, binary long division)
├── string/       String manipulation (no string.h)
├── memory/       Custom allocator (64KB pool, first-fit, coalescing)
├── screen/       Terminal rendering (ANSI escape codes)
├── keyboard/     Non-blocking input (POSIX termios, raw mode)
├── timer/        System clock (gettimeofday)
├── random/       PRNG seeded from /dev/urandom (xorshift32)
├── fileio/       Raw file I/O (open/read/write/close syscalls)
├── audio/        Non-blocking sound (fork + exec)
├── game/         Game logic + main entry point
├── sounds/       Sound assets
└── frontend/     Web frontend (C compiled to WASM via Emscripten)
```

---

## Controls

| Key | Action |
|-----|--------|
| `W/A/S/D` or Arrow Keys | Move |
| `P` | Pause |
| `Q` / `ESC` | Quit |

---

## Features

- Two game modes: Endless and Level (5 levels)
- Wrap-around and walled levels with obstacles
- Bonus food, combo multiplier system
- Speed increases over time
- High score persistence (file I/O)
- Sound effects (fork+exec)
- Color gradient snake, blinking food, trail effect, death flash
- Web frontend using actual C code compiled to WebAssembly

---

## OS Concepts Demonstrated

| Library | OS Concept |
|---------|-----------|
| memory.c | Heap management, first-fit allocation, fragmentation |
| keyboard.c | Terminal I/O, raw mode, file descriptors (termios) |
| timer.c | System clocks, gettimeofday syscall |
| random.c | /dev/urandom device file, entropy |
| fileio.c | File descriptors, open/read/write/close syscalls |
| audio.c | Process creation (fork), exec, child processes |
| math.c | Bit manipulation (shift-and-add multiply, binary long division) |
