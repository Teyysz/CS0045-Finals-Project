#ifndef CAMPUS_DRIP_INTERFACE_H
#define CAMPUS_DRIP_INTERFACE_H

#include "../App/App.h"

void drawRect(float x0, float y0, float x1, float y1, const Color& c);
void drawOutline(float x0, float y0, float x1, float y1,
                 const Color& c, float lineWidth);
void drawPixelPanel(float x0, float y0, float x1, float y1,
                    const Color& fill, const Color& border);
void drawText(float x, float y, void* font, const char* text, const Color& c);
float textWidth(void* font, const char* text);
void drawTextCentered(float centerX, float y, void* font,
                      const char* text, const Color& c);
void drawTextRight(float rightX, float y, void* font,
                   const char* text, const Color& c);
void drawBottomBar(const char* line1, const char* line2);
void drawStatusMessage();

void drawWelcomeScreen();
void drawMainMenuScreen();
void drawSavedScreen();
void drawCustomizeScreen();
void drawShowcaseScreen();
void drawInterface();

void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);

#endif
