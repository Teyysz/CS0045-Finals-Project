#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>
#include <cstring>

// ============================================================
// CAMPUS DRIP: 3D PIXEL CHARACTER CUSTOMIZER
// Uses only concepts from CS0045 Modules 1-8.
// ============================================================

const float PI = 3.1415926f;

// Customization choices
int genderChoice = 0;      // 0 = male, 1 = female
int hairChoice = 0;
int topChoice = 0;
int bottomChoice = 0;
int shoesChoice = 0;
int backgroundChoice = 0;
int selectedCategory = 0;

const int CATEGORY_COUNT = 5;
const int CHOICE_COUNT = 3;

// Orbit-camera values
float cameraYaw = 25.0f;
float cameraPitch = 12.0f;
float cameraDistance = 8.0f;

bool dragging = false;
int previousMouseX = 0;
int previousMouseY = 0;
bool confirmed = false;
int celebrationStartTime = 0;

int windowWidth = 1100;
int windowHeight = 700;

const char* categoryNames[CATEGORY_COUNT] = {
    "HAIR", "TOP", "BOTTOM", "SHOES", "BACKGROUND"
};

const char* hairNames[CHOICE_COUNT] = {
    "CLASSIC", "SIDE PART", "SPIKY"
};

const char* topNames[CHOICE_COUNT] = {
    "VARSITY", "SCHOOL", "TAMBAY"
};

const char* bottomNames[CHOICE_COUNT] = {
    "CARGO", "JEANS", "SHORTS"
};

const char* shoesNames[CHOICE_COUNT] = {
    "SNEAKERS", "RUNNERS", "DARK SHOES"
};

const char* backgroundNames[CHOICE_COUNT] = {
    "NIGHT CAMPUS", "SUNSET", "DAY CAMPUS"
};

// Function prototypes
void display();
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void initializeScene();
void drawCharacter();
void drawInterface();
void idle();

// ============================================================
// MODULES 1-2: COLORS, PRIMITIVES, AND BASIC OBJECTS
// MODULE 6: TRANSLATION AND SCALING
// Draws one block using a transformed GLUT cube.
// ============================================================
void drawBlock(float x, float y, float z,
               float width, float height, float depth,
               float red, float green, float blue)
{
    glColor3f(red, green, blue);

    glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(width, height, depth);
        glutSolidCube(1.0f);
    glPopMatrix();
}

// ============================================================
// MODULE 3: BITMAP TEXT
// ============================================================
void drawText(float x, float y, void* font, const char* text)
{
    glRasterPos2f(x, y);
    glutBitmapString(font,
        reinterpret_cast<const unsigned char*>(text));
}

// ============================================================
// MODULE 8: BASIC LIGHTING AND MATERIALS
// ============================================================
void initializeScene()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    // Allows glColor3f() to provide the material color.
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat lightAmbient[]  = {0.20f, 0.20f, 0.28f, 1.0f};
    GLfloat lightDiffuse[]  = {0.95f, 0.95f, 1.00f, 1.0f};
    GLfloat lightSpecular[] = {1.00f, 1.00f, 1.00f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    GLfloat materialSpecular[] = {0.35f, 0.35f, 0.35f, 1.0f};
    GLfloat materialShine[] = {32.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShine);

    glShadeModel(GL_SMOOTH);
}

// ============================================================
// SIMPLE CAMPUS BACKGROUND MADE FROM BLOCKS
// ============================================================
void drawCampusBackground()
{
    // Ground and character platform
    drawBlock(0.0f, -2.72f, 0.0f, 12.0f, 0.18f, 8.0f,
              0.12f, 0.14f, 0.22f);
    drawBlock(0.0f, -2.48f, 0.0f, 3.2f, 0.25f, 2.4f,
              0.22f, 0.25f, 0.42f);

    // Simple school buildings behind the character
    drawBlock(-4.0f, -0.7f, -3.5f, 2.6f, 3.8f, 1.0f,
              0.22f, 0.18f, 0.35f);
    drawBlock(4.0f, -0.7f, -3.5f, 2.6f, 3.8f, 1.0f,
              0.22f, 0.18f, 0.35f);

    // Lit windows
    for (int side = -1; side <= 1; side += 2)
    {
        float buildingX = 4.0f * side;
        for (int row = 0; row < 3; row++)
        {
            for (int column = -1; column <= 1; column++)
            {
                drawBlock(buildingX + column * 0.65f,
                          -1.4f + row * 1.0f,
                          -2.94f,
                          0.35f, 0.45f, 0.05f,
                          0.95f, 0.72f, 0.25f);
            }
        }
    }
}

// ============================================================
// 3D PIXEL HAIR OPTIONS
// ============================================================
void drawHair(float skinR, float skinG, float skinB)
{
    (void)skinR;
    (void)skinG;
    (void)skinB;

    if (hairChoice == 0) // Classic
    {
        drawBlock(0.0f, 2.23f, 0.0f, 1.35f, 0.35f, 1.10f,
                  0.16f, 0.07f, 0.04f);
        drawBlock(-0.55f, 1.95f, 0.0f, 0.25f, 0.45f, 1.05f,
                  0.16f, 0.07f, 0.04f);
    }
    else if (hairChoice == 1) // Side part
    {
        drawBlock(-0.15f, 2.24f, 0.0f, 1.50f, 0.30f, 1.10f,
                  0.08f, 0.04f, 0.02f);
        drawBlock(0.48f, 2.00f, 0.0f, 0.35f, 0.55f, 1.05f,
                  0.08f, 0.04f, 0.02f);
    }
    else // Spiky
    {
        drawBlock(0.0f, 2.18f, 0.0f, 1.35f, 0.26f, 1.10f,
                  0.22f, 0.12f, 0.04f);
        drawBlock(-0.45f, 2.48f, 0.0f, 0.30f, 0.45f, 0.60f,
                  0.22f, 0.12f, 0.04f);
        drawBlock(0.0f, 2.55f, 0.0f, 0.30f, 0.55f, 0.60f,
                  0.22f, 0.12f, 0.04f);
        drawBlock(0.45f, 2.45f, 0.0f, 0.30f, 0.40f, 0.60f,
                  0.22f, 0.12f, 0.04f);
    }
}

// ============================================================
// COMPLETE VOXEL CHARACTER
// ============================================================
void drawCharacter()
{
    float skinR = 0.76f;
    float skinG = 0.50f;
    float skinB = 0.34f;

    // A small proportion change distinguishes the two base choices.
    float bodyWidth = (genderChoice == 0) ? 1.65f : 1.48f;

    glPushMatrix();

        // Head, ears, eyes, and hair
        drawBlock(0.0f, 1.75f, 0.0f, 1.25f, 1.15f, 1.05f,
                  skinR, skinG, skinB);
        drawBlock(-0.70f, 1.75f, 0.0f, 0.18f, 0.35f, 0.45f,
                  skinR, skinG, skinB);
        drawBlock(0.70f, 1.75f, 0.0f, 0.18f, 0.35f, 0.45f,
                  skinR, skinG, skinB);
        drawBlock(-0.27f, 1.86f, 0.54f, 0.13f, 0.18f, 0.05f,
                  0.05f, 0.04f, 0.04f);
        drawBlock(0.27f, 1.86f, 0.54f, 0.13f, 0.18f, 0.05f,
                  0.05f, 0.04f, 0.04f);
        drawHair(skinR, skinG, skinB);

        // Top choices
        if (topChoice == 0) // Varsity jacket
        {
            drawBlock(0.0f, 0.40f, 0.0f, bodyWidth, 1.45f, 0.80f,
                      0.48f, 0.05f, 0.12f);
            drawBlock(0.0f, 0.42f, 0.43f, 0.55f, 1.28f, 0.08f,
                      0.92f, 0.92f, 0.88f);
            drawBlock(-1.02f, 0.40f, 0.0f, 0.38f, 1.35f, 0.65f,
                      0.88f, 0.82f, 0.72f);
            drawBlock(1.02f, 0.40f, 0.0f, 0.38f, 1.35f, 0.65f,
                      0.88f, 0.82f, 0.72f);
        }
        else if (topChoice == 1) // School uniform
        {
            drawBlock(0.0f, 0.40f, 0.0f, bodyWidth, 1.45f, 0.80f,
                      0.90f, 0.92f, 0.96f);
            drawBlock(0.0f, 0.55f, 0.43f, 0.18f, 0.90f, 0.08f,
                      0.10f, 0.22f, 0.52f);
            drawBlock(-1.02f, 0.40f, 0.0f, 0.38f, 1.35f, 0.65f,
                      skinR, skinG, skinB);
            drawBlock(1.02f, 0.40f, 0.0f, 0.38f, 1.35f, 0.65f,
                      skinR, skinG, skinB);
        }
        else // Tambay shirt
        {
            drawBlock(0.0f, 0.40f, 0.0f, bodyWidth, 1.45f, 0.80f,
                      0.12f, 0.52f, 0.42f);
            drawBlock(-1.02f, 0.55f, 0.0f, 0.38f, 0.95f, 0.65f,
                      skinR, skinG, skinB);
            drawBlock(1.02f, 0.55f, 0.0f, 0.38f, 0.95f, 0.65f,
                      skinR, skinG, skinB);
        }

        // Hands
        drawBlock(-1.02f, -0.40f, 0.0f, 0.40f, 0.38f, 0.65f,
                  skinR, skinG, skinB);
        drawBlock(1.02f, -0.40f, 0.0f, 0.40f, 0.38f, 0.65f,
                  skinR, skinG, skinB);

        // Bottom choices
        if (bottomChoice == 0) // Cargo pants
        {
            drawBlock(-0.43f, -1.20f, 0.0f, 0.70f, 1.65f, 0.75f,
                      0.12f, 0.15f, 0.24f);
            drawBlock(0.43f, -1.20f, 0.0f, 0.70f, 1.65f, 0.75f,
                      0.12f, 0.15f, 0.24f);
            drawBlock(-0.82f, -1.05f, 0.0f, 0.18f, 0.48f, 0.82f,
                      0.18f, 0.21f, 0.30f);
            drawBlock(0.82f, -1.05f, 0.0f, 0.18f, 0.48f, 0.82f,
                      0.18f, 0.21f, 0.30f);
        }
        else if (bottomChoice == 1) // Jeans
        {
            drawBlock(-0.43f, -1.20f, 0.0f, 0.70f, 1.65f, 0.72f,
                      0.10f, 0.26f, 0.52f);
            drawBlock(0.43f, -1.20f, 0.0f, 0.70f, 1.65f, 0.72f,
                      0.10f, 0.26f, 0.52f);
        }
        else // Shorts
        {
            drawBlock(-0.43f, -0.80f, 0.0f, 0.72f, 0.85f, 0.75f,
                      0.23f, 0.18f, 0.34f);
            drawBlock(0.43f, -0.80f, 0.0f, 0.72f, 0.85f, 0.75f,
                      0.23f, 0.18f, 0.34f);
            drawBlock(-0.43f, -1.65f, 0.0f, 0.62f, 0.85f, 0.62f,
                      skinR, skinG, skinB);
            drawBlock(0.43f, -1.65f, 0.0f, 0.62f, 0.85f, 0.62f,
                      skinR, skinG, skinB);
        }

        // Shoes
        if (shoesChoice == 0)
        {
            drawBlock(-0.43f, -2.20f, 0.18f, 0.78f, 0.42f, 1.05f,
                      0.92f, 0.92f, 0.95f);
            drawBlock(0.43f, -2.20f, 0.18f, 0.78f, 0.42f, 1.05f,
                      0.92f, 0.92f, 0.95f);
        }
        else if (shoesChoice == 1)
        {
            drawBlock(-0.43f, -2.20f, 0.18f, 0.78f, 0.42f, 1.05f,
                      0.12f, 0.72f, 0.82f);
            drawBlock(0.43f, -2.20f, 0.18f, 0.78f, 0.42f, 1.05f,
                      0.12f, 0.72f, 0.82f);
        }
        else
        {
            drawBlock(-0.43f, -2.20f, 0.18f, 0.78f, 0.42f, 1.05f,
                      0.06f, 0.06f, 0.08f);
            drawBlock(0.43f, -2.20f, 0.18f, 0.78f, 0.42f, 1.05f,
                      0.06f, 0.06f, 0.08f);
        }

    glPopMatrix();
}

const char* currentChoiceName()
{
    if (selectedCategory == 0)
        return hairNames[hairChoice];
    if (selectedCategory == 1)
        return topNames[topChoice];
    if (selectedCategory == 2)
        return bottomNames[bottomChoice];
    if (selectedCategory == 3)
        return shoesNames[shoesChoice];
    return backgroundNames[backgroundChoice];
}

// ============================================================
// MODULE 3: 2D TEXT AND INSTRUCTION PANEL
// The projection is temporarily changed to an orthographic view.
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

    // Title panel
    glColor3f(0.06f, 0.04f, 0.16f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, windowHeight - 85.0f);
        glVertex2f(windowWidth, windowHeight - 85.0f);
        glVertex2f(windowWidth, windowHeight);
        glVertex2f(0.0f, windowHeight);
    glEnd();

    glColor3f(0.50f, 0.85f, 1.0f);
    drawText(35.0f, windowHeight - 40.0f,
             GLUT_BITMAP_HELVETICA_18,
             "CAMPUS DRIP: 3D PIXEL CHARACTER CUSTOMIZER");

    glColor3f(0.85f, 0.82f, 1.0f);
    drawText(35.0f, windowHeight - 65.0f,
             GLUT_BITMAP_HELVETICA_12,
             "Create your campus look and rotate the preview freely.");

    // Customization panel
    glColor3f(0.07f, 0.05f, 0.18f);
    glBegin(GL_QUADS);
        glVertex2f(windowWidth - 300.0f, 125.0f);
        glVertex2f(windowWidth - 25.0f, 125.0f);
        glVertex2f(windowWidth - 25.0f, windowHeight - 115.0f);
        glVertex2f(windowWidth - 300.0f, windowHeight - 115.0f);
    glEnd();

    glColor3f(0.45f, 0.35f, 0.90f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(windowWidth - 300.0f, 125.0f);
        glVertex2f(windowWidth - 25.0f, 125.0f);
        glVertex2f(windowWidth - 25.0f, windowHeight - 115.0f);
        glVertex2f(windowWidth - 300.0f, windowHeight - 115.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(windowWidth - 275.0f, windowHeight - 150.0f,
             GLUT_BITMAP_HELVETICA_18,
             genderChoice == 0 ? "BASE: MALE" : "BASE: FEMALE");

    for (int i = 0; i < CATEGORY_COUNT; i++)
    {
        float y = windowHeight - 205.0f - i * 62.0f;

        if (i == selectedCategory)
        {
            glColor3f(0.16f, 0.32f, 0.58f);
            glBegin(GL_QUADS);
                glVertex2f(windowWidth - 280.0f, y - 20.0f);
                glVertex2f(windowWidth - 45.0f, y - 20.0f);
                glVertex2f(windowWidth - 45.0f, y + 22.0f);
                glVertex2f(windowWidth - 280.0f, y + 22.0f);
            glEnd();
            glColor3f(0.50f, 0.95f, 1.0f);
        }
        else
        {
            glColor3f(0.78f, 0.75f, 0.95f);
        }

        drawText(windowWidth - 260.0f, y,
                 GLUT_BITMAP_HELVETICA_18,
                 categoryNames[i]);
    }

    glColor3f(1.0f, 0.82f, 0.25f);
    drawText(windowWidth - 275.0f, 150.0f,
             GLUT_BITMAP_HELVETICA_12,
             currentChoiceName());

    // Bottom instruction panel
    glColor3f(0.04f, 0.03f, 0.12f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(windowWidth, 0.0f);
        glVertex2f(windowWidth, 95.0f);
        glVertex2f(0.0f, 95.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(30.0f, 62.0f, GLUT_BITMAP_HELVETICA_12,
             "UP/DOWN: Category    LEFT/RIGHT: Change    G: Gender    C: Confirm");
    drawText(30.0f, 32.0f, GLUT_BITMAP_HELVETICA_12,
             "DRAG MOUSE: Orbit Camera    MOUSE WHEEL: Zoom    R: Reset View    ESC: Exit");

    if (confirmed)
    {
        glColor3f(0.30f, 1.0f, 0.55f);
        drawText(windowWidth / 2.0f - 90.0f, 112.0f,
                 GLUT_BITMAP_HELVETICA_18,
                 "OUTFIT CONFIRMED!");
    }

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void setBackgroundColor()
{
    if (backgroundChoice == 0)
        glClearColor(0.025f, 0.020f, 0.10f, 1.0f);
    else if (backgroundChoice == 1)
        glClearColor(0.32f, 0.10f, 0.22f, 1.0f);
    else
        glClearColor(0.25f, 0.48f, 0.72f, 1.0f);
}

// ============================================================
// MODULE 7: PERSPECTIVE, VIEWING, AND MATRIX STACK
// ============================================================
void display()
{
    setBackgroundColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float yawRadians = cameraYaw * PI / 180.0f;
    float pitchRadians = cameraPitch * PI / 180.0f;

    float cameraX = cameraDistance * cos(pitchRadians) * sin(yawRadians);
    float cameraY = cameraDistance * sin(pitchRadians);
    float cameraZ = cameraDistance * cos(pitchRadians) * cos(yawRadians);

    // Free orbit camera always looks at the character's center.
    gluLookAt(cameraX, cameraY, cameraZ,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    GLfloat lightPosition[] = {-3.0f, 6.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    drawCampusBackground();

    glPushMatrix();
        if (confirmed)
        {
            float elapsedSeconds =
                (glutGet(GLUT_ELAPSED_TIME) - celebrationStartTime) / 1000.0f;
            float movementPhase = elapsedSeconds * 4.0f;
            float sideOffset = std::sin(movementPhase) * 1.05f;
            float jumpValue = std::sin(movementPhase * 1.5f);
            float jumpOffset = (jumpValue > 0.0f) ? jumpValue * 0.75f : 0.0f;
            float tilt = std::sin(movementPhase) * 4.0f;

            glTranslatef(sideOffset, jumpOffset, 0.0f);
            glRotatef(tilt, 0.0f, 0.0f, 1.0f);
        }
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
    gluPerspective(45.0,
                   static_cast<float>(width) / height,
                   0.5,
                   50.0);

    glMatrixMode(GL_MODELVIEW);
}

// ============================================================
// MODULE 3: KEYBOARD CALLBACKS AND REDISPLAY
// ============================================================
void keyboard(unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);

    if (key == 'g' || key == 'G')
    {
        genderChoice = 1 - genderChoice;
        confirmed = false;
    }
    else if (key == 'c' || key == 'C')
    {
        confirmed = true;
        celebrationStartTime = glutGet(GLUT_ELAPSED_TIME);
    }
    else if (key == 'r' || key == 'R')
    {
        cameraYaw = 25.0f;
        cameraPitch = 12.0f;
        cameraDistance = 8.0f;
    }

    glutPostRedisplay();
    (void)x;
    (void)y;
}

void specialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
        selectedCategory = (selectedCategory - 1 + CATEGORY_COUNT)
                           % CATEGORY_COUNT;
    else if (key == GLUT_KEY_DOWN)
        selectedCategory = (selectedCategory + 1)
                           % CATEGORY_COUNT;
    else if (key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT)
    {
        int direction = (key == GLUT_KEY_RIGHT) ? 1 : -1;

        if (selectedCategory == 0)
            hairChoice = (hairChoice + direction + CHOICE_COUNT)
                         % CHOICE_COUNT;
        else if (selectedCategory == 1)
            topChoice = (topChoice + direction + CHOICE_COUNT)
                        % CHOICE_COUNT;
        else if (selectedCategory == 2)
            bottomChoice = (bottomChoice + direction + CHOICE_COUNT)
                           % CHOICE_COUNT;
        else if (selectedCategory == 3)
            shoesChoice = (shoesChoice + direction + CHOICE_COUNT)
                          % CHOICE_COUNT;
        else
            backgroundChoice =
                (backgroundChoice + direction + CHOICE_COUNT)
                % CHOICE_COUNT;

        confirmed = false;
    }

    glutPostRedisplay();
    (void)x;
    (void)y;
}

// ============================================================
// MODULE 3: MOUSE CALLBACKS
// Mouse dragging changes the Module 7 camera values.
// ============================================================
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            dragging = true;
            previousMouseX = x;
            previousMouseY = y;
        }
        else
        {
            dragging = false;
        }
    }

    // FreeGLUT reports the mouse wheel as buttons 3 and 4.
    if (state == GLUT_DOWN && button == 3)
        cameraDistance -= 0.4f;
    else if (state == GLUT_DOWN && button == 4)
        cameraDistance += 0.4f;

    if (cameraDistance < 5.0f)
        cameraDistance = 5.0f;
    if (cameraDistance > 13.0f)
        cameraDistance = 13.0f;

    glutPostRedisplay();
}

void mouseMotion(int x, int y)
{
    if (!dragging)
        return;

    cameraYaw += (x - previousMouseX) * 0.5f;
    cameraPitch += (previousMouseY - y) * 0.4f;

    if (cameraPitch > 70.0f)
        cameraPitch = 70.0f;
    if (cameraPitch < -45.0f)
        cameraPitch = -45.0f;

    previousMouseX = x;
    previousMouseY = y;

    glutPostRedisplay();
}

void idle()
{
    if (confirmed)
        glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(80, 50);
    glutCreateWindow("Campus Drip - 3D Pixel Character Customizer");

    initializeScene();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}


