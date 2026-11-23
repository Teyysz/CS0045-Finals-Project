#ifndef CAMPUS_DRIP_CHARACTER_H
#define CAMPUS_DRIP_CHARACTER_H

#include "../App/App.h"

// Draws the whole character (feet at y = 0, facing +Z) using the current
// base, selections and pose from the shared state.
void drawCharacter();

// Display name of a pose (safe for any index)
const char* poseName(int pose);

#endif
