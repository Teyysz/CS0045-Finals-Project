# Campus Drip

**Campus Drip** is an interactive 3D pixel-character customizer developed in C++ with legacy OpenGL, GLEW, and FreeGLUT. It allows users to create a campus-inspired character, preview different outfit combinations, change the campus atmosphere, and view the finished character from any angle.

This project was created as the final requirement for **CS0045 – Computer Graphics**. Its implementation applies the concepts covered in Modules 1–8, including graphical primitives, event-driven interaction, vertex arrays, vertex buffer objects, transformations, camera viewing, lighting, and bitmap text.

## Features

- Male and female block-style character bases
- Six customization categories:
  - Hair: Classic, Side Part, or Spiky
  - Top: Varsity, School, or Tambay
  - Bottom: Cargo, Jeans, or Shorts
  - Shoes: Sneakers, Runners, or Dark Shoes
  - Accessory: None, Glasses, Cap, or Backpack
  - Background: Night Campus, Sunset, or Day Campus
- Campus environment with buildings, pathways, trees, benches, and lamp posts
- Theme-dependent sky colors, lighting, windows, and environmental details
- Welcome, customization, and outfit-showcase screens
- Mouse-controlled orbit camera with smooth movement and zoom
- Automatic character rotation on the welcome and showcase screens
- Small showcase animation after confirming an outfit
- Responsive on-screen menus, controls, and outfit summary
- VBO and vertex-array rendering for the campus ground and pathways
- Perspective projection, depth testing, lighting, material properties, and emissive objects

## Controls

| Input | Action |
|---|---|
| `Enter` | Continue from the welcome screen |
| `Up Arrow` / `Down Arrow` | Select a customization category |
| `Left Arrow` / `Right Arrow` | Change the option in the selected category |
| `G` | Switch between the male and female character bases |
| `C` | Confirm the outfit and open the showcase screen |
| `Backspace` | Return from the showcase screen to customization |
| Hold `Left Mouse Button` + drag | Orbit the camera around the character |
| Mouse wheel | Zoom in or out |
| `R` | Reset the camera |
| `Esc` | Exit the program |

## Project Structure

```text
CampusDrip-Modular/
├── App/
│   ├── App.cpp
│   └── App.h
├── Character/
│   ├── Character.cpp
│   └── Character.h
├── Interface/
│   ├── Interface.cpp
│   └── Interface.h
├── World/
│   ├── World.cpp
│   └── World.h
├── .vscode/
│   └── tasks.json
├── build-modular.bat
├── .gitignore
└── README.md
```

| Component | Responsibility |
|---|---|
| `App` | Shared program state, camera, themes, display loop, initialization, and program entry point |
| `Character` | Character body, face, hairstyles, clothing, shoes, accessories, and final assembly |
| `Interface` | Bitmap text, screen overlays, customization menus, keyboard input, mouse input, and zoom |
| `World` | Reusable drawing helpers, VBO setup, lighting, ground, buildings, trees, benches, lamps, and sky decorations |

## Requirements

- Windows 10 or Windows 11
- Visual Studio Code with the Microsoft C/C++ extension
- Visual Studio Build Tools with **Desktop development with C++**
- C++17-compatible Microsoft C++ compiler
- GLEW
- FreeGLUT
- OpenGL and GLU
- A `vcpkg_installed` folder containing the x64 Windows dependencies

The included build script expects the following layout:

```text
SOURCE CODE/
├── CampusDrip-Modular/
└── vcpkg_installed/
    └── x64-windows/
        ├── bin/
        ├── include/
        └── lib/
```

The script currently uses the Visual Studio 2026 Build Tools path. If another Visual Studio version is installed, update the `VsDevCmd.bat` path inside `build-modular.bat`.

## Build and Run

### Using Visual Studio Code

1. Open the `CampusDrip-Modular` folder in Visual Studio Code.
2. Press `Ctrl + Shift + B`.
3. Select **Build Campus Drip (all modules)** if VS Code asks for a build task.
4. Wait for the terminal to display `Build completed successfully.`
5. Run the generated program:

```bash
./Campus_Drip.exe
```

In Windows Command Prompt, use:

```cmd
Campus_Drip.exe
```

### Using the Build Script

Run the included script from the project folder:

```cmd
build-modular.bat
```

The build compiles and links all four implementation files, creates `Campus_Drip.exe`, and copies the required GLEW and FreeGLUT DLL files into the project folder.

Do not use a generic **build active file** task because the program depends on all four `.cpp` modules.

## Computer Graphics Concepts Applied

| Module | Application in Campus Drip |
|---|---|
| Modules 1–2 | Colors, coordinates, graphical primitives, cubes, and construction of complex objects from simple shapes |
| Module 3 | Keyboard callbacks, special-key callbacks, mouse callbacks, timers, redisplay, and GLUT bitmap text |
| Module 4 | Vertex arrays and `glDrawArrays()` for the campus ground and pathways |
| Module 5 | A vertex buffer object that stores the ground and pathway vertices in GPU memory |
| Module 6 | Translation, rotation, scaling, matrix stacks, character assembly, and showcase movement |
| Module 7 | Perspective projection, `gluLookAt()`, orbit-camera calculations, viewport resizing, and depth testing |
| Module 8 | Directional lighting, ambient and diffuse colors, materials, normalization, smooth shading, and emission |

## Technical Notes

- The project uses the fixed-function OpenGL pipeline to remain aligned with the course coverage.
- The character and environment are built procedurally from cubes and other basic shapes; no imported 3D models are required.
- No external image, texture, sound, shader, database, or online service is needed.
- `App/App.h` contains declarations shared by all modules, so changes to it may affect the entire project.
- Every `glPushMatrix()` must have a matching `glPopMatrix()` to prevent transformations from affecting unrelated objects.
- Temporary OpenGL states, such as material emission, are reset after use.

## Testing Checklist

Before submitting or presenting the project, verify that:

- The project builds without compiler or linker errors.
- The welcome screen opens and `Enter` starts customization.
- Every category can be selected with the up and down arrow keys.
- Every option can be changed with the left and right arrow keys.
- Both character bases display correctly.
- Glasses, cap, and backpack appear correctly from multiple camera angles.
- Night, sunset, and day themes update the environment and lighting.
- Mouse dragging orbits the camera smoothly.
- Mouse-wheel zoom remains within its limits.
- `R` restores the default camera position.
- `C` opens the showcase screen and `Backspace` returns to customization.
- The interface remains readable after resizing the window.
- `Esc` closes the application normally.

## Known Limitations

- The application is currently designed and built for Windows.
- It uses legacy OpenGL instead of modern shaders to match the course modules.
- The models intentionally use block-based geometry and simple animations.
- Custom selections remain available only during the current program session; there is no save/load feature.
- The build script assumes a specific Visual Studio Build Tools and local vcpkg folder arrangement.

## Contributors

Developed as a group final project for **CS0045 – Computer Graphics**. Add the names and assigned responsibilities of each member below before submission.

| Member | Responsibility |
|---|---|
| Member 1 | Application core, shared state, camera, and screen transitions |
| Member 2 | Campus environment, lighting, themes, and VBO ground |
| Member 3 | Character models, hairstyles, clothing, shoes, and accessories |
| Member 4 | Interface, controls, interaction feedback, testing, and documentation |
