#include "App.h"
#include "World.h"
#include "Character.h"
#include "Interface.h"

#ifdef _MSC_VER
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#endif

// ============================================================
// MODULE 1 OF 4: APPLICATION CORE
// Owns shared state, the camera, display loop, and program start.
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

const char* categoryNames[CATEGORY_COUNT] =
{
    "HAIR", "TOP", "BOTTOM", "SHOES", "ACCESSORY", "BACKGROUND"
};

// How many options each category really has (keeps indexes valid)
const int choiceCounts[CATEGORY_COUNT] = { 3, 3, 3, 3, 4, 3 };

const char* optionNames[CATEGORY_COUNT][MAX_CHOICES] =
{
    { "CLASSIC", "SIDE PART", "SPIKY", "" },
    { "VARSITY", "SCHOOL", "TAMBAY", "" },
    { "CARGO", "JEANS", "SHORTS", "" },
    { "SNEAKERS", "RUNNERS", "DARK SHOES", "" },
    { "NONE", "GLASSES", "CAP", "BACKPACK" },
    { "NIGHT CAMPUS", "SUNSET", "DAY CAMPUS", "" }
};

// Current selection for every category
int selection[CATEGORY_COUNT] = { 0, 0, 0, 0, 0, 0 };
int selectedCategory = 0;
int genderChoice = 0;                 // 0 = male base, 1 = female base
ScreenState screenState = SCREEN_WELCOME;

const float CAMERA_DEFAULT_YAW = 25.0f;
const float CAMERA_DEFAULT_PITCH = 12.0f;
const float CAMERA_DEFAULT_DISTANCE = 11.5f;
const float LOOK_Y = 0.3f;            // height of the point the camera looks at

float cameraYaw = CAMERA_DEFAULT_YAW, targetYaw = CAMERA_DEFAULT_YAW;
float cameraPitch = CAMERA_DEFAULT_PITCH, targetPitch = CAMERA_DEFAULT_PITCH;
float cameraDistance = CAMERA_DEFAULT_DISTANCE,
      targetDistance = CAMERA_DEFAULT_DISTANCE;

bool dragging = false;
int previousMouseX = 0;
int previousMouseY = 0;

int windowWidth = 1100;
int windowHeight = 700;

// World layout
const float GROUND_Y = -2.5f;         // top of the ground plane
const float CHAR_BASE_Y = -2.36f;     // character's feet rest on the platform

// Character proportions
const float ARM_W = 0.42f;
const float ARM_D = 0.58f;

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


const Theme themes[3] =
{
    { // NIGHT CAMPUS
        { 0.02f, 0.02f, 0.09f }, { 0.16f, 0.17f, 0.30f }, { 0.55f, 0.62f, 0.95f },
        { -0.30f, 1.00f, 0.70f },
        { 0.05f, 0.17f, 0.12f }, { 0.28f, 0.28f, 0.38f }, { 0.20f, 0.18f, 0.34f },
        { 0.12f, 0.10f, 0.22f }, { 1.00f, 0.78f, 0.30f }, { 0.06f, 0.30f, 0.20f },
        { 0.95f, 0.95f, 0.80f }, 0.90f, 1.00f
    },
    { // SUNSET
        { 0.60f, 0.25f, 0.28f }, { 0.30f, 0.20f, 0.26f }, { 1.00f, 0.62f, 0.32f },
        { -1.00f, 0.60f, 0.70f },
        { 0.22f, 0.24f, 0.12f }, { 0.52f, 0.38f, 0.36f }, { 0.45f, 0.28f, 0.32f },
        { 0.30f, 0.16f, 0.22f }, { 1.00f, 0.65f, 0.25f }, { 0.32f, 0.40f, 0.12f },
        { 1.00f, 0.55f, 0.15f }, 0.50f, 0.60f
    },
    { // DAY CAMPUS
        { 0.42f, 0.70f, 0.95f }, { 0.40f, 0.40f, 0.45f }, { 1.00f, 0.97f, 0.88f },
        { -0.30f, 1.00f, 0.60f },
        { 0.28f, 0.58f, 0.26f }, { 0.74f, 0.72f, 0.68f }, { 0.80f, 0.62f, 0.50f },
        { 0.50f, 0.25f, 0.20f }, { 0.55f, 0.75f, 0.90f }, { 0.18f, 0.55f, 0.20f },
        { 1.00f, 0.95f, 0.55f }, 0.00f, 0.00f
    }
};

const Theme& currentTheme()
{
    return themes[selection[CAT_BACKGROUND]];
}

// MODULE 5: handle of the buffer that stores the ground vertices
GLuint groundVBO = 0;

// ============================================================
// MODULE 7: PERSPECTIVE AND CAMERA
// ============================================================
void display()
{
    const Theme& t = currentTheme();
    glClearColor(t.sky.r, t.sky.g, t.sky.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    // Small hop animation on the showcase screen (MODULE 6: translation)
    float hop = 0.0f;
    if (screenState == SCREEN_SHOWCASE)
        hop = 0.08f * fabs(sinf(glutGet(GLUT_ELAPSED_TIME) * 0.004f));

    glPushMatrix();
        glTranslatef(0.0f, CHAR_BASE_Y + hop, 0.0f);
        drawCharacter();
    glPopMatrix();

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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 0.5, 60.0);

    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// CAMERA HELPERS
// ============================================================

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
    targetYaw = CAMERA_DEFAULT_YAW;
    targetPitch = CAMERA_DEFAULT_PITCH;
    targetDistance = CAMERA_DEFAULT_DISTANCE;
}

// Smooth camera: current values ease toward target values every frame
void timer(int value)
{
    (void)value;

    // Slow auto-rotation on Welcome and Showcase screens
    if (screenState != SCREEN_CUSTOMIZE && !dragging)
        targetYaw += 0.45f;

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
