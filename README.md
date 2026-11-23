# Campus Drip: 3D Pixel Character Customizer

Campus Drip is a 3D character-customization program written in C++ using legacy OpenGL, GLEW, and FreeGLUT. Players can create a block-style campus character, change its outfit and background, and freely rotate the camera around the finished look.

The project stays within the concepts introduced in **CS0045 Modules 1–8**: primitives, callbacks, vertex arrays and VBOs, transformations, perspective viewing, depth testing, lighting, and bitmap text.

## Current features

- Block-style 3D male and female base characters
- Customizable hair, top, bottom, shoes, and accessory
- Night, sunset, and daytime campus themes
- Campus environment with buildings, trees, benches, and lamps
- Mouse-controlled orbit camera and zoom
- Perspective projection, depth testing, and basic lighting
- VBO-rendered ground
- Welcome, customization, and outfit-confirmation screens
- On-screen instructions using GLUT bitmap text

## Project modules

| Module | Files | Responsibility |
|---|---|---|
| Application core | `App/App.cpp`, `App/App.h` | Shared state, themes, camera, display loop, initialization, and `main()` |
| Campus world | `World/World.cpp`, `World/World.h` | Drawing helpers, VBO setup, lighting, ground, buildings, trees, and props |
| Character | `Character/Character.cpp`, `Character/Character.h` | Body, face, hair, clothes, shoes, accessories, and character assembly |
| Interface and controls | `Interface/Interface.cpp`, `Interface/Interface.h` | Bitmap text, menus, overlays, keyboard controls, mouse orbit, and zoom |

## Folder structure

```text
Final Project/
|-- App/
|   |-- App.cpp
|   `-- App.h
|-- World/
|   |-- World.cpp
|   `-- World.h
|-- Character/
|   |-- Character.cpp
|   `-- Character.h
|-- Interface/
|   |-- Interface.cpp
|   `-- Interface.h
|-- .vscode/
|   `-- tasks.json
|-- build-modular.bat
|-- .gitignore
`-- README.md
```

## Requirements

- Windows 10 or Windows 11
- Visual Studio Build Tools 2026 with **Desktop development with C++**
- VS Code with the Microsoft C/C++ extension
- vcpkg installation containing GLEW, FreeGLUT, and OpenGL

The build script expects `vcpkg_installed` beside the project:

```text
SOURCE CODE/
|-- Final Project/
`-- vcpkg_installed/
    `-- x64-windows/
```

## Build and run

1. Open the **Final Project** folder itself in VS Code.
2. Press **Ctrl+Shift+B**.
3. Select **Build Campus Drip (all modules)** if prompted.
4. Wait for `Build completed successfully.`
5. Run from Bash:

```bash
./Campus_Drip.exe
```

In Windows Command Prompt, run:

```cmd
Campus_Drip.exe
```

Do not select a generic **build active file** task. All four implementation files must be compiled and linked together.

## Controls

| Control | Action |
|---|---|
| Enter | Start from the welcome screen |
| Up/Down Arrow | Move through customization categories |
| Left/Right Arrow | Change the selected option |
| G | Switch between male and female bases |
| C | Confirm the outfit and enter showcase mode |
| Backspace | Return to outfit customization |
| Hold Left Mouse + Drag | Orbit the camera |
| Mouse Wheel | Zoom in or out |
| R | Reset the camera |
| Esc | Exit |

## Suggested responsibility for four members

- **Member 1 — App:** camera behavior, screen transitions, shared state, and timing
- **Member 2 — World:** campus scenery, VBO ground, lighting, and themes
- **Member 3 — Character:** body design, outfits, hair, shoes, and accessories
- **Member 4 — Interface:** menus, text, layout, controls, and interaction feedback

Coordinate edits to `App/App.h` because it contains shared declarations used by the other modules.

## Git workflow

Create a feature branch before changing a module:

```bash
git switch main
git pull origin main
git switch -c feature/character-improvements
```

After making and testing changes:

```bash
git add Character
git commit -m "Improve character customization"
git push -u origin feature/character-improvements
```

Create a pull request on GitHub and merge only after another member reviews and tests the change. Generated `.exe`, `.dll`, `.obj`, `.pdb`, and `.ilk` files should not be committed.

## Coding guidelines

- Keep improvements inside their corresponding module whenever possible.
- Put declarations in header files and implementations in `.cpp` files.
- Reuse existing drawing helpers instead of duplicating OpenGL code.
- Match every `glPushMatrix()` with `glPopMatrix()`.
- Reset temporary OpenGL states after use.
- Build and run the complete program before committing.
- Stay within the covered course modules unless the group and instructor approve otherwise.

## Possible improvements

- Add more campus-themed outfits, hairstyles, and accessories
- Add clearer selection feedback and small UI animations
- Add more campus props and environmental details
- Add simple character poses or movement after confirmation
- Improve camera easing and zoom limits
- Add a randomize-outfit option
- Add save/load functionality if allowed by the project scope

## Known limitations

- The program uses legacy OpenGL to follow the course modules.
- The character and environment use basic block geometry.
- No external texture assets are currently required.
- The build script targets the group’s Visual Studio 2026 and vcpkg folder layout.

