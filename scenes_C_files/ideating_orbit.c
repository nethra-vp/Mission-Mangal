#include <GL/freeglut.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef fminf
#define fminf(a, b) ((a) < (b) ? (a) : (b))
#endif

/* ------------------------------------------------------------------------- */
/* OpenGL geometry (no GLUT drawing helpers)                                 */
/* ------------------------------------------------------------------------- */

/* Axis-aligned cube, center (0,0,0), side length = size (same convention as oglSolidCube). */
static void oglSolidCube(float size) {
    float h = size * 0.5f;
    glBegin(GL_QUADS);
    /* +Z */
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-h, -h, h);
    glVertex3f(h, -h, h);
    glVertex3f(h, h, h);
    glVertex3f(-h, h, h);
    /* -Z */
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-h, h, -h);
    glVertex3f(h, h, -h);
    glVertex3f(h, -h, -h);
    glVertex3f(-h, -h, -h);
    /* +X */
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(h, -h, h);
    glVertex3f(h, -h, -h);
    glVertex3f(h, h, -h);
    glVertex3f(h, h, h);
    /* -X */
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-h, h, h);
    glVertex3f(-h, h, -h);
    glVertex3f(-h, -h, -h);
    glVertex3f(-h, -h, h);
    /* +Y */
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-h, h, h);
    glVertex3f(h, h, h);
    glVertex3f(h, h, -h);
    glVertex3f(-h, h, -h);
    /* -Y */
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-h, -h, -h);
    glVertex3f(h, -h, -h);
    glVertex3f(h, -h, h);
    glVertex3f(-h, -h, h);
    glEnd();
}

static GLUquadric *oglQuadric(void) {
    static GLUquadric *q = NULL;
    if (!q) {
        q = gluNewQuadric();
        gluQuadricNormals(q, GLU_SMOOTH);
    }
    return q;
}

static void oglSolidSphere(float radius, int slices, int stacks) {
    GLUquadric *q = oglQuadric();
    gluQuadricDrawStyle(q, GLU_FILL);
    gluSphere(q, (GLdouble)radius, slices, stacks);
}

static void oglWireSphere(float radius, int slices, int stacks) {
    GLUquadric *q = oglQuadric();
    gluQuadricDrawStyle(q, GLU_LINE);
    gluSphere(q, (GLdouble)radius, slices, stacks);
    gluQuadricDrawStyle(q, GLU_FILL);
}

/* ------------------------------------------------------------------------- */
/* Text: OpenGL display lists (WGL on Windows); GLUT bitmaps elsewhere       */
/* ------------------------------------------------------------------------- */

#if OGL_USE_WGL_FONTS
static GLuint g_oglFontBase = 0;
static GLuint g_oglFontSmallBase = 0;

static void oglInitBitmapFonts(void) {
    static int done = 0;
    HDC hdc;
    HFONT hOld, h18, h12;

    if (done)
        return;
    hdc = wglGetCurrentDC();
    if (!hdc)
        return;

    hOld = (HFONT)GetCurrentObject(hdc, OBJ_FONT);

    h18 = CreateFontA(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET,
                      OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                      DEFAULT_PITCH | FF_DONTCARE, "Arial");
    SelectObject(hdc, h18);
    g_oglFontBase = 1000;
    wglUseFontBitmapsA(hdc, 32, 96, g_oglFontBase);
    DeleteObject(h18);

    h12 = CreateFontA(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET,
                      OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                      DEFAULT_PITCH | FF_DONTCARE, "Arial");
    SelectObject(hdc, h12);
    g_oglFontSmallBase = 2000;
    wglUseFontBitmapsA(hdc, 32, 96, g_oglFontSmallBase);
    DeleteObject(h12);

    SelectObject(hdc, hOld);
    done = 1;
}

static void oglDrawString(const char *s, int useSmall) {
    GLuint base = useSmall ? g_oglFontSmallBase : g_oglFontBase;
    if (!base)
        return;
    glListBase(base - 32);
    glCallLists((GLsizei)strlen(s), GL_UNSIGNED_BYTE, (const GLubyte *)s);
}
#else
static void oglInitBitmapFonts(void) {}
static void oglDrawString(const char *s, int useSmall) {
    (void)useSmall;
    for (; *s; ++s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (unsigned char)*s);
}
#endif

/* ------------------------------------------------------------------------- */
/* Globals                                                                   */
/* ------------------------------------------------------------------------- */

static int g_winW = 1024;
static int g_winH = 768;

static int currentScene = 1;

/* Time (seconds) */
static float sceneClock = 0.0f;
static float globalTime = 0.0f;

/* Scene 1: full briefing length, then fade to Scene 2 */
#define SCENE1_DURATION_SEC 30.0f
#define TRANSITION_SEC       3.0f

/* 0 = normal scene1, 1 = transitioning to scene2 */
static int inTransition = 0;
static float transitionT = 0.0f; /* 0..1 */

/* Scene 2 — stages match briefing diagram (perigee start -> apogee burn -> Hohmann -> new orbit -> escape burn -> escape -> freeze) */
static int orbitStage = 0;
#define STAGE_PERI_START    0
#define STAGE_APO_MANEUVER  1
#define STAGE_HOHMANN       2
#define STAGE_NEW_ORBIT     3
#define STAGE_ESCAPE_BURN   4
#define STAGE_ESCAPE_TRAJ   5
#define STAGE_FROZEN        6

static float satAngle = 0.0f; /* true anomaly; perigee at 0 */
static float earthSpin = 0.0f;

static const float rp = 2.2f; /* perigee distance (fixed, scaled scene units) */
static float ra = 3.0f;       /* apogee grows across maneuvers */
static float stageTimer = 0.0f;

/* Earth's sphere of influence (visual boundary in scene units) */
static const float SOI_RADIUS = 11.8f;

/* Escape arc animation 0..1, then hold still tableau */
static float escapeAnimT = 0.0f;

/* Saved still shot (STAGE_FROZEN): satellite, camera, Earth rotation, escape curve extent */
static float freezeSx, freezeSy, freezeSz;
static float freezeCamEye[3], freezeCamCenter[3], freezeCamUp[3];
static float freezeEarthSpin = 0.0f;
static float freezeEscapeU = 1.0f;
static float freezeRa = 11.0f; /* ellipse used to build escape curve in still shot */

/* Scene 2 draw path: satellite position set in display(), consumed by drawScene2() */
static float g_s2x, g_s2y, g_s2z;

/* Camera */
static float camEye[3] = { 0.0f, 3.5f, 14.0f };
static float camCenter[3] = { 0.0f, 1.0f, 0.0f };
static float camUp[3] = { 0.0f, 1.0f, 0.0f };

/* Boardroom camera (Scene 1 only — Scene 2 uses its own look-at each frame) */
static const float brUp[3] = { 0.0f, 1.0f, 0.0f };

/* Scene 1: dominant screen placement (world units, back wall) */
#define BR_SCR_CX 0.0f
#define BR_SCR_CY 2.35f
#define BR_SCR_CZ -8.05f
#define BR_SCR_W 16.0f   /* wide so screen fills ~70% of view with tuned camera */
#define BR_SCR_H 9.0f

/* Scene 1 cinematic animation */
static float pointerPhase = 0.0f;
static float g_presenterHand[3]; /* world position for laser pointer line */
static float screenSatAngle = 0.0f;       /* target true anomaly (integrated in timer) */
static float screenSatAngleSmooth = 0.0f; /* smoothed angle for drawing (reduces stutter) */
static float screenApoExpand = 0.0f;      /* 0..1 apogee stretch (timed with narration) */
static float screenBurnFlicker = 0.0f;    /* flicker phase for plume */

/* Stars (fixed random) */
#define NUM_STARS 400
static float starX[NUM_STARS], starY[NUM_STARS], starZ[NUM_STARS];

/* ------------------------------------------------------------------------- */
/* Utility                                                                   */
/* ------------------------------------------------------------------------- */

/* HUD / subtitles (2D pixel coordinates, bottom-left origin) */
static void drawText(int x, int y, const char *s) {
    oglInitBitmapFonts();
    glRasterPos2i(x, y);
#if OGL_USE_WGL_FONTS
    if (g_oglFontBase)
        oglDrawString(s, 0);
    else
#endif
    {
        for (; *s; ++s)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (unsigned char)*s);
    }
}

static void drawText2D(int x, int y, const char *s) {
    drawText(x, y, s);
}

static void drawText2DSmall(int x, int y, const char *s) {
    oglInitBitmapFonts();
    glRasterPos2i(x, y);
#if OGL_USE_WGL_FONTS
    if (g_oglFontSmallBase)
        oglDrawString(s, 1);
    else
#endif
    {
        for (; *s; ++s)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (unsigned char)*s);
    }
}

/* Approximate centered bitmap text (Scene 1 subtitles) */
static void drawTextCenteredLarge(int y, const char *s) {
    int n = (int)strlen(s);
    int approxW = n * 10;
    int x = (g_winW - approxW) / 2;
    if (x < 16)
        x = 16;
    drawText(x, y, s);
}

/* 3D billboard-ish label: disable depth for readability */
static void drawText3D(float x, float y, float z, const char *s) {
    GLboolean depthOn;
    glGetBooleanv(GL_DEPTH_TEST, &depthOn);
    glDisable(GL_DEPTH_TEST);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(x, y, z);
    oglInitBitmapFonts();
#if OGL_USE_WGL_FONTS
    if (g_oglFontSmallBase)
        oglDrawString(s, 1);
    else
#endif
    {
        for (; *s; ++s)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (unsigned char)*s);
    }
    if (depthOn) glEnable(GL_DEPTH_TEST);
}

static float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

static void vecLerp3(float *out, const float *a, const float *b, float t) {
    out[0] = lerp(a[0], b[0], t);
    out[1] = lerp(a[1], b[1], t);
    out[2] = lerp(a[2], b[2], t);
}

/* Ellipse polar radius with Earth at focus; theta=0 at perigee */
static void orbitRadiusFromAngle(float theta, float rp_, float ra_, float *r_out) {
    float a = 0.5f * (rp_ + ra_);
    float e = (ra_ - rp_) / (ra_ + rp_ + 1e-6f);
    float num = a * (1.0f - e * e);
    float den = 1.0f + e * (float)cos(theta);
    if (den < 0.01f) den = 0.01f;
    *r_out = num / den;
}

static void satellitePosEllipse(float theta, float rp_, float ra_, float *sx, float *sy, float *sz) {
    float r;
    orbitRadiusFromAngle(theta, rp_, ra_, &r);
    *sx = r * (float)cos(theta);
    *sy = 0.0f;
    *sz = r * (float)sin(theta);
}

/* Kepler: areal velocity ~const => d(theta)/dt ~ h / r^2 (faster near perigee, slower at apogee) */
static void keplerAdvance(float dt, float rp_, float ra_, float *theta) {
    float r;
    orbitRadiusFromAngle(*theta, rp_, ra_, &r);
    if (r < 0.2f)
        r = 0.2f;
    {
        float h = sqrtf(fmaxf(rp_ * ra_, 0.05f));
        *theta += dt * 2.35f * h / (r * r);
    }
    if (*theta > (float)(2.0 * M_PI))
        *theta -= (float)(2.0 * M_PI);
}

static float smoothstep(float t) {
    if (t <= 0.0f)
        return 0.0f;
    if (t >= 1.0f)
        return 1.0f;
    return t * t * (3.0f - 2.0f * t);
}

/* Escape path sample: open red trajectory leaving perigee, crossing SOI */
static void escapePathSample(float u, float rp_, float ra_, float *ox, float *oy, float *oz) {
    float rPeri;
    float su = smoothstep(u);
    orbitRadiusFromAngle(0.0f, rp_, ra_, &rPeri);
    {
        float rout = lerp(rPeri, SOI_RADIUS * 1.48f, su);
        float bend = 0.62f * su;
        *ox = rout * (float)cos(bend);
        *oy = 0.0f;
        *oz = rout * (float)sin(bend);
    }
}

/* Prograde tangent on elliptical arc (XZ); returns direction spacecraft moves */
static void satelliteTangent(float theta, float rp_, float ra_, float *tx, float *tz) {
    float e = (ra_ - rp_) / (ra_ + rp_ + 1e-6f);
    float a = 0.5f * (rp_ + ra_);
    float den = 1.0f + e * (float)cos(theta);
    if (den < 0.02f)
        den = 0.02f;
    {
        float num = a * (1.0f - e * e);
        float r = num / den;
        float drd = num * e * (float)sin(theta) / (den * den);
        float dxdt = drd * (float)cos(theta) - r * (float)sin(theta);
        float dzdt = drd * (float)sin(theta) + r * (float)cos(theta);
        float len = sqrtf(dxdt * dxdt + dzdt * dzdt);
        if (len < 1e-4f)
            len = 1.0f;
        *tx = dxdt / len;
        *tz = dzdt / len;
    }
}

static void escapePathTangent(float u, float raEllipse, float *tx, float *tz) {
    float x0, y0, z0, x1, y1, z1;
    float du = 0.03f;
    float u1 = fminf(1.0f, u + du);
    escapePathSample(u, rp, raEllipse, &x0, &y0, &z0);
    escapePathSample(u1, rp, raEllipse, &x1, &y1, &z1);
    {
        float dx = x1 - x0, dz = z1 - z0;
        float len = sqrtf(dx * dx + dz * dz);
        if (len < 1e-5f) {
            *tx = 0.0f;
            *tz = -1.0f;
            return;
        }
        *tx = dx / len;
        *tz = dz / len;
    }
}

static void snapshotFreeze(float sx, float sy, float sz) {
    float ang = atan2f(sz, sx);
    float dist = 21.0f;

    freezeSx = sx;
    freezeSy = sy;
    freezeSz = sz;
    freezeRa = ra;
    freezeEarthSpin = earthSpin;
    freezeEscapeU = 1.0f;

    freezeCamCenter[0] = 0.0f;
    freezeCamCenter[1] = 0.0f;
    freezeCamCenter[2] = 0.0f;
    freezeCamEye[0] = dist * 0.82f * (float)cos(ang + 0.5f);
    freezeCamEye[1] = 13.0f;
    freezeCamEye[2] = dist * 0.82f * (float)sin(ang + 0.5f);
    freezeCamUp[0] = 0.0f;
    freezeCamUp[1] = 1.0f;
    freezeCamUp[2] = 0.0f;
}

/* ------------------------------------------------------------------------- */
/* Scene 1: high-quality boardroom (drawBoardroom / drawTable / drawScreen / */
/*          drawOrbitAnimation / drawPeople — Scene 2 untouched)               */
/* ------------------------------------------------------------------------- */

static void drawChair(float x, float z, float rotY) {
    glPushMatrix();
    glTranslatef(x, 0.35f, z);
    glRotatef(rotY, 0, 1, 0);
    glColor3f(0.1f, 0.09f, 0.08f);
    glPushMatrix();
    glScalef(0.48f, 0.36f, 0.48f);
    oglSolidCube(1.0f);
    glPopMatrix();
    glTranslatef(0.0f, 0.46f, -0.16f);
    glScalef(0.42f, 0.56f, 0.09f);
    oglSolidCube(1.0f);
    glPopMatrix();
}

/* Large centered table with visible thickness */
static void drawTable(void) {
    glColor3f(0.08f, 0.055f, 0.04f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, -0.75f);
    glScalef(11.5f, 0.28f, 3.35f);
    oglSolidCube(1.0f);
    glPopMatrix();
    /* thin edge / apron */
    glColor3f(0.06f, 0.04f, 0.03f);
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, -0.75f);
    glScalef(11.35f, 0.12f, 3.22f);
    oglSolidCube(1.0f);
    glPopMatrix();
}

/*
 * Seated figure: head (sphere), torso (cube), arms (thin boxes).
 * scale = 1 front row, <1 for back rows; darkSilhouette for audience.
 */
static void drawSeatedPersonDetailed(float x, float z, float scale, float phase, float fade, int darkSilhouette) {
    float sway = sinf(globalTime * 0.55f + phase) * 0.09f;
    float tr = darkSilhouette ? 0.07f : 0.14f;
    float tg = darkSilhouette ? 0.08f : 0.22f;
    float tb = darkSilhouette ? 0.11f : 0.52f;

    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(scale, scale, scale);
    glRotatef(sway * 16.0f, 0.0f, 0.0f, 1.0f);

    glColor3f(tr * fade, tg * fade, tb * fade);
    glPushMatrix();
    glTranslatef(0.0f, 0.95f, 0.0f);
    glScalef(0.4f, 0.54f, 0.27f);
    oglSolidCube(1.0f);
    glPopMatrix();

    /* arms along table */
    glPushMatrix();
    glTranslatef(-0.34f, 1.02f, 0.1f);
    glScalef(0.1f, 0.08f, 0.38f);
    oglSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.34f, 1.02f, 0.1f);
    glScalef(0.1f, 0.08f, 0.38f);
    oglSolidCube(1.0f);
    glPopMatrix();

    glTranslatef(sinf(globalTime * 0.45f + phase) * 0.025f, 1.42f, 0.0f);
    glRotatef(sinf(globalTime * 0.7f + phase) * 5.0f, 0.0f, 1.0f, 0.0f);
    glColor3f(0.82f * fade, 0.72f * fade, 0.62f * fade);
    if (darkSilhouette)
        glColor3f(0.12f * fade, 0.11f * fade, 0.13f * fade);
    oglSolidSphere(0.21f, 18, 18);
    glPopMatrix();
}

/* Presenter (left): arm swings to point at screen; sets g_presenterHand */
static void drawPresenter(float fade) {
    float armSwing = 22.0f * (0.55f + 0.45f * sinf(globalTime * 0.9f + pointerPhase));
    float px = -5.35f, pz = -6.05f;

    glPushMatrix();
    glTranslatef(px, 0.0f, pz);
    /* Face screen: slight Y rotation toward +Z */
    glRotatef(-18.0f, 0.0f, 1.0f, 0.0f);

    glColor3f(0.12f * fade, 0.22f * fade, 0.55f * fade);
    glPushMatrix();
    glTranslatef(-0.12f, 0.45f, 0.0f);
    glScalef(0.14f, 0.45f, 0.14f);
    oglSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.12f, 0.45f, 0.0f);
    glScalef(0.14f, 0.45f, 0.14f);
    oglSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.18f * fade, 0.32f * fade, 0.72f * fade);
    glPushMatrix();
    glTranslatef(0.0f, 1.05f, 0.0f);
    glScalef(0.34f, 0.62f, 0.24f);
    oglSolidCube(1.0f);
    glPopMatrix();

    /* Right arm: shoulder at torso corner, rotate toward screen */
    glPushMatrix();
    glTranslatef(0.16f, 1.28f, 0.08f);
    glRotatef(-armSwing, 1.0f, 0.0f, 0.0f);
    glRotatef(-35.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, -0.12f, 0.0f);
    glColor3f(0.16f * fade, 0.3f * fade, 0.68f * fade);
    glPushMatrix();
    glScalef(0.1f, 0.38f, 0.1f);
    oglSolidCube(1.0f);
    glPopMatrix();
    /* Forearm + hand tip for pointer */
    glTranslatef(0.0f, -0.42f, 0.0f);
    glRotatef(8.0f * sinf(globalTime * 1.4f), 0.0f, 0.0f, 1.0f);
    glPushMatrix();
    glScalef(0.09f, 0.28f, 0.09f);
    oglSolidCube(1.0f);
    glPopMatrix();
    glTranslatef(0.0f, -0.2f, 0.0f);
    glColor3f(0.85f * fade, 0.75f * fade, 0.65f * fade);
    oglSolidSphere(0.09f, 10, 10);

    {
        float m[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
        /* Column-major: translation in indices 12,13,14 */
        g_presenterHand[0] = m[12];
        g_presenterHand[1] = m[13];
        g_presenterHand[2] = m[14];
    }
    glPopMatrix();

    glTranslatef(0.0f, 1.55f, 0.0f);
    glRotatef(sinf(globalTime * 0.8f) * 6.0f, 0.0f, 1.0f, 0.0f);
    glColor3f(0.88f * fade, 0.78f * fade, 0.68f * fade);
    oglSolidSphere(0.23f, 14, 14);
    glPopMatrix();
}

/* Assistant (right): nod and occasional point */
static void drawAssistant(float fade) {
    float nod = sinf(globalTime * 1.05f) * 7.0f;
    float pointArm = (sinf(globalTime * 0.35f) > 0.65f) ? 1.0f : 0.0f;

    glPushMatrix();
    glTranslatef(5.25f, 0.0f, -5.95f);
    glRotatef(16.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(nod, 1.0f, 0.0f, 0.0f);

    glColor3f(0.22f * fade, 0.22f * fade, 0.24f * fade);
    glPushMatrix();
    glTranslatef(-0.12f, 0.45f, 0.0f);
    glScalef(0.14f, 0.45f, 0.14f);
    oglSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.12f, 0.45f, 0.0f);
    glScalef(0.14f, 0.45f, 0.14f);
    oglSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.32f * fade, 0.32f * fade, 0.34f * fade);
    glPushMatrix();
    glTranslatef(0.0f, 1.05f, 0.0f);
    glScalef(0.4f, 0.66f, 0.28f);
    oglSolidCube(1.0f);
    glPopMatrix();

    if (pointArm > 0.5f) {
        glPushMatrix();
        glTranslatef(-0.18f, 1.25f, 0.05f);
        glRotatef(-40.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
        glColor3f(0.3f * fade, 0.3f * fade, 0.32f * fade);
        glPushMatrix();
        glScalef(0.09f, 0.32f, 0.09f);
        oglSolidCube(1.0f);
        glPopMatrix();
        glPopMatrix();
    }

    glTranslatef(0.0f, 1.55f, 0.0f);
    glColor3f(0.85f * fade, 0.76f * fade, 0.66f * fade);
    oglSolidSphere(0.23f, 14, 14);
    glPopMatrix();
}

/* ~10 audience + presenter + assistant; depth via scale and Z */
static void drawPeople(float fade) {
    drawChair(-3.05f, -2.35f, 0.0f);
    drawChair(-3.05f, -0.85f, 0.0f);
    drawChair(-3.05f, 0.65f, 0.0f);
    drawChair(-3.05f, 2.05f, 0.0f);
    drawChair(3.05f, -2.35f, 180.0f);
    drawChair(3.05f, -0.85f, 180.0f);
    drawChair(3.05f, 0.65f, 180.0f);
    drawChair(3.05f, 2.05f, 180.0f);

    /* Left block (back → front smaller) */
    drawSeatedPersonDetailed(-2.42f, -2.35f, 0.88f, 0.0f, fade, 1);
    drawSeatedPersonDetailed(-2.42f, -0.85f, 0.92f, 1.1f, fade, 1);
    drawSeatedPersonDetailed(-2.42f, 0.65f, 0.95f, 2.0f, fade, 1);
    drawSeatedPersonDetailed(-2.42f, 2.05f, 0.9f, 3.0f, fade, 1);
    /* Right block */
    drawSeatedPersonDetailed(2.42f, -2.35f, 0.88f, 4.0f, fade, 1);
    drawSeatedPersonDetailed(2.42f, -0.85f, 0.92f, 5.0f, fade, 1);
    drawSeatedPersonDetailed(2.42f, 0.65f, 0.95f, 0.5f, fade, 1);
    drawSeatedPersonDetailed(2.42f, 2.05f, 0.9f, 1.7f, fade, 1);
    /* Front row (closer to camera — slightly larger) */
    drawSeatedPersonDetailed(-1.15f, 0.35f, 1.05f, 6.0f, fade, 1);
    drawSeatedPersonDetailed(1.2f, 0.28f, 1.08f, 6.8f, fade, 1);

    drawPresenter(fade);
    drawAssistant(fade);
}

/* Particles at screen satellite during “burn” segments */
static void drawScreenBurnParticles(float cx, float cy, float cz, float sx, float sy, float sz) {
    int k, seed = 777;
    float dx, dy, dz;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPointSize(3.2f);
    glBegin(GL_POINTS);
    for (k = 0; k < 14; ++k) {
        seed = seed * 1103515245 + 12345;
        dx = ((seed >> 8) % 100) / 100.0f * 0.18f - 0.09f;
        seed = seed * 1103515245 + 12345;
        dy = ((seed >> 8) % 100) / 100.0f * 0.14f - 0.02f;
        seed = seed * 1103515245 + 12345;
        dz = ((seed >> 8) % 100) / 100.0f * 0.12f;
        glColor4f(1.0f, 0.55f + dz, 0.1f, 0.65f);
        glVertex3f(sx + dx, sy + dy, sz + dz + 0.02f);
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

/* Thick bezel + multi-layer glow + black panel (dominant screen) */
static void drawScreen(float fade) {
    float cx = BR_SCR_CX, cy = BR_SCR_CY, cz = BR_SCR_CZ;
    float w = BR_SCR_W, h = BR_SCR_H;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

    /* Wide outer bloom */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(0.2f * fade, 0.55f * fade, 1.0f * fade, 0.18f * fade);
    glBegin(GL_QUADS);
    glVertex3f(-w * 0.56f, cy - h * 0.56f, cz + 0.01f);
    glVertex3f( w * 0.56f, cy - h * 0.56f, cz + 0.01f);
    glVertex3f( w * 0.56f, cy + h * 0.56f, cz + 0.01f);
    glVertex3f(-w * 0.56f, cy + h * 0.56f, cz + 0.01f);
    glEnd();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.35f * fade, 0.65f * fade, 1.0f * fade, 0.55f * fade);
    glBegin(GL_QUADS);
    glVertex3f(-w * 0.53f, cy - h * 0.53f, cz + 0.03f);
    glVertex3f( w * 0.53f, cy - h * 0.53f, cz + 0.03f);
    glVertex3f( w * 0.53f, cy + h * 0.53f, cz + 0.03f);
    glVertex3f(-w * 0.53f, cy + h * 0.53f, cz + 0.03f);
    glEnd();

    glEnable(GL_LIGHTING);
    /* Outer frame (thick, metallic) */
    glColor3f(0.14f * fade, 0.15f * fade, 0.2f * fade);
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glScalef(w + 0.55f, h + 0.55f, 0.14f);
    oglSolidCube(1.0f);
    glPopMatrix();
    glColor3f(0.22f * fade, 0.28f * fade, 0.38f * fade);
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glScalef(w + 0.32f, h + 0.32f, 0.12f);
    oglSolidCube(1.0f);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor4f(0.02f * fade, 0.03f * fade, 0.07f * fade, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-w * 0.5f, cy - h * 0.5f, cz + 0.09f);
    glVertex3f( w * 0.5f, cy - h * 0.5f, cz + 0.09f);
    glVertex3f( w * 0.5f, cy + h * 0.5f, cz + 0.09f);
    glVertex3f(-w * 0.5f, cy + h * 0.5f, cz + 0.09f);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

/* Smooth orbit lines + Earth + satellite (uses smoothed angle) */
static void drawOrbitAnimation(float fade) {
    float cx = BR_SCR_CX, cy = BR_SCR_CY, cz = BR_SCR_CZ;
    float w = BR_SCR_W, h = BR_SCR_H;
    float fCirc, fEllip, fLbl, fEsc, apoStretch;
    float ex, ey, satX, satY, satZ;
    float angVis = screenSatAngleSmooth;
    int i, k, screenBurn;

    fCirc = smoothstep((sceneClock - 0.4f) / 2.8f);
    fEllip = smoothstep((sceneClock - 5.5f) / 3.2f);
    fLbl = smoothstep((sceneClock - 6.0f) / 2.5f);
    fEsc = smoothstep((sceneClock - 23.5f) / 4.5f);
    apoStretch = 1.0f + 0.42f * screenApoExpand;

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Earth (high segment count = smooth sphere) */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(0.28f * fade, 0.58f * fade, 1.0f * fade, 0.32f * fade);
    glPushMatrix();
    glTranslatef(cx, cy - 0.22f, cz + 0.11f);
    oglSolidSphere(1.05f, 40, 40);
    glPopMatrix();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.18f * fade, 0.42f * fade, 0.95f * fade, 1.0f);
    glPushMatrix();
    glTranslatef(cx, cy - 0.22f, cz + 0.13f);
    oglSolidSphere(0.92f, 40, 40);
    glPopMatrix();
    glColor3f(0.5f * fade, 0.78f * fade, 1.0f * fade);
    glPushMatrix();
    glTranslatef(cx, cy - 0.22f, cz + 0.135f);
    oglWireSphere(0.94f, 24, 20);
    glPopMatrix();

    /* Circular LEO */
    glLineWidth(1.6f);
    glColor4f(0.95f * fade, 0.97f * fade, 1.0f * fade, fCirc * fade);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i <= 96; ++i) {
        float t = (float)(2.0 * M_PI * i / 96.0);
        ex = 1.58f * (float)cos(t);
        ey = 1.02f * (float)sin(t);
        glVertex3f(cx + ex, cy - 0.22f + ey * 0.34f, cz + 0.12f);
    }
    glEnd();

    /* Ellipse — apogee expands with narration */
    glColor4f(0.9f * fade, 0.92f * fade, 1.0f * fade, fEllip * fade);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= 128; ++i) {
        float u = (float)(2.0 * M_PI * i / 128.0);
        ex = 1.88f * apoStretch * (float)cos(u);
        ey = 1.18f * (0.92f + 0.08f * screenApoExpand) * (float)sin(u);
        glVertex3f(cx + ex, cy - 0.22f + ey * 0.42f, cz + 0.12f);
    }
    glEnd();

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    glColor4f(0.5f * fade, 0.55f * fade, 0.68f * fade, fEllip * 0.88f * fade);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= 128; ++i) {
        float u = (float)(2.0 * M_PI * i / 128.0);
        ex = 2.12f * apoStretch * (float)cos(u);
        ey = 1.28f * (float)sin(u);
        glVertex3f(cx + ex, cy - 0.22f + ey * 0.42f, cz + 0.12f);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    /* Direction ticks */
    glColor4f(1.0f * fade, 1.0f * fade, 1.0f * fade, fEllip * fade);
    glLineWidth(1.2f);
    for (k = 0; k < 10; ++k) {
        float u = (float)(2.0 * M_PI * k / 10.0);
        float px = cx + 1.82f * apoStretch * (float)cos(u);
        float py = cy - 0.22f + 0.5f * (0.92f + 0.08f * screenApoExpand) * (float)sin(u);
        float nx = -0.22f * (float)sin(u);
        float ny = 0.06f * (float)cos(u);
        glBegin(GL_LINES);
        glVertex3f(px, py, cz + 0.125f);
        glVertex3f(px + nx, py + ny, cz + 0.125f);
        glEnd();
    }

    /* Escape path */
    glColor4f(1.0f * fade, 0.22f * fade, 0.12f * fade, fEsc * fade);
    glLineWidth(2.4f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= 40; ++i) {
        float u = (float)i / 40.0f;
        ex = lerp(3.35f * apoStretch, 6.2f * apoStretch, u);
        ey = lerp(0.0f, 2.85f, u * u);
        glVertex3f(cx + ex, cy - 0.22f + ey, cz + 0.12f);
    }
    glEnd();
    glLineWidth(1.0f);

    if (fLbl > 0.05f) {
        glColor4f(1.0f * fade, 1.0f * fade, 1.0f * fade, fLbl * fade);
        drawText3D(cx - 3.6f, cy - 1.65f, cz + 0.14f, "Perigee");
        drawText3D(cx + 2.1f, cy + 1.45f * apoStretch, cz + 0.14f, "Apogee");
        drawText3D(cx + 3.6f, cy + 0.25f, cz + 0.14f, "Escape Orbit");
        drawText3D(-w * 0.46f + cx, cy + h * 0.38f, cz + 0.14f, "PERIGEE / APOGEE TRANSFER & ESCAPE");
    }

    /* Satellite on ellipse — uses smoothed angle */
    ex = 1.88f * apoStretch * (float)cos(angVis);
    ey = 1.18f * (0.92f + 0.08f * screenApoExpand) * (float)sin(angVis);
    satX = cx + ex;
    satY = cy - 0.22f + ey * 0.42f;
    satZ = cz + 0.16f;

    glColor4f(0.78f * fade, 0.8f * fade, 0.84f * fade, fEllip * fade);
    glPushMatrix();
    glTranslatef(satX, satY, satZ);
    glScalef(0.2f, 0.1f, 0.16f);
    oglSolidCube(1.0f);
    glPopMatrix();
    glColor4f(0.22f * fade, 0.42f * fade, 0.92f * fade, fEllip * fade);
    glPushMatrix();
    glTranslatef(satX + 0.09f, satY, satZ + 0.03f);
    glScalef(0.1f, 0.035f, 0.06f);
    oglSolidCube(1.0f);
    glPopMatrix();

    screenBurn = 0;
    if (sceneClock >= 15.0f && sceneClock < 21.0f && (float)cos(angVis) > 0.88f)
        screenBurn = 1;
    if (sceneClock >= 20.0f && sceneClock < 28.0f && (float)cos(angVis) > 0.82f)
        screenBurn = 1;

    if (screenBurn && fEllip > 0.2f) {
        float tx = -0.18f * (float)sin(angVis);
        float ty = 0.08f * (float)cos(angVis);
        float flick = 0.75f + 0.25f * screenBurnFlicker;
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 0.95f * flick, 0.28f * flick, 0.95f);
        glVertex3f(satX + tx * 0.2f, satY + ty * 0.2f, satZ + 0.06f);
        glColor4f(1.0f, 0.42f * flick, 0.06f * flick, 0.6f);
        glVertex3f(satX - tx * 0.32f - 0.05f, satY - 0.1f, satZ + 0.06f);
        glColor4f(1.0f, 0.28f * flick, 0.04f * flick, 0.5f);
        glVertex3f(satX - tx * 0.32f + 0.05f, satY + 0.1f, satZ + 0.06f);
        glEnd();
        drawScreenBurnParticles(cx, cy, cz, satX, satY, satZ);
    }

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_LIGHTING);
}

/* Laser line from presenter hand to Earth on screen */
static void drawPresenterPointer(void) {
    float tx = BR_SCR_CX, ty = BR_SCR_CY - 0.25f, tz = BR_SCR_CZ + 0.22f;
    glDisable(GL_LIGHTING);
    glLineWidth(2.8f);
    glColor3f(1.0f, 0.92f, 0.45f);
    glBegin(GL_LINES);
    glVertex3f(g_presenterHand[0], g_presenterHand[1], g_presenterHand[2]);
    glVertex3f(tx, ty, tz);
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

/* Floor / ceiling gradient + walls (dark blue → black cinematic) */
static void drawBoardroom(float fade) {
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    /* Floor: black near camera, deep blue toward back wall */
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-18.0f, 0.0f, 10.0f);
    glVertex3f( 18.0f, 0.0f, 10.0f);
    glColor3f(0.02f * fade, 0.03f * fade, 0.09f * fade);
    glVertex3f( 18.0f, 0.0f, -14.0f);
    glVertex3f(-18.0f, 0.0f, -14.0f);
    glEnd();

    /* Simple glossy strip (fake reflection) */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.04f * fade, 0.06f * fade, 0.1f * fade, 0.22f * fade);
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, 0.015f, 4.0f);
    glVertex3f( 10.0f, 0.015f, 4.0f);
    glVertex3f( 8.0f, 0.015f, -4.0f);
    glVertex3f(-8.0f, 0.015f, -4.0f);
    glEnd();
    glDisable(GL_BLEND);

    /* Ceiling: dark blue down to black at horizon */
    glBegin(GL_QUADS);
    glColor3f(0.05f * fade, 0.08f * fade, 0.18f * fade);
    glVertex3f(-18.0f, 6.8f, -14.0f);
    glVertex3f(-18.0f, 6.8f,  10.0f);
    glColor3f(0.01f * fade, 0.015f * fade, 0.04f * fade);
    glVertex3f( 18.0f, 6.8f,  10.0f);
    glVertex3f( 18.0f, 6.8f, -14.0f);
    glEnd();

    glColor3f(0.04f * fade, 0.05f * fade, 0.09f * fade);
    glBegin(GL_QUADS);
    glVertex3f(-18.0f, 0.0f, -14.0f);
    glVertex3f(-18.0f, 6.8f, -14.0f);
    glVertex3f( 18.0f, 6.8f, -14.0f);
    glVertex3f( 18.0f, 0.0f, -14.0f);
    glEnd();

    glColor3f(0.028f * fade, 0.035f * fade, 0.06f * fade);
    glBegin(GL_QUADS);
    glVertex3f(-18.0f, 0.0f, 10.0f);
    glVertex3f(-18.0f, 6.8f, 10.0f);
    glVertex3f(-18.0f, 6.8f, -14.0f);
    glVertex3f(-18.0f, 0.0f, -14.0f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex3f(18.0f, 0.0f, -14.0f);
    glVertex3f(18.0f, 6.8f, -14.0f);
    glVertex3f(18.0f, 6.8f, 10.0f);
    glVertex3f(18.0f, 0.0f, 10.0f);
    glEnd();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
}

/* Scene 1 root: gentle scale-in + draw order for depth */
static void drawBoardroomScene(float fade) {
    float zoom = 1.0f + 0.1f * smoothstep(sceneClock / 27.0f);

    glPushMatrix();
    glTranslatef(0.0f, 1.2f, -2.4f);
    glScalef(zoom, zoom, zoom);
    glTranslatef(0.0f, -1.2f, 2.4f);

    drawBoardroom(fade);
    drawScreen(fade);
    drawOrbitAnimation(fade);
    drawTable();
    drawPeople(fade);
    drawPresenterPointer();

    glPopMatrix();
}

/* ------------------------------------------------------------------------- */
/* Scene 2: space                                                            */
/* ------------------------------------------------------------------------- */

static void initStars(void) {
    int i;
    for (i = 0; i < NUM_STARS; ++i) {
        starX[i] = (float)(rand() % 2000 - 1000) * 0.04f;
        starY[i] = (float)(rand() % 2000 - 1000) * 0.04f;
        starZ[i] = (float)(rand() % 2000 - 1000) * 0.04f;
    }
}

static void drawStars(void) {
    int i;
    glDisable(GL_LIGHTING);
    glPointSize(1.8f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (i = 0; i < NUM_STARS; ++i)
        glVertex3f(starX[i], starY[i], starZ[i]);
    glEnd();
    glEnable(GL_LIGHTING);
}

static void drawEarth(float spinDeg) {
    glPushMatrix();
    glRotatef(spinDeg, 0.0f, 1.0f, 0.0f);
    glColor3f(0.12f, 0.28f, 0.75f);
    oglSolidSphere(1.0f, 32, 32);
    glColor3f(0.35f, 0.65f, 0.35f);
    /* simple "continents" bands */
    glPushMatrix();
    glRotatef(40.0f, 1, 0, 0);
    oglWireSphere(1.01f, 12, 8);
    glPopMatrix();
    glPopMatrix();
}

static void drawOrbit(float rp_, float ra_, int segments) {
    int i;
    glDisable(GL_LIGHTING);
    glLineWidth(1.8f);
    glColor3f(0.95f, 0.96f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= segments; ++i) {
        float th = (float)(2.0 * M_PI * i / segments);
        float sx, sy, sz;
        satellitePosEllipse(th, rp_, ra_, &sx, &sy, &sz);
        glVertex3f(sx, sy, sz);
    }
    glEnd();

    /* faint second ellipse (transfer) */
    if (ra_ > rp_ + 0.3f) {
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x5555);
        glColor3f(0.5f, 0.55f, 0.65f);
        glBegin(GL_LINE_STRIP);
        for (i = 0; i <= segments; ++i) {
            float th = (float)(2.0 * M_PI * i / segments);
            float sx, sy, sz;
            satellitePosEllipse(th, rp_, ra_ * 1.08f, &sx, &sy, &sz);
            glVertex3f(sx, sy, sz);
        }
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

/* scale: 1 = apogee-type burn, ~2.2 = escape burn (diagram "BIG FIRE") */
static void drawFlame(float sx, float sy, float sz, float dirX, float dirZ, float intensity, float scale) {
    int i;
    float len = (0.35f + intensity * 0.55f) * scale;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (i = 0; i < (scale > 1.5f ? 8 : 5); ++i) {
        float jitter = 0.04f * (float)(rand() % 100) / 100.0f;
        glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 0.95f, 0.2f, 0.9f);
        glVertex3f(sx + dirX * (len * 0.2f), sy, sz + dirZ * (len * 0.2f));
        glColor4f(1.0f, 0.45f, 0.05f, 0.5f);
        glVertex3f(sx - dirX * (len * 0.35f) + jitter, sy - 0.08f * scale, sz - dirZ * (len * 0.35f) + jitter);
        glColor4f(1.0f, 0.25f, 0.0f, 0.35f);
        glVertex3f(sx - dirX * (len * 0.35f) - jitter, sy + 0.08f * scale, sz - dirZ * (len * 0.35f) - jitter);
        glEnd();
    }
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

/* burnKind: 0 none, 1 apogee maneuver, 2 perigee / circularization, 3 escape burn, 4 escape coast plume */
static void drawSatellite(float sx, float sy, float sz, float dirX, float dirZ, int burnKind) {
    glPushMatrix();
    glTranslatef(sx, sy, sz);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.4f, 0.75f, 1.0f, 0.25f);
    oglSolidSphere(0.35f, 16, 16);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    glColor3f(0.75f, 0.78f, 0.82f);
    glPushMatrix();
    glScalef(0.45f, 0.22f, 0.32f);
    oglSolidCube(1.0f);
    glPopMatrix();
    glColor3f(0.3f, 0.5f, 0.9f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.22f);
    glScalef(0.25f, 0.08f, 0.1f);
    oglSolidCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    if (burnKind == 1)
        drawFlame(sx - dirX * 0.22f, sy, sz - dirZ * 0.22f, dirX, dirZ, 0.75f, 1.0f);
    else if (burnKind == 2)
        drawFlame(sx - dirX * 0.24f, sy, sz - dirZ * 0.24f, dirX, dirZ, 0.6f, 0.95f);
    else if (burnKind == 3)
        drawFlame(sx - dirX * 0.28f, sy, sz - dirZ * 0.28f, dirX, dirZ, 1.35f, 2.25f);
    else if (burnKind == 4)
        drawFlame(sx - dirX * 0.26f, sy, sz - dirZ * 0.26f, dirX, dirZ, 0.5f, 1.15f);
}

static void drawSphereOfInfluence(void) {
    int i;
    glDisable(GL_LIGHTING);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xF0F0);
    glLineWidth(1.5f);
    glColor3f(0.62f, 0.66f, 0.78f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i <= 80; ++i) {
        float t = (float)(2.0 * M_PI * i / 80.0);
        glVertex3f(SOI_RADIUS * (float)cos(t), 0.0f, SOI_RADIUS * (float)sin(t));
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

/* Red open escape trajectory; uEnd in [0,1] animates drawing then holds at 1 */
static void drawEscapeArc(float uEnd, float raEllipse) {
    int i;
    const int n = 72;
    float uClamped = fminf(1.0f, fmaxf(0.0f, uEnd));

    glDisable(GL_LIGHTING);
    glLineWidth(2.5f);
    glColor3f(1.0f, 0.2f, 0.1f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= n; ++i) {
        float u = uClamped * (float)i / (float)n;
        float x, y, z;
        escapePathSample(u, rp, raEllipse, &x, &y, &z);
        glVertex3f(x, y, z);
    }
    glEnd();
    /* Direction arrow past SOI */
    if (uClamped > 0.2f) {
        float x0, y0, z0, x1, y1, z1;
        escapePathSample(uClamped * 0.82f, rp, raEllipse, &x0, &y0, &z0);
        escapePathSample(uClamped * 0.95f, rp, raEllipse, &x1, &y1, &z1);
        glColor3f(1.0f, 0.45f, 0.3f);
        glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x1 + (x1 - x0) * 2.8f, y1, z1 + (z1 - z0) * 2.8f);
        glEnd();
    }
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

static void drawOrbitDashedGrey(float rp_, float ra_, int segments) {
    int i;
    glDisable(GL_LIGHTING);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0xAAAA);
    glColor3f(0.48f, 0.52f, 0.6f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= segments; ++i) {
        float th = (float)(2.0 * M_PI * i / segments);
        float sx, sy, sz;
        satellitePosEllipse(th, rp_, ra_, &sx, &sy, &sz);
        glVertex3f(sx, sy, sz);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glEnable(GL_LIGHTING);
}

/* ------------------------------------------------------------------------- */
/* Camera & logic                                                            */
/* ------------------------------------------------------------------------- */

static void updateCamera(float sx, float sy, float sz) {
    float dist = 9.0f;
    float height = 3.2f;

    if (orbitStage == STAGE_ESCAPE_BURN) {
        /* Pull back so the final orbit + upcoming escape read like the poster */
        dist = lerp(9.0f, 15.0f, fminf(1.0f, stageTimer / 4.5f));
        height = lerp(3.2f, 6.0f, fminf(1.0f, stageTimer / 4.5f));
    } else if (orbitStage == STAGE_ESCAPE_TRAJ) {
        dist = lerp(13.0f, 20.0f, smoothstep(escapeAnimT));
        height = lerp(5.5f, 9.0f, smoothstep(escapeAnimT));
    }

    camCenter[0] = sx * 0.24f;
    camCenter[1] = 0.0f;
    camCenter[2] = sz * 0.24f;

    camEye[0] = sx + (float)cos(globalTime * 0.28f) * dist * 0.4f;
    camEye[1] = height;
    camEye[2] = sz + dist;

    camUp[0] = 0.0f;
    camUp[1] = 1.0f;
    camUp[2] = 0.0f;
}

static void updateOrbitStages(float dt) {
    int nearPeri = (satAngle < 0.3f || satAngle > (float)(2.0 * M_PI) - 0.3f);
    int nearApo = fabsf(satAngle - (float)M_PI) < 0.34f;

    stageTimer += dt;

    switch (orbitStage) {
    case STAGE_PERI_START:
        ra = lerp(ra, 3.15f, dt * 0.07f);
        if (stageTimer > 5.8f) {
            orbitStage = STAGE_APO_MANEUVER;
            stageTimer = 0.0f;
        }
        break;
    case STAGE_APO_MANEUVER:
        if (nearApo && stageTimer > 0.35f)
            ra = lerp(ra, 4.65f, dt * 0.42f);
        if (stageTimer > 6.8f) {
            orbitStage = STAGE_HOHMANN;
            stageTimer = 0.0f;
        }
        break;
    case STAGE_HOHMANN:
        ra = lerp(ra, 5.35f, dt * 0.07f);
        if (stageTimer > 4.5f) {
            orbitStage = STAGE_NEW_ORBIT;
            stageTimer = 0.0f;
        }
        break;
    case STAGE_NEW_ORBIT:
        if (nearPeri && stageTimer > 0.45f)
            ra = lerp(ra, 8.4f, dt * 0.32f);
        if (stageTimer > 6.8f) {
            orbitStage = STAGE_ESCAPE_BURN;
            stageTimer = 0.0f;
        }
        break;
    case STAGE_ESCAPE_BURN:
        if (nearPeri && stageTimer > 0.55f)
            ra = lerp(ra, 11.0f, dt * 0.5f);
        if (stageTimer > 5.8f) {
            orbitStage = STAGE_ESCAPE_TRAJ;
            stageTimer = 0.0f;
            escapeAnimT = 0.0f;
        }
        break;
    case STAGE_ESCAPE_TRAJ:
        escapeAnimT += dt / 5.85f;
        if (escapeAnimT >= 1.0f) {
            float fsx, fsy, fsz;
            escapeAnimT = 1.0f;
            escapePathSample(1.0f, rp, ra, &fsx, &fsy, &fsz);
            snapshotFreeze(fsx, fsy, fsz);
            orbitStage = STAGE_FROZEN;
        }
        break;
    case STAGE_FROZEN:
        break;
    }
}

static int nearPerigeeBurnWindow(void) {
    return (satAngle < 0.32f || satAngle > (float)(2.0 * M_PI) - 0.32f);
}

static int nearApogeeBurnWindow(void) {
    return fabsf(satAngle - (float)M_PI) < 0.34f;
}

/* ------------------------------------------------------------------------- */
/* Display                                                                   */
/* ------------------------------------------------------------------------- */

static void drawHUD(void) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_winW, 0, g_winH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    if (currentScene == 1 && !inTransition) {
        const char *line = "";
        float t = sceneClock;
        float segStart = 0.0f;
        if (t < 3.0f) {
            line = "Understanding Orbital Transfer";
            segStart = 0.0f;
        } else if (t < 6.0f) {
            line = "A satellite orbits Earth in Low Earth Orbit";
            segStart = 3.0f;
        } else if (t < 10.0f) {
            line = "Perigee is the closest point to Earth";
            segStart = 6.0f;
        } else if (t < 15.0f) {
            line = "At perigee, velocity is maximum";
            segStart = 10.0f;
        } else if (t < 20.0f) {
            line = "A burn increases velocity (Oberth Effect)";
            segStart = 15.0f;
        } else if (t < 25.0f) {
            line = "This raises the apogee of the orbit";
            segStart = 20.0f;
        } else {
            line = "Repeated burns lead to escape trajectory";
            segStart = 25.0f;
        }
        {
            float subFade = smoothstep((t - segStart) / 0.5f);
            glColor3f(0.92f, 0.94f, 1.0f);
            drawTextCenteredLarge(g_winH - 58, "Orbital Mechanics Briefing");
            glColor3f(0.76f * subFade, 0.84f * subFade, 0.98f * subFade);
            drawTextCenteredLarge(g_winH - 88, line);
        }
        glColor3f(0.55f, 0.6f, 0.72f);
        drawText2DSmall(24, 24, "Space/Enter: skip to Scene 2   1/2: jump   R: restart");
    } else if (currentScene == 1 && inTransition) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText2D(g_winW / 2 - 120, g_winH / 2, "Entering simulation...");
    } else {
        const char *title = "";
        const char *sub = "";
        switch (orbitStage) {
        case STAGE_PERI_START:
            title = "PERIGEE AND APOGEE TRANSFER & ESCAPE";
            sub = "Start at PERIGEE (closest approach) — inner orbit.";
            break;
        case STAGE_APO_MANEUVER:
            title = "APOGEE MANEUVER (velocity increase)";
            sub = "Firing engines at apogee to raise orbit size.";
            break;
        case STAGE_HOHMANN:
            title = "HOHMANN TRANSFER";
            sub = "Coasting along transfer ellipse to higher orbit.";
            break;
        case STAGE_NEW_ORBIT:
            title = "NEW PERIGEE — CIRCULARIZATION";
            sub = "Burn at perigee to settle into a larger stable orbit.";
            break;
        case STAGE_ESCAPE_BURN:
            title = "ESCAPE BURN AT PERIGEE";
            sub = "Big thrust — approaching ~11.2 km/s scale (schematic).";
            break;
        case STAGE_ESCAPE_TRAJ:
            title = "Leaving Earth's gravitational sphere";
            sub = "Open escape path crosses the SOI boundary.";
            break;
        case STAGE_FROZEN:
            title = "ESCAPE from Earth's gravitational sphere";
            sub = "Still shot — satellite on interplanetary trajectory (R=restart).";
            break;
        default:
            title = "Orbital simulation";
            sub = "";
            break;
        }
        glColor3f(0.92f, 0.94f, 1.0f);
        drawText(24, g_winH - 52, title);
        glColor3f(0.72f, 0.78f, 0.92f);
        drawText2DSmall(24, g_winH - 76, sub);
        drawText2DSmall(24, 24, "1/2: scenes   R: restart");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/* Scene 2: orbital animation (logic lives in timer/updateOrbitStages; this is draw-only). */
static void drawScene2(void) {
    float sx = g_s2x;
    float sy = g_s2y;
    float sz = g_s2z;
    float earthAng = (orbitStage == STAGE_FROZEN) ? freezeEarthSpin : earthSpin;

    drawStars();
    drawSphereOfInfluence();
    drawEarth(earthAng);

    if (orbitStage == STAGE_ESCAPE_TRAJ || orbitStage == STAGE_FROZEN) {
        float raE = (orbitStage == STAGE_FROZEN) ? freezeRa : ra;
        float uE = (orbitStage == STAGE_FROZEN) ? 1.0f : escapeAnimT;
        drawOrbitDashedGrey(rp, raE, 128);
        drawEscapeArc(uE, raE);
    } else {
        drawOrbit(rp, ra, 128);
        if (orbitStage == STAGE_HOHMANN)
            drawOrbitDashedGrey(rp, (rp + ra) * 0.52f + 0.65f, 88);
    }

    {
        float tdx, tdz;
        int burnKind = 0;

        if (orbitStage == STAGE_ESCAPE_TRAJ) {
            escapePathTangent(escapeAnimT, ra, &tdx, &tdz);
            tdx = -tdx;
            tdz = -tdz;
        } else if (orbitStage != STAGE_FROZEN) {
            satelliteTangent(satAngle, rp, ra, &tdx, &tdz);
            tdx = -tdx;
            tdz = -tdz;
        } else {
            escapePathTangent(1.0f, freezeRa, &tdx, &tdz);
            tdx = -tdx;
            tdz = -tdz;
        }

        if (orbitStage == STAGE_APO_MANEUVER && nearApogeeBurnWindow())
            burnKind = 1;
        else if (orbitStage == STAGE_NEW_ORBIT && nearPerigeeBurnWindow())
            burnKind = 2;
        else if (orbitStage == STAGE_ESCAPE_BURN && nearPerigeeBurnWindow())
            burnKind = 3;
        else if (orbitStage == STAGE_ESCAPE_TRAJ && escapeAnimT < 0.28f)
            burnKind = 4;

        drawSatellite(sx, sy, sz, tdx, tdz, burnKind);
    }
}

static void display(void) {
    float fade = 1.0f;
    float sx, sy, sz;

    if (currentScene == 1) {
        if (inTransition) {
            fade = 1.0f - transitionT;
            /* Fade boardroom black toward Scene 2 deep space */
            glClearColor(lerp(0.05f, 0.02f, transitionT) * fade + 0.02f * transitionT,
                         lerp(0.05f, 0.02f, transitionT) * fade + 0.02f * transitionT,
                         lerp(0.12f, 0.06f, transitionT) * fade + 0.06f * transitionT, 1.0f);
        } else {
            glClearColor(0.05f, 0.05f, 0.12f, 1.0f);
        }
    } else {
        glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (currentScene == 1)
        gluPerspective(42.0, (double)g_winW / (double)g_winH, 0.1, 200.0);
    else
        gluPerspective(50.0, (double)g_winW / (double)g_winH, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (currentScene == 1) {
        float eye[3], ctr[3], up[3];
        float t = inTransition ? transitionT : 0.0f;
        float dolly = -0.7f * smoothstep(sceneClock / 28.0f);
        /* Start slightly back; slow dolly-in + transition pushes toward screen */
        float eye0[3] = { 0.0f, 3.08f, 12.4f + dolly };
        float ctr0[3] = { 0.0f, 2.12f, BR_SCR_CZ + 0.6f };
        float zoomEye[3] = { 0.0f, 2.9f, 6.2f };
        float zoomCtr[3] = { 0.0f, 2.05f, BR_SCR_CZ + 0.2f };
        vecLerp3(eye, eye0, zoomEye, t);
        vecLerp3(ctr, ctr0, zoomCtr, t);
        vecLerp3(up, brUp, brUp, t);
        gluLookAt(eye[0], eye[1], eye[2], ctr[0], ctr[1], ctr[2], up[0], up[1], up[2]);

        /* Key light toward presentation wall */
        {
            GLfloat Lp[] = { 0.0f, 4.2f, -4.5f, 1.0f };
            GLfloat Ld[] = { 0.88f, 0.9f, 1.0f, 1.0f };
            glLightfv(GL_LIGHT0, GL_POSITION, Lp);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, Ld);
        }

        GLfloat amb[] = { 0.28f * fade, 0.28f * fade, 0.34f * fade, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
        drawBoardroomScene(fade);

        /* Fade overlay */
        if (inTransition && transitionT > 0.01f) {
            glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            gluOrtho2D(0, 1, 0, 1);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            glColor4f(0.0f, 0.0f, 0.05f, transitionT * 0.92f);
            glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(1, 0);
            glVertex2f(1, 1);
            glVertex2f(0, 1);
            glEnd();
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glDisable(GL_BLEND);
            glEnable(GL_LIGHTING);
        }
    } else {
        /* Restore default light for space scene (Scene 2 logic unchanged) */
        {
            GLfloat Lp0[] = { 4.0f, 10.0f, 6.0f, 1.0f };
            GLfloat Ld0[] = { 0.9f, 0.9f, 0.85f, 1.0f };
            glLightfv(GL_LIGHT0, GL_POSITION, Lp0);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, Ld0);
        }
        GLfloat amb[] = { 0.15f, 0.15f, 0.2f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

        if (orbitStage == STAGE_FROZEN) {
            sx = freezeSx;
            sy = freezeSy;
            sz = freezeSz;
            gluLookAt(freezeCamEye[0], freezeCamEye[1], freezeCamEye[2],
                      freezeCamCenter[0], freezeCamCenter[1], freezeCamCenter[2],
                      freezeCamUp[0], freezeCamUp[1], freezeCamUp[2]);
        } else {
            if (orbitStage == STAGE_ESCAPE_TRAJ)
                escapePathSample(escapeAnimT, rp, ra, &sx, &sy, &sz);
            else
                satellitePosEllipse(satAngle, rp, ra, &sx, &sy, &sz);
            updateCamera(sx, sy, sz);
            gluLookAt(camEye[0], camEye[1], camEye[2],
                      camCenter[0], camCenter[1], camCenter[2],
                      camUp[0], camUp[1], camUp[2]);
        }

        g_s2x = sx;
        g_s2y = sy;
        g_s2z = sz;
        drawScene2();
    }

    drawHUD();
    glutSwapBuffers();
}

/* ------------------------------------------------------------------------- */
/* Timer & input                                                             */
/* ------------------------------------------------------------------------- */

static void timer(int id) {
    const float dt = 0.016f;
    (void)id;
    globalTime += dt;

    if (currentScene == 1) {
        sceneClock += dt;
        pointerPhase += dt * 1.8f;
        if (!inTransition) {
            /* Smooth orbit: small angular velocity step (Kepler-ish), no big jumps */
            {
                float omega = 0.11f + 0.085f * (float)cos(screenSatAngle);
                screenSatAngle += dt * omega;
                if (screenSatAngle > (float)(2.0 * M_PI))
                    screenSatAngle -= (float)(2.0 * M_PI);
            }
            /* Interpolate display angle toward target (handles wrap cleanly) */
            {
                float d = screenSatAngle - screenSatAngleSmooth;
                if (d > (float)M_PI)
                    d -= (float)(2.0 * M_PI);
                if (d < -(float)M_PI)
                    d += (float)(2.0 * M_PI);
                screenSatAngleSmooth += d * fminf(1.0f, dt * 16.0f);
                if (screenSatAngleSmooth > (float)(2.0 * M_PI))
                    screenSatAngleSmooth -= (float)(2.0 * M_PI);
                if (screenSatAngleSmooth < 0.0f)
                    screenSatAngleSmooth += (float)(2.0 * M_PI);
            }
            screenApoExpand = smoothstep((sceneClock - 17.5f) / 11.0f);
            screenBurnFlicker = sinf(globalTime * 22.0f) * 0.5f + 0.5f;
        }
        if (!inTransition && sceneClock >= SCENE1_DURATION_SEC) {
            inTransition = 1;
            transitionT = 0.0f;
        }
        if (inTransition) {
            transitionT += dt / TRANSITION_SEC;
            if (transitionT >= 1.0f) {
                currentScene = 2;
                inTransition = 0;
                transitionT = 0.0f;
                sceneClock = 0.0f;
                orbitStage = STAGE_PERI_START;
                satAngle = 0.0f;
                ra = 3.0f;
                stageTimer = 0.0f;
                escapeAnimT = 0.0f;
                earthSpin = 0.0f;
            }
        }
    } else {
        if (orbitStage != STAGE_FROZEN && orbitStage != STAGE_ESCAPE_TRAJ)
            keplerAdvance(dt, rp, ra, &satAngle);
        if (orbitStage != STAGE_FROZEN)
            updateOrbitStages(dt);
        if (orbitStage != STAGE_FROZEN)
            earthSpin += 21.0f * dt;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

static void reshape(int w, int h) {
    if (h <= 0) h = 1;
    g_winW = w;
    g_winH = h;
    glViewport(0, 0, w, h);
}

static void keyboard(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    switch (key) {
    case 27: /* Esc */
        exit(0);
        break;
    case ' ':
    case 13: /* Enter */
        if (currentScene == 1) {
            inTransition = 1;
            transitionT = 0.0f;
        }
        break;
    case '1':
        currentScene = 1;
        sceneClock = 0.0f;
        inTransition = 0;
        transitionT = 0.0f;
        screenSatAngle = 0.0f;
        screenSatAngleSmooth = 0.0f;
        screenApoExpand = 0.0f;
        screenBurnFlicker = 0.0f;
        break;
    case '2':
        currentScene = 2;
        inTransition = 0;
        orbitStage = STAGE_PERI_START;
        satAngle = 0.0f;
        ra = 3.0f;
        stageTimer = 0.0f;
        escapeAnimT = 0.0f;
        earthSpin = 0.0f;
        break;
    case 'r':
    case 'R':
        currentScene = 1;
        sceneClock = 0.0f;
        inTransition = 0;
        transitionT = 0.0f;
        screenSatAngle = 0.0f;
        screenSatAngleSmooth = 0.0f;
        screenApoExpand = 0.0f;
        screenBurnFlicker = 0.0f;
        orbitStage = STAGE_PERI_START;
        satAngle = 0.0f;
        ra = 3.0f;
        stageTimer = 0.0f;
        escapeAnimT = 0.0f;
        earthSpin = 0.0f;
        break;
    default:
        break;
    }
}

static void setupLighting(void) {
    GLfloat diff[] = { 0.9f, 0.9f, 0.85f, 1.0f };
    GLfloat pos[] = { 4.0f, 10.0f, 6.0f, 1.0f };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(g_winW, g_winH);
    glutCreateWindow("Orbital Cinematic — Boardroom to Space");
    srand(12345);
    initStars();
    setupLighting();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
