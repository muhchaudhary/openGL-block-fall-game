# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Workflow

After every change, always commit and push all changes to the remote repository.

## Build

```bash
make          # build the 'blockfall' executable
make clean    # remove all build artifacts
./blockfall   # run the game
```

Dependencies: freeglut, libGL, libGLU. The `flake.nix` provides a Nix dev shell with all deps (`nix develop`).

There are no tests.

## Architecture

### Data model

The board is a `std::vector<std::vector<Cell>> board` of size `numRows × numCols` (18 × 11, defined in `GLwindow.h`). Each `Cell` holds a `blockType` char:

| char | meaning |
|------|---------|
| `'n'` | empty cell |
| `'T'` `'I'` `'J'` `'L'` `'O'` `'S'` `'Z'` | placed or active block |
| `'p'` | ghost piece (preview drop shadow) |

The active block is also tracked as `std::vector<Point> currBlockPoints` with **5 elements**: indices 0–3 are the four occupied cells; **index 4 is the lower-left anchor** used by the rotation math in `block.cc:rotate()`. Always maintain this invariant when modifying block points.

### File responsibilities

- **`blockfall.cc`** — entry point; owns all global game state (`board`, `currBlockPoints`, `score`, `fallSpeed`, `gameState`); implements GLUT callbacks (`display`, `fps`, `timedDrop`, `key_movement`, `drawFallingBlock`, `char_keys`, `process_Normal_Keys`) and the settings screen renderer.
- **`GLwindow.cc/h`** — low-level OpenGL helpers: `drawCell` (3D beveled block), `drawEmptyCell` (grid cell), `reshape` (aspect-ratio letterboxing). Constants `WIDTH`/`HEIGHT` (900×750) and `numRows`/`numCols` live here.
- **`Cell.cc/h`** — `Cell` class; `draw(offset)` dispatches to `drawCell`/`drawEmptyCell` based on `blockType`.
- **`block.cc/h`** — block geometry definitions, movement (`shift`), rotation (`rotate`), hard drop (`drop`), ghost piece (`drawPreviewDrop`), row clearing (`checkClearRow`), next-piece preview (`drawPreview`).
- **`font.cc/h`** — bitmap raster font for the custom `printString` used in `display()`.

### Timer architecture

Three concurrent GLUT timers drive the game:

1. **`fps`** (≈16 ms) — calls `glutSwapBuffers` + `glutPostRedisplay`; drives the render loop.
2. **`drawFallingBlock`** (`fallSpeed` ms, default 1000) — gravity; attempts `shift(0,1,…)` each tick.
3. **`key_movement`** (≈16 ms) — processes `move_left/right/down`, `drop_down`, `rotate_cc/ccw` flags set by keyboard callbacks.

The `dropped` and `player_moved` booleans coordinate between timers to avoid double-shifting a block in the same frame.

### Coordinate system

OpenGL origin is top-left (`glOrtho(0, WIDTH, HEIGHT, 0, …)`). Board x-offset is `offset = 100`, y-offset is `yOFFSET = 100` (hardcoded in `Cell::draw`). Cell size is 30 px.
