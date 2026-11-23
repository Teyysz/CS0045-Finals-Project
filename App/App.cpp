#include "App.h"
#include "../World/World.h"
#include "../Character/Character.h"
#include "../Interface/Interface.h"

#ifdef _MSC_VER
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#endif

// ============================================================
// PART 1 OF 4: APPLICATION CORE
// Owns shared state, screen flow, save slots, the camera,
// the display/reshape/timer callbacks and program start.
// ============================================================

const float PI = 3.1415926f;

Color rgb(float r, float g, float b)
{
    Color c = { r, g, b };
    return c;
}

// Multiplies a color (used for highlights and shadows)
Color shade(const Color& c, float k)
{
    Color s = { c.r * k, c.g * k, c.b * k };
    if (s.r > 1.0f) s.r = 1.0f;
    if (s.g > 1.0f) s.g = 1.0f;
    if (s.b > 1.0f) s.b = 1.0f;
    return s;
}

float clampFloat(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// ============================================================
// REQUIREMENT: customization data (names depend on the base)
// ============================================================
const char* categoryNames[CATEGORY_COUNT] =
{
    "HAIR", "TOP", "BOTTOM", "SHOES", "ACCESSORY", "BACKGROUND"
};

// How many options each category really has (keeps indexes valid)
const int choiceCounts[CATEGORY_COUNT] = { 3, 3, 3, 3, 4, 3 };

namespace
{
// [gender][category][option]
const char* OPTION_NAMES[2][CATEGORY_COUNT][MAX_CHOICES] =
{
    { // male base
        { "CLASSIC", "SIDE PART", "SPIKY", "" },
        { "VARSITY", "SCHOOL", "CAMPUS TEE", "" },
        { "CARGO", "JEANS", "SHORTS", "" },
        { "SNEAKERS", "RUNNERS", "DARK SHOES", "" },
        { "NONE", "GLASSES", "CAP", "BACKPACK" },
        { "NIGHT CAMPUS", "SUNSET", "DAY CAMPUS", "" }
    },
    { // female base
        { "LONG HAIR", "PONYTAIL", "TWIN BUNS", "" },
        { "CROPPED VARSITY", "SCHOOL BLOUSE", "OVERSIZED TEE", "" },
        { "PLEATED SKIRT", "WIDE-LEG PANTS", "SHORTS", "" },
        { "CHUNKY SNEAKERS", "MARY JANES", "ANKLE BOOTS", "" },
        { "NONE", "GLASSES", "CAP", "BACKPACK" },
        { "NIGHT CAMPUS", "SUNSET", "DAY CAMPUS", "" }
    }
};
}

// Safe lookup: out-of-range values return an empty string
const char* optionName(int category, int index, int gender)
{
    if (category < 0 || category >= CATEGORY_COUNT) return "";
    if (index < 0 || index >= choiceCounts[category]) return "";
    if (gender < 0 || gender > 1) gender = 0;
    return OPTION_NAMES[gender][category][index];
}

// ============================================================
// Shared state
// ============================================================
int selection[CATEGORY_COUNT] = { 0, 0, 0, 0, 0, 0 };
int selectedCategory = 0;
int genderChoice = 0;                 // 0 = male base, 1 = female base
int currentPose = POSE_NEUTRAL;
bool poseConfirmed = false;

ScreenState screenState = SCREEN_WELCOME;
int mainMenuSelection = 0;
int selectedSlot = 0;
int activeSlot = -1;
bool savePickMode = false;
bool deleteArmed = false;
SavedCharacter savedSlots[SLOT_COUNT] = {};

static char statusText[64] = "";
static int statusUntil = 0;

const float LOOK_Y = 1.0f;            // height of the point the camera looks at

float cameraYaw = 25.0f, targetYaw = 25.0f;
float cameraPitch = 10.0f, targetPitch = 10.0f;
float cameraDistance = 17.0f, targetDistance = 17.0f;

bool dragging = false;
int previousMouseX = 0;
int previousMouseY = 0;

int windowWidth = 1100;
int windowHeight = 700;

// World layout: ground top is GROUND_Y, the character stands on the platform
const float GROUND_Y = -2.5f;
const float CHAR_BASE_Y = -1.88f;

// Colors used by several functions
const Color SKIN = rgb(0.78f, 0.52f, 0.36f);
const Color SKIN_SHADE = rgb(0.66f, 0.42f, 0.28f);

// Interface colors
const Color UI_BG = rgb(0.05f, 0.04f, 0.15f);
const Color UI_PANEL = rgb(0.09f, 0.08f, 0.22f);
const Color UI_BORDER = rgb(0.45f, 0.38f, 0.95f);
const Color UI_ACCENT = rgb(0.45f, 0.92f, 1.00f);
const Color UI_GOLD = rgb(1.00f, 0.82f, 0.25f);
const Color UI_TEXT = rgb(1.00f, 1.00f, 1.00f);
const Color UI_DIM = rgb(0.72f, 0.70f, 0.90f);
const Color UI_HILITE = rgb(0.16f, 0.30f, 0.58f);
const Color UI_GREEN = rgb(0.30f, 1.00f, 0.55f);

// ============================================================
// MODULE 8: one theme per background (colors + light settings)
// ============================================================
const Theme themes[3] =
{
    { // NIGHT CAMPUS
        { 0.02f, 0.02f, 0.09f }, { 0.16f, 0.17f, 0.30f }, { 0.55f, 0.62f, 0.95f },
        { -0.30f, 1.00f, 0.70f },
        { 0.05f, 0.17f, 0.12f }, { 0.28f, 0.28f, 0.38f }, { 0.22f, 0.20f, 0.38f },
        { 0.12f, 0.10f, 0.22f }, { 1.00f, 0.78f, 0.30f }, { 0.06f, 0.30f, 0.20f },
        { 0.95f, 0.95f, 0.80f }, 0.90f, 1.00f
    },
    { // SUNSET
        { 0.60f, 0.25f, 0.28f }, { 0.30f, 0.20f, 0.26f }, { 1.00f, 0.62f, 0.32f },
        { -1.00f, 0.60f, 0.70f },
        { 0.22f, 0.24f, 0.12f }, { 0.52f, 0.38f, 0.36f }, { 0.52f, 0.32f, 0.34f },
        { 0.30f, 0.16f, 0.22f }, { 1.00f, 0.65f, 0.25f }, { 0.32f, 0.40f, 0.12f },
        { 1.00f, 0.55f, 0.15f }, 0.50f, 0.60f
    },
    { // DAY CAMPUS
        { 0.42f, 0.70f, 0.95f }, { 0.40f, 0.40f, 0.45f }, { 1.00f, 0.97f, 0.88f },
        { -0.30f, 1.00f, 0.60f },
        { 0.28f, 0.58f, 0.26f }, { 0.74f, 0.72f, 0.68f }, { 0.82f, 0.64f, 0.52f },
        { 0.50f, 0.25f, 0.20f }, { 0.55f, 0.75f, 0.90f }, { 0.18f, 0.55f, 0.20f },
        { 1.00f, 0.95f, 0.55f }, 0.00f, 0.00f
    }
};

const Theme& currentTheme()
{
    int bg = selection[CAT_BACKGROUND];
    if (bg < 0 || bg > 2) bg = 0;
    return themes[bg];
}

// MODULE 5: handle of the buffer that stores the ground vertices
GLuint groundVBO = 0;

// ============================================================
// REQUIREMENT: status message (short feedback line on screen)
// ============================================================
void setStatus(const char* text)
{
    snprintf(statusText, sizeof(statusText), "%s", text);
    statusUntil = glutGet(GLUT_ELAPSED_TIME) + 2500;
}

const char* getStatus()
{
    if (statusText[0] != '\0' && glutGet(GLUT_ELAPSED_TIME) < statusUntil)
        return statusText;
    return nullptr;
}

// ============================================================
// REQUIREMENT: character-save data (three session-only slots)
// ============================================================
void captureCurrent(SavedCharacter& out)
{
    out.occupied = true;
    out.gender = genderChoice;
    for (int i = 0; i < CATEGORY_COUNT; i++)
        out.selection[i] = selection[i];
    out.pose = currentPose;
    out.name[0] = '\0';
}

// Copies a saved character into the working character (values are range-checked)
void applyCharacter(const SavedCharacter& in)
{
    genderChoice = (in.gender == 1) ? 1 : 0;
    for (int i = 0; i < CATEGORY_COUNT; i++)
    {
        int v = in.selection[i];
        if (v < 0 || v >= choiceCounts[i]) v = 0;
        selection[i] = v;
    }
    currentPose = (in.pose >= 0 && in.pose < POSE_COUNT) ? in.pose : POSE_NEUTRAL;
}

void startNewCharacter(int targetSlot)
{
    genderChoice = 0;
    for (int i = 0; i < CATEGORY_COUNT; i++)
        selection[i] = 0;
    currentPose = POSE_NEUTRAL;
    poseConfirmed = false;
    selectedCategory = 0;
    activeSlot = targetSlot;
    enterScreen(SCREEN_CUSTOMIZE);
}

bool loadSlotIntoCurrent(int slot)
{
    if (slot < 0 || slot >= SLOT_COUNT || !savedSlots[slot].occupied)
        return false;

    applyCharacter(savedSlots[slot]);
    activeSlot = slot;
    poseConfirmed = true;
    return true;
}

void saveCurrentToSlot(int slot)
{
    if (slot < 0 || slot >= SLOT_COUNT)
        return;

    captureCurrent(savedSlots[slot]);
    snprintf(savedSlots[slot].name, sizeof(savedSlots[slot].name), "CHARACTER %d", slot + 1);
    activeSlot = slot;
}

void deleteSlot(int slot)
{
    if (slot < 0 || slot >= SLOT_COUNT)
        return;

    SavedCharacter empty = {};
    savedSlots[slot] = empty;
    if (activeSlot == slot)
        activeSlot = -1;
}

// Opens the Saved screen as a "choose a slot" picker for the current character
void openSavePicker()
{
    enterScreen(SCREEN_SAVED);
    savePickMode = true;

    if (activeSlot >= 0)
        selectedSlot = activeSlot;
    else
    {
        selectedSlot = 0;
        for (int i = 0; i < SLOT_COUNT; i++)
        {
            if (!savedSlots[i].occupied)
            {
                selectedSlot = i;
                break;
            }
        }
    }
}

// ============================================================
// MODULE 7: CAMERA PRESETS AND FRAMING
// ============================================================
struct CameraPreset
{
    float yaw, pitch, distance;
};

// Same order as ScreenState: welcome, menu, saved, customize, showcase
static const CameraPreset PRESETS[SCREEN_COUNT] =
{
    { 25.0f, 10.0f, 17.0f },
    { 20.0f,  9.0f, 15.0f },
    { 20.0f,  9.0f, 15.0f },
    { 25.0f, 10.0f, 13.5f },
    { 15.0f,  8.0f, 13.0f }
};

// Keeps yaw numbers small so a reset takes the short way around
void normalizeCameraYaw()
{
    float shift = floorf((cameraYaw + 180.0f) / 360.0f) * 360.0f;
    cameraYaw -= shift;
    targetYaw -= shift;
}

void resetCamera()
{
    normalizeCameraYaw();
    const CameraPreset& p = PRESETS[screenState];
    targetYaw = p.yaw;
    targetPitch = p.pitch;
    targetDistance = p.distance;
}

void enterScreen(ScreenState s)
{
    screenState = s;
    savePickMode = false;
    deleteArmed = false;
    resetCamera();
}

float customizePanelWidth()
{
    return clampFloat(windowWidth * 0.27f, 250.0f, 340.0f);
}

// Horizontal pixel where the character should appear on each screen,
// so it stays in the free area next to the panels
float sceneCenterX()
{
    float w = (float)windowWidth;

    if (screenState == SCREEN_MAIN_MENU || screenState == SCREEN_SAVED)
        return (LEFT_PANEL_W + 40.0f + w) * 0.5f;
    if (screenState == SCREEN_CUSTOMIZE)
        return (w - customizePanelWidth() - 20.0f) * 0.5f;
    if (screenState == SCREEN_SHOWCASE)
        return (SHOWCASE_CARD_W + 40.0f + w) * 0.5f;
    return w * 0.5f;
}

// MODULE 7: viewport + perspective for the 3D scene.
// The viewport may be wider than the window (and start left of it) so the
// character can sit off-center without changing its size or leaving gaps.
static void setSceneViewport()
{
    float w = (float)windowWidth;
    float h = (float)windowHeight;
    float c = sceneCenterX();
    float half = (c > w - c) ? c : (w - c);

    glViewport((int)(c - half), 0, (int)(half * 2.0f), windowHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, (double)(half * 2.0f) / (double)h, 1.0, 400.0);
    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// MODULE 7: DISPLAY CALLBACK
// ============================================================
void display()
{
    // Saved screen: preview the highlighted slot by temporarily swapping it in
    SavedCharacter backup = {};
    bool previewing = false;
    bool showCharacter = true;

    if (screenState == SCREEN_SAVED && !savePickMode)
    {
        if (savedSlots[selectedSlot].occupied)
        {
            captureCurrent(backup);
            applyCharacter(savedSlots[selectedSlot]);
            previewing = true;
        }
        else
        {
            showCharacter = false;
        }
    }

    const Theme& t = currentTheme();
    glClearColor(t.sky.r, t.sky.g, t.sky.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setSceneViewport();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Spherical orbit camera that always looks at the character
    float yawRadians = cameraYaw * PI / 180.0f;
    float pitchRadians = cameraPitch * PI / 180.0f;

    float cameraX = cameraDistance * cosf(pitchRadians) * sinf(yawRadians);
    float cameraY = cameraDistance * sinf(pitchRadians) + LOOK_Y;
    float cameraZ = cameraDistance * cosf(pitchRadians) * cosf(yawRadians);

    gluLookAt(cameraX, cameraY, cameraZ,
              0.0f, LOOK_Y, 0.0f,
              0.0f, 1.0f, 0.0f);

    updateLighting();                   // MODULE 8
    drawCampusBackground();

    if (showCharacter)
    {
        glPushMatrix();
            glTranslatef(0.0f, CHAR_BASE_Y, 0.0f);   // MODULE 6
            drawCharacter();
        glPopMatrix();
    }

    if (previewing)
        applyCharacter(backup);

    // The interface always uses the whole window
    glViewport(0, 0, windowWidth, windowHeight);
    drawInterface();
    glutSwapBuffers();
}

void reshape(int width, int height)
{
    if (height == 0)
        height = 1;

    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);

    // The layout needs a minimum size, so ask for it if the window is smaller
    if (width < MIN_WINDOW_W || height < MIN_WINDOW_H)
    {
        glutReshapeWindow(width < MIN_WINDOW_W ? MIN_WINDOW_W : width,
                          height < MIN_WINDOW_H ? MIN_WINDOW_H : height);
    }
}

// Smooth camera: current values ease toward target values every frame
void timer(int value)
{
    (void)value;

    // Slow auto-rotation on the welcome, menu and saved screens
    bool autoRotate = (screenState == SCREEN_WELCOME ||
                       screenState == SCREEN_MAIN_MENU ||
                       screenState == SCREEN_SAVED);
    if (autoRotate && !dragging)
        targetYaw += 0.40f;

    cameraYaw += (targetYaw - cameraYaw) * 0.15f;
    cameraPitch += (targetPitch - cameraPitch) * 0.15f;
    cameraDistance += (targetDistance - cameraDistance) * 0.15f;

    // Keep the angles from growing forever
    if (cameraYaw > 540.0f && targetYaw > 540.0f) { cameraYaw -= 360.0f; targetYaw -= 360.0f; }
    if (cameraYaw < -540.0f && targetYaw < -540.0f) { cameraYaw += 360.0f; targetYaw += 360.0f; }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// ============================================================
// MODULE 1: PROGRAM START
// ============================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(80, 50);
    glutCreateWindow("Campus Drip - 3D Pixel Character Customizer");

    // GLEW must be initialized AFTER the window (OpenGL context) exists
    glewExperimental = GL_TRUE;
    GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK)
    {
        fprintf(stderr, "GLEW error: %s\n",
                reinterpret_cast<const char*>(glewGetErrorString(glewStatus)));
        return 1;
    }

    initializeScene();
    initializeVBO();
    resetCamera();
    cameraYaw = targetYaw;
    cameraPitch = targetPitch;
    cameraDistance = targetDistance;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
