#include "Character.h"
#include "../World/World.h"

// ============================================================
// PART 3 OF 4: CHARACTER
// Owns the male and female models, body parts, hair, clothing,
// shoes, accessories and poses.
//
// Local coordinates: feet at y = 0, the character faces +Z.
// "side" is -1 for the left half of the screen (x < 0) and +1 for the
// right half when you look at the front of the character.
//
// MODULE 6/7: limbs are hierarchical. Each arm/leg is drawn inside
// glPushMatrix()/glPopMatrix() and moved to its pivot with
// glTranslatef(), then bent with glRotatef() at the shoulder/hip and
// again at the elbow/knee. Clothes are drawn inside the same transforms,
// so they follow every pose.
// ============================================================

namespace
{

// ---------- body landmarks (y positions) ----------
const float HIP_Y = 2.55f;
const float THIGH_LEN = 1.15f;
const float SHIN_LEN = 1.00f;          // ankle ends at y = 0.4
const float SHOULDER_Y = 3.95f;
const float UPPER_ARM_LEN = 0.95f;
const float FOREARM_LEN = 0.85f;
const float NECK_Y = 4.30f;            // pivot used when the head turns
const float HEAD_Y = 5.10f;            // center of the head

float maxF(float a, float b) { return (a > b) ? a : b; }
float minF(float a, float b) { return (a < b) ? a : b; }

// ============================================================
// REQUIREMENT: distinct male and female proportions
// ============================================================
struct Shape
{
    float chestRx, chestRz;
    float waistRx, waistRz;
    float hipRx, hipRz;
    float shoulderX;     // distance from the center to a shoulder pivot
    float armR;
    float legX;          // distance from the center to a hip pivot
    float legR;
    float headR;
};

Shape getShape()
{
    Shape s;
    if (genderChoice == 0)   // male: broad shoulders, straighter waist
    {
        s.chestRx = 0.80f; s.chestRz = 0.46f;
        s.waistRx = 0.68f; s.waistRz = 0.42f;
        s.hipRx = 0.66f;   s.hipRz = 0.42f;
        s.shoulderX = 1.08f; s.armR = 0.26f;
        s.legX = 0.36f;    s.legR = 0.31f;
        s.headR = 0.62f;
    }
    else                     // female: narrower shoulders, defined waist, rounder hips
    {
        s.chestRx = 0.64f; s.chestRz = 0.40f;
        s.waistRx = 0.50f; s.waistRz = 0.35f;
        s.hipRx = 0.68f;   s.hipRz = 0.42f;
        s.shoulderX = 0.88f; s.armR = 0.21f;
        s.legX = 0.34f;    s.legR = 0.27f;
        s.headR = 0.64f;
    }
    return s;
}

// The torso is three oval cylinders stacked on each other
struct Segment
{
    float y0, y1, rx, rz;
};

void getSegments(const Shape& s, Segment out[3])
{
    Segment hips = { 2.35f, 2.95f, s.hipRx, s.hipRz };
    Segment waist = { 2.95f, 3.45f, s.waistRx, s.waistRz };
    Segment chest = { 3.45f, 4.05f, s.chestRx, s.chestRz };
    out[0] = hips;
    out[1] = waist;
    out[2] = chest;
}

// Draws the part of the torso between two heights, slightly "grown"
// outward so clothes sit on top of the skin.
void drawTorsoBand(const Shape& s, float yBottom, float yTop,
                   float grow, const Color& c)
{
    Segment segs[3];
    getSegments(s, segs);

    for (int i = 0; i < 3; i++)
    {
        float lo = maxF(yBottom, segs[i].y0);
        float hi = minF(yTop, segs[i].y1);
        if (hi - lo > 0.001f)
            drawCylinderY(0.0f, lo, 0.0f,
                          segs[i].rx + grow, segs[i].rz + grow, hi - lo, c);
    }
}

// Depth (z) of the chest surface at a given x, used to place details
float chestZ(const Shape& s, float x, float grow)
{
    float k = 1.0f - (x * x) / ((s.chestRx + grow) * (s.chestRx + grow));
    if (k < 0.0f) k = 0.0f;
    return (s.chestRz + grow) * sqrtf(k);
}

// ============================================================
// REQUIREMENT: poses (angles in degrees)
// shoulderPitch: + raises the arm forward   shoulderRoll: + moves it outward
// elbowBend:     + bends the forearm forward  elbowFold: + folds it outward
// hipPitch:      + swings the leg forward     knee: + bends the shin backward
// ============================================================
struct PoseAngles
{
    float shoulderPitch[2];
    float shoulderRoll[2];
    float elbowBend[2];
    float elbowFold[2];
    float hipPitch[2];
    float knee[2];
    float headYaw;
    float headTilt;
    float bodyDrop;
    bool peaceHand[2];
};

PoseAngles getPoseAngles(int pose)
{
    PoseAngles p = {};

    // Relaxed default for every pose
    for (int i = 0; i < 2; i++)
    {
        p.shoulderRoll[i] = 7.0f;
        p.elbowBend[i] = 8.0f;
    }

    if (pose == POSE_WAVE)   // index 1 = right side of the screen
    {
        float t = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
        p.shoulderRoll[1] = 95.0f;
        p.elbowBend[1] = 0.0f;
        p.elbowFold[1] = 80.0f + 22.0f * sinf(t * 7.0f);   // waving hand
        p.headTilt = 6.0f;
    }
    else if (pose == POSE_PEACE)
    {
        p.shoulderPitch[1] = 80.0f;
        p.shoulderRoll[1] = -22.0f;
        p.elbowBend[1] = 100.0f;
        p.peaceHand[1] = true;
        p.headTilt = -5.0f;
        p.headYaw = -8.0f;
    }
    else if (pose == POSE_HIP)   // both hands on the hips
    {
        for (int i = 0; i < 2; i++)
        {
            p.shoulderPitch[i] = -12.0f;
            p.shoulderRoll[i] = 30.0f;
            p.elbowBend[i] = 0.0f;
            p.elbowFold[i] = -90.0f;
        }
        p.hipPitch[1] = 10.0f;
        p.knee[1] = 14.0f;
        p.headYaw = 10.0f;
        p.headTilt = 4.0f;
    }
    else if (pose == POSE_WALK)  // mid-stride with swinging arms
    {
        p.hipPitch[1] = 25.0f;
        p.knee[1] = 10.0f;
        p.hipPitch[0] = -20.0f;
        p.knee[0] = 30.0f;
        p.shoulderPitch[1] = -30.0f;
        p.elbowBend[1] = 15.0f;
        p.shoulderPitch[0] = 30.0f;
        p.elbowBend[0] = 35.0f;
        p.bodyDrop = 0.14f;
    }
    return p;
}

// ============================================================
// REQUIREMENT: separate male and female outfits
// ============================================================
struct TopStyle
{
    Color body, sleeve, trim, accent;
    float upperCover;    // 0..1 of the upper arm covered from the shoulder
    float lowerCover;    // 0..1 of the forearm covered from the elbow
    float sleeveGrow;
    float bodyGrow;
    float bodyBottom;    // lowest y of the jacket/shirt
    bool cuff;
    bool stripes;
};

TopStyle getTopStyle()
{
    TopStyle t = {};
    int style = selection[CAT_TOP];

    if (genderChoice == 0)
    {
        if (style == 0)        // varsity jacket
        {
            t.body = rgb(0.52f, 0.06f, 0.13f); t.sleeve = rgb(0.90f, 0.84f, 0.72f);
            t.trim = rgb(0.34f, 0.04f, 0.09f); t.accent = rgb(0.95f, 0.78f, 0.20f);
            t.upperCover = 1.0f; t.lowerCover = 1.0f; t.sleeveGrow = 0.08f;
            t.bodyGrow = 0.09f; t.bodyBottom = 2.45f; t.cuff = true; t.stripes = true;
        }
        else if (style == 1)   // school uniform
        {
            t.body = rgb(0.93f, 0.94f, 0.97f); t.sleeve = rgb(0.93f, 0.94f, 0.97f);
            t.trim = rgb(0.08f, 0.16f, 0.45f); t.accent = rgb(0.72f, 0.75f, 0.82f);
            t.upperCover = 0.60f; t.lowerCover = 0.0f; t.sleeveGrow = 0.06f;
            t.bodyGrow = 0.05f; t.bodyBottom = 2.50f;
        }
        else                   // campus tee
        {
            t.body = rgb(0.12f, 0.55f, 0.45f); t.sleeve = rgb(0.12f, 0.55f, 0.45f);
            t.trim = rgb(0.08f, 0.36f, 0.30f); t.accent = rgb(0.98f, 0.82f, 0.25f);
            t.upperCover = 0.62f; t.lowerCover = 0.0f; t.sleeveGrow = 0.12f;
            t.bodyGrow = 0.12f; t.bodyBottom = 2.40f;
        }
    }
    else
    {
        if (style == 0)        // cropped varsity jacket
        {
            t.body = rgb(0.55f, 0.07f, 0.20f); t.sleeve = rgb(0.92f, 0.86f, 0.76f);
            t.trim = rgb(0.36f, 0.04f, 0.12f); t.accent = rgb(0.95f, 0.78f, 0.20f);
            t.upperCover = 1.0f; t.lowerCover = 1.0f; t.sleeveGrow = 0.07f;
            t.bodyGrow = 0.08f; t.bodyBottom = 3.00f; t.cuff = true; t.stripes = true;
        }
        else if (style == 1)   // school blouse with ribbon
        {
            t.body = rgb(0.97f, 0.97f, 1.00f); t.sleeve = rgb(0.97f, 0.97f, 1.00f);
            t.trim = rgb(0.08f, 0.16f, 0.45f); t.accent = rgb(0.85f, 0.15f, 0.25f);
            t.upperCover = 0.42f; t.lowerCover = 0.0f; t.sleeveGrow = 0.13f;
            t.bodyGrow = 0.05f; t.bodyBottom = 2.70f;
        }
        else                   // oversized tee
        {
            t.body = rgb(0.68f, 0.60f, 0.90f); t.sleeve = rgb(0.68f, 0.60f, 0.90f);
            t.trim = rgb(0.50f, 0.42f, 0.75f); t.accent = rgb(1.00f, 0.88f, 0.30f);
            t.upperCover = 0.72f; t.lowerCover = 0.0f; t.sleeveGrow = 0.17f;
            t.bodyGrow = 0.17f; t.bodyBottom = 2.30f;
        }
    }
    return t;
}

struct BottomStyle
{
    Color main, trim, belt, sock;
    float thighCover;    // 0..1 of the thigh covered from the hip
    float shinCover;     // 0..1 of the shin covered from the knee
    float thighGrow;
    float shinGrow;
    float sockCover;     // 0..1 of the shin covered by a sock from the ankle
    float hipTop;        // top of the waistband
    float hipGrow;
    bool skirt;
    bool cuff;
    bool cargoPockets;
    bool backPockets;
};

BottomStyle getBottomStyle()
{
    BottomStyle b = {};
    int style = selection[CAT_BOTTOM];
    b.sock = rgb(0.95f, 0.95f, 0.95f);

    if (genderChoice == 0)
    {
        b.hipTop = 3.00f; b.hipGrow = 0.07f; b.backPockets = true;
        if (style == 0)        // cargo pants
        {
            b.main = rgb(0.34f, 0.38f, 0.22f); b.trim = rgb(0.24f, 0.27f, 0.15f);
            b.belt = rgb(0.15f, 0.12f, 0.10f);
            b.thighCover = 1.0f; b.shinCover = 1.0f;
            b.thighGrow = 0.10f; b.shinGrow = 0.08f;
            b.cuff = true; b.cargoPockets = true;
        }
        else if (style == 1)   // jeans
        {
            b.main = rgb(0.13f, 0.28f, 0.55f); b.trim = rgb(0.25f, 0.45f, 0.75f);
            b.belt = rgb(0.30f, 0.18f, 0.09f);
            b.thighCover = 1.0f; b.shinCover = 1.0f;
            b.thighGrow = 0.06f; b.shinGrow = 0.05f;
            b.cuff = true;
        }
        else                   // shorts
        {
            b.main = rgb(0.78f, 0.66f, 0.45f); b.trim = rgb(0.60f, 0.50f, 0.32f);
            b.belt = rgb(0.30f, 0.30f, 0.38f);
            b.thighCover = 0.72f; b.shinCover = 0.0f;
            b.thighGrow = 0.09f; b.sockCover = 0.30f;
        }
    }
    else
    {
        if (style == 0)        // pleated skirt with knee socks
        {
            b.main = rgb(0.20f, 0.22f, 0.45f); b.trim = rgb(0.90f, 0.35f, 0.45f);
            b.belt = rgb(0.12f, 0.13f, 0.30f);
            b.skirt = true; b.hipTop = 3.10f; b.hipGrow = 0.07f;
            b.sockCover = 0.85f;
        }
        else if (style == 1)   // wide-leg pants (flare toward the ankle)
        {
            b.main = rgb(0.52f, 0.60f, 0.52f); b.trim = rgb(0.40f, 0.48f, 0.40f);
            b.belt = rgb(0.40f, 0.48f, 0.40f);
            b.thighCover = 1.0f; b.shinCover = 1.0f;
            b.thighGrow = 0.06f; b.shinGrow = 0.17f;
            b.hipTop = 3.20f; b.hipGrow = 0.06f;
        }
        else                   // denim shorts with ankle socks
        {
            b.main = rgb(0.35f, 0.55f, 0.80f); b.trim = rgb(0.80f, 0.90f, 1.00f);
            b.belt = rgb(0.25f, 0.40f, 0.62f);
            b.thighCover = 0.60f; b.shinCover = 0.0f;
            b.thighGrow = 0.07f; b.sockCover = 0.35f;
            b.hipTop = 2.95f; b.hipGrow = 0.06f;
        }
    }
    return b;
}

// ============================================================
// SHOES (drawn at the ankle; the floor is 0.4 below the ankle)
// ============================================================
void drawShoe(int side)
{
    int style = selection[CAT_SHOES];
    bool female = (genderChoice == 1);
    float shinR = 0.27f * 0.82f;

    float soleH = 0.14f;
    Color sole, upper, accent;

    if (!female)
    {
        if (style == 0)      { soleH = 0.14f; sole = rgb(0.78f, 0.80f, 0.85f); upper = rgb(0.95f, 0.95f, 0.97f); accent = rgb(0.80f, 0.10f, 0.15f); }
        else if (style == 1) { soleH = 0.20f; sole = rgb(0.97f, 0.97f, 0.97f); upper = rgb(0.08f, 0.70f, 0.82f); accent = rgb(1.00f, 0.55f, 0.10f); }
        else                 { soleH = 0.10f; sole = rgb(0.22f, 0.13f, 0.08f); upper = rgb(0.07f, 0.07f, 0.09f); accent = rgb(0.40f, 0.40f, 0.45f); }
    }
    else
    {
        if (style == 0)      { soleH = 0.24f; sole = rgb(0.97f, 0.97f, 0.97f); upper = rgb(0.96f, 0.90f, 0.95f); accent = rgb(0.95f, 0.45f, 0.65f); }
        else if (style == 1) { soleH = 0.10f; sole = rgb(0.12f, 0.10f, 0.10f); upper = rgb(0.08f, 0.08f, 0.10f); accent = rgb(0.90f, 0.75f, 0.30f); }
        else                 { soleH = 0.14f; sole = rgb(0.20f, 0.12f, 0.07f); upper = rgb(0.42f, 0.25f, 0.14f); accent = rgb(0.85f, 0.70f, 0.30f); }
    }

    float base = -0.40f;                       // floor level in this frame
    float uy = base + soleH + 0.15f;           // center height of the upper

    drawBlock(0.0f, base + soleH * 0.5f, 0.22f, 0.66f, soleH, 1.30f, sole);   // sole
    drawBlock(0.0f, uy, 0.12f, 0.58f, 0.30f, 0.80f, upper);                   // body
    drawSphere(0.0f, uy - 0.02f, 0.52f, 0.29f, 0.17f, 0.30f, upper);          // rounded toe
    drawSphere(0.0f, uy, -0.20f, 0.29f, 0.16f, 0.20f, upper);                 // heel

    if (!female && style == 0)           // sneakers: laces + heel tab
    {
        for (int i = 0; i < 3; i++)
            drawBlock(0.0f, uy + 0.16f, 0.02f + i * 0.17f, 0.36f, 0.03f, 0.07f, rgb(0.55f, 0.62f, 0.80f));
        drawBlock(0.0f, uy + 0.04f, -0.34f, 0.30f, 0.20f, 0.05f, accent);
    }
    else if (!female && style == 1)      // runners: side stripe + tongue
    {
        drawBlock(side * 0.30f, uy, 0.12f, 0.03f, 0.12f, 0.55f, accent);
        drawBlock(0.0f, uy + 0.16f, 0.05f, 0.30f, 0.05f, 0.30f, rgb(0.98f, 0.85f, 0.25f));
    }
    else if (!female && style == 2)      // dark shoes: shine and laces
    {
        drawBlock(0.0f, uy + 0.16f, 0.55f, 0.40f, 0.03f, 0.20f, accent);
        drawBlock(0.0f, uy + 0.16f, 0.05f, 0.36f, 0.03f, 0.05f, accent);
        drawBlock(0.0f, uy + 0.16f, 0.20f, 0.36f, 0.03f, 0.05f, accent);
    }
    else if (female && style == 0)       // chunky sneakers: pink accents
    {
        drawBlock(0.0f, uy + 0.16f, 0.15f, 0.36f, 0.04f, 0.40f, accent);
        drawBlock(side * 0.30f, uy, 0.0f, 0.03f, 0.14f, 0.50f, accent);
    }
    else if (female && style == 1)       // mary janes: strap and gold buckle
    {
        drawBlock(0.0f, uy + 0.08f, 0.14f, 0.62f, 0.07f, 0.10f, upper);
        drawSphere(0.0f, uy + 0.10f, 0.14f, 0.07f, 0.07f, 0.07f, accent);
    }
    else                                 // ankle boots: shaft, strap and buckle
    {
        drawCylinderY(0.0f, base + soleH, 0.0f, shinR + 0.10f, shinR + 0.10f, 0.55f, upper);
        drawCylinderY(0.0f, 0.05f, 0.0f, shinR + 0.12f, shinR + 0.12f, 0.08f, accent);
    }
}

// ============================================================
// HAND (peace sign adds two spread fingers)
// ============================================================
void drawHand(bool peace)
{
    glPushMatrix();
        glTranslatef(0.0f, -FOREARM_LEN - 0.04f, 0.0f);
        drawSphere(0.0f, 0.0f, 0.0f, 0.21f, 0.23f, 0.21f, SKIN);
        drawSphere(0.0f, -0.12f, 0.10f, 0.12f, 0.10f, 0.08f, SKIN);   // curled fingers

        if (peace)
        {
            for (int f = -1; f <= 1; f += 2)
            {
                glPushMatrix();
                    glRotatef(f * 14.0f, 0.0f, 0.0f, 1.0f);
                    drawCylinderY(0.0f, -0.66f, 0.0f, 0.055f, 0.055f, 0.55f, SKIN);
                    drawSphere(0.0f, -0.66f, 0.0f, 0.06f, 0.06f, 0.06f, SKIN);
                glPopMatrix();
            }
        }
    glPopMatrix();
}

// ============================================================
// ARM: shoulder joint -> upper arm -> elbow joint -> forearm -> hand
// ============================================================
void drawArm(int side, const PoseAngles& p, const TopStyle& ts, const Shape& s)
{
    int i = (side < 0) ? 0 : 1;
    float r = s.armR;
    float foreR = r * 0.88f;

    glPushMatrix();
        glTranslatef(side * s.shoulderX, SHOULDER_Y, 0.0f);          // shoulder pivot
        glRotatef(-p.shoulderPitch[i], 1.0f, 0.0f, 0.0f);
        glRotatef(side * p.shoulderRoll[i], 0.0f, 0.0f, 1.0f);

        // Shoulder joint and upper arm
        drawSphere(0.0f, 0.0f, 0.0f, r * 1.2f, r * 1.2f, r * 1.2f, SKIN);
        drawCylinderY(0.0f, -UPPER_ARM_LEN, 0.0f, r, r, UPPER_ARM_LEN, SKIN);

        if (ts.upperCover > 0.0f)
        {
            float len = UPPER_ARM_LEN * ts.upperCover;
            float sr = r + ts.sleeveGrow;
            drawSphere(0.0f, 0.0f, 0.0f, sr * 1.15f, sr * 1.15f, sr * 1.15f, ts.sleeve);
            drawCylinderY(0.0f, -len, 0.0f, sr, sr, len, ts.sleeve);
            if (ts.stripes)
            {
                drawCylinderY(0.0f, -0.40f, 0.0f, sr + 0.02f, sr + 0.02f, 0.07f, ts.body);
                drawCylinderY(0.0f, -0.55f, 0.0f, sr + 0.02f, sr + 0.02f, 0.07f, ts.body);
            }
            else if (ts.upperCover < 0.99f)   // short sleeve hem
            {
                drawCylinderY(0.0f, -len, 0.0f, sr + 0.015f, sr + 0.015f, 0.08f, ts.trim);
            }
        }

        // Elbow
        glTranslatef(0.0f, -UPPER_ARM_LEN, 0.0f);
        glRotatef(-p.elbowBend[i], 1.0f, 0.0f, 0.0f);
        glRotatef(side * p.elbowFold[i], 0.0f, 0.0f, 1.0f);

        drawSphere(0.0f, 0.0f, 0.0f, r * 0.98f, r * 0.98f, r * 0.98f, SKIN);
        drawCylinderY(0.0f, -FOREARM_LEN, 0.0f, foreR, foreR, FOREARM_LEN, SKIN);

        if (ts.lowerCover > 0.0f)
        {
            float len = FOREARM_LEN * ts.lowerCover;
            float sr = foreR + ts.sleeveGrow;
            drawSphere(0.0f, 0.0f, 0.0f, sr * 1.05f, sr * 1.05f, sr * 1.05f, ts.sleeve);
            drawCylinderY(0.0f, -len, 0.0f, sr, sr, len, ts.sleeve);
            if (ts.cuff)
                drawCylinderY(0.0f, -FOREARM_LEN, 0.0f, sr + 0.02f, sr + 0.02f, 0.16f, ts.body);
        }

        drawHand(p.peaceHand[i]);
    glPopMatrix();
}

// ============================================================
// LEG: hip joint -> thigh -> knee joint -> shin -> shoe
// ============================================================
void drawLeg(int side, const PoseAngles& p, const BottomStyle& bs, const Shape& s)
{
    int i = (side < 0) ? 0 : 1;
    float thighR = s.legR;
    float shinR = s.legR * 0.82f;

    glPushMatrix();
        glTranslatef(side * s.legX, HIP_Y, 0.0f);                    // hip pivot
        glRotatef(-p.hipPitch[i], 1.0f, 0.0f, 0.0f);

        drawSphere(0.0f, 0.0f, 0.0f, thighR * 1.05f, thighR * 1.05f, thighR * 1.05f, SKIN);
        drawCylinderY(0.0f, -THIGH_LEN, 0.0f, thighR, thighR, THIGH_LEN, SKIN);

        if (bs.thighCover > 0.0f)
        {
            float len = THIGH_LEN * bs.thighCover;
            float lr = thighR + bs.thighGrow;
            drawCylinderY(0.0f, -len, 0.0f, lr, lr, len, bs.main);
            if (bs.thighCover < 0.99f)       // shorts hem
                drawCylinderY(0.0f, -len, 0.0f, lr + 0.02f, lr + 0.02f, 0.10f, bs.trim);
            if (bs.cargoPockets)
            {
                drawBlock(side * (lr + 0.04f), -0.55f, 0.0f, 0.14f, 0.45f, 0.45f, bs.trim);
                drawBlock(side * (lr + 0.04f), -0.30f, 0.0f, 0.16f, 0.08f, 0.47f, bs.main);
            }
        }

        // Knee
        glTranslatef(0.0f, -THIGH_LEN, 0.0f);
        glRotatef(p.knee[i], 1.0f, 0.0f, 0.0f);

        drawSphere(0.0f, 0.0f, 0.0f, shinR * 1.1f, shinR * 1.1f, shinR * 1.1f, SKIN);
        drawCylinderY(0.0f, -SHIN_LEN, 0.0f, shinR, shinR, SHIN_LEN, SKIN);

        if (bs.sockCover > 0.0f)
        {
            float len = SHIN_LEN * bs.sockCover;
            drawCylinderY(0.0f, -SHIN_LEN, 0.0f, shinR + 0.03f, shinR + 0.03f, len, bs.sock);
            drawCylinderY(0.0f, -SHIN_LEN + len - 0.10f, 0.0f,
                          shinR + 0.04f, shinR + 0.04f, 0.07f, bs.trim);
        }

        if (bs.shinCover > 0.0f)
        {
            float len = SHIN_LEN * bs.shinCover;
            float lr = shinR + bs.shinGrow;
            drawSphere(0.0f, 0.0f, 0.0f, thighR + bs.thighGrow, thighR * 0.9f, thighR + bs.thighGrow, bs.main);
            drawCylinderY(0.0f, -len, 0.0f, lr, lr, len, bs.main);
            if (bs.cuff)
                drawCylinderY(0.0f, -SHIN_LEN, 0.0f, lr + 0.02f, lr + 0.02f, 0.20f, bs.trim);
        }

        // Ankle and shoe
        glTranslatef(0.0f, -SHIN_LEN, 0.0f);
        drawShoe(side);
    glPopMatrix();
}

// ============================================================
// TORSO: body, bottoms (waist/hips/skirt) and tops
// ============================================================
void drawSkirt(const Shape& s, const BottomStyle& bs)
{
    drawTorsoBand(s, 2.35f, bs.hipTop, bs.hipGrow, bs.main);          // waistband + hips
    drawTorsoBand(s, 2.90f, 3.02f, bs.hipGrow + 0.02f, bs.belt);

    // A cone makes the skirt flare out from the waist
    drawConeY(0.0f, 1.65f, 0.0f, 1.12f, 1.75f, bs.main, 14);
    drawCylinderY(0.0f, 1.62f, 0.0f, 1.14f, 1.14f, 0.08f, bs.trim);   // hem

    // Pleat lines: thin darker blocks tilted to match the cone slope
    Color pleat = shade(bs.main, 0.7f);
    for (int i = 0; i < 7; i++)
    {
        glPushMatrix();
            glRotatef(i * (360.0f / 7.0f), 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 2.45f, 0.62f);
            glRotatef(-32.6f, 1.0f, 0.0f, 0.0f);
            drawBlock(0.0f, 0.0f, 0.0f, 0.04f, 1.50f, 0.03f, pleat);
        glPopMatrix();
    }
}

void drawBottomTorso(const Shape& s, const BottomStyle& bs)
{
    if (bs.skirt)
    {
        drawSkirt(s, bs);
        return;
    }

    drawTorsoBand(s, 2.35f, bs.hipTop, bs.hipGrow, bs.main);
    drawTorsoBand(s, bs.hipTop - 0.14f, bs.hipTop, bs.hipGrow + 0.02f, bs.belt);   // belt

    if (genderChoice == 0)   // belt buckle
        drawBlock(0.0f, bs.hipTop - 0.07f, s.waistRz + bs.hipGrow + 0.03f,
                  0.20f, 0.12f, 0.04f, rgb(0.85f, 0.75f, 0.30f));

    if (bs.backPockets)
    {
        for (int side = -1; side <= 1; side += 2)
            drawBlock(side * 0.28f, 2.60f, -(s.hipRz + bs.hipGrow + 0.01f),
                      0.34f, 0.34f, 0.05f, bs.trim);
    }
}

void drawTopTorso(const Shape& s, const TopStyle& ts)
{
    Color white = rgb(0.97f, 0.97f, 1.00f);
    float fz = s.chestRz + ts.bodyGrow;      // front surface at the center

    // Main layer plus a rounded shoulder cap
    drawTorsoBand(s, ts.bodyBottom, 4.05f, ts.bodyGrow, ts.body);
    drawSphere(0.0f, 4.05f, 0.0f, s.chestRx + ts.bodyGrow, 0.20f, s.chestRz + ts.bodyGrow, ts.body);
    drawTorsoBand(s, ts.bodyBottom, ts.bodyBottom + 0.16f, ts.bodyGrow + 0.02f, ts.trim);   // hem

    if (genderChoice == 0)
    {
        int style = selection[CAT_TOP];

        if (style == 0)   // ----- male varsity jacket -----
        {
            drawCylinderY(0.0f, 4.02f, 0.0f, 0.40f, 0.40f, 0.26f, ts.trim);               // collar
            drawBlock(0.0f, 3.25f, fz - 0.02f, 0.30f, 1.55f, 0.08f, ts.sleeve);            // opening
            for (int i = 0; i < 4; i++)                                                    // snaps
                drawSphere(0.0f, 3.85f - i * 0.38f, fz + 0.03f, 0.05f, 0.05f, 0.04f, ts.accent);
            float ez = chestZ(s, -0.42f, ts.bodyGrow);
            drawSphere(-0.42f, 3.55f, ez, 0.17f, 0.17f, 0.04f, ts.accent);                 // chest emblem
            drawSphere(-0.42f, 3.55f, ez + 0.03f, 0.09f, 0.09f, 0.03f, ts.body);
            drawSphere(0.0f, 3.40f, -(fz + 0.01f), 0.40f, 0.40f, 0.04f, ts.accent);        // back emblem
            drawSphere(0.0f, 3.40f, -(fz + 0.04f), 0.26f, 0.26f, 0.03f, ts.body);
            drawSphere(0.0f, 3.40f, -(fz + 0.07f), 0.12f, 0.12f, 0.03f, ts.accent);
        }
        else if (style == 1)   // ----- male school uniform -----
        {
            drawCylinderY(0.0f, 4.02f, 0.0f, 0.38f, 0.38f, 0.18f, white);                  // collar ring
            for (int side = -1; side <= 1; side += 2)                                      // collar flaps
                drawRotatedBlock(side * 0.20f, 4.08f, fz - 0.02f, 0.30f, 0.20f, 0.07f, side * -35.0f, white);
            drawSphere(0.0f, 3.95f, fz + 0.02f, 0.11f, 0.10f, 0.06f, ts.trim);             // tie knot
            drawBlock(0.0f, 3.45f, fz + 0.01f, 0.20f, 0.85f, 0.05f, ts.trim);              // tie
            drawBlock(0.0f, 2.95f, fz, 0.26f, 0.22f, 0.05f, ts.trim);
            drawBlock(0.0f, 3.45f, fz + 0.04f, 0.20f, 0.05f, 0.02f, ts.accent);            // tie stripe
            drawSphere(0.0f, 3.05f, fz + 0.01f, 0.04f, 0.04f, 0.03f, ts.accent);           // buttons
            drawSphere(0.0f, 2.80f, fz + 0.01f, 0.04f, 0.04f, 0.03f, ts.accent);
            float pz = chestZ(s, -0.40f, ts.bodyGrow);
            drawBlock(-0.40f, 3.45f, pz + 0.01f, 0.30f, 0.32f, 0.04f, rgb(0.80f, 0.85f, 0.95f)); // pocket
        }
        else   // ----- male campus tee -----
        {
            drawCylinderY(0.0f, 4.02f, 0.0f, 0.34f, 0.34f, 0.14f, ts.trim);                // neckline
            float yz = chestZ(s, 0.0f, ts.bodyGrow);
            drawBlock(0.0f, 3.65f, yz + 0.01f, 1.10f, 0.12f, 0.05f, ts.accent);            // stripes
            drawBlock(0.0f, 3.42f, yz + 0.01f, 1.10f, 0.12f, 0.05f, white);
            drawBlock(0.0f, 3.19f, yz + 0.01f, 1.10f, 0.12f, 0.05f, ts.accent);
            drawSphere(0.0f, 3.40f, -(yz + 0.01f), 0.30f, 0.30f, 0.04f, ts.accent);        // back print
            drawSphere(0.0f, 3.40f, -(yz + 0.04f), 0.18f, 0.18f, 0.03f, ts.trim);
        }
    }
    else
    {
        int style = selection[CAT_TOP];

        if (style == 0)   // ----- female cropped varsity -----
        {
            drawTorsoBand(s, 2.85f, 3.05f, 0.045f, white);                                 // shirt under the crop
            drawCylinderY(0.0f, 4.02f, 0.0f, 0.36f, 0.36f, 0.24f, ts.trim);                // collar
            drawBlock(0.0f, 3.55f, fz - 0.02f, 0.26f, 1.05f, 0.08f, ts.sleeve);            // opening
            for (int i = 0; i < 3; i++)
                drawSphere(0.0f, 3.85f - i * 0.30f, fz + 0.03f, 0.045f, 0.045f, 0.04f, ts.accent);
            float ez = chestZ(s, -0.32f, ts.bodyGrow);
            drawSphere(-0.32f, 3.70f, ez, 0.14f, 0.14f, 0.04f, ts.accent);                 // chest emblem
            drawSphere(0.0f, 3.55f, -(fz + 0.01f), 0.34f, 0.34f, 0.04f, ts.accent);        // back emblem
            drawSphere(0.0f, 3.55f, -(fz + 0.04f), 0.20f, 0.20f, 0.03f, ts.body);
        }
        else if (style == 1)   // ----- female school blouse -----
        {
            drawSphere(0.0f, 3.98f, 0.0f, s.chestRx + 0.14f, 0.11f, s.chestRz + 0.14f, ts.trim); // sailor collar
            drawSphere(-0.20f, 3.86f, fz + 0.01f, 0.17f, 0.12f, 0.07f, ts.accent);               // ribbon loops
            drawSphere(0.20f, 3.86f, fz + 0.01f, 0.17f, 0.12f, 0.07f, ts.accent);
            drawSphere(0.0f, 3.86f, fz + 0.04f, 0.08f, 0.08f, 0.06f, shade(ts.accent, 0.8f));    // knot
            drawRotatedBlock(-0.07f, 3.58f, fz + 0.02f, 0.09f, 0.45f, 0.04f, 10.0f, ts.accent);  // ribbon tails
            drawRotatedBlock(0.07f, 3.58f, fz + 0.02f, 0.09f, 0.45f, 0.04f, -10.0f, ts.accent);
            drawSphere(0.0f, 3.30f, fz + 0.01f, 0.04f, 0.04f, 0.03f, ts.trim);                   // buttons
            drawSphere(0.0f, 3.05f, fz + 0.01f, 0.04f, 0.04f, 0.03f, ts.trim);
        }
        else   // ----- female oversized tee -----
        {
            drawCylinderY(0.0f, 4.02f, 0.0f, 0.34f, 0.34f, 0.14f, ts.trim);                // neckline
            float yz = chestZ(s, 0.0f, ts.bodyGrow);
            drawSphere(0.0f, 3.45f, yz + 0.01f, 0.30f, 0.30f, 0.04f, ts.accent);           // graphic
            drawSphere(0.0f, 3.45f, yz + 0.04f, 0.16f, 0.16f, 0.03f, ts.trim);
            drawSphere(0.0f, 3.50f, -(yz + 0.01f), 0.26f, 0.26f, 0.04f, ts.accent);        // back print
        }
    }
}

void drawBackpack(const Shape& s)
{
    Color orange = rgb(0.92f, 0.46f, 0.10f);
    Color orangeDark = rgb(0.62f, 0.28f, 0.06f);
    Color strap = rgb(0.20f, 0.20f, 0.24f);

    drawBlock(0.0f, 3.15f, -0.95f, 1.15f, 1.50f, 0.62f, orange);                 // main bag
    drawSphere(0.0f, 3.90f, -0.95f, 0.58f, 0.22f, 0.32f, orangeDark);             // rounded top
    drawBlock(0.0f, 2.70f, -1.33f, 0.85f, 0.60f, 0.16f, orangeDark);              // front pocket
    drawBlock(0.0f, 2.97f, -1.42f, 0.70f, 0.04f, 0.02f, rgb(0.9f, 0.9f, 0.9f));   // zipper
    drawCylinderY(-0.66f, 2.45f, -0.95f, 0.14f, 0.14f, 0.7f, orangeDark);         // side bottle pockets
    drawCylinderY(0.66f, 2.45f, -0.95f, 0.14f, 0.14f, 0.7f, orangeDark);

    float sx = s.chestRx * 0.55f;
    for (int side = -1; side <= 1; side += 2)
    {
        drawBlock(side * sx, 4.20f, -0.20f, 0.18f, 0.07f, 1.45f, strap);          // over the shoulder
        drawBlock(side * sx, 3.45f, 0.64f, 0.18f, 1.40f, 0.06f, strap);           // down the chest
    }
}

// ============================================================
// HEAD: head, face, hair and head accessories
// Everything inside is drawn around the head center (0,0,0).
// ============================================================
void drawFace(const Shape& s)
{
    bool female = (genderChoice == 1);
    float hr = s.headR;
    Color dark = rgb(0.05f, 0.04f, 0.04f);
    Color browColor = rgb(0.10f, 0.06f, 0.04f);
    Color lips = rgb(0.62f, 0.22f, 0.22f);

    // Head and ears
    drawSphere(0.0f, 0.0f, 0.0f, hr, hr * 1.08f, hr * 0.95f, SKIN);
    drawSphere(-(hr + 0.02f), 0.0f, 0.0f, 0.07f, 0.13f, 0.10f, SKIN_SHADE);
    drawSphere((hr + 0.02f), 0.0f, 0.0f, 0.07f, 0.13f, 0.10f, SKIN_SHADE);

    // Face details are all on the FRONT (+Z) of the head
    for (int side = -1; side <= 1; side += 2)
    {
        float ex = side * 0.24f;
        drawSphere(ex, 0.07f, 0.50f, 0.12f, 0.14f, 0.08f, rgb(0.97f, 0.97f, 0.97f));   // eye white
        drawSphere(ex, 0.07f, 0.55f, 0.065f, 0.09f, 0.05f, dark);                      // pupil
        drawSphere(ex + side * 0.025f, 0.11f, 0.59f, 0.025f, 0.025f, 0.02f,
                   rgb(1.0f, 1.0f, 1.0f));                                              // sparkle

        if (female)
        {
            drawRotatedBlock(ex, 0.27f, 0.47f, 0.24f, 0.04f, 0.06f, side * -8.0f, browColor);  // thin brow
            drawRotatedBlock(ex, 0.19f, 0.50f, 0.28f, 0.04f, 0.05f, side * -6.0f, dark);       // lashes
        }
        else
        {
            drawRotatedBlock(ex, 0.27f, 0.47f, 0.28f, 0.08f, 0.06f, side * 6.0f, browColor);   // thick brow
        }
    }

    drawSphere(0.0f, -0.06f, 0.60f, 0.07f, 0.09f, 0.08f, SKIN_SHADE);                  // nose
    drawSphere(0.0f, -0.30f, 0.52f, 0.12f, 0.025f, 0.04f, lips);                       // mouth
    drawSphere(-0.14f, -0.27f, 0.51f, 0.035f, 0.035f, 0.04f, lips);                    // smile corners
    drawSphere(0.14f, -0.27f, 0.51f, 0.035f, 0.035f, 0.04f, lips);

    if (female)
    {
        Color blush = rgb(0.88f, 0.45f, 0.42f);
        drawSphere(-0.38f, -0.17f, 0.42f, 0.10f, 0.06f, 0.04f, blush);
        drawSphere(0.38f, -0.17f, 0.42f, 0.10f, 0.06f, 0.04f, blush);
    }
}

// Rotated oval used for swept hair pieces
void drawTiltedHair(float x, float y, float z, float rx, float ry, float rz,
                    float angleZ, const Color& c)
{
    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(angleZ, 0.0f, 0.0f, 1.0f);
        drawSphere(0.0f, 0.0f, 0.0f, rx, ry, rz, c);
    glPopMatrix();
}

// One tilted cone used for spiky hair
void drawSpike(float yaw, float ringRadius, float tilt, float baseY, float radius, float height,
               const Color& c)
{
    glPushMatrix();
        glTranslatef(0.0f, baseY, -0.05f);
        glRotatef(yaw, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, ringRadius);
        glRotatef(tilt, 1.0f, 0.0f, 0.0f);
        drawConeY(0.0f, 0.0f, 0.0f, radius, height, c, 6);
    glPopMatrix();
}

void drawHair(const Shape& s)
{
    int style = selection[CAT_HAIR];
    bool capOn = (selection[CAT_ACCESSORY] == ACC_CAP);
    bool female = (genderChoice == 1);
    float hr = s.headR;

    Color hair;
    if (!female)
    {
        if (style == 0)      hair = rgb(0.20f, 0.10f, 0.05f);
        else if (style == 1) hair = rgb(0.08f, 0.06f, 0.05f);
        else                 hair = rgb(0.32f, 0.17f, 0.06f);
    }
    else
    {
        if (style == 0)      hair = rgb(0.12f, 0.07f, 0.06f);
        else if (style == 1) hair = rgb(0.42f, 0.20f, 0.08f);
        else                 hair = rgb(0.20f, 0.10f, 0.07f);
    }
    Color light = shade(hair, 1.6f);
    Color tie = rgb(0.95f, 0.40f, 0.55f);

    // Hair cap: sits back and up from the face, which leaves a natural hairline
    drawSphere(0.0f, 0.13f, -0.10f, hr + 0.05f, hr, hr + 0.05f, hair);

    if (!female)
    {
        drawSphere(-(hr + 0.01f), -0.02f, 0.12f, 0.06f, 0.20f, 0.14f, hair);        // sideburns
        drawSphere((hr + 0.01f), -0.02f, 0.12f, 0.06f, 0.20f, 0.14f, hair);

        if (capOn)
            return;

        if (style == 0)       // CLASSIC: rounded top and short fringe
        {
            drawSphere(0.0f, 0.58f, -0.05f, 0.50f, 0.22f, 0.50f, hair);
            drawSphere(0.0f, 0.42f, 0.42f, 0.46f, 0.12f, 0.16f, hair);
            drawSphere(-0.18f, 0.74f, 0.0f, 0.28f, 0.06f, 0.30f, light);
        }
        else if (style == 1)  // SIDE PART: swept fringe over one side
        {
            drawSphere(-0.08f, 0.56f, -0.02f, 0.55f, 0.20f, 0.52f, hair);
            drawTiltedHair(-0.16f, 0.42f, 0.40f, 0.42f, 0.13f, 0.17f, -16.0f, hair);
            drawTiltedHair(0.30f, 0.44f, 0.38f, 0.20f, 0.07f, 0.14f, 10.0f, hair);
            drawBlock(0.20f, 0.77f, 0.05f, 0.03f, 0.03f, 0.70f, light);                 // part line
        }
        else                  // SPIKY: a ring of cones around a tall center spike
        {
            for (int i = 0; i < 6; i++)
                drawSpike(20.0f + i * 60.0f, 0.34f, 30.0f, 0.58f, 0.20f, 0.62f, hair);
            drawSpike(0.0f, 0.0f, 0.0f, 0.66f, 0.22f, 0.80f, light);
            drawSpike(0.0f, 0.40f, 55.0f, 0.46f, 0.18f, 0.50f, hair);              // front spike
        }
    }
    else
    {
        if (style == 0)       // LONG HAIR: curtain behind and locks beside the face
        {
            drawSphere(0.0f, -0.60f, -0.38f, 0.62f, 1.15f, 0.38f, hair);
            drawSphere(-(hr + 0.02f), -0.30f, 0.05f, 0.12f, 0.58f, 0.24f, hair);
            drawSphere((hr + 0.02f), -0.30f, 0.05f, 0.12f, 0.58f, 0.24f, hair);
        }
        else if (style == 1)  // PONYTAIL: tie and tail behind the head
        {
            drawSphere(0.0f, 0.46f, -0.62f, 0.11f, 0.11f, 0.11f, tie);
            drawSphere(0.0f, 0.26f, -0.80f, 0.22f, 0.26f, 0.22f, hair);
            drawSphere(0.0f, -0.10f, -0.86f, 0.24f, 0.30f, 0.22f, hair);
            drawSphere(0.0f, -0.48f, -0.82f, 0.20f, 0.30f, 0.18f, hair);
            drawSphere(0.0f, -0.80f, -0.74f, 0.14f, 0.24f, 0.14f, hair);
        }
        else                  // TWIN BUNS: short back hair and two buns
        {
            drawSphere(0.0f, -0.25f, -0.45f, 0.55f, 0.45f, 0.25f, hair);
            if (!capOn)
            {
                for (int side = -1; side <= 1; side += 2)
                {
                    drawSphere(side * 0.50f, 0.62f, -0.10f, 0.30f, 0.30f, 0.30f, hair);
                    drawSphere(side * 0.42f, 0.48f, -0.06f, 0.09f, 0.09f, 0.09f, tie);
                }
            }
        }

        if (!capOn)   // bangs for the female hairstyles
        {
            drawSphere(0.0f, 0.40f, 0.42f, 0.50f, 0.14f, 0.18f, hair);
            drawTiltedHair(-0.36f, 0.30f, 0.44f, 0.16f, 0.10f, 0.12f, 20.0f, hair);
            drawTiltedHair(0.36f, 0.30f, 0.44f, 0.16f, 0.10f, 0.12f, -20.0f, hair);
        }
    }
}

void drawHeadAccessory(const Shape& s)
{
    int acc = selection[CAT_ACCESSORY];
    float hr = s.headR;

    if (acc == ACC_GLASSES)
    {
        Color frame = rgb(0.08f, 0.08f, 0.10f);

        for (int side = -1; side <= 1; side += 2)
        {
            glPushMatrix();
                glTranslatef(side * 0.24f, 0.07f, 0.57f);
                glColor3f(frame.r, frame.g, frame.b);
                glutSolidTorus(0.025, 0.17, 6, 14);          // round lens frame
            glPopMatrix();
            drawBlock(side * 0.50f, 0.10f, 0.33f, 0.04f, 0.04f, 0.56f, frame);   // temple arm
        }
        drawBlock(0.0f, 0.10f, 0.57f, 0.14f, 0.04f, 0.04f, frame);               // bridge
    }
    else if (acc == ACC_CAP)
    {
        Color navy = rgb(0.12f, 0.18f, 0.45f);
        Color navyDark = rgb(0.08f, 0.12f, 0.32f);

        drawSphere(0.0f, 0.55f, -0.03f, hr + 0.04f, 0.38f, hr + 0.06f, navy);              // dome
        drawCylinderY(0.0f, 0.34f, -0.02f, hr + 0.07f, hr + 0.08f, 0.22f, navy);           // band
        drawBlock(0.0f, 0.36f, 0.78f, 0.92f, 0.06f, 0.55f, navyDark);                      // brim
        drawSphere(0.0f, 0.88f, -0.03f, 0.07f, 0.06f, 0.07f, navyDark);                    // top button
        drawSphere(0.0f, 0.52f, hr + 0.07f, 0.10f, 0.08f, 0.03f, rgb(0.98f, 0.82f, 0.25f)); // logo
    }
}

void drawHeadGroup(const Shape& s, const PoseAngles& p)
{
    glPushMatrix();
        glTranslatef(0.0f, NECK_Y, 0.0f);               // turn the head around the neck
        glRotatef(p.headYaw, 0.0f, 1.0f, 0.0f);
        glRotatef(p.headTilt, 0.0f, 0.0f, 1.0f);
        glTranslatef(0.0f, HEAD_Y - NECK_Y, 0.0f);

        drawFace(s);
        drawHair(s);
        drawHeadAccessory(s);
    glPopMatrix();
}

}   // end of private helpers

// ============================================================
// PUBLIC: draw the whole character
// ============================================================
void drawCharacter()
{
    Shape s = getShape();
    TopStyle ts = getTopStyle();
    BottomStyle bs = getBottomStyle();
    PoseAngles p = getPoseAngles(currentPose);

    // Soft blob shadow on the platform
    drawCylinderY(0.0f, 0.03f, 0.0f, 1.7f, 1.2f, 0.02f, rgb(0.08f, 0.08f, 0.14f));

    glPushMatrix();
        glTranslatef(0.0f, -p.bodyDrop, 0.0f);

        // Skin torso, neck and shoulders
        drawTorsoBand(s, 2.35f, 4.05f, 0.0f, SKIN);
        drawSphere(0.0f, 4.05f, 0.0f, s.chestRx, 0.20f, s.chestRz, SKIN);
        drawCylinderY(0.0f, 3.95f, 0.0f, 0.20f, 0.20f, 0.50f, SKIN_SHADE);

        drawBottomTorso(s, bs);
        drawTopTorso(s, ts);

        if (selection[CAT_ACCESSORY] == ACC_BACKPACK)
            drawBackpack(s);

        drawArm(-1, p, ts, s);
        drawArm(1, p, ts, s);
        drawLeg(-1, p, bs, s);
        drawLeg(1, p, bs, s);
        drawHeadGroup(s, p);
    glPopMatrix();
}

const char* poseName(int pose)
{
    static const char* names[POSE_COUNT] =
    {
        "NEUTRAL", "WAVING", "PEACE SIGN", "HAND ON HIP", "WALKING"
    };
    if (pose < 0 || pose >= POSE_COUNT)
        return "";
    return names[pose];
}
