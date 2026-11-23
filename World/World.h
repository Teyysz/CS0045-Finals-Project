#ifndef CAMPUS_DRIP_WORLD_H
#define CAMPUS_DRIP_WORLD_H

#include "App.h"

void drawBlock(float x, float y, float z,
               float width, float height, float depth,
               const Color& c);
void drawRotatedBlock(float x, float y, float z,
                      float width, float height, float depth,
                      float angleZ, const Color& c);
void drawGlowBlock(float x, float y, float z,
                   float width, float height, float depth,
                   const Color& c, float glow);

void initializeScene();
void initializeVBO();
void drawGround();
void updateLighting();
void drawBuilding(float x, float z, float w, float h, float d);
void drawTree(float x, float z);
void drawLampPost(float x, float z);
void drawBench(float x, float z, int side);
void drawSkyDecor();
void drawPlatform();
void drawCampusBackground();

#endif
