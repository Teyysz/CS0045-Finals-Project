# Campus Drip: 3D Pixel Character Customizer

A beginner-friendly C++ / legacy OpenGL / FreeGLUT / GLEW project for CS0045 (Modules 1-8).
Create a 3D low-poly campus student, pick a pose, and keep up to three characters for the current session.

## Screens

Welcome -> Main Menu -> (Create New Character | Saved Characters) -> Customize -> Showcase (poses) -> Save

| Screen | What it does |
|---|---|
| Welcome | "PRESS ANY KEY TO CONTINUE". Any normal key continues, Esc exits. |
| Main Menu | CREATE NEW CHARACTER / SAVED CHARACTERS / EXIT |
| Saved Characters | Three session-only slots: view, edit, create, overwrite, delete |
| Customize | Base (male/female), hair, top, bottom, shoes, accessory, background |
| Showcase | Outfit summary, pose selection, save |

## Controls

| Key / Mouse | Where | Action |
|---|---|---|
| Any normal key | Welcome | Continue to the main menu |
| Up / Down | Main menu | Move selection |
| Enter | Main menu | Choose the highlighted item |
| Up / Down | Saved screen | Select slot |
| Enter or V | Saved screen | View a saved character (or create one in an empty slot) |
| E | Saved screen | Edit the selected character |
| D (press twice) | Saved screen | Delete the selected character |
| Backspace | Saved screen | Back to the main menu |
| Up / Down | Customize | Select category |
| Left / Right | Customize | Change option |
| G | Customize | Switch male/female base |
| C | Customize | Confirm outfit (go to Showcase) |
| Backspace | Customize | Back to the main menu |
| Left / Right | Showcase | Change pose |
| Enter | Showcase | Confirm pose |
| S | Showcase | Save: opens the slot picker (Enter saves, overwriting an occupied slot; Backspace cancels) |
| Backspace | Showcase | Back to Customize |
| M | Showcase | Main menu |
| Mouse drag | All | Orbit camera |
| Mouse wheel | All | Zoom |
| R | All (after Welcome) | Reset the camera for the current screen |
| Esc | All | Exit |

## Saving

Characters are saved for this session only. The three slots are a plain array of structs in memory
(`savedSlots` in `App.cpp`), so they reset when the program closes. Each slot stores: whether it is occupied,
base, hair, top, bottom, shoes, accessory, background, pose and an automatic name ("CHARACTER 1", ...).

## Module responsibilities

| Folder | Responsibility |
|---|---|
| `App/` | Shared state and enums, screen flow, save slots, camera presets and framing, display / reshape / timer callbacks, `main()` |
| `World/` | Shape helpers (blocks, spheres, cylinders, cones, gable roof), VBO ground, lighting, campus buildings, trees, lamps, benches, sky |
| `Character/` | Male and female models, hierarchical limbs, poses, hair, clothing, shoes, accessories |
| `Interface/` | Bitmap text, all screens and menus, keyboard, special-key and mouse input |

Headers include each other with relative paths (for example `#include "../App/App.h"`), so no extra include
directories are required.

## Build

Use the existing VS Code build task (Ctrl+Shift+B), which runs `build-modular.bat`. The four `.cpp` files
(`App/App.cpp`, `World/World.cpp`, `Character/Character.cpp`, `Interface/Interface.cpp`) must all be compiled
and linked together with `glew32.lib`, `freeglut.lib`, `opengl32.lib` and `glu32.lib`
(the `#pragma comment(lib, ...)` lines in `App.cpp` also request them). Keep `glew32.dll` and `freeglut.dll`
next to the `.exe` or on `PATH`.

## Limitations

- Save slots are session-only (no file saving).
- The window has a minimum size of 900 x 640; smaller windows are resized up.
- The campus is a diorama: the far side of the buildings is plain when the camera orbits behind them.
- Wheel zoom relies on FreeGLUT reporting the wheel as buttons 3 and 4.
