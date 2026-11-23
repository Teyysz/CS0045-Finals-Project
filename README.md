# Campus Drip — Modular Source

The original single-file program is separated into four responsibilities while preserving its behavior.

| Module | Files | Responsibility |
|---|---|---|
| Application core | `App.h`, `App.cpp` | Shared state, themes, camera, display loop, initialization, and `main()` |
| Campus world | `World.h`, `World.cpp` | Drawing helpers, VBO, lighting, ground, buildings, trees, and props |
| Character | `Character.h`, `Character.cpp` | Body, face, hair, clothes, shoes, accessories, and character assembly |
| Interface and controls | `Interface.h`, `Interface.cpp` | Bitmap text, menus, screen overlays, keyboard controls, mouse orbit, and zoom |

## Build and run

1. Put this folder inside your **Final Project** folder. The script searches one or two parent folders for `vcpkg_installed`.
2. Open `CampusDrip-Modular` itself as the VS Code folder.
3. Press **Ctrl+Shift+B** to run **Build Campus Drip (all modules)**.

Alternatively, open a Bash terminal and run:

```bash
cmd.exe /c build-modular.bat
```

4. Run the program from Bash:

```bash
./Campus_Drip.exe
```

Do not build only the active `.cpp` file. A modular C++ program must compile and link all four implementation files together.

## Suggested ownership for four members

- Member 1: `App.cpp` — camera behavior, screen transitions, scene timing.
- Member 2: `World.cpp` — campus scenery, lighting, environment themes.
- Member 3: `Character.cpp` — outfits, hair, accessories, character details.
- Member 4: `Interface.cpp` — UI layout, text, controls, interaction feedback.

Headers should mostly contain declarations. Coordinate changes to `App.h` because every module includes it.

## Safe improvement workflow

1. Create a feature branch.
2. Edit primarily the module assigned to you.
3. Build and test before committing.
4. Commit source files only; generated `.exe`, `.obj`, `.pdb`, `.ilk`, and `.dll` files should remain ignored.
5. Pull/rebase before pushing, then open a pull request.

## Good next improvements

- Add more outfit choices inside `Character.cpp`.
- Add selection animations and clearer control hints inside `Interface.cpp`.
- Add campus props or refine the three themes inside `World.cpp`.
- Add controlled character movement or improved camera easing inside `App.cpp`.
- Later, move shared state into a dedicated struct if the class covers object-oriented design.
