#include "World.h"

// ============================================================
// MODULE 2 OF 4: CAMPUS WORLD
// Owns drawing helpers, VBO setup, lighting, and the environment.
// ============================================================

// ============================================================
// MODULES 1-2 + 6: BASIC DRAWING HELPERS
// A block is a GLUT cube that is translated and scaled.
// ============================================================
void drawBlock(float x, float y, float z,
               float width, float height, float depth,
               const Color& c)
{
    glColor3f(c.r, c.g, c.b);
    glPushMatrix();
        glTranslatef(x, y, z);            // MODULE 6: translation
        glScalef(width, height, depth);   // MODULE 6: scaling
        glutSolidCube(1.0);
    glPopMatrix();
}

// Same as drawBlock, but tilted around the Z axis (MODULE 6: rotation)
void drawRotatedBlock(float x, float y, float z,
                      float width, float height, float depth,
                      float angleZ, const Color& c)
{
    glColor3f(c.r, c.g, c.b);
    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(angleZ, 0.0f, 0.0f, 1.0f);
        glScalef(width, height, depth);
        glutSolidCube(1.0);
    glPopMatrix();
}

// MODULE 8: a block that glows (emission). Emission is reset afterwards
// so it cannot leak into the objects drawn later.
void drawGlowBlock(float x, float y, float z,
                   float width, float height, float depth,
                   const Color& c, float glow)
{
    GLfloat emission[4] = { c.r * glow, c.g * glow, c.b * glow, 1.0f };
    GLfloat none[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
    drawBlock(x, y, z, width, height, depth, c);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, none);
}

// ============================================================
// MODULE 8: BASIC LIGHTING AND MATERIALS (one-time setup)
// ============================================================
void initializeScene()
{
    // MODULE 7: depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);          // needed because cubes are scaled

    // glColor3f() supplies the ambient and diffuse material color
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat materialSpecular[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat materialShine[] = { 24.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShine);

    GLfloat globalAmbient[] = { 0.10f, 0.10f, 0.12f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    glShadeModel(GL_SMOOTH);
}

// MODULE 5: VBO initialization
// One buffer holds 12 vertices = 3 flat quads:
//   vertices 0-3  : big grass ground
//   vertices 4-7  : main pathway to the platform
//   vertices 8-11 : cross pathway in front of the buildings
void initializeVBO()
{
    const float groundVertices[] =
    {
        // grass
        -30.0f, -2.500f, -30.0f,   -30.0f, -2.500f,  30.0f,
         30.0f, -2.500f,  30.0f,    30.0f, -2.500f, -30.0f,
        // main path
         -1.4f, -2.490f,  -3.9f,    -1.4f, -2.490f,  30.0f,
          1.4f, -2.490f,  30.0f,     1.4f, -2.490f,  -3.9f,
        // cross path
        -12.0f, -2.485f,  -3.9f,   -12.0f, -2.485f,  -2.9f,
         12.0f, -2.485f,  -2.9f,    12.0f, -2.485f,  -3.9f
    };

    glGenBuffers(1, &groundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices),
                 groundVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);   // unbind
}

// MODULE 4 + 5: draw the ground with vertex arrays stored in the VBO
void drawGround()
{
    const Theme& t = currentTheme();

    glNormal3f(0.0f, 1.0f, 0.0f);       // every ground quad faces up

    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);   // data comes from the VBO

    glColor3f(t.grass.r, t.grass.g, t.grass.b);
    glDrawArrays(GL_QUADS, 0, 4);

    glColor3f(t.path.r, t.path.g, t.path.b);
    glDrawArrays(GL_QUADS, 4, 4);

    Color cross = shade(t.path, 0.92f);
    glColor3f(cross.r, cross.g, cross.b);
    glDrawArrays(GL_QUADS, 8, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    // Unbind so later GLUT shapes (which use client memory) are not affected
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// MODULE 8: light color, strength and direction depend on the background.
// Called after gluLookAt so the light stays fixed in the world.
void updateLighting()
{
    const Theme& t = currentTheme();

    GLfloat ambient[4]  = { t.ambient.r, t.ambient.g, t.ambient.b, 1.0f };
    GLfloat diffuse[4]  = { t.diffuse.r, t.diffuse.g, t.diffuse.b, 1.0f };
    GLfloat specular[4] = { 0.60f, 0.60f, 0.60f, 1.0f };
    GLfloat position[4] = { t.lightDir[0], t.lightDir[1], t.lightDir[2], 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

// ============================================================
// CAMPUS BACKGROUND PIECES (all made from blocks)
// ============================================================
void drawBuilding(float x, float z, float w, float h, float d)
{
    const Theme& t = currentTheme();

    drawBlock(x, GROUND_Y + h * 0.5f, z, w, h, d, t.building);
    drawBlock(x, GROUND_Y + h + 0.12f, z, w + 0.25f, 0.24f, d + 0.25f, t.roof);

    // Door
    drawBlock(x, GROUND_Y + 0.55f, z + d * 0.5f + 0.03f,
              0.8f, 1.1f, 0.06f, rgb(0.25f, 0.15f, 0.10f));

    // Windows in a grid on the front face
    float frontZ = z + d * 0.5f + 0.03f;
    int columns = (int)(w / 1.1f);
    int rows = (int)((h - 1.4f) / 1.2f);

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < columns; c++)
        {
            float wx = x + (c - (columns - 1) * 0.5f) * (w / columns);
            float wy = GROUND_Y + 1.7f + r * 1.2f;
            drawGlowBlock(wx, wy, frontZ, 0.5f, 0.6f, 0.06f,
                          t.windowColor, t.windowGlow);
        }
    }
}

void drawTree(float x, float z)
{
    const Theme& t = currentTheme();

    drawBlock(x, GROUND_Y + 0.7f, z, 0.4f, 1.4f, 0.4f, rgb(0.33f, 0.20f, 0.10f));
    drawBlock(x, GROUND_Y + 1.7f, z, 1.7f, 0.9f, 1.7f, t.leaves);
    drawBlock(x, GROUND_Y + 2.45f, z, 1.3f, 0.8f, 1.3f, shade(t.leaves, 1.12f));
    drawBlock(x, GROUND_Y + 3.10f, z, 0.8f, 0.6f, 0.8f, shade(t.leaves, 1.25f));
}

void drawLampPost(float x, float z)
{
    const Theme& t = currentTheme();

    drawBlock(x, GROUND_Y + 1.3f, z, 0.12f, 2.6f, 0.12f, rgb(0.15f, 0.15f, 0.18f));
    drawGlowBlock(x, GROUND_Y + 2.7f, z, 0.5f, 0.22f, 0.5f,
                  rgb(1.0f, 0.9f, 0.6f), t.lampGlow);
    drawBlock(x, GROUND_Y + 2.88f, z, 0.62f, 0.08f, 0.62f, rgb(0.15f, 0.15f, 0.18f));
}

// side = -1 puts the backrest on the left, +1 on the right
void drawBench(float x, float z, int side)
{
    Color wood = rgb(0.50f, 0.32f, 0.16f);
    Color metal = rgb(0.18f, 0.18f, 0.22f);

    drawBlock(x, GROUND_Y + 0.5f, z, 0.5f, 0.12f, 1.3f, wood);
    drawBlock(x + side * 0.22f, GROUND_Y + 0.85f, z, 0.10f, 0.5f, 1.3f, wood);
    drawBlock(x, GROUND_Y + 0.25f, z - 0.5f, 0.4f, 0.5f, 0.1f, metal);
    drawBlock(x, GROUND_Y + 0.25f, z + 0.5f, 0.4f, 0.5f, 0.1f, metal);
}

// Moon / sun / stars / clouds far behind the scene
void drawSkyDecor()
{
    const Theme& t = currentTheme();
    int bg = selection[CAT_BACKGROUND];

    if (bg == 0)        // night: moon and stars
    {
        drawGlowBlock(-9.0f, 6.5f, -17.0f, 2.0f, 2.0f, 0.5f, t.skyObject, 1.0f);
        for (int i = 0; i < 24; i++)
        {
            float sx = -16.0f + (float)((i * 13) % 33);
            float sy = 2.0f + (float)((i * 7) % 9);
            drawGlowBlock(sx, sy, -18.0f, 0.12f, 0.12f, 0.12f,
                          rgb(1.0f, 1.0f, 0.9f), 1.0f);
        }
    }
    else if (bg == 1)   // sunset: big low sun
    {
        drawGlowBlock(8.0f, 1.5f, -17.0f, 3.2f, 3.2f, 0.5f, t.skyObject, 1.0f);
    }
    else                // day: sun and clouds
    {
        drawGlowBlock(-8.0f, 8.0f, -17.0f, 2.4f, 2.4f, 0.5f, t.skyObject, 0.6f);
        Color cloud = rgb(1.0f, 1.0f, 1.0f);
        drawGlowBlock(5.0f, 7.0f, -16.0f, 4.0f, 0.8f, 1.0f, cloud, 0.3f);
        drawGlowBlock(6.0f, 7.6f, -16.0f, 2.2f, 0.7f, 1.0f, cloud, 0.3f);
        drawGlowBlock(-3.0f, 5.5f, -16.0f, 3.2f, 0.7f, 1.0f, cloud, 0.3f);
    }
}

void drawPlatform()
{
    drawBlock(0.0f, -2.45f, 0.0f, 4.0f, 0.10f, 3.0f, rgb(0.20f, 0.22f, 0.40f));
    drawBlock(0.0f, -2.39f, 0.0f, 3.2f, 0.06f, 2.4f, rgb(0.34f, 0.38f, 0.62f));
}

void drawCampusBackground()
{
    drawSkyDecor();
    drawGround();            // MODULES 4-5
    drawPlatform();

    drawBuilding(-5.5f, -5.0f, 3.2f, 4.6f, 2.0f);
    drawBuilding(5.5f, -5.0f, 3.2f, 4.0f, 2.0f);
    drawBuilding(0.0f, -9.5f, 6.0f, 7.0f, 2.5f);

    drawTree(-6.5f, -2.0f);
    drawTree(6.5f, -2.0f);
    drawTree(-8.5f, 3.0f);
    drawTree(8.5f, 3.0f);

    drawLampPost(-2.9f, 2.8f);
    drawLampPost(2.9f, 2.8f);
    drawLampPost(-2.9f, -3.0f);
    drawLampPost(2.9f, -3.0f);

    drawBench(-3.8f, -1.2f, -1);
    drawBench(3.8f, -1.2f, 1);
}

// ============================================================
// CHARACTER PARTS
// Local coordinates: feet at y = 0, character faces +Z.
// Torso y 1.9..3.5, head y 3.7..4.8, legs y 0.4..1.95.
// ============================================================

// Neck, arms and hands (skin). Clothes are drawn over these.
