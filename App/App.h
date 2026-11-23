#ifndef CAMPUS_DRIP_APP_H
#define CAMPUS_DRIP_APP_H

// ============================================================
// PART 1 OF 4: APPLICATION CORE (header)
// Shared declarations used by all four program parts.
// GLEW must be included before FreeGLUT.
// ============================================================
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

struct Color
{
    float r, g, b;
};

Color rgb(float r, float g, float b);
Color shade(const Color& c, float k);
float clampFloat(float v, float lo, float hi);

// ------------------------------------------------------------
// Screen flow:
// Welcome -> Main Menu -> (Create New | Saved Characters)
//         -> Customize -> Showcase (poses) -> Save (slot picker)
// ------------------------------------------------------------
enum ScreenState
{
    SCREEN_WELCOME = 0,
    SCREEN_MAIN_MENU = 1,
    SCREEN_SAVED = 2,        // also used as the "choose a slot to save" picker
    SCREEN_CUSTOMIZE = 3,
    SCREEN_SHOWCASE = 4
};
constexpr int SCREEN_COUNT = 5;

enum MainMenuChoice
{
    MENU_CREATE = 0,
    MENU_SAVED = 1,
    MENU_EXIT = 2
};
constexpr int MAIN_MENU_COUNT = 3;

enum Category
{
    CAT_HAIR = 0,
    CAT_TOP,
    CAT_BOTTOM,
    CAT_SHOES,
    CAT_ACCESSORY,
    CAT_BACKGROUND
};

enum Accessory
{
    ACC_NONE = 0,
    ACC_GLASSES = 1,
    ACC_CAP = 2,
    ACC_BACKPACK = 3
};

enum Pose
{
    POSE_NEUTRAL = 0,
    POSE_WAVE = 1,
    POSE_PEACE = 2,
    POSE_HIP = 3,
    POSE_WALK = 4
};

constexpr int CATEGORY_COUNT = 6;
constexpr int MAX_CHOICES = 4;
constexpr int POSE_COUNT = 5;
constexpr int SLOT_COUNT = 3;

// Layout constants shared by the interface and the camera framing
constexpr float LEFT_PANEL_W = 400.0f;     // main menu + saved screen panel
constexpr float SHOWCASE_CARD_W = 280.0f;  // showcase summary cards
constexpr int MIN_WINDOW_W = 900;
constexpr int MIN_WINDOW_H = 640;

// ------------------------------------------------------------
// Character-save data (session only: lives in an array)
// ------------------------------------------------------------
struct SavedCharacter
{
    bool occupied;
    int gender;                       // 0 = male base, 1 = female base
    int selection[CATEGORY_COUNT];    // hair, top, bottom, shoes, accessory, background
    int pose;
    char name[16];
};

extern const float PI;

extern const char* categoryNames[CATEGORY_COUNT];
extern const int choiceCounts[CATEGORY_COUNT];
const char* optionName(int category, int index, int gender);

// Current (working) character
extern int selection[CATEGORY_COUNT];
extern int selectedCategory;
extern int genderChoice;
extern int currentPose;
extern bool poseConfirmed;

// Screens, menus and save slots
extern ScreenState screenState;
extern int mainMenuSelection;
extern int selectedSlot;
extern int activeSlot;            // slot being edited/viewed, -1 = new character
extern bool savePickMode;         // Saved screen acts as a "save into slot" picker
extern bool deleteArmed;          // first D press waits for a second one
extern SavedCharacter savedSlots[SLOT_COUNT];

void setStatus(const char* text);
const char* getStatus();          // nullptr when the message has expired

void captureCurrent(SavedCharacter& out);
void applyCharacter(const SavedCharacter& in);
void startNewCharacter(int targetSlot);
bool loadSlotIntoCurrent(int slot);
void saveCurrentToSlot(int slot);
void deleteSlot(int slot);
void openSavePicker();
void enterScreen(ScreenState s);

// Camera
extern const float LOOK_Y;
extern float cameraYaw;
extern float targetYaw;
extern float cameraPitch;
extern float targetPitch;
extern float cameraDistance;
extern float targetDistance;
extern bool dragging;
extern int previousMouseX;
extern int previousMouseY;
extern int windowWidth;
extern int windowHeight;

float customizePanelWidth();
float sceneCenterX();

// World layout
extern const float GROUND_Y;
extern const float CHAR_BASE_Y;

// Colors
extern const Color SKIN;
extern const Color SKIN_SHADE;
extern const Color UI_BG;
extern const Color UI_PANEL;
extern const Color UI_BORDER;
extern const Color UI_ACCENT;
extern const Color UI_GOLD;
extern const Color UI_TEXT;
extern const Color UI_DIM;
extern const Color UI_HILITE;
extern const Color UI_GREEN;

struct Theme
{
    Color sky;
    Color ambient;
    Color diffuse;
    float lightDir[3];
    Color grass;
    Color path;
    Color building;
    Color roof;
    Color windowColor;
    Color leaves;
    Color skyObject;
    float windowGlow;
    float lampGlow;
};

extern const Theme themes[3];
const Theme& currentTheme();
extern GLuint groundVBO;

void display();
void reshape(int width, int height);
void normalizeCameraYaw();
void resetCamera();
void timer(int value);

#endif
