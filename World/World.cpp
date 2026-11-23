#include "World.h"

// ============================================================
// PART 2 OF 4: CAMPUS WORLD
// Owns shape helpers, VBO setup, lighting and the courtyard.
// Scale guide: the character is about 5.8 units tall, so a door
// is ~7 units, a floor is 9 units and a lamp post is ~12 units.
// ============================================================

// ============================================================
// MODULES 1-2 + 6: BASIC SHAPE HELPERS
// Every helper uses push/pop so it cannot change later objects.
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

// Low-resolution GLUT sphere, scaled into an ellipsoid (heads, joints, leaves)
void drawSphere(float x, float y, float z,
                float rx, float ry, float rz, const Color& c)
{
    glColor3f(c.r, c.g, c.b);
    glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(rx, ry, rz);
        glutSolidSphere(1.0, 10, 8);
    glPopMatrix();
}

void drawGlowSphere(float x, float y, float z, float radius,
                    const Color& c, float glow)
{
    GLfloat emission[4] = { c.r * glow, c.g * glow, c.b * glow, 1.0f };
    GLfloat none[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
    drawSphere(x, y, z, radius, radius, radius, c);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, none);
}

// Upright GLUT cylinder. GLUT builds cylinders along +Z, so we rotate it
// -90 degrees around X to make it stand along +Y. rx/rz can differ
// to make an oval cross-section (used for torsos).
void drawCylinderY(float x, float yBottom, float z,
                   float rx, float rz, float height, const Color& c)
{
    glColor3f(c.r, c.g, c.b);
    glPushMatrix();
        glTranslatef(x, yBottom, z);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(rx, rz, height);
        glutSolidCylinder(1.0, 1.0, 12, 1);
    glPopMatrix();
}

// Upright cone: base at yBottom, tip at yBottom + height
void drawConeY(float x, float yBottom, float z,
               float radius, float height, const Color& c, int slices)
{
    glColor3f(c.r, c.g, c.b);
    glPushMatrix();
        glTranslatef(x, yBottom, z);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(radius, radius, height);
        glutSolidCone(1.0, 1.0, slices, 1);
    glPopMatrix();
}

// MODULES 1-2: a gable roof built from GL_QUADS and GL_TRIANGLES.
// The ridge runs along X; the triangular ends are at +X and -X.
void drawPrismRoof(float x, float y, float z,
                   float width, float height, float depth, const Color& c)
{
    float hw = width * 0.5f;
    float hd = depth * 0.5f;
    float len = sqrtf(hd * hd + height * height);
    float ny = hd / len;        // slope normals point up and outward
    float nz = height / len;

    glColor3f(c.r, c.g, c.b);
    glPushMatrix();
        glTranslatef(x, y, z);

        glBegin(GL_QUADS);
            glNormal3f(0.0f, ny, nz);                 // front slope
            glVertex3f(-hw, 0.0f, hd);
            glVertex3f(hw, 0.0f, hd);
            glVertex3f(hw, height, 0.0f);
            glVertex3f(-hw, height, 0.0f);

            glNormal3f(0.0f, ny, -nz);                // back slope
            glVertex3f(hw, 0.0f, -hd);
            glVertex3f(-hw, 0.0f, -hd);
            glVertex3f(-hw, height, 0.0f);
            glVertex3f(hw, height, 0.0f);
        glEnd();

        glBegin(GL_TRIANGLES);
            glNormal3f(1.0f, 0.0f, 0.0f);             // right gable
            glVertex3f(hw, 0.0f, hd);
            glVertex3f(hw, 0.0f, -hd);
            glVertex3f(hw, height, 0.0f);

            glNormal3f(-1.0f, 0.0f, 0.0f);            // left gable
            glVertex3f(-hw, 0.0f, -hd);
            glVertex3f(-hw, 0.0f, hd);
            glVertex3f(-hw, height, 0.0f);
        glEnd();
    glPopMatrix();
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
    glEnable(GL_NORMALIZE);          // needed because shapes are scaled

    // glColor3f() supplies the ambient and diffuse material color
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat materialSpecular[] = { 0.20f, 0.20f, 0.20f, 1.0f };
    GLfloat materialShine[] = { 20.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShine);

    GLfloat globalAmbient[] = { 0.10f, 0.10f, 0.12f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    glShadeModel(GL_SMOOTH);
}

// ============================================================
// MODULE 5: VBO INITIALIZATION
// One buffer holds the flat ground pieces:
//   vertices 0-3   : big grass ground        (GL_QUADS)
//   vertices 4-7   : main path to the hall   (GL_QUADS)
//   vertices 8-11  : cross path              (GL_QUADS)
//   vertices 12-29 : round plaza around the platform (GL_TRIANGLE_FAN)
// ============================================================
static const int PLAZA_SEGMENTS = 16;
static const int FAN_START = 12;
static const int FAN_VERTICES = PLAZA_SEGMENTS + 2;    // center + rim + closing point

void initializeVBO()
{
    float vertices[(FAN_START + FAN_VERTICES) * 3] =
    {
        // grass
        -200.0f, -2.500f, -200.0f,   -200.0f, -2.500f,  200.0f,
         200.0f, -2.500f,  200.0f,    200.0f, -2.500f, -200.0f,
        // main path (x from -5 to 5, from the camera side to the hall steps)
          -5.0f, -2.470f,  -30.0f,     -5.0f, -2.470f,  80.0f,
           5.0f, -2.470f,   80.0f,      5.0f, -2.470f, -30.0f,
        // cross path in front of the buildings
         -90.0f, -2.465f,  -22.0f,    -90.0f, -2.465f, -16.0f,
          90.0f, -2.465f,  -16.0f,     90.0f, -2.465f, -22.0f
    };

    // Plaza: a triangle fan (center first, then points around a circle)
    const float plazaRadius = 10.5f;
    int index = FAN_START * 3;
    vertices[index++] = 0.0f;
    vertices[index++] = -2.485f;
    vertices[index++] = 0.0f;
    for (int i = 0; i <= PLAZA_SEGMENTS; i++)
    {
        float angle = 2.0f * PI * (float)i / (float)PLAZA_SEGMENTS;
        vertices[index++] = plazaRadius * cosf(angle);
        vertices[index++] = -2.485f;
        vertices[index++] = plazaRadius * sinf(angle);
    }

    glGenBuffers(1, &groundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);   // unbind
}

// MODULE 4 + 5: draw the ground with vertex arrays stored in the VBO
void drawGround()
{
    const Theme& t = currentTheme();

    glNormal3f(0.0f, 1.0f, 0.0f);       // every ground piece faces up

    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);   // data comes from the VBO

    glColor3f(t.grass.r, t.grass.g, t.grass.b);
    glDrawArrays(GL_QUADS, 0, 4);

    Color plaza = shade(t.path, 1.06f);
    glColor3f(plaza.r, plaza.g, plaza.b);
    glDrawArrays(GL_TRIANGLE_FAN, FAN_START, FAN_VERTICES);

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
    GLfloat specular[4] = { 0.50f, 0.50f, 0.50f, 1.0f };
    GLfloat position[4] = { t.lightDir[0], t.lightDir[1], t.lightDir[2], 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

// ============================================================
// CAMPUS PIECES (all built from basic shapes)
// ============================================================
static const float FLOOR_HEIGHT = 9.0f;

// A campus building facing +Z. The grand entrance adds steps, columns
// and a porch roof; other buildings get a smaller side door.
void drawBuilding(float x, float z, float w, float h, float d, bool grandEntrance)
{
    const Theme& t = currentTheme();
    Color trim = shade(t.building, 0.75f);
    float frontZ = z + d * 0.5f;

    // Body, base trim, roof slab and gable roof
    drawBlock(x, GROUND_Y + h * 0.5f, z, w, h, d, t.building);
    drawBlock(x, GROUND_Y + 0.6f, z, w + 0.6f, 1.2f, d + 0.6f, trim);
    drawBlock(x, GROUND_Y + h + 0.4f, z, w + 1.4f, 0.8f, d + 1.4f, trim);
    drawPrismRoof(x, GROUND_Y + h + 0.8f, z, w + 1.4f, 2.4f, d + 1.4f, t.roof);

    // Floor dividers
    int floors = (int)(h / FLOOR_HEIGHT);
    for (int f = 1; f < floors; f++)
        drawBlock(x, GROUND_Y + f * FLOOR_HEIGHT, z, w + 0.3f, 0.4f, d + 0.3f, trim);

    // Door
    float doorX = grandEntrance ? x : x - w * 0.3f;
    Color wood = rgb(0.27f, 0.16f, 0.10f);
    drawBlock(doorX, GROUND_Y + 3.6f, frontZ + 0.1f, 4.0f, 7.2f, 0.3f, trim);
    drawBlock(doorX - 0.9f, GROUND_Y + 3.5f, frontZ + 0.28f, 1.7f, 6.8f, 0.12f, wood);
    drawBlock(doorX + 0.9f, GROUND_Y + 3.5f, frontZ + 0.28f, 1.7f, 6.8f, 0.12f, wood);
    drawSphere(doorX - 0.25f, GROUND_Y + 3.4f, frontZ + 0.42f, 0.12f, 0.12f, 0.12f,
               rgb(0.85f, 0.75f, 0.30f));
    drawSphere(doorX + 0.25f, GROUND_Y + 3.4f, frontZ + 0.42f, 0.12f, 0.12f, 0.12f,
               rgb(0.85f, 0.75f, 0.30f));

    // Windows in a grid on the front face (the door area is skipped)
    int columns = (int)(w / 6.0f);
    if (columns < 1) columns = 1;
    for (int f = 0; f < floors; f++)
    {
        for (int c = 0; c < columns; c++)
        {
            float wx = x + (c - (columns - 1) * 0.5f) * (w / columns);
            float wy = GROUND_Y + f * FLOOR_HEIGHT + 5.4f;
            if (f == 0 && fabsf(wx - doorX) < 3.6f)
                continue;

            drawBlock(wx, wy, frontZ + 0.04f, 3.2f, 4.4f, 0.1f, trim);            // frame
            drawGlowBlock(wx, wy, frontZ + 0.1f, 2.6f, 3.8f, 0.1f,
                          t.windowColor, t.windowGlow);                           // glass
            drawBlock(wx, wy, frontZ + 0.18f, 2.6f, 0.12f, 0.06f, trim);          // crossbar
            drawBlock(wx, wy - 2.2f, frontZ + 0.4f, 3.4f, 0.25f, 0.7f, trim);     // sill
        }
    }

    if (grandEntrance)
    {
        // Steps
        drawBlock(x, GROUND_Y + 0.25f, frontZ + 4.6f, 13.0f, 0.5f, 2.2f, trim);
        drawBlock(x, GROUND_Y + 0.75f, frontZ + 3.4f, 12.0f, 0.5f, 2.2f, shade(trim, 1.05f));

        // Porch slab, columns and gable roof
        drawBlock(x, GROUND_Y + 9.0f, frontZ + 2.6f, 12.0f, 0.7f, 5.2f, trim);
        for (int side = -1; side <= 1; side += 2)
        {
            drawCylinderY(x + side * 5.0f, GROUND_Y + 1.0f, frontZ + 4.4f,
                          0.7f, 0.7f, 8.0f, shade(t.building, 1.15f));
            drawCylinderY(x + side * 5.0f, GROUND_Y + 1.0f, frontZ + 4.4f,
                          0.95f, 0.95f, 0.35f, trim);
        }
        drawPrismRoof(x, GROUND_Y + 9.35f, frontZ + 2.6f, 12.6f, 2.6f, 5.8f, t.roof);

        // Clock tower with a pyramid spire
        drawBlock(x, GROUND_Y + h + 4.0f, z, 8.0f, 8.0f, 8.0f, t.building);
        drawGlowBlock(x, GROUND_Y + h + 5.0f, z + 4.1f, 3.4f, 3.4f, 0.2f,
                      shade(t.windowColor, 1.1f), t.windowGlow);
        glPushMatrix();
            glTranslatef(x, 0.0f, z);
            glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
            drawConeY(0.0f, GROUND_Y + h + 8.0f, 0.0f, 6.2f, 5.0f, t.roof, 4);
        glPopMatrix();
    }
}

void drawTree(float x, float z)
{
    const Theme& t = currentTheme();

    drawCylinderY(x, GROUND_Y, z, 0.8f, 0.8f, 7.0f, rgb(0.33f, 0.20f, 0.10f));
    drawSphere(x, GROUND_Y + 10.0f, z, 4.4f, 4.0f, 4.4f, t.leaves);
    drawSphere(x + 2.4f, GROUND_Y + 8.6f, z + 1.0f, 3.2f, 3.0f, 3.2f, shade(t.leaves, 0.9f));
    drawSphere(x - 2.2f, GROUND_Y + 8.8f, z - 1.2f, 3.3f, 3.0f, 3.3f, shade(t.leaves, 1.1f));
    drawSphere(x, GROUND_Y + 12.6f, z, 2.9f, 2.6f, 2.9f, shade(t.leaves, 1.2f));
}

void drawBush(float x, float z, float size)
{
    const Theme& t = currentTheme();
    drawSphere(x, GROUND_Y + 0.9f * size, z, 1.9f * size, 1.3f * size, 1.5f * size,
               shade(t.leaves, 0.95f));
    drawSphere(x + 1.2f * size, GROUND_Y + 0.7f * size, z + 0.3f * size,
               1.3f * size, 1.0f * size, 1.2f * size, shade(t.leaves, 1.1f));
}

void drawLampPost(float x, float z)
{
    const Theme& t = currentTheme();
    Color iron = rgb(0.15f, 0.15f, 0.18f);

    drawCylinderY(x, GROUND_Y, z, 0.6f, 0.6f, 1.0f, iron);
    drawCylinderY(x, GROUND_Y + 1.0f, z, 0.22f, 0.22f, 10.5f, iron);
    drawGlowSphere(x, GROUND_Y + 11.9f, z, 1.0f, rgb(1.0f, 0.9f, 0.6f), t.lampGlow);
    drawConeY(x, GROUND_Y + 12.6f, z, 1.4f, 1.0f, iron, 8);
}

// Bench facing +Z in its own frame; rotationY turns it toward the path
void drawBench(float x, float z, float rotationY)
{
    Color wood = rgb(0.50f, 0.32f, 0.16f);
    Color metal = rgb(0.18f, 0.18f, 0.22f);

    glPushMatrix();
        glTranslatef(x, GROUND_Y, z);
        glRotatef(rotationY, 0.0f, 1.0f, 0.0f);

        drawBlock(0.0f, 1.6f, 0.0f, 5.4f, 0.35f, 1.8f, wood);          // seat
        drawBlock(0.0f, 2.9f, -0.85f, 5.4f, 1.6f, 0.3f, wood);         // backrest
        for (int side = -1; side <= 1; side += 2)
        {
            drawBlock(side * 2.4f, 0.8f, 0.0f, 0.35f, 1.6f, 1.6f, metal);        // legs
            drawBlock(side * 2.6f, 2.2f, 0.0f, 0.3f, 0.3f, 1.6f, metal);         // armrests
            drawBlock(side * 2.6f, 1.9f, 0.7f, 0.3f, 0.7f, 0.3f, metal);
        }
    glPopMatrix();
}

void drawFlagPole(float x, float z)
{
    drawCylinderY(x, GROUND_Y, z, 0.2f, 0.2f, 20.0f, rgb(0.8f, 0.8f, 0.82f));
    drawSphere(x, GROUND_Y + 20.2f, z, 0.35f, 0.35f, 0.35f, rgb(0.9f, 0.75f, 0.25f));
    drawBlock(x + 1.9f, GROUND_Y + 18.4f, z, 3.6f, 2.4f, 0.08f, rgb(0.55f, 0.08f, 0.14f));
}

// Moon / sun / stars / clouds far behind the scene
void drawSkyDecor()
{
    const Theme& t = currentTheme();
    int bg = selection[CAT_BACKGROUND];

    if (bg == 0)        // night: moon and stars
    {
        drawGlowSphere(-60.0f, 50.0f, -190.0f, 9.0f, t.skyObject, 1.0f);
        for (int i = 0; i < 40; i++)
        {
            float sx = -150.0f + (float)((i * 37) % 300);
            float sy = 25.0f + (float)((i * 23) % 70);
            drawGlowBlock(sx, sy, -195.0f, 1.2f, 1.2f, 1.2f, rgb(1.0f, 1.0f, 0.9f), 1.0f);
        }
    }
    else if (bg == 1)   // sunset: big low sun
    {
        drawGlowSphere(45.0f, 16.0f, -190.0f, 16.0f, t.skyObject, 1.0f);
    }
    else                // day: sun and clouds
    {
        drawGlowSphere(-70.0f, 70.0f, -190.0f, 10.0f, t.skyObject, 0.7f);
        Color cloud = rgb(1.0f, 1.0f, 1.0f);
        drawSphere(40.0f, 55.0f, -170.0f, 14.0f, 4.0f, 5.0f, cloud);
        drawSphere(52.0f, 58.0f, -170.0f, 9.0f, 4.5f, 5.0f, cloud);
        drawSphere(-20.0f, 42.0f, -175.0f, 12.0f, 3.5f, 5.0f, cloud);
    }
}

// Round stage for the character: two stacked cylinders and a glowing ring
void drawPlatform()
{
    drawCylinderY(0.0f, GROUND_Y, 0.0f, 4.8f, 4.8f, 0.45f, rgb(0.20f, 0.22f, 0.40f));
    drawCylinderY(0.0f, GROUND_Y + 0.40f, 0.0f, 4.9f, 4.9f, 0.08f, rgb(0.45f, 0.40f, 0.95f));
    drawCylinderY(0.0f, GROUND_Y + 0.45f, 0.0f, 4.1f, 4.1f, 0.17f, rgb(0.34f, 0.38f, 0.62f));
}

void drawCampusBackground()
{
    drawSkyDecor();
    drawGround();            // MODULES 4-5
    drawPlatform();

    // Buildings stand well behind the character
    drawBuilding(0.0f, -42.0f, 46.0f, 18.0f, 14.0f, true);
    drawBuilding(-41.0f, -36.0f, 28.0f, 18.0f, 14.0f, false);
    drawBuilding(41.0f, -36.0f, 28.0f, 18.0f, 14.0f, false);

    // Trees line the courtyard
    drawTree(-17.0f, 4.0f);
    drawTree(17.0f, 4.0f);
    drawTree(-18.0f, -10.0f);
    drawTree(18.0f, -10.0f);
    drawTree(-30.0f, -4.0f);
    drawTree(30.0f, -4.0f);

    // Lamp posts along the main path
    drawLampPost(-6.8f, 8.0f);
    drawLampPost(6.8f, 8.0f);
    drawLampPost(-6.8f, -9.0f);
    drawLampPost(6.8f, -9.0f);
    drawLampPost(-6.8f, -26.0f);
    drawLampPost(6.8f, -26.0f);

    // Benches face the path
    drawBench(-10.5f, -4.0f, 90.0f);
    drawBench(10.5f, -4.0f, -90.0f);
    drawBench(-10.5f, 12.0f, 90.0f);
    drawBench(10.5f, 12.0f, -90.0f);

    // Bushes along the building fronts and a flag pole
    for (int i = -2; i <= 2; i++)
    {
        if (i == 0) continue;
        drawBush(i * 8.0f, -34.0f, 1.0f);
    }
    drawBush(-41.0f, -28.0f, 1.2f);
    drawBush(-34.0f, -28.0f, 1.0f);
    drawBush(41.0f, -28.0f, 1.2f);
    drawBush(34.0f, -28.0f, 1.0f);
    drawFlagPole(-14.0f, -27.0f);
}
