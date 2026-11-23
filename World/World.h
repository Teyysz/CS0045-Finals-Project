#ifndef CAMPUS_DRIP_WORLD_H
#define CAMPUS_DRIP_WORLD_H

#include "../App/App.h"

// ------------------------------------------------------------
// Reusable shape helpers (every helper restores the matrix stack)
// ------------------------------------------------------------
void drawBlock(float x, float y, float z,
               float width, float height, float depth,
               const Color& c);
void drawRotatedBlock(float x, float y, float z,
                      float width, float height, float depth,
                      float angleZ, const Color& c);
void drawGlowBlock(float x, float y, float z,
                   float width, float height, float depth,
                   const Color& c, float glow);
void drawSphere(float x, float y, float z,
                float rx, float ry, float rz, const Color& c);
void drawGlowSphere(float x, float y, float z, float radius,
                    const Color& c, float glow);
void drawCylinderY(float x, float yBottom, float z,
                   float rx, float rz, float height, const Color& c);
void drawConeY(float x, float yBottom, float z,
               float radius, float height, const Color& c, int slices);
void drawPrismRoof(float x, float y, float z,
                   float width, float height, float depth, const Color& c);

// ------------------------------------------------------------
// Setup, lighting and the campus environment
// ------------------------------------------------------------
void initializeScene();
void initializeVBO();
void drawGround();
void updateLighting();
void drawBuilding(float x, float z, float w, float h, float d, bool grandEntrance);
void drawTree(float x, float z);
void drawBush(float x, float z, float size);
void drawLampPost(float x, float z);
void drawBench(float x, float z, float rotationY);
void drawFlagPole(float x, float z);
void drawSkyDecor();
void drawPlatform();
void drawCampusBackground();

#endif
