# Repository Guidelines

This repository is a small C++17 console RPG. Source files live at the repo root alongside headers; there is no build system checked in. Keep changes minimal, cross‑platform, and consistent with current style.

## Project Structure & Module Organization
- Source: `*.cpp` and `*.h` in the root (entry: `main.cpp`).
- Core modules: `Game.h`, `Player.h`, `Map.h`, `Battle.h`, `Enemy.h`, `Item.*`, `Skill.*`, `Colors.*`, `AudioManager.*`, `SaveGame.h`, `Room.h`, `AsciiMap.h`.
- Third‑party: `miniaudio.h` (header‑only).
- Binary: `game_latest.exe` (example build). Assets are expected under `music/` (see `AudioManager`, e.g., `music/DDRKirby(ISQ) - Dawn.mp3`).

## Build, Test, and Development Commands
- Windows (MinGW): `g++ -std=c++17 -O2 -Wall main.cpp AudioManager.cpp Colors.cpp Item.cpp Skill.cpp -o game.exe`
- Windows (MSVC): `cl /std:c++17 /EHsc /O2 main.cpp AudioManager.cpp Colors.cpp Item.cpp Skill.cpp`
- Linux/macOS (Clang/GCC): `g++ -std=c++17 -O2 -Wall main.cpp AudioManager.cpp Colors.cpp Item.cpp Skill.cpp -o game`
- Run: `./game.exe` (Windows) or `./game` (Unix). Ensure the `music/` folder exists next to the binary.
- Debug build tip: add `-g` (GCC/Clang) or `/Zi` (MSVC).

## Coding Style & Naming Conventions
- C++17, 4‑space indentation, brace on same line.
- Classes: `PascalCase` (e.g., `Game`); methods/functions: `camelCase` (e.g., `gameLoop`); members are simple lowerCamel (e.g., `gameMap`).
- Header guards as used in existing headers. Prefer current code’s patterns; avoid introducing new dependencies.

## Testing Guidelines
- No formal test suite yet. Prefer small, buildable demos and assertions.
- Example: `g++ -std=c++17 -O2 map_demo.cpp Colors.cpp -o map_demo` to exercise map rendering.
- Keep new demos self‑contained and in root unless a `tests/` folder is introduced.

## Commit & Pull Request Guidelines
- Commits: clear, focused, imperative mood (optionally Conventional Commits, e.g., `feat: add dodge skill`).
- PRs: include summary, rationale, build/run instructions, and any screenshots/text output. Link related issues.
- Verify: builds cleanly on Windows and one Unix‑like environment; no warnings added at `-Wall`.

## Agent‑Specific Instructions
- Keep file layout flat (root). Do not rename or move existing files without discussion.
- Preserve cross‑platform macros (`_WIN32`, termios) and audio integration via `miniaudio.h`.
- Minimal patches: touch only what you change; avoid unrelated refactors.
