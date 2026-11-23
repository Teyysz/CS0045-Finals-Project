#ifndef CAMPUS_DRIP_CHARACTER_H
#define CAMPUS_DRIP_CHARACTER_H

#include "App.h"

void drawBodyBase(float bodyWidth);
void drawHead();
void drawHair();
void drawSleeve(float bodyWidth, int side, float yBottom, float yTop,
                float grow, const Color& c);
void drawTop(float bodyWidth);
void drawBottom();
void drawShoes();
void drawAccessory(float bodyWidth);
void drawCharacter();

#endif
