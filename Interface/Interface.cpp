#include "Interface.h"
#include "../Character/Character.h"

// ============================================================
// PART 4 OF 4: INTERFACE AND CONTROLS
// Owns bitmap text, all screens and menus, keyboard input,
// special-key input and mouse input.
// ============================================================

namespace
{
const char* MENU_LABELS[MAIN_MENU_COUNT] =
{
    "CREATE NEW CHARACTER", "SAVED CHARACTERS", "EXIT"
};

int wrapIndex(int value, int count)
{
    return (value % count + count) % count;
}
}

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

// Short feedback line (e.g. "CHARACTER SAVED") above the bottom bar
void drawStatusMessage()
{
    const char* message = getStatus();
    if (message == nullptr)
        return;

    float cx = windowWidth * 0.5f;
    float half = textWidth(GLUT_BITMAP_HELVETICA_18, message) * 0.5f + 20.0f;
    drawPixelPanel(cx - half, 96.0f, cx + half, 130.0f, UI_BG, UI_GREEN);
    drawTextCentered(cx, 107.0f, GLUT_BITMAP_HELVETICA_18, message, UI_GREEN);
}

// ============================================================
// SCREEN: WELCOME
// ============================================================
void drawWelcomeScreen()
{
    float w = (float)windowWidth;
    float h = (float)windowHeight;
    float cx = w * 0.5f;

    // Title banner (drawn twice, one pixel apart, for a bold pixel look)
    drawRect(0.0f, h - 124.0f, w, h, UI_BG);
    drawRect(0.0f, h - 128.0f, w, h - 124.0f, UI_BORDER);
    drawTextCentered(cx + 1.0f, h - 62.0f, GLUT_BITMAP_TIMES_ROMAN_24, "CAMPUS DRIP", UI_GOLD);
    drawTextCentered(cx, h - 62.0f, GLUT_BITMAP_TIMES_ROMAN_24, "CAMPUS DRIP", UI_GOLD);
    drawTextCentered(cx, h - 100.0f, GLUT_BITMAP_HELVETICA_18,
                     "3D PIXEL CHARACTER CUSTOMIZER", UI_ACCENT);

    // Start prompt
    drawPixelPanel(cx - 270.0f, 28.0f, cx + 270.0f, 104.0f, UI_PANEL, UI_BORDER);
    bool blinkOn = ((glutGet(GLUT_ELAPSED_TIME) / 500) % 2) == 0;
    Color promptColor = blinkOn ? UI_GREEN : UI_DIM;
    drawTextCentered(cx, 70.0f, GLUT_BITMAP_HELVETICA_18, "PRESS ANY KEY TO CONTINUE", promptColor);
    drawTextCentered(cx, 44.0f, GLUT_BITMAP_HELVETICA_12, "ESC: EXIT", UI_DIM);
}

// ============================================================
// SCREEN: MAIN MENU
// ============================================================
void drawMainMenuScreen()
{
    float h = (float)windowHeight;
    float px0 = 30.0f;
    float px1 = 30.0f + LEFT_PANEL_W;
    float py0 = 110.0f;
    float py1 = h - 60.0f;
    float cx = (px0 + px1) * 0.5f;

    drawPixelPanel(px0, py0, px1, py1, UI_PANEL, UI_BORDER);
    drawTextCentered(cx + 1.0f, py1 - 60.0f, GLUT_BITMAP_TIMES_ROMAN_24, "CAMPUS DRIP", UI_GOLD);
    drawTextCentered(cx, py1 - 60.0f, GLUT_BITMAP_TIMES_ROMAN_24, "CAMPUS DRIP", UI_GOLD);
    drawTextCentered(cx, py1 - 86.0f, GLUT_BITMAP_HELVETICA_12,
                     "3D PIXEL CHARACTER CUSTOMIZER", UI_DIM);
    drawRect(px0 + 20.0f, py1 - 102.0f, px1 - 20.0f, py1 - 99.0f, UI_BORDER);

    for (int i = 0; i < MAIN_MENU_COUNT; i++)
    {
        float yc = py1 - 170.0f - i * 70.0f;
        bool isSelected = (i == mainMenuSelection);

        if (isSelected)
        {
            drawRect(px0 + 16.0f, yc - 26.0f, px1 - 16.0f, yc + 26.0f, UI_HILITE);
            drawOutline(px0 + 16.0f, yc - 26.0f, px1 - 16.0f, yc + 26.0f, UI_ACCENT, 2.0f);
            drawText(px0 + 34.0f, yc - 6.0f, GLUT_BITMAP_HELVETICA_18, ">", UI_GOLD);
        }
        drawText(px0 + 62.0f, yc - 6.0f, GLUT_BITMAP_HELVETICA_18, MENU_LABELS[i],
                 isSelected ? UI_ACCENT : UI_DIM);
    }

    drawTextCentered(cx, py0 + 24.0f, GLUT_BITMAP_HELVETICA_12,
                     "Characters are saved for this session only.", UI_GOLD);

    drawBottomBar("UP/DOWN: Select    ENTER: Choose    ESC: Exit",
                  "MOUSE DRAG: Orbit camera    MOUSE WHEEL: Zoom    R: Reset view");
}

// ============================================================
// SCREEN: SAVED CHARACTERS (also the "choose a slot to save" picker)
// ============================================================
static void drawSlotCard(int slot, float x0, float y0, float x1, float y1)
{
    const SavedCharacter& c = savedSlots[slot];
    bool isSelected = (slot == selectedSlot);
    Color border = isSelected ? UI_ACCENT : UI_BORDER;

    drawPixelPanel(x0, y0, x1, y1, isSelected ? rgb(0.12f, 0.16f, 0.34f) : UI_PANEL, border);

    char line[96];
    snprintf(line, sizeof(line), "SLOT %d", slot + 1);
    drawText(x0 + 18.0f, y1 - 28.0f, GLUT_BITMAP_HELVETICA_18, line, isSelected ? UI_ACCENT : UI_TEXT);

    if (!c.occupied)
    {
        drawText(x0 + 18.0f, y1 - 62.0f, GLUT_BITMAP_HELVETICA_18, "EMPTY SLOT", UI_DIM);
        if (isSelected)
        {
            const char* hint = savePickMode ? "ENTER: SAVE HERE" : "ENTER: CREATE HERE";
            drawText(x0 + 18.0f, y1 - 88.0f, GLUT_BITMAP_HELVETICA_12, hint, UI_GOLD);
        }
        return;
    }

    drawTextRight(x1 - 18.0f, y1 - 28.0f, GLUT_BITMAP_HELVETICA_12, c.name, UI_GOLD);

    snprintf(line, sizeof(line), "BASE: %s     POSE: %s",
             c.gender == 0 ? "MALE" : "FEMALE", poseName(c.pose));
    drawText(x0 + 18.0f, y1 - 52.0f, GLUT_BITMAP_HELVETICA_12, line, UI_TEXT);

    snprintf(line, sizeof(line), "HAIR: %s     TOP: %s",
             optionName(CAT_HAIR, c.selection[CAT_HAIR], c.gender),
             optionName(CAT_TOP, c.selection[CAT_TOP], c.gender));
    drawText(x0 + 18.0f, y1 - 70.0f, GLUT_BITMAP_HELVETICA_12, line, UI_DIM);

    snprintf(line, sizeof(line), "BOTTOM: %s     SHOES: %s",
             optionName(CAT_BOTTOM, c.selection[CAT_BOTTOM], c.gender),
             optionName(CAT_SHOES, c.selection[CAT_SHOES], c.gender));
    drawText(x0 + 18.0f, y1 - 88.0f, GLUT_BITMAP_HELVETICA_12, line, UI_DIM);

    snprintf(line, sizeof(line), "ACCESSORY: %s     BACKGROUND: %s",
             optionName(CAT_ACCESSORY, c.selection[CAT_ACCESSORY], c.gender),
             optionName(CAT_BACKGROUND, c.selection[CAT_BACKGROUND], c.gender));
    drawText(x0 + 18.0f, y1 - 106.0f, GLUT_BITMAP_HELVETICA_12, line, UI_DIM);

    if (isSelected && savePickMode)
        drawTextRight(x1 - 18.0f, y0 + 10.0f, GLUT_BITMAP_HELVETICA_12, "ENTER: OVERWRITE", UI_GOLD);
    else if (isSelected && deleteArmed)
        drawTextRight(x1 - 18.0f, y0 + 10.0f, GLUT_BITMAP_HELVETICA_12,
                      "PRESS D AGAIN TO DELETE", rgb(1.0f, 0.45f, 0.45f));
}

void drawSavedScreen()
{
    float w = (float)windowWidth;
    float h = (float)windowHeight;

    // Header
    drawRect(0.0f, h - 70.0f, w, h, UI_BG);
    drawRect(0.0f, h - 74.0f, w, h - 70.0f, UI_BORDER);
    drawText(24.0f, h - 36.0f, GLUT_BITMAP_HELVETICA_18,
             savePickMode ? "SAVE CHARACTER: CHOOSE A SLOT" : "SAVED CHARACTERS", UI_ACCENT);
    drawText(24.0f, h - 58.0f, GLUT_BITMAP_HELVETICA_12,
             "Characters are saved for this session only.", UI_GOLD);

    // Three slot cards
    const float cardH = 120.0f;
    for (int i = 0; i < SLOT_COUNT; i++)
    {
        float y1 = h - 110.0f - i * (cardH + 16.0f);
        drawSlotCard(i, 30.0f, y1 - cardH, 30.0f + LEFT_PANEL_W, y1);
    }

    if (savePickMode)
    {
        drawBottomBar("UP/DOWN: Select slot    ENTER: Save here (overwrites an occupied slot)",
                      "BACKSPACE: Cancel    MOUSE DRAG: Orbit camera    MOUSE WHEEL: Zoom    ESC: Exit");
    }
    else
    {
        drawBottomBar("UP/DOWN: Select slot    ENTER or V: View    E: Edit    D: Delete (press twice)",
                      "BACKSPACE: Main menu    MOUSE DRAG: Orbit    WHEEL: Zoom    R: Reset view    ESC: Exit");
    }
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
    float panelW = customizePanelWidth();
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
    float boxTop = py0 + 125.0f;
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
                      optionName(i, selection[i], genderChoice), valueColor);
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
                     optionName(selectedCategory, current, genderChoice), UI_GOLD);

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
        "MOUSE DRAG: Orbit camera    WHEEL: Zoom    R: Reset view    BACKSPACE: Main menu    ESC: Exit");
}

// ============================================================
// SCREEN: CONFIRMED / SHOWCASE (outfit summary + pose selection)
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
                     "Pick a pose, press S to save, or drag the mouse to see every side.",
                     UI_DIM);

    // Summary card
    float x0 = 24.0f;
    float x1 = x0 + SHOWCASE_CARD_W;
    float y1 = h - 100.0f;
    float y0 = y1 - 270.0f;
    drawPixelPanel(x0, y0, x1, y1, UI_PANEL, UI_GREEN);

    drawText(x0 + 16.0f, y1 - 30.0f, GLUT_BITMAP_HELVETICA_18, "YOUR LOOK", UI_ACCENT);
    drawRect(x0 + 12.0f, y1 - 42.0f, x1 - 12.0f, y1 - 39.0f, UI_BORDER);

    float rowY = y1 - 64.0f;
    drawText(x0 + 16.0f, rowY, GLUT_BITMAP_HELVETICA_12, "BASE", UI_DIM);
    drawTextRight(x1 - 16.0f, rowY, GLUT_BITMAP_HELVETICA_12,
                  genderChoice == 0 ? "MALE" : "FEMALE", UI_GOLD);

    for (int i = 0; i < CATEGORY_COUNT; i++)
    {
        rowY = y1 - 64.0f - (i + 1) * 28.0f;
        drawText(x0 + 16.0f, rowY, GLUT_BITMAP_HELVETICA_12, categoryNames[i], UI_DIM);
        drawTextRight(x1 - 16.0f, rowY, GLUT_BITMAP_HELVETICA_12,
                      optionName(i, selection[i], genderChoice), UI_GOLD);
    }

    // Pose card
    float py1 = y0 - 22.0f;
    float py0 = py1 - 130.0f;
    float pcx = (x0 + x1) * 0.5f;
    drawPixelPanel(x0, py0, x1, py1, UI_PANEL, UI_ACCENT);

    drawTextCentered(pcx, py1 - 24.0f, GLUT_BITMAP_HELVETICA_12, "POSE", UI_DIM);
    drawTextCentered(pcx, py1 - 50.0f, GLUT_BITMAP_HELVETICA_18, poseName(currentPose), UI_GOLD);
    drawText(x0 + 16.0f, py1 - 50.0f, GLUT_BITMAP_HELVETICA_18, "<", UI_TEXT);
    drawTextRight(x1 - 16.0f, py1 - 50.0f, GLUT_BITMAP_HELVETICA_18, ">", UI_TEXT);

    char counter[32];
    snprintf(counter, sizeof(counter), "%d / %d", currentPose + 1, POSE_COUNT);
    drawTextCentered(pcx, py1 - 70.0f, GLUT_BITMAP_HELVETICA_12, counter, UI_TEXT);

    float dotsWidth = POSE_COUNT * 18.0f - 8.0f;
    float dotX = pcx - dotsWidth * 0.5f;
    for (int i = 0; i < POSE_COUNT; i++)
    {
        Color dotColor = (i == currentPose) ? UI_GOLD : rgb(0.30f, 0.28f, 0.50f);
        drawRect(dotX + i * 18.0f, py1 - 88.0f, dotX + i * 18.0f + 10.0f, py1 - 78.0f, dotColor);
    }

    if (poseConfirmed)
        drawTextCentered(pcx, py1 - 112.0f, GLUT_BITMAP_HELVETICA_12, "POSE CONFIRMED", UI_GREEN);
    else
        drawTextCentered(pcx, py1 - 112.0f, GLUT_BITMAP_HELVETICA_12, "ENTER: CONFIRM POSE", UI_DIM);

    drawBottomBar(
        "LEFT/RIGHT: Change pose    ENTER: Confirm pose    S: Save character    BACKSPACE: Edit outfit",
        "MOUSE DRAG: Orbit camera    WHEEL: Zoom    R: Reset view    M: Main menu    ESC: Exit");
}

// ============================================================
// MODULE 7: 2D INTERFACE PASS (orthographic projection)
// Lighting and depth testing are turned off, then restored.
// The matrices are pushed/popped so the 3D camera is untouched
// and the interface never rotates with the scene. The interface is
// drawn last, so the environment can never cover it.
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
    else if (screenState == SCREEN_MAIN_MENU)
        drawMainMenuScreen();
    else if (screenState == SCREEN_SAVED)
        drawSavedScreen();
    else if (screenState == SCREEN_CUSTOMIZE)
        drawCustomizeScreen();
    else
        drawShowcaseScreen();

    if (screenState != SCREEN_WELCOME)
        drawStatusMessage();

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
namespace
{
void activateMainMenu()
{
    if (mainMenuSelection == MENU_CREATE)
        startNewCharacter(-1);
    else if (mainMenuSelection == MENU_SAVED)
        enterScreen(SCREEN_SAVED);
    else
        exit(0);
}

void handleSavedKey(unsigned char key)
{
    bool occupied = savedSlots[selectedSlot].occupied;
    bool isDelete = (key == 'd' || key == 'D');

    if (key == 8)    // BACKSPACE: cancel the picker or go back to the menu
    {
        enterScreen(savePickMode ? SCREEN_SHOWCASE : SCREEN_MAIN_MENU);
        return;
    }

    if (!isDelete)
        deleteArmed = false;

    if (savePickMode)    // choosing where to save the current character
    {
        if (key == 13)
        {
            saveCurrentToSlot(selectedSlot);
            enterScreen(SCREEN_SHOWCASE);
            setStatus(occupied ? "SLOT OVERWRITTEN" : "CHARACTER SAVED");
        }
        return;
    }

    if (key == 13 || key == 'v' || key == 'V')   // view, or create in an empty slot
    {
        if (occupied)
        {
            loadSlotIntoCurrent(selectedSlot);
            enterScreen(SCREEN_SHOWCASE);
        }
        else
        {
            startNewCharacter(selectedSlot);
        }
    }
    else if (key == 'e' || key == 'E')           // edit
    {
        if (occupied)
        {
            loadSlotIntoCurrent(selectedSlot);
            selectedCategory = 0;
            enterScreen(SCREEN_CUSTOMIZE);
        }
        else
        {
            setStatus("SLOT IS EMPTY - PRESS ENTER TO CREATE");
        }
    }
    else if (isDelete)                           // delete needs two presses
    {
        if (!occupied)
        {
            setStatus("NOTHING TO DELETE");
        }
        else if (!deleteArmed)
        {
            deleteArmed = true;
            setStatus("PRESS D AGAIN TO DELETE");
        }
        else
        {
            deleteSlot(selectedSlot);
            deleteArmed = false;
            setStatus("CHARACTER DELETED");
        }
    }
}

void handleCustomizeKey(unsigned char key)
{
    if (key == 'g' || key == 'G')
    {
        genderChoice = 1 - genderChoice;
    }
    else if (key == 'c' || key == 'C')
    {
        poseConfirmed = false;
        enterScreen(SCREEN_SHOWCASE);
    }
    else if (key == 8)
    {
        enterScreen(SCREEN_MAIN_MENU);
    }
}

void handleShowcaseKey(unsigned char key)
{
    if (key == 13)
    {
        poseConfirmed = true;
        setStatus("POSE CONFIRMED");
    }
    else if (key == 's' || key == 'S')
    {
        openSavePicker();
    }
    else if (key == 8)
    {
        enterScreen(SCREEN_CUSTOMIZE);
    }
    else if (key == 'm' || key == 'M')
    {
        enterScreen(SCREEN_MAIN_MENU);
    }
}
}

void keyboard(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    if (key == 27)                                  // ESC always exits
        exit(0);

    if (screenState == SCREEN_WELCOME)              // any other key continues
    {
        enterScreen(SCREEN_MAIN_MENU);
    }
    else if (key == 'r' || key == 'R')
    {
        resetCamera();
    }
    else if (screenState == SCREEN_MAIN_MENU)
    {
        if (key == 13)
            activateMainMenu();
    }
    else if (screenState == SCREEN_SAVED)
    {
        handleSavedKey(key);
    }
    else if (screenState == SCREEN_CUSTOMIZE)
    {
        handleCustomizeKey(key);
    }
    else
    {
        handleShowcaseKey(key);
    }

    glutPostRedisplay();
}

// MODULE 3: special-key callback (arrow keys)
void specialKeys(int key, int x, int y)
{
    (void)x;
    (void)y;

    bool up = (key == GLUT_KEY_UP);
    bool down = (key == GLUT_KEY_DOWN);
    bool left = (key == GLUT_KEY_LEFT);
    bool right = (key == GLUT_KEY_RIGHT);

    if (screenState == SCREEN_MAIN_MENU)
    {
        if (up)   mainMenuSelection = wrapIndex(mainMenuSelection - 1, MAIN_MENU_COUNT);
        if (down) mainMenuSelection = wrapIndex(mainMenuSelection + 1, MAIN_MENU_COUNT);
    }
    else if (screenState == SCREEN_SAVED)
    {
        if (up)   selectedSlot = wrapIndex(selectedSlot - 1, SLOT_COUNT);
        if (down) selectedSlot = wrapIndex(selectedSlot + 1, SLOT_COUNT);
        deleteArmed = false;
    }
    else if (screenState == SCREEN_CUSTOMIZE)
    {
        if (up)   selectedCategory = wrapIndex(selectedCategory - 1, CATEGORY_COUNT);
        if (down) selectedCategory = wrapIndex(selectedCategory + 1, CATEGORY_COUNT);

        if (left || right)
        {
            int direction = right ? 1 : -1;
            int count = choiceCounts[selectedCategory];

            // Wrap around inside the valid range for this category
            selection[selectedCategory] =
                wrapIndex(selection[selectedCategory] + direction, count);
        }
    }
    else if (screenState == SCREEN_SHOWCASE)
    {
        if (left || right)
        {
            currentPose = wrapIndex(currentPose + (right ? 1 : -1), POSE_COUNT);
            poseConfirmed = false;
        }
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
        targetDistance -= 0.8f;
    else if (state == GLUT_DOWN && button == 4)
        targetDistance += 0.8f;

    targetDistance = clampFloat(targetDistance, 7.0f, 18.0f);

    glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
    if (!dragging)
        return;

    targetYaw += (x - previousMouseX) * 0.5f;
    targetPitch += (previousMouseY - y) * 0.4f;     // drag up = look from above
    targetPitch = clampFloat(targetPitch, 0.0f, 70.0f);

    previousMouseX = x;
    previousMouseY = y;

    glutPostRedisplay();
}
