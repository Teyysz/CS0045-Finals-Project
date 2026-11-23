# Campus Drip: 3D Pixel Character Customizer

Campus Drip is a beginner-friendly 3D character-customization program made in C++ using legacy OpenGL and FreeGLUT. The player can customize a block-style campus character, change its outfit, select a background, and rotate the camera freely around the character.

This is a final project for **CS0045 – Computer Graphics**. The program intentionally stays within the lessons covered in **Modules 1–8**.

## Current Features

- Block-style 3D character made from scaled GLUT cubes
- Male and female base choices
- Three choices for each category:
  - Hair
  - Top
  - Bottom
  - Shoes
  - Background
- Free orbit camera controlled by mouse dragging
- Mouse-wheel zoom
- Simple 3D campus background
- Perspective projection and depth testing
- Basic lighting and material properties
- On-screen customization menu and instructions
- Outfit-confirmation message

## Controls

| Control | Action |
|---|---|
| `Up Arrow` / `Down Arrow` | Move through customization categories |
| `Left Arrow` / `Right Arrow` | Change the selected item |
| `G` | Switch between male and female base characters |
| Hold and drag left mouse button | Revolve the camera around the character |
| Mouse wheel | Zoom in or out |
| `R` | Reset the camera |
| `C` | Confirm the current outfit |
| `Esc` | Close the program |

## Files Needed

The basic version only requires:

```text
Campus-Drip-3D.cpp
```

No image texture, imported model, shader, or sound file is currently required.

## Software Requirements

- Windows 10 or Windows 11
- Visual Studio Code
- Visual Studio Build Tools with Desktop Development with C++
- FreeGLUT installed through vcpkg
- OpenGL and GLU libraries

The current source file includes:

```cpp
#include <GL/freeglut.h>
```

## Building the Program

1. Put `Campus-Drip-3D.cpp` inside the group's OpenGL project folder.
2. Open the project folder in Visual Studio Code.
3. Open `Campus-Drip-3D.cpp` so it is the active file.
4. Press `Ctrl + Shift + B`.
5. Select the group's working FreeGLUT/OpenGL build task if VS Code asks which task to use.
6. Wait for the terminal to show that the build completed successfully.

The existing build task should link these libraries:

```text
freeglut.lib
opengl32.lib
glu32.lib
```

If the build task also links GLEW, that is acceptable, but this version of the program does not directly require GLEW.

## Running the Program in Git Bash

Go to the folder containing the generated `.exe` file:

```bash
cd "/c/Users/T.xiao052106/Documents/Coding/CS0045/SOURCE CODE/Final Project"
```

Run the program:

```bash
./Campus-Drip-3D.exe
```

Change the folder or executable name if your copy uses a different name.

## Module Coverage

| Module | Concepts Used in the Project |
|---|---|
| Modules 1–2 | Colors, graphical primitives, coordinates, and basic object construction |
| Module 3 | Keyboard callbacks, mouse callbacks, redisplay, raster position, and bitmap text |
| Module 4 | Concepts may be added later through a vertex-array object |
| Module 5 | Concepts may be added later through a simple VBO-based object |
| Module 6 | Translation, scaling, matrices, and object transformations |
| Module 7 | Perspective projection, camera viewing, depth testing, and matrix-stack operations |
| Module 8 | Basic lighting, material properties, and optional texture concepts |

The current version mainly demonstrates Modules 1–3 and 6–8. A later improvement can add one appropriate vertex-array or VBO object so Modules 4–5 are also visibly represented.

## Important Scope Limitation

To keep the project within Modules 1–8, contributors should use the fixed-function OpenGL techniques discussed in class.

Allowed project techniques include:

- `glBegin()` and `glEnd()`
- GLUT geometric objects such as `glutSolidCube()`
- Vertex arrays
- Basic VBOs
- `glTranslatef()`, `glRotatef()`, and `glScalef()`
- `glPushMatrix()` and `glPopMatrix()`
- `gluPerspective()` and `gluLookAt()`
- GLUT keyboard and mouse callbacks
- GLUT bitmap text
- Basic OpenGL lighting and materials
- Basic image textures if the group later attempts them

Avoid adding features outside the module scope, such as:

- Modern GLSL shaders
- External game engines
- Physics engines
- Complex skeletal animation
- Imported animated 3D models
- Online accounts or databases
- Advanced shadow-mapping systems

## Current Program Structure

| Function or Section | Responsibility |
|---|---|
| `drawBlock()` | Creates each voxel part using a translated and scaled cube |
| `drawText()` | Displays reusable GLUT bitmap text |
| `initializeScene()` | Enables depth testing, lighting, and material behavior |
| `drawCampusBackground()` | Draws the ground, platform, buildings, and windows |
| `drawHair()` | Draws the selected hairstyle |
| `drawCharacter()` | Draws the complete character and selected outfit |
| `drawInterface()` | Draws the title, customization panel, and controls |
| `display()` | Positions the camera and draws the complete frame |
| `reshape()` | Updates the perspective projection when the window changes size |
| `keyboard()` | Handles ordinary keyboard controls |
| `specialKeys()` | Handles arrow-key customization |
| `mouse()` | Starts camera dragging and controls zoom |
| `mouseMotion()` | Updates the orbit camera while the mouse is dragged |

## Areas for Improvement

Members may choose one or more tasks below. Any improvement should remain within Modules 1–8.

### 1. Improve Character Design

- Refine the head and body proportions.
- Add eyebrows, a mouth, or other simple facial details.
- Make the male and female base designs more visually distinct.
- Improve the hands and shoe shapes using additional cubes.

### 2. Add More Outfit Choices

- Add organization shirts.
- Add sports uniforms.
- Add a school PE uniform.
- Add a hoodie, polo, skirt, or jacket.
- Add more color combinations for existing outfits.

Keep every outfit as a combination of basic OpenGL shapes and transformations.

### 3. Add Accessories

- Glasses
- Cap
- Backpack
- ID lace
- Watch
- Headphones

Accessories should be optional choices and should rotate with the character.

### 4. Improve the Campus Background

- Add trees, benches, lamps, or pathways.
- Improve the school-building design.
- Create clearer visual differences between night, sunset, and daytime.
- Use a vertex array or VBO for one background object.

### 5. Improve the User Interface

- Show the current item number, such as `Top 2/3`.
- Add colored arrows beside the selected option.
- Add a separate starting screen.
- Add a final character-summary screen after confirmation.
- Improve text positioning when the window is resized.

### 6. Add Module 4 Vertex-Array Rendering

Convert one appropriate object, such as the platform or a background sign, to:

```cpp
glEnableClientState(GL_VERTEX_ARRAY);
glVertexPointer(...);
glDrawArrays(...);
glDisableClientState(GL_VERTEX_ARRAY);
```

### 7. Add a Module 5 VBO

Store one simple object in GPU-side memory using:

```cpp
glGenBuffers(...);
glBindBuffer(GL_ARRAY_BUFFER, ...);
glBufferData(...);
```

Keep the VBO addition small and easy to explain during the presentation.

### 8. Improve Lighting

- Adjust the position and color of `GL_LIGHT0`.
- Add a second simple light if permitted.
- Give daytime, sunset, and nighttime backgrounds different lighting values.
- Adjust material properties for clothes, skin, and shoes.

### 9. Optional Basic Textures

If the group decides to attempt Module 8 texture mapping:

- Add a simple school logo to a shirt.
- Add a basic wall or ground texture.
- Keep a non-textured version available as a backup.

Textures are optional and should only be added after the basic version is stable.

### 10. Testing and Cleanup

- Test every keyboard and mouse control.
- Check the program at different window sizes.
- Confirm that the camera cannot zoom too close or too far.
- Remove duplicated code when a reusable function is appropriate.
- Add comments explaining each member's contribution.

## Suggested Task Assignment

| Member | Possible Responsibility |
|---|---|
| Member 1 | Character body, face, and gender-base improvements |
| Member 2 | Hair, clothing, shoes, and accessories |
| Member 3 | Campus background and lighting |
| Member 4 | Interface, text, controls, testing, and documentation |

Adjust the assignments according to the actual number of group members.

## Contribution Guidelines

Before editing:

1. Make a backup of the last working `.cpp` file.
2. Tell the group which section you will modify.
3. Work on one feature at a time.

Before submitting an update:

1. Confirm that the code compiles.
2. Run the program and test the changed feature.
3. Do not remove another member's working feature without discussing it.
4. Add a short comment near the main part you changed.
5. Tell the group what changed and whether any problem remains.

## Known Limitations

- The character is constructed from simple cubes rather than an imported model.
- Clothing choices currently change block arrangements and colors only.
- The background is made from basic shapes and does not use textures.
- The interface is keyboard-controlled and does not yet contain clickable buttons.
- There is no file-saving system for customized characters.
- There is no complex character animation.

These limitations are intentional because the first version must stay understandable, stable, and within the covered modules.

## Main Goal

The priority is to produce a working and explainable OpenGL program. Visual improvements are welcome, but every member should understand the OpenGL functions used in their assigned section.
