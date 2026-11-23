#ifndef CAMPUS_DRIP_APP_H
#define CAMPUS_DRIP_APP_H

// Shared declarations used by all four program modules.
// GLEW must be included before FreeGLUT.
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

enum ScreenState
{
    SCREEN_WELCOME = 0,
    SCREEN_CUSTOMIZE = 1,
    SCREEN_SHOWCASE = 2
};

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

constexpr int CATEGORY_COUNT = 6;
constexpr int MAX_CHOICES = 4;

extern const float PI;

extern const char* categoryNames[CATEGORY_COUNT];
extern const int choiceCounts[CATEGORY_COUNT];
extern const char* optionNames[CATEGORY_COUNT][MAX_CHOICES];
extern int selection[CATEGORY_COUNT];
extern int selectedCategory;
extern int genderChoice;
extern ScreenState screenState;

extern const float CAMERA_DEFAULT_YAW;
extern const float CAMERA_DEFAULT_PITCH;
extern const float CAMERA_DEFAULT_DISTANCE;
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

extern const float GROUND_Y;
extern const float CHAR_BASE_Y;
extern const float ARM_W;
extern const float ARM_D;

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
