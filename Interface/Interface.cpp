#include "Interface.h"

// ============================================================
// MODULE 4 OF 4: INTERFACE AND CONTROLS
// Owns bitmap text, menus, keyboard input, and mouse input.
// ============================================================

// ============================================================
// MODULES 1-3: 2D INTERFACE HELPERS
// Only GL_QUADS, GL_LINE_LOOP, glColor3f, glRasterPos2f and
// glutBitmapString are used here.
// ============================================================
void drawRect(float x0, float y0, float x1, float y1, const Color& c)
{
    glColor3f(c.r, c.g, c.b);
    glBegin(GL_QUADS);
        glVertex2f(x0, y0);
        glVertex2f(x1, y0);
        glVertex2f(x1, y1);
        glVertex2f(x0, y1);
    glEnd();
}

void drawOutline(float x0, float y0, float x1, float y1,
                 const Color& c, float lineWidth)
{
    glColor3f(c.r, c.g, c.b);
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x0, y0);
        glVertex2f(x1, y0);
        glVertex2f(x1, y1);
        glVertex2f(x0, y1);
    glEnd();
    glLineWidth(1.0f);
}

// Pixel-style panel: shadow, fill, border and corner pixels
void drawPixelPanel(float x0, float y0, float x1, float y1,
                    const Color& fill, const Color& border)
{
    drawRect(x0 + 5.0f, y0 - 5.0f, x1 + 5.0f, y1 - 5.0f, rgb(0.02f, 0.02f, 0.06f));
    drawRect(x0, y0, x1, y1, fill);
    drawOutline(x0, y0, x1, y1, border, 3.0f);

    drawRect(x0 - 3.0f, y0 - 3.0f, x0 + 5.0f, y0 + 5.0f, border);
    drawRect(x1 - 5.0f, y0 - 3.0f, x1 + 3.0f, y0 + 5.0f, border);
    drawRect(x0 - 3.0f, y1 - 5.0f, x0 + 5.0f, y1 + 3.0f, border);
    drawRect(x1 - 5.0f, y1 - 5.0f, x1 + 3.0f, y1 + 3.0f, border);
}

// MODULE 3: bitmap text (the color must be set before glRasterPos2f)
void drawText(float x, float y, void* font, const char* text, const Color& c)
{
    glColor3f(c.r, c.g, c.b);
    glRasterPos2f(x, y);
    glutBitmapString(font, reinterpret_cast<const unsigned char*>(text));
}

float textWidth(void* font, const char* text)
{
    return (float)glutBitmapLength(font, reinterpret_cast<const unsigned char*>(text));
}

void drawTextCentered(float centerX, float y, void* font,
                      const char* text, const Color& c)
{
    drawText(centerX - textWidth(font, text) * 0.5f, y, font, text, c);
}

void drawTextRight(float rightX, float y, void* font,
                   const char* text, const Color& c)
{
    drawText(rightX - textWidth(font, text), y, font, text, c);
}

// Bottom instruction bar shared by the screens
void drawBottomBar(const char* line1, const char* line2)
{
    float w = (float)windowWidth;
    drawRect(0.0f, 0.0f, w, 80.0f, UI_BG);
    drawRect(0.0f, 80.0f, w, 84.0f, UI_BORDER);
    drawText(24.0f, 50.0f, GLUT_BITMAP_HELVETICA_12, line1, UI_TEXT);
    drawText(24.0f, 26.0f, GLUT_BITMAP_HELVETICA_12, line2, UI_DIM);
}

// ============================================================
// SCREEN: WELCOME
// ============================================================
void drawWelcomeScreen()
{
    float w = (float)windowWidth;
    float h = (float)windowHeight;
    float cx = w * 0.5f;

    // Title banner
    drawRect(0.0f, h - 110.0f, w, h, UI_BG);
    drawRect(0.0f, h - 114.0f, w, h - 110.0f, UI_BORDER);
    drawTextCentered(cx, h - 55.0f, GLUT_BITMAP_TIMES_ROMAN_24, "CAMPUS DRIP", UI_GOLD);
    drawTextCentered(cx, h - 88.0f, GLUT_BITMAP_HELVETICA_18,
                     "3D PIXEL CHARACTER CUSTOMIZER", UI_ACCENT);

    // Info panel at the bottom
    float panelW = (w - 40.0f < 700.0f) ? (w - 40.0f) : 700.0f;
    float x0 = cx - panelW * 0.5f;
    float x1 = cx + panelW * 0.5f;
    drawPixelPanel(x0, 20.0f, x1, 130.0f, UI_PANEL, UI_BORDER);

    drawTextCentered(cx, 108.0f, GLUT_BITMAP_HELVETICA_12,
        "Mix and match hair, outfits, shoes and accessories on a 3D voxel student.", UI_TEXT);
    drawTextCentered(cx, 88.0f, GLUT_BITMAP_HELVETICA_12,
        "Choose a Night, Sunset or Day campus and rotate the preview with the mouse.", UI_DIM);

    // Blinking start prompt
    bool blinkOn = ((glutGet(GLUT_ELAPSED_TIME) / 500) % 2) == 0;
    Color promptColor = blinkOn ? UI_GREEN : UI_DIM;
    drawOutline(cx - 130.0f, 30.0f, cx + 130.0f, 62.0f, promptColor, 2.0f);
    drawTextCentered(cx, 40.0f, GLUT_BITMAP_HELVETICA_18, "PRESS ENTER TO START", promptColor);
}

// ============================================================
// SCREEN: CUSTOMIZE
// ============================================================
void drawCustomizeScreen()
{
    float w = (float)windowWidth;
    float h = (float)windowHeight;

    // Header
    drawRect(0.0f, h - 70.0f, w, h, UI_BG);
    drawRect(0.0f, h - 74.0f, w, h - 70.0f, UI_BORDER);
    drawText(24.0f, h - 36.0f, GLUT_BITMAP_HELVETICA_18,
             "CAMPUS DRIP: 3D PIXEL CHARACTER CUSTOMIZER", UI_ACCENT);
    drawText(24.0f, h - 58.0f, GLUT_BITMAP_HELVETICA_12,
             "Create your campus look and rotate the preview freely.", UI_DIM);

    // Right-hand customization panel (width follows the window width)
    float panelW = clampFloat(w * 0.27f, 250.0f, 340.0f);
    float px1 = w - 20.0f;
    float px0 = px1 - panelW;
    float py0 = 100.0f;
    float py1 = h - 90.0f;
    drawPixelPanel(px0, py0, px1, py1, UI_PANEL, UI_BORDER);

    // Base character
    drawText(px0 + 18.0f, py1 - 34.0f, GLUT_BITMAP_HELVETICA_18,
             genderChoice == 0 ? "BASE: MALE" : "BASE: FEMALE", UI_TEXT);
    drawText(px0 + 18.0f, py1 - 54.0f, GLUT_BITMAP_HELVETICA_12,
             "PRESS G TO SWITCH BASE", UI_DIM);
    drawRect(px0 + 14.0f, py1 - 66.0f, px1 - 14.0f, py1 - 63.0f, UI_BORDER);

    // Category list: row height adapts to the window height
    float boxTop = py0 + 120.0f;
    float startY = py1 - 100.0f;
    float rowH = clampFloat((startY - boxTop - 25.0f) / 5.0f, 24.0f, 50.0f);

    for (int i = 0; i < CATEGORY_COUNT; i++)
    {
        float yc = startY - i * rowH;
        bool isSelected = (i == selectedCategory);

        if (isSelected)
        {
            drawRect(px0 + 10.0f, yc - rowH * 0.5f + 2.0f,
                     px1 - 10.0f, yc + rowH * 0.5f - 2.0f, UI_HILITE);
            drawOutline(px0 + 10.0f, yc - rowH * 0.5f + 2.0f,
                        px1 - 10.0f, yc + rowH * 0.5f - 2.0f, UI_ACCENT, 2.0f);
            drawRect(px0 + 14.0f, yc - 4.0f, px0 + 22.0f, yc + 4.0f, UI_ACCENT);
        }

        Color nameColor = isSelected ? UI_ACCENT : UI_DIM;
        Color valueColor = isSelected ? UI_GOLD : rgb(0.60f, 0.60f, 0.80f);

        drawText(px0 + 30.0f, yc - 6.0f, GLUT_BITMAP_HELVETICA_18,
                 categoryNames[i], nameColor);
        drawTextRight(px1 - 18.0f, yc - 4.0f, GLUT_BITMAP_HELVETICA_12,
                      optionNames[i][selection[i]], valueColor);
    }

    // Current option box
    float bx0 = px0 + 14.0f;
    float bx1 = px1 - 14.0f;
    float bcx = (bx0 + bx1) * 0.5f;
    drawPixelPanel(bx0, py0 + 14.0f, bx1, boxTop, rgb(0.04f, 0.03f, 0.12f), UI_ACCENT);

    int count = choiceCounts[selectedCategory];
    int current = selection[selectedCategory];

    drawTextCentered(bcx, boxTop - 22.0f, GLUT_BITMAP_HELVETICA_12,
                     categoryNames[selectedCategory], UI_DIM);
    drawTextCentered(bcx, boxTop - 48.0f, GLUT_BITMAP_HELVETICA_18,
                     optionNames[selectedCategory][current], UI_GOLD);

    char counter[32];
    snprintf(counter, sizeof(counter), "%d / %d", current + 1, count);
    drawTextCentered(bcx, boxTop - 68.0f, GLUT_BITMAP_HELVETICA_12, counter, UI_TEXT);

    drawText(bx0 + 14.0f, boxTop - 52.0f, GLUT_BITMAP_HELVETICA_18, "<", UI_TEXT);
    drawTextRight(bx1 - 14.0f, boxTop - 52.0f, GLUT_BITMAP_HELVETICA_18, ">", UI_TEXT);

    // Choice dots
    float dotsWidth = count * 18.0f - 8.0f;
    float dotX = bcx - dotsWidth * 0.5f;
    for (int i = 0; i < count; i++)
    {
        Color dotColor = (i == current) ? UI_GOLD : rgb(0.30f, 0.28f, 0.50f);
        drawRect(dotX + i * 18.0f, boxTop - 96.0f,
                 dotX + i * 18.0f + 10.0f, boxTop - 86.0f, dotColor);
    }

    drawBottomBar(
        "UP/DOWN: Category    LEFT/RIGHT: Change option    G: Switch base    C: Confirm outfit",
        "MOUSE DRAG: Orbit camera    MOUSE WHEEL: Zoom    R: Reset view    ESC: Exit");
}

// ============================================================
// SCREEN: CONFIRMED / SHOWCASE
// ============================================================
void drawShowcaseScreen()
{
    float w = (float)windowWidth;
    float h = (float)windowHeight;

    // Confirmation banner
    drawRect(0.0f, h - 70.0f, w, h, UI_BG);
    drawRect(0.0f, h - 74.0f, w, h - 70.0f, UI_GREEN);
    drawTextCentered(w * 0.5f, h - 38.0f, GLUT_BITMAP_HELVETICA_18,
                     "OUTFIT CONFIRMED!", UI_GREEN);
    drawTextCentered(w * 0.5f, h - 58.0f, GLUT_BITMAP_HELVETICA_12,
                     "Your campus look is ready. Drag the mouse to admire it from every side.",
                     UI_DIM);

    // Summary card
    float x0 = 20.0f;
    float x1 = 300.0f;
    float y1 = h - 100.0f;
    float y0 = y1 - 290.0f;
    drawPixelPanel(x0, y0, x1, y1, UI_PANEL, UI_GREEN);

    drawText(x0 + 16.0f, y1 - 32.0f, GLUT_BITMAP_HELVETICA_18, "YOUR LOOK", UI_ACCENT);
    drawRect(x0 + 12.0f, y1 - 44.0f, x1 - 12.0f, y1 - 41.0f, UI_BORDER);

    float rowY = y1 - 72.0f;
    drawText(x0 + 16.0f, rowY, GLUT_BITMAP_HELVETICA_12, "BASE", UI_DIM);
    drawTextRight(x1 - 16.0f, rowY, GLUT_BITMAP_HELVETICA_18,
                  genderChoice == 0 ? "MALE" : "FEMALE", UI_GOLD);

    for (int i = 0; i < CATEGORY_COUNT; i++)
    {
        rowY = y1 - 72.0f - (i + 1) * 30.0f;
        drawText(x0 + 16.0f, rowY, GLUT_BITMAP_HELVETICA_12, categoryNames[i], UI_DIM);
        drawTextRight(x1 - 16.0f, rowY, GLUT_BITMAP_HELVETICA_18,
                      optionNames[i][selection[i]], UI_GOLD);
    }

    drawBottomBar(
        "BACKSPACE: Edit outfit    R: Reset view    MOUSE DRAG: Orbit camera    MOUSE WHEEL: Zoom",
        "ESC: Exit");
}

// ============================================================
// MODULE 7: 2D INTERFACE PASS (orthographic projection)
// Lighting and depth testing are turned off, then restored.
// The matrices are pushed/popped so the 3D camera is untouched
// and the interface never rotates with the scene.
// ============================================================
void drawInterface()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if (screenState == SCREEN_WELCOME)
        drawWelcomeScreen();
    else if (screenState == SCREEN_CUSTOMIZE)
        drawCustomizeScreen();
    else
        drawShowcaseScreen();

    glPopMatrix();                      // MODELVIEW

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();                      // PROJECTION

    glMatrixMode(GL_MODELVIEW);
    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ============================================================
// MODULE 3: KEYBOARD CALLBACKS
// ============================================================
void keyboard(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    if (key == 27)                                  // ESC
        exit(0);

    if (key == 13 && screenState == SCREEN_WELCOME) // ENTER
    {
        screenState = SCREEN_CUSTOMIZE;
        resetCamera();
    }
    else if ((key == 'g' || key == 'G') && screenState == SCREEN_CUSTOMIZE)
    {
        genderChoice = 1 - genderChoice;
    }
    else if ((key == 'c' || key == 'C') && screenState == SCREEN_CUSTOMIZE)
    {
        screenState = SCREEN_SHOWCASE;
        normalizeCameraYaw();
        targetPitch = 10.0f;
        targetDistance = 10.5f;
    }
    else if (key == 8 && screenState == SCREEN_SHOWCASE)  // BACKSPACE
    {
        screenState = SCREEN_CUSTOMIZE;
        resetCamera();
    }
    else if (key == 'r' || key == 'R')
    {
        resetCamera();
    }

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y)
{
    (void)x;
    (void)y;

    if (screenState != SCREEN_CUSTOMIZE)
        return;

    if (key == GLUT_KEY_UP)
    {
        selectedCategory = (selectedCategory - 1 + CATEGORY_COUNT) % CATEGORY_COUNT;
    }
    else if (key == GLUT_KEY_DOWN)
    {
        selectedCategory = (selectedCategory + 1) % CATEGORY_COUNT;
    }
    else if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT)
    {
        int direction = (key == GLUT_KEY_RIGHT) ? 1 : -1;
        int count = choiceCounts[selectedCategory];

        // Wrap around inside the valid range for this category
        selection[selectedCategory] =
            (selection[selectedCategory] + direction + count) % count;
    }

    glutPostRedisplay();
}

// ============================================================
// MODULE 3: MOUSE CALLBACKS
// Dragging changes the orbit-camera targets; the wheel zooms.
// ============================================================
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        dragging = (state == GLUT_DOWN);
        previousMouseX = x;
        previousMouseY = y;
    }

    // FreeGLUT reports the mouse wheel as buttons 3 and 4
    if (state == GLUT_DOWN && button == 3)
        targetDistance -= 0.6f;
    else if (state == GLUT_DOWN && button == 4)
        targetDistance += 0.6f;

    targetDistance = clampFloat(targetDistance, 6.0f, 18.0f);

    glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
    if (!dragging)
        return;

    targetYaw += (x - previousMouseX) * 0.5f;
    targetPitch += (y - previousMouseY) * 0.4f;     // drag up = look from above
    targetPitch = clampFloat(targetPitch, 0.0f, 75.0f);

    previousMouseX = x;
    previousMouseY = y;

    glutPostRedisplay();
}
