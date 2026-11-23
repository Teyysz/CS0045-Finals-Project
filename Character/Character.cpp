#include "Character.h"
#include "World.h"

// ============================================================
// MODULE 3 OF 4: CHARACTER
// Owns the block character, body parts, outfits, and accessories.
// ============================================================

void drawBodyBase(float bodyWidth)
{
    float armX = bodyWidth * 0.5f + 0.20f;

    drawBlock(0.0f, 3.62f, 0.0f, 0.44f, 0.40f, 0.44f, SKIN_SHADE);   // neck

    for (int side = -1; side <= 1; side += 2)
    {
        drawBlock(side * armX, 3.05f, 0.0f, ARM_W, 0.90f, ARM_D, SKIN);   // upper arm
        drawBlock(side * armX, 2.25f, 0.0f, ARM_W, 0.70f, ARM_D, SKIN);   // forearm
        drawBlock(side * armX, 1.72f, 0.0f, 0.40f, 0.36f, 0.50f, SKIN);   // hand
    }
}

// Face details are all on the FRONT (+Z) of the head
void drawHead()
{
    bool female = (genderChoice == 1);
    Color dark = rgb(0.05f, 0.04f, 0.04f);
    Color browColor = rgb(0.10f, 0.06f, 0.04f);

    drawBlock(0.0f, 4.25f, 0.0f, 1.20f, 1.10f, 1.10f, SKIN);          // head
    drawBlock(-0.65f, 4.15f, 0.0f, 0.12f, 0.34f, 0.30f, SKIN_SHADE);  // ears
    drawBlock(0.65f, 4.15f, 0.0f, 0.12f, 0.34f, 0.30f, SKIN_SHADE);

    for (int side = -1; side <= 1; side += 2)
    {
        float ex = side * 0.27f;
        drawBlock(ex, 4.36f, 0.55f, 0.24f, 0.20f, 0.04f, rgb(0.97f, 0.97f, 0.97f)); // eye white
        drawBlock(ex, 4.35f, 0.565f, 0.12f, 0.16f, 0.03f, dark);                    // pupil
        drawBlock(ex + side * 0.03f, 4.40f, 0.58f, 0.04f, 0.04f, 0.01f,
                  rgb(1.0f, 1.0f, 1.0f));                                           // sparkle

        if (female)
        {
            drawBlock(ex, 4.54f, 0.55f, 0.26f, 0.06f, 0.04f, browColor);            // thin brow
            drawBlock(ex, 4.46f, 0.56f, 0.30f, 0.04f, 0.04f, dark);                 // lashes
        }
        else
        {
            drawBlock(ex, 4.54f, 0.55f, 0.30f, 0.09f, 0.04f, browColor);            // thick brow
        }
    }

    drawBlock(0.0f, 4.12f, 0.59f, 0.16f, 0.20f, 0.12f, SKIN_SHADE);                 // nose

    Color lips = rgb(0.62f, 0.22f, 0.22f);
    drawBlock(0.0f, 3.89f, 0.55f, 0.34f, 0.06f, 0.04f, lips);                       // mouth
    drawBlock(-0.20f, 3.93f, 0.55f, 0.06f, 0.06f, 0.04f, lips);                     // smile
    drawBlock(0.20f, 3.93f, 0.55f, 0.06f, 0.06f, 0.04f, lips);

    if (female)
    {
        Color blush = rgb(0.88f, 0.45f, 0.42f);
        drawBlock(-0.40f, 4.00f, 0.55f, 0.16f, 0.08f, 0.03f, blush);
        drawBlock(0.40f, 4.00f, 0.55f, 0.16f, 0.08f, 0.03f, blush);
    }
}

void drawHair()
{
    int style = selection[CAT_HAIR];
    bool capOn = (selection[CAT_ACCESSORY] == ACC_CAP);
    bool female = (genderChoice == 1);

    Color hair;
    if (style == 0)      hair = rgb(0.20f, 0.10f, 0.05f);
    else if (style == 1) hair = rgb(0.08f, 0.06f, 0.05f);
    else                 hair = rgb(0.32f, 0.17f, 0.06f);
    Color light = shade(hair, 1.6f);

    // Back and sides are shared by all styles (ears stay visible)
    drawBlock(0.0f, 4.35f, -0.60f, 1.34f, 0.95f, 0.22f, hair);
    drawBlock(-0.64f, 4.62f, 0.0f, 0.14f, 0.40f, 1.04f, hair);
    drawBlock(0.64f, 4.62f, 0.0f, 0.14f, 0.40f, 1.04f, hair);

    // Base-specific length
    if (female)
    {
        if (style == 2)   // ponytail for the spiky style
        {
            drawBlock(0.0f, 4.55f, -0.80f, 0.32f, 0.80f, 0.32f, hair);
            drawBlock(0.0f, 4.90f, -0.78f, 0.36f, 0.10f, 0.36f, rgb(0.95f, 0.35f, 0.45f));
        }
        else              // long hair down the back
        {
            drawBlock(0.0f, 3.90f, -0.66f, 1.34f, 1.40f, 0.20f, hair);
        }
    }

    // The cap covers the top of the hair, so skip the top pieces
    if (capOn)
        return;

    if (style == 0)       // CLASSIC: rounded top, short fringe
    {
        drawBlock(0.0f, 4.88f, 0.0f, 1.34f, 0.30f, 1.22f, hair);
        drawBlock(0.0f, 4.72f, 0.62f, 1.34f, 0.20f, 0.14f, hair);
        drawBlock(-0.25f, 5.05f, 0.10f, 0.60f, 0.06f, 0.70f, light);
    }
    else if (style == 1)  // SIDE PART: swept fringe, longer left side
    {
        drawBlock(-0.05f, 4.88f, 0.0f, 1.38f, 0.30f, 1.22f, hair);
        drawBlock(-0.25f, 4.70f, 0.62f, 0.90f, 0.24f, 0.14f, hair);
        drawBlock(0.40f, 4.77f, 0.62f, 0.50f, 0.10f, 0.14f, hair);
        drawBlock(0.12f, 5.04f, 0.10f, 0.05f, 0.04f, 1.00f, light);
        drawBlock(-0.68f, 4.55f, 0.05f, 0.14f, 0.60f, 0.95f, hair);
    }
    else                  // SPIKY: tilted spikes with lighter tips
    {
        drawBlock(0.0f, 4.84f, 0.0f, 1.34f, 0.20f, 1.22f, hair);
        for (int i = -2; i <= 2; i++)
        {
            float h = 0.62f - fabs((float)i) * 0.10f;
            float angle = -14.0f * i;
            float x = 0.27f * i;
            float cy = 4.90f + h * 0.5f;
            float rad = angle * PI / 180.0f;

            drawRotatedBlock(x, cy, 0.0f, 0.26f, h, 0.80f, angle, hair);
            drawRotatedBlock(x - sinf(rad) * (h * 0.5f),
                             cy + cosf(rad) * (h * 0.5f), 0.0f,
                             0.22f, 0.14f, 0.72f, angle, light);
        }
        drawBlock(-0.40f, 4.74f, 0.62f, 0.30f, 0.22f, 0.14f, hair);
        drawBlock(0.00f, 4.76f, 0.62f, 0.30f, 0.28f, 0.14f, hair);
        drawBlock(0.40f, 4.74f, 0.62f, 0.30f, 0.22f, 0.14f, hair);
    }
}

// Sleeve helper: wraps the arm between two heights
void drawSleeve(float bodyWidth, int side, float yBottom, float yTop,
                float grow, const Color& c)
{
    float armX = bodyWidth * 0.5f + 0.20f;
    drawBlock(side * armX, (yBottom + yTop) * 0.5f, 0.0f,
              ARM_W + grow, yTop - yBottom, ARM_D + grow, c);
}

void drawTop(float bodyWidth)
{
    int style = selection[CAT_TOP];

    if (style == 0)   // VARSITY jacket
    {
        Color maroon = rgb(0.52f, 0.06f, 0.13f);
        Color dark = rgb(0.34f, 0.04f, 0.09f);
        Color cream = rgb(0.90f, 0.84f, 0.72f);
        Color gold = rgb(0.95f, 0.78f, 0.20f);

        drawBlock(0.0f, 2.70f, 0.0f, bodyWidth, 1.60f, 0.86f, maroon);        // jacket body
        drawBlock(0.0f, 1.97f, 0.0f, bodyWidth + 0.04f, 0.14f, 0.90f, dark);  // bottom band
        drawBlock(0.0f, 2.75f, 0.44f, 0.44f, 1.40f, 0.06f, rgb(0.92f, 0.92f, 0.88f)); // shirt under opening
        drawBlock(-0.25f, 2.75f, 0.455f, 0.06f, 1.45f, 0.05f, dark);          // opening edges
        drawBlock(0.25f, 2.75f, 0.455f, 0.06f, 1.45f, 0.05f, dark);
        drawBlock(0.0f, 3.56f, 0.0f, 0.86f, 0.22f, 0.86f, dark);              // collar

        for (int i = 0; i < 4; i++)                                           // gold snaps
            drawBlock(-0.36f, 3.20f - i * 0.40f, 0.45f, 0.08f, 0.08f, 0.05f, gold);

        // chest emblem and back emblem
        drawBlock(-0.56f, 3.05f, 0.44f, 0.30f, 0.30f, 0.05f, gold);
        drawBlock(-0.56f, 3.05f, 0.45f, 0.14f, 0.14f, 0.05f, maroon);
        drawBlock(0.0f, 2.90f, -0.44f, 0.70f, 0.70f, 0.04f, gold);
        drawBlock(0.0f, 2.90f, -0.45f, 0.50f, 0.50f, 0.04f, maroon);
        drawBlock(0.0f, 2.90f, -0.47f, 0.22f, 0.22f, 0.04f, gold);

        for (int side = -1; side <= 1; side += 2)
        {
            drawSleeve(bodyWidth, side, 1.98f, 3.52f, 0.05f, cream);          // long sleeves
            drawSleeve(bodyWidth, side, 3.00f, 3.08f, 0.08f, maroon);         // stripes
            drawSleeve(bodyWidth, side, 2.82f, 2.90f, 0.08f, maroon);
            drawSleeve(bodyWidth, side, 1.98f, 2.12f, 0.08f, dark);           // cuff
        }
    }
    else if (style == 1)   // SCHOOL uniform
    {
        Color shirt = rgb(0.93f, 0.94f, 0.97f);
        Color navy = rgb(0.08f, 0.16f, 0.45f);
        Color gray = rgb(0.72f, 0.75f, 0.82f);

        drawBlock(0.0f, 2.70f, 0.0f, bodyWidth, 1.60f, 0.86f, shirt);         // shirt
        drawBlock(0.0f, 3.56f, 0.0f, 0.70f, 0.20f, 0.70f, rgb(0.98f, 0.98f, 1.0f)); // collar ring
        drawBlock(-0.20f, 3.45f, 0.40f, 0.28f, 0.22f, 0.12f, rgb(0.98f, 0.98f, 1.0f)); // collar flaps
        drawBlock(0.20f, 3.45f, 0.40f, 0.28f, 0.22f, 0.12f, rgb(0.98f, 0.98f, 1.0f));

        drawBlock(0.0f, 3.38f, 0.46f, 0.20f, 0.16f, 0.08f, navy);             // tie knot
        drawBlock(0.0f, 2.95f, 0.45f, 0.17f, 0.72f, 0.06f, navy);             // tie body
        drawBlock(0.0f, 2.50f, 0.45f, 0.22f, 0.20f, 0.06f, navy);             // tie tip
        drawBlock(0.0f, 2.95f, 0.49f, 0.17f, 0.06f, 0.02f, gray);             // tie stripe

        drawBlock(0.0f, 2.20f, 0.44f, 0.07f, 0.07f, 0.05f, gray);             // buttons
        drawBlock(0.0f, 2.00f, 0.44f, 0.07f, 0.07f, 0.05f, gray);

        drawBlock(-0.35f, 2.95f, 0.44f, 0.28f, 0.30f, 0.04f, rgb(0.80f, 0.85f, 0.95f)); // pocket
        drawBlock(-0.35f, 3.08f, 0.46f, 0.28f, 0.04f, 0.02f, gray);

        for (int side = -1; side <= 1; side += 2)
        {
            drawSleeve(bodyWidth, side, 2.62f, 3.52f, 0.05f, shirt);          // short sleeves
            drawSleeve(bodyWidth, side, 2.62f, 2.70f, 0.08f, navy);           // sleeve trim
        }
    }
    else   // TAMBAY casual tee
    {
        Color teal = rgb(0.12f, 0.55f, 0.45f);
        Color tealDark = rgb(0.08f, 0.36f, 0.30f);
        Color yellow = rgb(0.98f, 0.82f, 0.25f);

        drawBlock(0.0f, 2.70f, 0.0f, bodyWidth, 1.60f, 0.86f, teal);          // tee
        drawBlock(0.0f, 1.97f, 0.0f, bodyWidth + 0.02f, 0.14f, 0.88f, tealDark); // hem
        drawBlock(0.0f, 3.54f, 0.0f, 0.64f, 0.14f, 0.60f, tealDark);          // round neckline

        drawBlock(0.0f, 3.10f, 0.44f, 0.80f, 0.10f, 0.04f, yellow);           // chest stripes
        drawBlock(0.0f, 2.90f, 0.44f, 0.80f, 0.10f, 0.04f, rgb(1.0f, 1.0f, 1.0f));
        drawBlock(0.0f, 2.70f, 0.44f, 0.80f, 0.10f, 0.04f, yellow);

        drawBlock(0.0f, 3.00f, -0.44f, 0.60f, 0.50f, 0.04f, yellow);          // back print
        drawBlock(0.0f, 3.00f, -0.45f, 0.40f, 0.30f, 0.04f, tealDark);

        for (int side = -1; side <= 1; side += 2)
        {
            drawSleeve(bodyWidth, side, 2.75f, 3.52f, 0.05f, teal);           // short sleeves
            drawSleeve(bodyWidth, side, 2.75f, 2.82f, 0.08f, tealDark);
        }
    }
}

void drawBottom()
{
    int style = selection[CAT_BOTTOM];

    if (style == 0)   // CARGO pants
    {
        Color olive = rgb(0.34f, 0.38f, 0.22f);
        Color oliveDark = rgb(0.24f, 0.27f, 0.15f);

        drawBlock(0.0f, 1.85f, 0.0f, 1.52f, 0.30f, 0.80f, olive);             // hips
        drawBlock(0.0f, 1.86f, 0.0f, 1.58f, 0.16f, 0.92f, rgb(0.15f, 0.12f, 0.10f)); // belt
        drawBlock(0.0f, 1.86f, 0.47f, 0.18f, 0.12f, 0.03f, rgb(0.85f, 0.75f, 0.30f)); // buckle

        for (int side = -1; side <= 1; side += 2)
        {
            float lx = side * 0.38f;
            drawBlock(lx, 1.175f, 0.0f, 0.74f, 1.55f, 0.76f, olive);          // leg
            drawBlock(lx, 0.50f, 0.0f, 0.78f, 0.20f, 0.80f, oliveDark);       // cuff
            drawBlock(side * 0.80f, 1.25f, 0.0f, 0.10f, 0.50f, 0.50f, shade(olive, 0.85f)); // cargo pocket
            drawBlock(side * 0.80f, 1.52f, 0.0f, 0.12f, 0.10f, 0.52f, oliveDark);           // flap
            drawBlock(lx, 1.60f, -0.39f, 0.30f, 0.28f, 0.04f, oliveDark);     // back pocket
        }
    }
    else if (style == 1)   // JEANS
    {
        Color denim = rgb(0.13f, 0.28f, 0.55f);
        Color denimLight = rgb(0.25f, 0.45f, 0.75f);

        drawBlock(0.0f, 1.85f, 0.0f, 1.52f, 0.30f, 0.80f, denim);             // hips
        drawBlock(0.0f, 1.86f, 0.0f, 1.58f, 0.14f, 0.92f, rgb(0.30f, 0.18f, 0.09f)); // brown belt
        drawBlock(0.0f, 1.86f, 0.47f, 0.14f, 0.10f, 0.03f, rgb(0.85f, 0.75f, 0.30f));

        for (int side = -1; side <= 1; side += 2)
        {
            float lx = side * 0.38f;
            drawBlock(lx, 1.175f, 0.0f, 0.74f, 1.55f, 0.76f, denim);          // leg
            drawBlock(lx, 0.50f, 0.0f, 0.78f, 0.18f, 0.80f, denimLight);      // rolled cuff
            drawBlock(side * 0.765f, 1.18f, 0.0f, 0.03f, 1.40f, 0.06f, denimLight); // side seam
            drawBlock(lx, 1.68f, 0.385f, 0.34f, 0.05f, 0.02f, denimLight);    // pocket stitch
            drawBlock(lx, 1.05f, 0.385f, 0.28f, 0.18f, 0.02f, denimLight);    // knee fade
            drawBlock(lx, 1.55f, -0.385f, 0.34f, 0.30f, 0.02f, denimLight);   // back pocket
        }
    }
    else   // SHORTS
    {
        Color khaki = rgb(0.78f, 0.66f, 0.45f);
        Color khakiDark = rgb(0.60f, 0.50f, 0.32f);

        drawBlock(0.0f, 1.85f, 0.0f, 1.52f, 0.30f, 0.80f, khaki);             // hips
        drawBlock(0.0f, 1.86f, 0.0f, 1.58f, 0.18f, 0.92f, rgb(0.30f, 0.30f, 0.38f)); // waistband
        drawBlock(-0.10f, 1.66f, 0.47f, 0.05f, 0.26f, 0.03f, rgb(1.0f, 1.0f, 1.0f));  // drawstrings
        drawBlock(0.10f, 1.66f, 0.47f, 0.05f, 0.26f, 0.03f, rgb(1.0f, 1.0f, 1.0f));

        for (int side = -1; side <= 1; side += 2)
        {
            float lx = side * 0.38f;
            drawBlock(lx, 0.75f, 0.0f, 0.66f, 0.70f, 0.66f, SKIN);            // bare legs
            drawBlock(lx, 1.475f, 0.0f, 0.78f, 0.95f, 0.80f, khaki);          // shorts
            drawBlock(lx, 1.06f, 0.0f, 0.80f, 0.12f, 0.82f, khakiDark);       // hem
            drawBlock(lx, 1.55f, -0.41f, 0.30f, 0.30f, 0.04f, khakiDark);     // back pocket
            drawBlock(lx, 0.54f, 0.0f, 0.68f, 0.28f, 0.68f, rgb(0.95f, 0.95f, 0.95f)); // socks
            drawBlock(lx, 0.66f, 0.0f, 0.70f, 0.05f, 0.70f, rgb(0.90f, 0.20f, 0.20f)); // sock stripe
        }
    }
}

void drawShoes()
{
    int style = selection[CAT_SHOES];

    for (int side = -1; side <= 1; side += 2)
    {
        float x = side * 0.38f;

        if (style == 0)   // SNEAKERS
        {
            drawBlock(x, 0.06f, 0.15f, 0.80f, 0.12f, 1.14f, rgb(0.75f, 0.77f, 0.82f)); // sole
            drawBlock(x, 0.26f, 0.15f, 0.76f, 0.28f, 1.10f, rgb(0.93f, 0.93f, 0.95f)); // upper
            drawBlock(x, 0.25f, 0.62f, 0.78f, 0.26f, 0.20f, rgb(0.82f, 0.84f, 0.90f)); // toe cap
            drawBlock(x, 0.27f, -0.42f, 0.50f, 0.22f, 0.05f, rgb(0.80f, 0.10f, 0.15f)); // heel tab
            for (int i = 0; i < 3; i++)                                                 // laces
                drawBlock(x, 0.405f, 0.10f + i * 0.14f, 0.50f, 0.03f, 0.07f,
                          rgb(0.45f, 0.55f, 0.75f));
        }
        else if (style == 1)   // RUNNERS
        {
            drawBlock(x, 0.08f, 0.15f, 0.80f, 0.16f, 1.14f, rgb(0.97f, 0.97f, 0.97f)); // thick sole
            drawBlock(x, 0.29f, 0.15f, 0.76f, 0.26f, 1.10f, rgb(0.08f, 0.70f, 0.82f)); // upper
            drawBlock(x, 0.22f, 0.68f, 0.78f, 0.12f, 0.12f, rgb(0.97f, 0.97f, 0.97f)); // toe bumper
            drawBlock(x + side * 0.395f, 0.29f, 0.10f, 0.04f, 0.10f, 0.55f,
                      rgb(1.00f, 0.55f, 0.10f));                                        // side stripe
            drawBlock(x, 0.44f, 0.10f, 0.30f, 0.06f, 0.30f, rgb(0.98f, 0.85f, 0.25f)); // tongue
        }
        else   // DARK SHOES
        {
            drawBlock(x, 0.05f, 0.15f, 0.80f, 0.10f, 1.14f, rgb(0.22f, 0.13f, 0.08f)); // sole
            drawBlock(x, 0.27f, 0.15f, 0.76f, 0.34f, 1.10f, rgb(0.07f, 0.07f, 0.09f)); // upper
            drawBlock(x, 0.45f, 0.55f, 0.50f, 0.04f, 0.20f, rgb(0.35f, 0.35f, 0.40f)); // shine
            drawBlock(x, 0.45f, 0.05f, 0.40f, 0.03f, 0.05f, rgb(0.40f, 0.40f, 0.45f)); // laces
            drawBlock(x, 0.45f, 0.20f, 0.40f, 0.03f, 0.05f, rgb(0.40f, 0.40f, 0.45f));
        }
    }
}

void drawAccessory(float bodyWidth)
{
    (void)bodyWidth;
    int acc = selection[CAT_ACCESSORY];

    if (acc == ACC_GLASSES)
    {
        Color frame = rgb(0.08f, 0.08f, 0.10f);

        for (int side = -1; side <= 1; side += 2)
        {
            float cx = side * 0.27f;
            drawBlock(cx, 4.52f, 0.60f, 0.46f, 0.05f, 0.05f, frame);    // top bar
            drawBlock(cx, 4.22f, 0.60f, 0.46f, 0.05f, 0.05f, frame);    // bottom bar
            drawBlock(cx - 0.21f, 4.37f, 0.60f, 0.05f, 0.30f, 0.05f, frame);
            drawBlock(cx + 0.21f, 4.37f, 0.60f, 0.05f, 0.30f, 0.05f, frame);
            drawBlock(side * 0.62f, 4.40f, 0.25f, 0.04f, 0.05f, 0.70f, frame); // temple arm
        }
        drawBlock(0.0f, 4.45f, 0.60f, 0.12f, 0.05f, 0.05f, frame);      // bridge
    }
    else if (acc == ACC_CAP)
    {
        Color navy = rgb(0.12f, 0.18f, 0.45f);
        Color navyDark = rgb(0.08f, 0.12f, 0.32f);

        drawBlock(0.0f, 4.94f, 0.0f, 1.46f, 0.34f, 1.44f, navy);        // dome
        drawBlock(0.0f, 5.15f, 0.0f, 1.20f, 0.16f, 1.20f, navy);        // crown
        drawBlock(0.0f, 5.28f, 0.0f, 0.16f, 0.08f, 0.16f, navyDark);    // button
        drawBlock(0.0f, 4.78f, 0.0f, 1.48f, 0.05f, 1.46f, navyDark);    // band
        drawBlock(0.0f, 4.80f, 1.02f, 1.24f, 0.07f, 0.60f, navyDark);   // brim
        drawBlock(0.0f, 4.98f, 0.73f, 0.30f, 0.20f, 0.04f, rgb(0.98f, 0.82f, 0.25f)); // logo
    }
    else if (acc == ACC_BACKPACK)
    {
        Color orange = rgb(0.92f, 0.46f, 0.10f);
        Color orangeDark = rgb(0.62f, 0.28f, 0.06f);
        Color strap = rgb(0.20f, 0.20f, 0.24f);

        drawBlock(0.0f, 2.75f, -0.66f, 1.10f, 1.50f, 0.52f, orange);            // main bag
        drawBlock(0.0f, 3.50f, -0.66f, 1.14f, 0.22f, 0.56f, orangeDark);        // top flap
        drawBlock(0.0f, 2.30f, -0.95f, 0.80f, 0.55f, 0.12f, orangeDark);        // front pocket
        drawBlock(0.0f, 2.30f, -1.02f, 0.70f, 0.04f, 0.02f, rgb(0.90f, 0.90f, 0.90f)); // zipper
        drawBlock(-0.60f, 2.20f, -0.66f, 0.10f, 0.60f, 0.35f, orangeDark);      // side pockets
        drawBlock(0.60f, 2.20f, -0.66f, 0.10f, 0.60f, 0.35f, orangeDark);

        for (int side = -1; side <= 1; side += 2)
        {
            drawBlock(side * 0.62f, 3.53f, -0.05f, 0.16f, 0.08f, 0.95f, strap); // over shoulder
            drawBlock(side * 0.62f, 2.75f, 0.46f, 0.16f, 1.50f, 0.07f, strap);  // down the chest
        }
    }
}

void drawCharacter()
{
    // Base choice changes the body proportions
    float bodyWidth = (genderChoice == 0) ? 1.70f : 1.50f;

    glPushMatrix();
        drawBodyBase(bodyWidth);
        drawHead();
        drawHair();
        drawTop(bodyWidth);
        drawBottom();
        drawShoes();
        drawAccessory(bodyWidth);
    glPopMatrix();
}
