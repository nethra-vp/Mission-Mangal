// /*
//  * ============================================================
//  *  ISRO MISSION CONTROL - MOM TRAJECTORY: EARTH TO MARS
//  *  OpenGL + GLUT Cinematic 2D/3D Hybrid Animation in C
//  * ============================================================
//  *
//  * BUILD INSTRUCTIONS:
//  *   Linux/Mac:
//  *     gcc isro_mission_control.c -o isro_mission -lGL -lGLU -lglut -lm
//  *   Windows (MinGW):
//  *     gcc isro_mission_control.c -o isro_mission -lopengl32 -lglu32 -lfreeglut -lm
//  *
//  * Run: ./isro_mission
//  *
//  * Controls:
//  *   ESC - Quit
//  *   R   - Reset camera
//  *   +/- - Zoom In/Out
//  * ============================================================
//  */

// #include <GL/glut.h>
// #include <GL/gl.h>
// #include <GL/glu.h>
// #include <math.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>

// /* ============================================================
//  *  CONSTANTS & CONFIGURATION
//  * ============================================================ */
// #define WIN_W           1280
// #define WIN_H           720
// #define PI              3.14159265358979323846
// #define TWO_PI          (2.0 * PI)
// #define NUM_SCIENTISTS  11
// #define NUM_PARTICLES   250
// #define NUM_SCREENS     6
// #define TIMER_MS        16   /* ~60 FPS */

// /* ============================================================
//  *  GLOBAL TIME & ANIMATION STATE
//  * ============================================================ */
// static float  gTime        = 0.0f;   /* global time in seconds */
// static float  gCameraAngle = 0.0f;   /* slow orbit angle */
// static float  gCameraZoom  = 14.0f;  /* distance from center */
// static float  gCameraHeight= 5.5f;   /* eye height */

// /* ============================================================
//  *  SCROLLING TEXT PANELS
//  * ============================================================ */
// typedef struct {
//     float x, y, z;          /* world position */
//     float rotY;              /* panel rotation */
//     float scrollOffset;      /* text scroll position */
//     const char *lines[8];    /* text lines */
//     int   numLines;
// } Screen;

// static Screen gScreens[NUM_SCREENS];

// /* ============================================================
//  *  SCIENTISTS
//  * ============================================================ */
// typedef struct {
//     float x, z;             /* position on XZ plane */
//     float rotY;             /* facing direction */
//     float scale;            /* slight size variation */
//     float breathPhase;      /* offset for breathing anim */
//     int   isFemale;         /* silhouette variation */
//     float leanAngle;        /* slight lean toward center */
// } Scientist;

// static Scientist gSci[NUM_SCIENTISTS];

// /* ============================================================
//  *  PARTICLE SYSTEM
//  * ============================================================ */
// typedef struct {
//     float x, y, z;
//     float vx, vy, vz;       /* velocity */
//     float life;             /* 0..1 */
//     float maxLife;
//     float r, g, b;
//     float size;
// } Particle;

// static Particle gParticles[NUM_PARTICLES];

// /* ============================================================
//  *  TRAJECTORY PULSE
//  * ============================================================ */
// static float gTrajPulse   = 0.0f;
// static float gTrajOffset  = 0.0f;  /* animated head of trajectory */

// /* ============================================================
//  *  HELPER: Random float in [lo, hi]
//  * ============================================================ */
// static float randf(float lo, float hi) {
//     return lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
// }

// /* ============================================================
//  *  HELPER: Draw a string with GLUT bitmap fonts at 2D overlay
//  * ============================================================ */
// static void drawString2D(float x, float y, const char *str, void *font) {
//     glRasterPos2f(x, y);
//     while (*str) {
//         glutBitmapCharacter(font, *str++);
//     }
// }

// /* ============================================================
//  *  HELPER: Draw 3D text (stroke) at current transform
//  * ============================================================ */
// static void drawString3D(const char *str, float scale) {
//     glPushMatrix();
//     glScalef(scale, scale, scale);
//     while (*str) {
//         glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *str++);
//     }
//     glPopMatrix();
// }

// /* ============================================================
//  *  HELPER: Wireframe sphere helper (latitude/longitude lines)
//  * ============================================================ */
// static void drawWireframeSphere(float radius, int latSegs, int lonSegs,
//                                 float r, float g, float b, float alpha) {
//     glColor4f(r, g, b, alpha);
//     glLineWidth(1.0f);

//     /* Latitude rings */
//     for (int i = 1; i < latSegs; i++) {
//         float phi = PI * i / latSegs - PI / 2.0f;
//         float y   = radius * sinf(phi);
//         float cr  = radius * cosf(phi);
//         glBegin(GL_LINE_LOOP);
//         for (int j = 0; j <= lonSegs; j++) {
//             float theta = TWO_PI * j / lonSegs;
//             glVertex3f(cr * cosf(theta), y, cr * sinf(theta));
//         }
//         glEnd();
//     }

//     /* Longitude lines */
//     for (int j = 0; j < lonSegs; j++) {
//         float theta = TWO_PI * j / lonSegs;
//         glBegin(GL_LINE_STRIP);
//         for (int i = 0; i <= latSegs; i++) {
//             float phi = PI * i / latSegs - PI / 2.0f;
//             float x   = radius * cosf(phi) * cosf(theta);
//             float yy  = radius * sinf(phi);
//             float z   = radius * cosf(phi) * sinf(theta);
//             glVertex3f(x, yy, z);
//         }
//         glEnd();
//     }
// }

// /* ============================================================
//  *  INIT: Scientists placed in semi-circle around table
//  * ============================================================ */
// static void initScientists(void) {
//     /* Place scientists in a ring, radius ~3.2, angles spread */
//     float angles[NUM_SCIENTISTS];
//     /* Spread semi-circle from -150° to +150° (front open for viewer) */
//     for (int i = 0; i < NUM_SCIENTISTS; i++) {
//         float t = (float)i / (NUM_SCIENTISTS - 1);
//         angles[i] = (-150.0f + 300.0f * t) * (float)PI / 180.0f;
//     }

//     float ringR = 3.2f;
//     for (int i = 0; i < NUM_SCIENTISTS; i++) {
//         gSci[i].x          = ringR * sinf(angles[i]);
//         gSci[i].z          = ringR * cosf(angles[i]);
//         /* Face toward center */
//         gSci[i].rotY       = -angles[i] * 180.0f / (float)PI;
//         gSci[i].scale      = randf(0.88f, 1.08f);
//         gSci[i].breathPhase= randf(0.0f, TWO_PI);
//         gSci[i].isFemale   = (i % 3 == 0) ? 1 : 0;
//         gSci[i].leanAngle  = randf(-3.0f, 3.0f);
//     }
// }

// /* ============================================================
//  *  INIT: Particles (floating data dust)
//  * ============================================================ */
// static void initParticle(Particle *p) {
//     p->x    = randf(-6.0f, 6.0f);
//     p->y    = randf(-1.0f, 0.5f);
//     p->z    = randf(-6.0f, 6.0f);
//     p->vx   = randf(-0.005f, 0.005f);
//     p->vy   = randf(0.015f, 0.035f);
//     p->vz   = randf(-0.005f, 0.005f);
//     p->maxLife = randf(3.0f, 8.0f);
//     p->life = randf(0.0f, p->maxLife);
//     /* Color: blue/cyan/white */
//     int type = rand() % 3;
//     if (type == 0)      { p->r=0.2f; p->g=0.6f; p->b=1.0f; }
//     else if (type == 1) { p->r=0.0f; p->g=1.0f; p->b=1.0f; }
//     else                { p->r=0.8f; p->g=0.9f; p->b=1.0f; }
//     p->size = randf(2.0f, 5.0f);
// }

// static void initParticles(void) {
//     for (int i = 0; i < NUM_PARTICLES; i++)
//         initParticle(&gParticles[i]);
// }

// /* ============================================================
//  *  INIT: Wall screens with scrolling text
//  * ============================================================ */
// static void initScreens(void) {
//     /* Back wall screens */
//     gScreens[0].x = -5.5f; gScreens[0].y = 2.5f; gScreens[0].z = -7.8f;
//     gScreens[0].rotY = 0.0f; gScreens[0].scrollOffset = 0.0f;
//     gScreens[0].numLines = 6;
//     gScreens[0].lines[0] = "MISSION STATUS: INITIALIZING";
//     gScreens[0].lines[1] = "TEAM SYNC IN PROGRESS...";
//     gScreens[0].lines[2] = "TRAJECTORY CALC: 78%";
//     gScreens[0].lines[3] = "FUEL LOAD: NOMINAL";
//     gScreens[0].lines[4] = "COMMS UPLINK: ACTIVE";
//     gScreens[0].lines[5] = "SYSTEM CHECK: OK";

//     gScreens[1].x = 0.0f; gScreens[1].y = 3.5f; gScreens[1].z = -7.8f;
//     gScreens[1].rotY = 0.0f; gScreens[1].scrollOffset = 1.5f;
//     gScreens[1].numLines = 6;
//     gScreens[1].lines[0] = "MOM TRAJECTORY: EARTH->MARS";
//     gScreens[1].lines[1] = "DELTA-V: 2.65 KM/S";
//     gScreens[1].lines[2] = "TRANSFER ORBIT: ELLIPTICAL";
//     gScreens[1].lines[3] = "LAUNCH WINDOW: T-48 HRS";
//     gScreens[1].lines[4] = "PAYLOAD MASS: 1337 KG";
//     gScreens[1].lines[5] = "ISRO PSLV-C25 READY";

//     gScreens[2].x = 5.5f; gScreens[2].y = 2.5f; gScreens[2].z = -7.8f;
//     gScreens[2].rotY = 0.0f; gScreens[2].scrollOffset = 3.0f;
//     gScreens[2].numLines = 5;
//     gScreens[2].lines[0] = "TELEMETRY: NOMINAL";
//     gScreens[2].lines[1] = "GROUND STATIONS: ONLINE";
//     gScreens[2].lines[2] = "ORBIT INSERTION: CALC...";
//     gScreens[2].lines[3] = "ANTENNA ALIGN: 99.3%";
//     gScreens[2].lines[4] = "DATA RATE: 64 KBPS";

//     /* Side wall screens */
//     gScreens[3].x = -8.8f; gScreens[3].y = 2.5f; gScreens[3].z = -2.0f;
//     gScreens[3].rotY = 90.0f; gScreens[3].scrollOffset = 0.8f;
//     gScreens[3].numLines = 5;
//     gScreens[3].lines[0] = "TEAM FORMATION COMPLETE";
//     gScreens[3].lines[1] = "SCIENTISTS ASSIGNED: 11";
//     gScreens[3].lines[2] = "ROLE: MISSION PLANNING";
//     gScreens[3].lines[3] = "CLEARANCE: TOP SECRET";
//     gScreens[3].lines[4] = "STATUS: BRIEFING";

//     gScreens[4].x = 8.8f; gScreens[4].y = 2.5f; gScreens[4].z = -2.0f;
//     gScreens[4].rotY = -90.0f; gScreens[4].scrollOffset = 2.0f;
//     gScreens[4].numLines = 5;
//     gScreens[4].lines[0] = "SUBSYSTEM POWER: 100%";
//     gScreens[4].lines[1] = "THERMAL CONTROL: NOMINAL";
//     gScreens[4].lines[2] = "ATTITUDE CONTROL: READY";
//     gScreens[4].lines[3] = "PROPULSION: PRE-ARMED";
//     gScreens[4].lines[4] = "OBC BOOT: COMPLETE";

//     gScreens[5].x = -8.8f; gScreens[5].y = 2.5f; gScreens[5].z = 2.0f;
//     gScreens[5].rotY = 90.0f; gScreens[5].scrollOffset = 4.0f;
//     gScreens[5].numLines = 5;
//     gScreens[5].lines[0] = "LAUNCH VEHICLE: PSLV-C25";
//     gScreens[5].lines[1] = "COUNTDOWN: T-48:00:00";
//     gScreens[5].lines[2] = "WEATHER: CLEAR";
//     gScreens[5].lines[3] = "WIND: 12 KM/H NE";
//     gScreens[5].lines[4] = "GO FOR LAUNCH";
// }

// /* ============================================================
//  *  INIT: OpenGL state
//  * ============================================================ */
// static void init(void) {
//     srand((unsigned int)time(NULL));

//     glClearColor(0.02f, 0.03f, 0.08f, 1.0f);
//     glEnable(GL_DEPTH_TEST);
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     glEnable(GL_LINE_SMOOTH);
//     glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//     glEnable(GL_POINT_SMOOTH);
//     glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

//     /* Lighting setup */
//     glEnable(GL_LIGHTING);
//     glEnable(GL_LIGHT0);  /* Table hologram light (cyan) */
//     glEnable(GL_LIGHT1);  /* Ambient fill light */
//     glEnable(GL_NORMALIZE);
//     glShadeModel(GL_SMOOTH);

//     /* Ambient light (dim blue) */
//     GLfloat ambLight[] = {0.05f, 0.07f, 0.15f, 1.0f};
//     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambLight);

//     /* Light1: soft overhead fill */
//     GLfloat light1Pos[] = {0.0f, 10.0f, 0.0f, 1.0f};
//     GLfloat light1Diff[] = {0.1f, 0.15f, 0.25f, 1.0f};
//     glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
//     glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1Diff);

//     initScientists();
//     initParticles();
//     initScreens();
// }

// /* ============================================================
//  *  DRAW: Reflective Floor
//  * ============================================================ */
// static void drawFloor(void) {
//     glDisable(GL_LIGHTING);
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//     /* Dark tiled floor */
//     float floorY = -1.0f;
//     int tiles = 12;
//     float tileSize = 1.5f;
//     float half = tiles * tileSize * 0.5f;

//     for (int i = 0; i < tiles; i++) {
//         for (int j = 0; j < tiles; j++) {
//             float fx = -half + i * tileSize;
//             float fz = -half + j * tileSize;
//             float bright = ((i + j) % 2 == 0) ? 0.06f : 0.04f;
//             glColor4f(bright, bright * 1.5f, bright * 2.5f, 0.9f);
//             glBegin(GL_QUADS);
//             glVertex3f(fx,          floorY, fz);
//             glVertex3f(fx+tileSize, floorY, fz);
//             glVertex3f(fx+tileSize, floorY, fz+tileSize);
//             glVertex3f(fx,          floorY, fz+tileSize);
//             glEnd();
//         }
//     }

//     /* Floor grid glow lines */
//     glColor4f(0.0f, 0.5f, 1.0f, 0.12f);
//     glLineWidth(0.5f);
//     glBegin(GL_LINES);
//     for (int i = 0; i <= tiles; i++) {
//         float p = -half + i * tileSize;
//         glVertex3f(p, floorY + 0.001f, -half);
//         glVertex3f(p, floorY + 0.001f,  half);
//         glVertex3f(-half, floorY + 0.001f, p);
//         glVertex3f( half, floorY + 0.001f, p);
//     }
//     glEnd();

//     glEnable(GL_LIGHTING);
// }

// /* ============================================================
//  *  DRAW: Ceiling with soft panels
//  * ============================================================ */
// static void drawCeiling(void) {
//     glDisable(GL_LIGHTING);
//     float ceilY = 7.0f;

//     /* Ceiling base */
//     glColor4f(0.03f, 0.04f, 0.08f, 1.0f);
//     glBegin(GL_QUADS);
//     glVertex3f(-10.0f, ceilY, -10.0f);
//     glVertex3f( 10.0f, ceilY, -10.0f);
//     glVertex3f( 10.0f, ceilY,  10.0f);
//     glVertex3f(-10.0f, ceilY,  10.0f);
//     glEnd();

//     /* Ceiling light panels (soft rectangles) */
//     float panelY = ceilY - 0.01f;
//     float panelPositions[4][2] = {
//         {-3.0f, -3.0f}, {3.0f, -3.0f},
//         {-3.0f,  3.0f}, {3.0f,  3.0f}
//     };
//     float pulse = 0.75f + 0.25f * sinf(gTime * 2.5f);

//     for (int i = 0; i < 4; i++) {
//         float px = panelPositions[i][0];
//         float pz = panelPositions[i][1];
//         glColor4f(0.6f * pulse, 0.75f * pulse, 1.0f * pulse, 0.35f);
//         glBegin(GL_QUADS);
//         glVertex3f(px - 1.0f, panelY, pz - 0.3f);
//         glVertex3f(px + 1.0f, panelY, pz - 0.3f);
//         glVertex3f(px + 1.0f, panelY, pz + 0.3f);
//         glVertex3f(px - 1.0f, panelY, pz + 0.3f);
//         glEnd();

//         /* Halo around panel */
//         glColor4f(0.3f * pulse, 0.5f * pulse, 1.0f * pulse, 0.08f);
//         glBegin(GL_QUADS);
//         glVertex3f(px - 2.0f, panelY + 0.01f, pz - 1.0f);
//         glVertex3f(px + 2.0f, panelY + 0.01f, pz - 1.0f);
//         glVertex3f(px + 2.0f, panelY + 0.01f, pz + 1.0f);
//         glVertex3f(px - 2.0f, panelY + 0.01f, pz + 1.0f);
//         glEnd();
//     }
//     glEnable(GL_LIGHTING);
// }

// /* ============================================================
//  *  DRAW: Walls
//  * ============================================================ */
// static void drawWalls(void) {
//     glDisable(GL_LIGHTING);
//     float wallColor[4] = {0.04f, 0.06f, 0.12f, 1.0f};
//     glColor4fv(wallColor);

//     /* Back wall */
//     glBegin(GL_QUADS);
//     glVertex3f(-10.0f, -1.0f, -8.0f);
//     glVertex3f( 10.0f, -1.0f, -8.0f);
//     glVertex3f( 10.0f,  7.0f, -8.0f);
//     glVertex3f(-10.0f,  7.0f, -8.0f);
//     glEnd();

//     /* Side walls */
//     glBegin(GL_QUADS);
//     glVertex3f(-9.0f, -1.0f, -8.0f);
//     glVertex3f(-9.0f, -1.0f,  8.0f);
//     glVertex3f(-9.0f,  7.0f,  8.0f);
//     glVertex3f(-9.0f,  7.0f, -8.0f);
//     glEnd();

//     glBegin(GL_QUADS);
//     glVertex3f( 9.0f, -1.0f,  8.0f);
//     glVertex3f( 9.0f, -1.0f, -8.0f);
//     glVertex3f( 9.0f,  7.0f, -8.0f);
//     glVertex3f( 9.0f,  7.0f,  8.0f);
//     glEnd();

//     /* Ambient glow on back wall */
//     float glow = 0.5f + 0.5f * sinf(gTime * 2.5f);
//     glColor4f(0.0f, 0.3f * glow, 0.5f * glow, 0.15f);
//     glBegin(GL_QUADS);
//     glVertex3f(-6.0f, -1.0f, -7.95f);
//     glVertex3f( 6.0f, -1.0f, -7.95f);
//     glVertex3f( 6.0f,  7.0f, -7.95f);
//     glVertex3f(-6.0f,  7.0f, -7.95f);
//     glEnd();

//     glEnable(GL_LIGHTING);
// }

// /* ============================================================
//  *  DRAW: A single data screen panel with scrolling text
//  * ============================================================ */
// static void drawScreen(Screen *sc) {
//     glDisable(GL_LIGHTING);
//     glPushMatrix();
//     glTranslatef(sc->x, sc->y, sc->z);
//     glRotatef(sc->rotY, 0, 1, 0);

//     float sw = 2.4f, sh = 1.6f;
//     float hw = sw * 0.5f, hh = sh * 0.5f;

//     /* Screen bezel */
//     glColor4f(0.05f, 0.07f, 0.15f, 1.0f);
//     glBegin(GL_QUADS);
//     glVertex3f(-hw - 0.08f, -hh - 0.08f, 0.0f);
//     glVertex3f( hw + 0.08f, -hh - 0.08f, 0.0f);
//     glVertex3f( hw + 0.08f,  hh + 0.08f, 0.0f);
//     glVertex3f(-hw - 0.08f,  hh + 0.08f, 0.0f);
//     glEnd();

//     /* Screen background */
//     glColor4f(0.02f, 0.06f, 0.12f, 0.95f);
//     glBegin(GL_QUADS);
//     glVertex3f(-hw, -hh, 0.01f);
//     glVertex3f( hw, -hh, 0.01f);
//     glVertex3f( hw,  hh, 0.01f);
//     glVertex3f(-hw,  hh, 0.01f);
//     glEnd();

//     /* Scan-line overlay (subtle) */
//     glColor4f(0.0f, 0.5f, 1.0f, 0.05f);
//     glLineWidth(0.5f);
//     int scanLines = (int)(sh / 0.04f);
//     glBegin(GL_LINES);
//     for (int s = 0; s < scanLines; s++) {
//         float sy = -hh + s * 0.04f;
//         glVertex3f(-hw, sy, 0.02f);
//         glVertex3f( hw, sy, 0.02f);
//     }
//     glEnd();

//     /* Border glow */
//     float bGlow = 0.6f + 0.4f * sinf(gTime * 1.2f + sc->x);
//     glColor4f(0.0f, bGlow * 0.8f, bGlow, 0.8f);
//     glLineWidth(1.5f);
//     glBegin(GL_LINE_LOOP);
//     glVertex3f(-hw, -hh, 0.02f);
//     glVertex3f( hw, -hh, 0.02f);
//     glVertex3f( hw,  hh, 0.02f);
//     glVertex3f(-hw,  hh, 0.02f);
//     glEnd();

//     /* Text rendering using raster pos trick (projected 3D->2D) */
//     /* We draw tiny scaled stroke text for the data lines */
//     float scroll = sc->scrollOffset;
//     float lineH  = 0.22f;
//     float textScale = 0.0007f;

//     for (int li = 0; li < sc->numLines; li++) {
//         float ty = hh - 0.25f - li * lineH + fmodf(scroll, sc->numLines * lineH);
//         if (ty < -hh || ty > hh) continue;

//         /* Alternating colors for visual interest */
//         if (li % 3 == 0)      glColor4f(0.0f, 1.0f, 1.0f, 0.9f);
//         else if (li % 3 == 1) glColor4f(0.2f, 0.8f, 0.4f, 0.85f);
//         else                  glColor4f(0.8f, 0.9f, 1.0f, 0.75f);

//         glPushMatrix();
//         glTranslatef(-hw + 0.08f, ty, 0.03f);
//         glScalef(textScale, textScale, textScale);
//         const char *s = sc->lines[li];
//         while (*s) { glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *s++); }
//         glPopMatrix();
//     }

//     glPopMatrix();
//     glEnable(GL_LIGHTING);
// }

// /* ============================================================
//  *  DRAW: Holographic Central Table
//  * ============================================================ */
// static void drawHoloTable(void) {
//     glDisable(GL_LIGHTING);

//     /* Table base - solid dark cylinder approximation using quads */
//     float tableR = 2.0f, tableY = -0.95f, tableTop = -0.5f;
//     int segs = 40;

//     /* Table side */
//     glColor4f(0.05f, 0.08f, 0.18f, 1.0f);
//     glBegin(GL_QUAD_STRIP);
//     for (int i = 0; i <= segs; i++) {
//         float a = TWO_PI * i / segs;
//         glVertex3f(tableR * cosf(a), tableY,   tableR * sinf(a));
//         glVertex3f(tableR * cosf(a), tableTop, tableR * sinf(a));
//     }
//     glEnd();

//     /* Table top surface */
//     float topGlow = 0.5f + 0.5f * sinf(gTime * 3.5f);
//     glColor4f(0.0f, 0.5f * topGlow, 0.9f * topGlow, 0.7f);
//     glBegin(GL_TRIANGLE_FAN);
//     glVertex3f(0.0f, tableTop, 0.0f);
//     for (int i = 0; i <= segs; i++) {
//         float a = TWO_PI * i / segs;
//         glVertex3f(tableR * cosf(a), tableTop, tableR * sinf(a));
//     }
//     glEnd();

//     /* Table top edge ring glow */
//     glColor4f(0.0f, topGlow, 1.0f, 0.9f);
//     glLineWidth(2.5f);
//     glBegin(GL_LINE_LOOP);
//     for (int i = 0; i < segs; i++) {
//         float a = TWO_PI * i / segs;
//         glVertex3f(tableR * cosf(a), tableTop + 0.01f, tableR * sinf(a));
//     }
//     glEnd();

//     /* Outer ring (decorative) */
//     glColor4f(0.0f, 0.6f * topGlow, 1.0f, 0.5f);
//     glLineWidth(1.0f);
//     glBegin(GL_LINE_LOOP);
//     for (int i = 0; i < segs; i++) {
//         float a = TWO_PI * i / segs;
//         glVertex3f((tableR + 0.3f) * cosf(a), tableTop, (tableR + 0.3f) * sinf(a));
//     }
//     glEnd();

//     /* ---- HOLOGRAM ABOVE TABLE ---- */

//     /* Update light0 (cyan from table) */
//     float lPulse = 0.6f + 0.4f * sinf(gTime * 4.5f);
//     GLfloat light0Pos[] = {0.0f, 1.5f, 0.0f, 1.0f};
//     GLfloat light0Diff[] = {0.0f, lPulse * 0.6f, lPulse * 1.0f, 1.0f};
//     glEnable(GL_LIGHTING);
//     glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
//     glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diff);
//     glDisable(GL_LIGHTING);

//     /* ---- Rotating Wireframe EARTH ---- */
//     glPushMatrix();
//     glTranslatef(-0.7f, 0.8f, 0.0f);
//     /* Earth: no spin rotation */
//     float earthAlpha = 0.6f + 0.4f * sinf(gTime * 2.5f);
//     drawWireframeSphere(0.45f, 12, 16, 0.0f, 0.8f, 1.0f, earthAlpha);

//     /* Earth axis ring */
//     glColor4f(0.0f, 1.0f, 1.0f, 0.3f);
//     glLineWidth(1.0f);
//     glBegin(GL_LINE_LOOP);
//     for (int i = 0; i < 36; i++) {
//         float a = TWO_PI * i / 36.0f;
//         glVertex3f(0.55f * cosf(a), 0.0f, 0.55f * sinf(a));
//     }
//     glEnd();
//     glPopMatrix();

//     /* ---- Rotating Wireframe MARS ---- */
//     glPushMatrix();
//     glTranslatef(0.9f, 0.8f, 0.3f);
//     /* Mars: no spin rotation */
//     float marsAlpha = 0.5f + 0.5f * sinf(gTime * 2.0f + 1.0f);
//     drawWireframeSphere(0.32f, 10, 14, 1.0f, 0.4f, 0.1f, marsAlpha);
//     glPopMatrix();

//     /* ---- TRAJECTORY LINE: Earth -> Mars ---- */
//     /* Bezier-style arc from Earth pos to Mars pos above table */
//     float ex = -0.7f, ey = 0.8f, ez = 0.0f;
//     float mx =  0.9f, my = 0.8f, mz = 0.3f;
//     float cx = 0.1f,  cy = 1.8f, cz = 0.15f;  /* control point (arc top) */

//     int steps = 60;
//     gTrajPulse  = 0.5f + 0.5f * sinf(gTime * 6.0f);
//     gTrajOffset = fmodf(gTime * 0.6f, 1.0f);

//     glLineWidth(2.5f);
//     glBegin(GL_LINE_STRIP);
//     for (int i = 0; i <= steps; i++) {
//         float t = (float)i / steps;
//         /* Quadratic Bezier */
//         float bx = (1-t)*(1-t)*ex + 2*(1-t)*t*cx + t*t*mx;
//         float by = (1-t)*(1-t)*ey + 2*(1-t)*t*cy + t*t*my;
//         float bz = (1-t)*(1-t)*ez + 2*(1-t)*t*cz + t*t*mz;

//         /* Pulse along trajectory */
//         float dist = fabsf(t - gTrajOffset);
//         if (dist > 0.5f) dist = 1.0f - dist;
//         float localBright = gTrajPulse * (0.4f + 1.2f * (1.0f - dist * 4.0f));
//         localBright = localBright < 0.2f ? 0.2f : localBright;
//         localBright = localBright > 1.0f ? 1.0f : localBright;

//         glColor4f(1.0f, 0.8f * localBright, 0.0f, localBright);
//         glVertex3f(bx, by, bz);
//     }
//     glEnd();

//     /* Trajectory HEAD sparkle dot */
//     float hx = (1-gTrajOffset)*(1-gTrajOffset)*ex + 2*(1-gTrajOffset)*gTrajOffset*cx + gTrajOffset*gTrajOffset*mx;
//     float hy = (1-gTrajOffset)*(1-gTrajOffset)*ey + 2*(1-gTrajOffset)*gTrajOffset*cy + gTrajOffset*gTrajOffset*my;
//     float hz = (1-gTrajOffset)*(1-gTrajOffset)*ez + 2*(1-gTrajOffset)*gTrajOffset*cz + gTrajOffset*gTrajOffset*mz;
//     glPointSize(8.0f * gTrajPulse);
//     glColor4f(1.0f, 0.9f, 0.2f, 1.0f);
//     glBegin(GL_POINTS);
//     glVertex3f(hx, hy, hz);
//     glEnd();

//     /* ---- Hologram base glow ring ---- */
//     float hGlow = 0.4f + 0.6f * sinf(gTime * 4.5f);
//     for (int ring = 0; ring < 3; ring++) {
//         float rr = 0.6f + ring * 0.4f;
//         float ra = hGlow * (1.0f - ring * 0.3f);
//         glColor4f(0.0f, ra * 0.8f, ra, ra * 0.5f);
//         glLineWidth(1.0f - ring * 0.2f);
//         glBegin(GL_LINE_LOOP);
//         for (int i = 0; i < 48; i++) {
//             float a = TWO_PI * i / 48.0f;
//             glVertex3f(rr * cosf(a), tableTop + 0.02f + ring * 0.02f, rr * sinf(a));
//         }
//         glEnd();
//     }

//     /* ---- "MOM TRAJECTORY" label floating above table ---- */
//     float labelGlow = 0.7f + 0.3f * sinf(gTime * 4.0f);
//     glColor4f(0.2f, labelGlow, 1.0f, labelGlow);
//     glPushMatrix();
//     glTranslatef(-1.1f, 1.5f, 0.0f);
//     glRotatef(-15.0f, 1, 0, 0);  /* tilt toward viewer */
//     float lscale = 0.0009f;
//     glScalef(lscale, lscale, lscale);
//     const char *label = "MOM TRAJECTORY: EARTH TO MARS";
//     while (*label) { glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *label++); }
//     glPopMatrix();

//     glEnable(GL_LIGHTING);
// }

// /* ============================================================
//  *  DRAW: A simple humanoid scientist
//  * ============================================================ */
// static void drawScientist(Scientist *sc) {
//     glPushMatrix();
//     glTranslatef(sc->x, -1.0f, sc->z);
//     glRotatef(sc->rotY, 0, 1, 0);
//     glScalef(sc->scale, sc->scale, sc->scale);

//     /* Breathing bob */
//     float breath = sinf(gTime * 3.0f + sc->breathPhase) * 0.05f;
//     glTranslatef(0.0f, breath, 0.0f);

//     /* Slight lean toward center */
//     glRotatef(sc->leanAngle + sinf(gTime * 1.2f + sc->breathPhase) * 2.5f, 0, 0, 1);

//     /* Disable lighting for cartoon flat look, we'll set colors manually */
//     glDisable(GL_LIGHTING);

//     /* ---- LEGS ---- */
//     glColor3f(0.15f, 0.2f, 0.35f);
//     /* Left leg */
//     glPushMatrix();
//     glTranslatef(-0.15f, 0.0f, 0.0f);
//     glScalef(0.25f, 0.7f, 0.25f);
//     glutSolidCube(1.0f);
//     glPopMatrix();
//     /* Right leg */
//     glPushMatrix();
//     glTranslatef(0.15f, 0.0f, 0.0f);
//     glScalef(0.25f, 0.7f, 0.25f);
//     glutSolidCube(1.0f);
//     glPopMatrix();

//     /* ---- BODY (lab coat: white) ---- */
//     glColor3f(0.88f, 0.90f, 0.95f);
//     glPushMatrix();
//     glTranslatef(0.0f, 0.7f, 0.0f);
//     glScalef(0.55f, 0.75f, 0.35f);
//     glutSolidCube(1.0f);
//     glPopMatrix();

//     /* Lab coat blue/colored inner (shirt visible) */
//     if (sc->isFemale) {
//         glColor3f(0.7f, 0.3f, 0.5f);  /* pink */
//     } else {
//         glColor3f(0.2f, 0.4f, 0.7f);  /* blue */
//     }
//     glPushMatrix();
//     glTranslatef(0.0f, 0.65f, 0.18f);
//     glScalef(0.2f, 0.5f, 0.05f);
//     glutSolidCube(1.0f);
//     glPopMatrix();

//     /* ---- ARMS ---- */
//     glColor3f(0.88f, 0.90f, 0.95f);
//     /* Left arm - slightly forward (holding tablet) */
//     glPushMatrix();
//     float armSwing = sinf(gTime * 2.0f + sc->breathPhase) * 5.0f;
//     glTranslatef(-0.38f, 0.65f, 0.0f);
//     glRotatef(armSwing - 20.0f, 1, 0, 0);
//     glScalef(0.2f, 0.65f, 0.2f);
//     glutSolidCube(1.0f);
//     glPopMatrix();

//     /* Right arm */
//     glPushMatrix();
//     glTranslatef(0.38f, 0.65f, 0.0f);
//     glRotatef(-armSwing + 10.0f, 1, 0, 0);
//     glScalef(0.2f, 0.65f, 0.2f);
//     glutSolidCube(1.0f);
//     glPopMatrix();

//     /* ---- NECK ---- */
//     glColor3f(0.75f, 0.6f, 0.5f);  /* skin tone */
//     glPushMatrix();
//     glTranslatef(0.0f, 1.1f, 0.0f);
//     glScalef(0.18f, 0.2f, 0.18f);
//     glutSolidCube(1.0f);
//     glPopMatrix();

//     /* ---- HEAD ---- */
//     /* Slight head tilt for discussion look */
//     glPushMatrix();
//     float headTilt = sinf(gTime * 1.5f + sc->breathPhase + 1.0f) * 7.0f;
//     glTranslatef(0.0f, 1.35f, 0.0f);
//     glRotatef(headTilt, 0, 1, 0);
//     glColor3f(0.75f, 0.6f, 0.5f);
//     glScalef(0.4f, 0.45f, 0.38f);
//     glutSolidSphere(0.5f, 12, 8);

//     /* Hair */
//     if (sc->isFemale) {
//         glColor3f(0.15f, 0.1f, 0.08f);
//         glScalef(1.1f, 0.6f, 1.2f);
//         glTranslatef(0.0f, 0.5f, -0.2f);
//         glutSolidSphere(0.5f, 8, 6);
//     } else {
//         glColor3f(0.3f, 0.25f, 0.2f);
//         glScalef(1.05f, 0.4f, 1.05f);
//         glTranslatef(0.0f, 0.8f, 0.0f);
//         glutSolidSphere(0.5f, 8, 6);
//     }
//     glPopMatrix();

//     /* ---- ID BADGE (tiny) ---- */
//     glColor3f(0.8f, 0.8f, 0.1f);
//     glPushMatrix();
//     glTranslatef(-0.05f, 0.85f, 0.19f);
//     glScalef(0.08f, 0.1f, 0.02f);
//     glutSolidCube(1.0f);
//     glPopMatrix();

//     glEnable(GL_LIGHTING);
//     glPopMatrix();
// }

// /* ============================================================
//  *  DRAW: Particle system (data dust)
//  * ============================================================ */
// static void drawParticles(void) {
//     glDisable(GL_LIGHTING);
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE);  /* additive for glow */
//     glDepthMask(GL_FALSE);

//     for (int i = 0; i < NUM_PARTICLES; i++) {
//         Particle *p = &gParticles[i];
//         float alpha = (p->life / p->maxLife);
//         if (alpha > 0.5f) alpha = 1.0f - alpha;
//         alpha *= 2.0f;  /* fade in/out */

//         glPointSize(p->size * alpha);
//         glColor4f(p->r, p->g, p->b, alpha * 0.8f);
//         glBegin(GL_POINTS);
//         glVertex3f(p->x, p->y, p->z);
//         glEnd();
//     }

//     glDepthMask(GL_TRUE);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//     glEnable(GL_LIGHTING);
// }

// /* ============================================================
//  *  DRAW: Main large screen (back wall center top)
//  * ============================================================ */
// static void drawMainScreen(void) {
//     glDisable(GL_LIGHTING);
//     glPushMatrix();
//     glTranslatef(0.0f, 4.8f, -7.7f);

//     float sw = 7.0f, sh = 3.0f;
//     float hw = sw * 0.5f, hh = sh * 0.5f;

//     /* Bezel */
//     glColor4f(0.03f, 0.05f, 0.12f, 1.0f);
//     glBegin(GL_QUADS);
//     glVertex3f(-hw - 0.15f, -hh - 0.15f, 0.0f);
//     glVertex3f( hw + 0.15f, -hh - 0.15f, 0.0f);
//     glVertex3f( hw + 0.15f,  hh + 0.15f, 0.0f);
//     glVertex3f(-hw - 0.15f,  hh + 0.15f, 0.0f);
//     glEnd();

//     /* Screen bg - deep space gradient effect */
//     glBegin(GL_QUADS);
//     glColor4f(0.01f, 0.03f, 0.10f, 1.0f);
//     glVertex3f(-hw, -hh, 0.01f);
//     glVertex3f( hw, -hh, 0.01f);
//     glColor4f(0.02f, 0.06f, 0.18f, 1.0f);
//     glVertex3f( hw,  hh, 0.01f);
//     glVertex3f(-hw,  hh, 0.01f);
//     glEnd();

//     /* Stars (static dots) */
//     glPointSize(1.5f);
//     glColor4f(0.8f, 0.9f, 1.0f, 0.6f);
//     glBegin(GL_POINTS);
//     /* Use deterministic "stars" */
//     srand(42);
//     for (int s = 0; s < 120; s++) {
//         float sx = -hw + (float)(rand() % 1000) / 1000.0f * sw;
//         float sy = -hh + (float)(rand() % 1000) / 1000.0f * sh;
//         glVertex3f(sx, sy, 0.02f);
//     }
//     srand((unsigned int)time(NULL));
//     glEnd();

//     /* Large Earth on screen */
//     glPushMatrix();
//     glTranslatef(-1.5f, 0.0f, 0.03f);
//     /* Earth on screen: no spin */
//     drawWireframeSphere(0.7f, 14, 18, 0.0f, 0.7f, 1.0f, 0.7f);
//     glPopMatrix();

//     /* Large Mars on screen */
//     glPushMatrix();
//     glTranslatef(2.2f, 0.1f, 0.03f);
//     /* Mars on screen: no spin */
//     drawWireframeSphere(0.5f, 12, 16, 1.0f, 0.35f, 0.1f, 0.7f);
//     glPopMatrix();

//     /* Trajectory arc on main screen */
//     float sEx=-1.5f, sEy=0.0f;
//     float sMx=2.2f,  sMy=0.1f;
//     float sCx=0.35f, sCy=1.1f;
//     float pulse = gTrajPulse;

//     glLineWidth(2.0f);
//     glBegin(GL_LINE_STRIP);
//     for (int i = 0; i <= 50; i++) {
//         float t = (float)i / 50.0f;
//         float bx = (1-t)*(1-t)*sEx + 2*(1-t)*t*sCx + t*t*sMx;
//         float by = (1-t)*(1-t)*sEy + 2*(1-t)*t*sCy + t*t*sMy;
//         float dist = fabsf(t - gTrajOffset);
//         if (dist > 0.5f) dist = 1.0f - dist;
//         float lb = pulse * (0.3f + 1.5f * (1.0f - dist * 5.0f));
//         lb = lb < 0.15f ? 0.15f : (lb > 1.0f ? 1.0f : lb);
//         glColor4f(1.0f, 0.7f * lb, 0.0f, lb);
//         glVertex3f(bx, by, 0.05f);
//     }
//     glEnd();

//     /* Title text */
//     float tGlow = 0.8f + 0.2f * sinf(gTime * 3.5f);
//     glColor4f(0.3f, tGlow, 1.0f, tGlow);
//     glPushMatrix();
//     glTranslatef(-2.6f, hh - 0.45f, 0.06f);
//     float tscale = 0.0012f;
//     glScalef(tscale, tscale, tscale);
//     const char *title = "MOM TRAJECTORY: EARTH TO MARS";
//     while (*title) { glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *title++); }
//     glPopMatrix();

//     /* ISRO label */
//     glColor4f(1.0f, 0.8f, 0.0f, 0.9f);
//     glPushMatrix();
//     glTranslatef(-2.6f, -hh + 0.15f, 0.06f);
//     float iscale = 0.0009f;
//     glScalef(iscale, iscale, iscale);
//     const char *isro = "INDIAN SPACE RESEARCH ORGANISATION  |  MISSION: MOM";
//     while (*isro) { glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *isro++); }
//     glPopMatrix();

//     /* Border */
//     float bG = 0.6f + 0.4f * sinf(gTime * 3.0f);
//     glColor4f(0.0f, bG, 1.0f, 0.9f);
//     glLineWidth(2.5f);
//     glBegin(GL_LINE_LOOP);
//     glVertex3f(-hw, -hh, 0.06f);
//     glVertex3f( hw, -hh, 0.06f);
//     glVertex3f( hw,  hh, 0.06f);
//     glVertex3f(-hw,  hh, 0.06f);
//     glEnd();

//     glPopMatrix();
//     glEnable(GL_LIGHTING);
// }

// /* ============================================================
//  *  DRAW: HUD overlay (2D)
//  * ============================================================ */
// static void drawHUD(void) {
//     /* Switch to 2D orthographic */
//     glMatrixMode(GL_PROJECTION);
//     glPushMatrix();
//     glLoadIdentity();
//     gluOrtho2D(0.0, WIN_W, 0.0, WIN_H);
//     glMatrixMode(GL_MODELVIEW);
//     glPushMatrix();
//     glLoadIdentity();
//     glDisable(GL_DEPTH_TEST);
//     glDisable(GL_LIGHTING);

//     /* Corner glow bars */
//     float hGlow = 0.5f + 0.5f * sinf(gTime * 1.2f);

//     /* Top bar */
//     glColor4f(0.0f, hGlow * 0.5f, hGlow, 0.4f);
//     glBegin(GL_QUADS);
//     glVertex2f(0.0f, WIN_H - 2.0f);
//     glVertex2f(WIN_W, WIN_H - 2.0f);
//     glVertex2f(WIN_W, WIN_H);
//     glVertex2f(0.0f, WIN_H);
//     glEnd();

//     /* Bottom bar */
//     glBegin(GL_QUADS);
//     glVertex2f(0.0f, 0.0f);
//     glVertex2f(WIN_W, 0.0f);
//     glVertex2f(WIN_W, 2.0f);
//     glVertex2f(0.0f, 2.0f);
//     glEnd();

//     /* Mission status text */
//     glColor4f(0.0f, 1.0f, 1.0f, 0.9f);
//     drawString2D(10.0f, WIN_H - 20.0f,
//                  "ISRO MISSION CONTROL  |  MOM  |  TEAM FORMATION STAGE",
//                  GLUT_BITMAP_HELVETICA_12);

//     /* Time display */
//     char timeBuf[64];
//     int elapsed = (int)gTime;
//     snprintf(timeBuf, sizeof(timeBuf),
//              "T+%02d:%02d:%02d  |  MISSION: ACTIVE  |  STATUS: BRIEFING",
//              elapsed / 3600, (elapsed % 3600) / 60, elapsed % 60);
//     glColor4f(0.3f, 1.0f, 0.5f, 0.9f);
//     drawString2D(10.0f, 10.0f, timeBuf, GLUT_BITMAP_HELVETICA_12);

//     /* Scanning corner marks */
//     glColor4f(0.0f, hGlow, 1.0f, 0.7f);
//     glLineWidth(2.0f);
//     float cs = 25.0f;  /* corner size */
//     /* Top-left */
//     glBegin(GL_LINES);
//     glVertex2f(5, WIN_H - 5);  glVertex2f(5 + cs, WIN_H - 5);
//     glVertex2f(5, WIN_H - 5);  glVertex2f(5, WIN_H - 5 - cs);
//     /* Top-right */
//     glVertex2f(WIN_W - 5, WIN_H - 5);  glVertex2f(WIN_W - 5 - cs, WIN_H - 5);
//     glVertex2f(WIN_W - 5, WIN_H - 5);  glVertex2f(WIN_W - 5, WIN_H - 5 - cs);
//     /* Bot-left */
//     glVertex2f(5, 5);  glVertex2f(5 + cs, 5);
//     glVertex2f(5, 5);  glVertex2f(5, 5 + cs);
//     /* Bot-right */
//     glVertex2f(WIN_W - 5, 5);  glVertex2f(WIN_W - 5 - cs, 5);
//     glVertex2f(WIN_W - 5, 5);  glVertex2f(WIN_W - 5, 5 + cs);
//     glEnd();

//     glEnable(GL_DEPTH_TEST);
//     glEnable(GL_LIGHTING);
//     glMatrixMode(GL_PROJECTION);
//     glPopMatrix();
//     glMatrixMode(GL_MODELVIEW);
//     glPopMatrix();
// }

// /* ============================================================
//  *  UPDATE: Animate particles
//  * ============================================================ */
// static void updateParticles(float dt) {
//     for (int i = 0; i < NUM_PARTICLES; i++) {
//         Particle *p = &gParticles[i];
//         p->life += dt;
//         if (p->life > p->maxLife) {
//             initParticle(p);
//             p->life = 0.0f;
//         }
//         p->x += p->vx;
//         p->y += p->vy;
//         p->z += p->vz;
//         /* Wrap vertically */
//         if (p->y > 6.5f) p->y = -1.0f;
//     }
// }

// /* ============================================================
//  *  UPDATE: Scroll screen text
//  * ============================================================ */
// static void updateScreens(float dt) {
//     for (int i = 0; i < NUM_SCREENS; i++) {
//         gScreens[i].scrollOffset += dt * 0.36f;
//         float maxScroll = gScreens[i].numLines * 0.22f;
//         if (gScreens[i].scrollOffset > maxScroll)
//             gScreens[i].scrollOffset -= maxScroll;
//     }
// }

// /* ============================================================
//  *  DISPLAY CALLBACK
//  * ============================================================ */
// static void display(void) {
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     /* ---- Camera setup ---- */
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluPerspective(55.0, (double)WIN_W / (double)WIN_H, 0.1, 100.0);

//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();

//     /* Fixed front camera (no rotation) */
//     gluLookAt(0.0, gCameraHeight, gCameraZoom,  /* eye: straight front */
//               0.0,  0.5,  0.0,                  /* center (table area) */
//               0.0,  1.0,  0.0);                 /* up */

//     /* ---- Draw scene elements ---- */
//     drawWalls();
//     drawCeiling();
//     drawFloor();
//     drawMainScreen();

//     /* Data screens on walls */
//     for (int i = 0; i < NUM_SCREENS; i++)
//         drawScreen(&gScreens[i]);

//     /* Central holographic table */
//     drawHoloTable();

//     /* Scientists */
//     for (int i = 0; i < NUM_SCIENTISTS; i++)
//         drawScientist(&gSci[i]);

//     /* Particles */
//     drawParticles();

//     /* 2D HUD overlay */
//     drawHUD();

//     glutSwapBuffers();
// }

// /* ============================================================
//  *  TIMER CALLBACK: Animation update
//  * ============================================================ */
// static void timer(int value) {
//     float dt = TIMER_MS / 1000.0f;
//     gTime += dt;

//     /* Camera is FIXED (no orbit rotation) */
//     /* Gentle zoom pulse only */
//     gCameraZoom = 12.5f + 0.5f * sinf(gTime * 0.4f);
//     gCameraHeight = 5.0f + 0.3f * sinf(gTime * 0.25f);

//     /* Slowly move scientists slightly inward (team forming) */
//     static float convergeFactor = 0.0f;
//     if (convergeFactor < 0.12f) convergeFactor += dt * 0.003f;
//     for (int i = 0; i < NUM_SCIENTISTS; i++) {
//         /* Nudge toward center each frame (very slowly) */
//         gSci[i].x *= (1.0f - convergeFactor * dt * 0.5f);
//         gSci[i].z *= (1.0f - convergeFactor * dt * 0.5f);
//     }

//     updateParticles(dt);
//     updateScreens(dt);

//     glutPostRedisplay();
//     glutTimerFunc(TIMER_MS, timer, 0);
// }

// /* ============================================================
//  *  RESHAPE CALLBACK
//  * ============================================================ */
// static void reshape(int w, int h) {
//     if (h == 0) h = 1;
//     glViewport(0, 0, w, h);
// }

// /* ============================================================
//  *  KEYBOARD CALLBACK
//  * ============================================================ */
// static void keyboard(unsigned char key, int x, int y) {
//     (void)x; (void)y;
//     switch (key) {
//         case 27: exit(0); break;      /* ESC */
//         case 'r': case 'R':
//             gCameraAngle = 0.0f;
//             gCameraZoom  = 14.0f;
//             break;
//         case '+': gCameraZoom -= 0.5f; break;
//         case '-': gCameraZoom += 0.5f; break;
//     }
// }

// /* ============================================================
//  *  MAIN
//  * ============================================================ */
// int main(int argc, char **argv) {
//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//     glutInitWindowSize(WIN_W, WIN_H);
//     glutInitWindowPosition(100, 50);
//     glutCreateWindow("ISRO Mission Control - MOM: Earth to Mars");

//     init();

//     glutDisplayFunc(display);
//     glutReshapeFunc(reshape);
//     glutKeyboardFunc(keyboard);
//     glutTimerFunc(TIMER_MS, timer, 0);

//     printf("\n");
//     printf("=========================================\n");
//     printf("  ISRO Mission Control Animation\n");
//     printf("  MOM Trajectory: Earth to Mars\n");
//     printf("=========================================\n");
//     printf("  Controls:\n");
//     printf("    ESC  - Quit\n");
//     printf("    R    - Reset camera\n");
//     printf("    +/-  - Zoom in/out\n");
//     printf("=========================================\n\n");

//     glutMainLoop();
//     return 0;
// }








/*
 * ============================================================
 *  ISRO MISSION CONTROL - MOM TRAJECTORY: EARTH TO MARS
 *  OpenGL + GLUT Cinematic 2D/3D Hybrid Animation in C
 * ============================================================
 *
 * BUILD INSTRUCTIONS:
 *   Linux/Mac:
 *     gcc isro_mission_control.c -o isro_mission -lGL -lGLU -lglut -lm
 *   Windows (MinGW):
 *     gcc isro_mission_control.c -o isro_mission -lopengl32 -lglu32 -lfreeglut -lm
 *
 * Run: ./isro_mission
 *
 * Controls:
 *   ESC - Quit
 *   R   - Reset camera
 *   +/- - Zoom In/Out
 * ============================================================
 */

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================
 *  CONSTANTS & CONFIGURATION
 * ============================================================ */
#define WIN_W           1280
#define WIN_H           720
#define PI              3.14159265358979323846
#define TWO_PI          (2.0 * PI)
#define NUM_SCIENTISTS  22
#define NUM_PARTICLES   250
#define NUM_SCREENS     6
#define TIMER_MS        16   /* ~60 FPS */

/* ============================================================
 *  GLOBAL TIME & ANIMATION STATE
 * ============================================================ */
static float  gTime        = 0.0f;   /* global time in seconds */
static float  gCameraAngle = 0.0f;   /* slow orbit angle */
static float  gCameraZoom  = 14.0f;  /* distance from center */
static float  gCameraHeight= 5.5f;   /* eye height */

/* ============================================================
 *  SCROLLING TEXT PANELS
 * ============================================================ */
typedef struct {
    float x, y, z;          /* world position */
    float rotY;              /* panel rotation */
    float scrollOffset;      /* text scroll position */
    const char *lines[8];    /* text lines */
    int   numLines;
} Screen;

static Screen gScreens[NUM_SCREENS];

/* ============================================================
 *  SCIENTISTS
 * ============================================================ */
typedef struct {
    float x, z;             /* position on XZ plane */
    float rotY;             /* facing direction (degrees) */
    float scale;            /* slight size variation */
    float breathPhase;      /* kept for color variation only */
    int   isFemale;         /* silhouette variation */
    float leanAngle;        /* fixed lean angle (static) */
    int   poseType;         /* 0=standing, 1=arms-forward, 2=tablet-held */
} Scientist;

static Scientist gSci[NUM_SCIENTISTS];

/* ============================================================
 *  PARTICLE SYSTEM
 * ============================================================ */
typedef struct {
    float x, y, z;
    float vx, vy, vz;       /* velocity */
    float life;             /* 0..1 */
    float maxLife;
    float r, g, b;
    float size;
} Particle;

static Particle gParticles[NUM_PARTICLES];

/* ============================================================
 *  TRAJECTORY PULSE
 * ============================================================ */
static float gTrajPulse   = 0.0f;
static float gTrajOffset  = 0.0f;  /* animated head of trajectory */

/* ============================================================
 *  HELPER: Random float in [lo, hi]
 * ============================================================ */
static float randf(float lo, float hi) {
    return lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
}

/* ============================================================
 *  HELPER: Draw a string with GLUT bitmap fonts at 2D overlay
 * ============================================================ */
static void drawString2D(float x, float y, const char *str, void *font) {
    glRasterPos2f(x, y);
    while (*str) {
        glutBitmapCharacter(font, *str++);
    }
}

/* (drawString3D available if needed for future 3D labels) */

/* ============================================================
 *  HELPER: Wireframe sphere helper (latitude/longitude lines)
 * ============================================================ */
static void drawWireframeSphere(float radius, int latSegs, int lonSegs,
                                float r, float g, float b, float alpha) {
    glColor4f(r, g, b, alpha);
    glLineWidth(1.0f);

    /* Latitude rings */
    for (int i = 1; i < latSegs; i++) {
        float phi = PI * i / latSegs - PI / 2.0f;
        float y   = radius * sinf(phi);
        float cr  = radius * cosf(phi);
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j <= lonSegs; j++) {
            float theta = TWO_PI * j / lonSegs;
            glVertex3f(cr * cosf(theta), y, cr * sinf(theta));
        }
        glEnd();
    }

    /* Longitude lines */
    for (int j = 0; j < lonSegs; j++) {
        float theta = TWO_PI * j / lonSegs;
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= latSegs; i++) {
            float phi = PI * i / latSegs - PI / 2.0f;
            float x   = radius * cosf(phi) * cosf(theta);
            float yy  = radius * sinf(phi);
            float z   = radius * cosf(phi) * sinf(theta);
            glVertex3f(x, yy, z);
        }
        glEnd();
    }
}

/* ============================================================
 *  INIT: Scientists — 22 total placed across the room
 *   Group A (0-10):  Ring around central table (semi-circle)
 *   Group B (11-16): Side workstation consoles (left & right walls)
 *   Group C (17-21): Back wall standing at screens / rear cluster
 * ============================================================ */
static void initScientists(void) {
    int idx = 0;

    /* ---- GROUP A: 11 scientists around central table ---- */
    /* Full circle, skip front arc (viewer side) — spread -150 to +150 deg */
    int tableCount = 11;
    float ringR = 3.0f;
    for (int i = 0; i < tableCount; i++) {
        float t     = (float)i / (tableCount - 1);
        float angle = (-150.0f + 300.0f * t) * (float)PI / 180.0f;
        gSci[idx].x         = ringR * sinf(angle);
        gSci[idx].z         = ringR * cosf(angle);
        gSci[idx].rotY      = -angle * 180.0f / (float)PI;  /* face center */
        gSci[idx].scale     = 0.92f + (i % 3) * 0.06f;
        gSci[idx].breathPhase = (float)i * 0.57f;
        gSci[idx].isFemale  = (i % 3 == 0) ? 1 : 0;
        gSci[idx].leanAngle = (i % 2 == 0) ? -2.0f : 2.0f;
        gSci[idx].poseType  = i % 3;
        idx++;
    }

    /* ---- GROUP B: 6 scientists at left/right wall consoles ---- */
    /* Left side: 3 scientists facing the wall screens */
    float sideZ[3] = { -4.0f, -1.5f, 1.0f };
    for (int i = 0; i < 3; i++) {
        gSci[idx].x         = -7.5f;
        gSci[idx].z         = sideZ[i];
        gSci[idx].rotY      = 90.0f;    /* face left wall */
        gSci[idx].scale     = 0.90f + i * 0.05f;
        gSci[idx].breathPhase = idx * 0.4f;
        gSci[idx].isFemale  = (i == 1) ? 1 : 0;
        gSci[idx].leanAngle = 0.0f;
        gSci[idx].poseType  = 1;
        idx++;
    }
    /* Right side: 3 scientists facing the wall screens */
    float sideZR[3] = { -4.5f, -2.0f, 0.5f };
    for (int i = 0; i < 3; i++) {
        gSci[idx].x         = 7.5f;
        gSci[idx].z         = sideZR[i];
        gSci[idx].rotY      = -90.0f;   /* face right wall */
        gSci[idx].scale     = 0.91f + i * 0.04f;
        gSci[idx].breathPhase = idx * 0.35f;
        gSci[idx].isFemale  = (i == 0) ? 1 : 0;
        gSci[idx].leanAngle = 0.0f;
        gSci[idx].poseType  = 2;
        idx++;
    }

    /* ---- GROUP C: 5 scientists near back — front of back screens ---- */
    float backX[5] = { -5.5f, -2.8f, 0.0f, 2.8f, 5.5f };
    for (int i = 0; i < 5; i++) {
        gSci[idx].x         = backX[i];
        gSci[idx].z         = -6.0f;
        gSci[idx].rotY      = 0.0f;     /* face back wall (screens) */
        gSci[idx].scale     = 0.88f + i * 0.04f;
        gSci[idx].breathPhase = idx * 0.45f;
        gSci[idx].isFemale  = (i % 2 == 1) ? 1 : 0;
        gSci[idx].leanAngle = (i == 2) ? -3.0f : 1.5f;
        gSci[idx].poseType  = i % 3;
        idx++;
    }
}

/* ============================================================
 *  INIT: Particles (floating data dust)
 * ============================================================ */
static void initParticle(Particle *p) {
    p->x    = randf(-6.0f, 6.0f);
    p->y    = randf(-1.0f, 0.5f);
    p->z    = randf(-6.0f, 6.0f);
    p->vx   = randf(-0.005f, 0.005f);
    p->vy   = randf(0.015f, 0.035f);
    p->vz   = randf(-0.005f, 0.005f);
    p->maxLife = randf(3.0f, 8.0f);
    p->life = randf(0.0f, p->maxLife);
    /* Color: blue/cyan/white */
    int type = rand() % 3;
    if (type == 0)      { p->r=0.2f; p->g=0.6f; p->b=1.0f; }
    else if (type == 1) { p->r=0.0f; p->g=1.0f; p->b=1.0f; }
    else                { p->r=0.8f; p->g=0.9f; p->b=1.0f; }
    p->size = randf(2.0f, 5.0f);
}

static void initParticles(void) {
    for (int i = 0; i < NUM_PARTICLES; i++)
        initParticle(&gParticles[i]);
}

/* ============================================================
 *  INIT: Wall screens with scrolling text
 * ============================================================ */
static void initScreens(void) {
    /* Back wall screens */
    gScreens[0].x = -5.5f; gScreens[0].y = 2.5f; gScreens[0].z = -7.8f;
    gScreens[0].rotY = 0.0f; gScreens[0].scrollOffset = 0.0f;
    gScreens[0].numLines = 6;
    gScreens[0].lines[0] = "MISSION STATUS: INITIALIZING";
    gScreens[0].lines[1] = "TEAM SYNC IN PROGRESS...";
    gScreens[0].lines[2] = "TRAJECTORY CALC: 78%";
    gScreens[0].lines[3] = "FUEL LOAD: NOMINAL";
    gScreens[0].lines[4] = "COMMS UPLINK: ACTIVE";
    gScreens[0].lines[5] = "SYSTEM CHECK: OK";

    gScreens[1].x = 0.0f; gScreens[1].y = 3.5f; gScreens[1].z = -7.8f;
    gScreens[1].rotY = 0.0f; gScreens[1].scrollOffset = 1.5f;
    gScreens[1].numLines = 6;
    gScreens[1].lines[0] = "MOM TRAJECTORY: EARTH->MARS";
    gScreens[1].lines[1] = "DELTA-V: 2.65 KM/S";
    gScreens[1].lines[2] = "TRANSFER ORBIT: ELLIPTICAL";
    gScreens[1].lines[3] = "LAUNCH WINDOW: T-48 HRS";
    gScreens[1].lines[4] = "PAYLOAD MASS: 1337 KG";
    gScreens[1].lines[5] = "ISRO PSLV-C25 READY";

    gScreens[2].x = 5.5f; gScreens[2].y = 2.5f; gScreens[2].z = -7.8f;
    gScreens[2].rotY = 0.0f; gScreens[2].scrollOffset = 3.0f;
    gScreens[2].numLines = 5;
    gScreens[2].lines[0] = "TELEMETRY: NOMINAL";
    gScreens[2].lines[1] = "GROUND STATIONS: ONLINE";
    gScreens[2].lines[2] = "ORBIT INSERTION: CALC...";
    gScreens[2].lines[3] = "ANTENNA ALIGN: 99.3%";
    gScreens[2].lines[4] = "DATA RATE: 64 KBPS";

    /* Side wall screens */
    gScreens[3].x = -8.8f; gScreens[3].y = 2.5f; gScreens[3].z = -2.0f;
    gScreens[3].rotY = 90.0f; gScreens[3].scrollOffset = 0.8f;
    gScreens[3].numLines = 5;
    gScreens[3].lines[0] = "TEAM FORMATION COMPLETE";
    gScreens[3].lines[1] = "SCIENTISTS ASSIGNED: 11";
    gScreens[3].lines[2] = "ROLE: MISSION PLANNING";
    gScreens[3].lines[3] = "CLEARANCE: TOP SECRET";
    gScreens[3].lines[4] = "STATUS: BRIEFING";

    gScreens[4].x = 8.8f; gScreens[4].y = 2.5f; gScreens[4].z = -2.0f;
    gScreens[4].rotY = -90.0f; gScreens[4].scrollOffset = 2.0f;
    gScreens[4].numLines = 5;
    gScreens[4].lines[0] = "SUBSYSTEM POWER: 100%";
    gScreens[4].lines[1] = "THERMAL CONTROL: NOMINAL";
    gScreens[4].lines[2] = "ATTITUDE CONTROL: READY";
    gScreens[4].lines[3] = "PROPULSION: PRE-ARMED";
    gScreens[4].lines[4] = "OBC BOOT: COMPLETE";

    gScreens[5].x = -8.8f; gScreens[5].y = 2.5f; gScreens[5].z = 2.0f;
    gScreens[5].rotY = 90.0f; gScreens[5].scrollOffset = 4.0f;
    gScreens[5].numLines = 5;
    gScreens[5].lines[0] = "LAUNCH VEHICLE: PSLV-C25";
    gScreens[5].lines[1] = "COUNTDOWN: T-48:00:00";
    gScreens[5].lines[2] = "WEATHER: CLEAR";
    gScreens[5].lines[3] = "WIND: 12 KM/H NE";
    gScreens[5].lines[4] = "GO FOR LAUNCH";
}

/* ============================================================
 *  INIT: OpenGL state
 * ============================================================ */
static void init(void) {
    srand((unsigned int)time(NULL));

    glClearColor(0.02f, 0.03f, 0.08f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    /* Lighting setup */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);  /* Table hologram light (cyan) */
    glEnable(GL_LIGHT1);  /* Ambient fill light */
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    /* Ambient light (dim blue) */
    GLfloat ambLight[] = {0.05f, 0.07f, 0.15f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambLight);

    /* Light1: soft overhead fill */
    GLfloat light1Pos[] = {0.0f, 10.0f, 0.0f, 1.0f};
    GLfloat light1Diff[] = {0.1f, 0.15f, 0.25f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1Diff);

    initScientists();
    initParticles();
    initScreens();
}

/* ============================================================
 *  DRAW: Reflective Floor
 * ============================================================ */
static void drawFloor(void) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Dark tiled floor */
    float floorY = -1.0f;
    int tiles = 12;
    float tileSize = 1.5f;
    float half = tiles * tileSize * 0.5f;

    for (int i = 0; i < tiles; i++) {
        for (int j = 0; j < tiles; j++) {
            float fx = -half + i * tileSize;
            float fz = -half + j * tileSize;
            float bright = ((i + j) % 2 == 0) ? 0.06f : 0.04f;
            glColor4f(bright, bright * 1.5f, bright * 2.5f, 0.9f);
            glBegin(GL_QUADS);
            glVertex3f(fx,          floorY, fz);
            glVertex3f(fx+tileSize, floorY, fz);
            glVertex3f(fx+tileSize, floorY, fz+tileSize);
            glVertex3f(fx,          floorY, fz+tileSize);
            glEnd();
        }
    }

    /* Floor grid glow lines */
    glColor4f(0.0f, 0.5f, 1.0f, 0.12f);
    glLineWidth(0.5f);
    glBegin(GL_LINES);
    for (int i = 0; i <= tiles; i++) {
        float p = -half + i * tileSize;
        glVertex3f(p, floorY + 0.001f, -half);
        glVertex3f(p, floorY + 0.001f,  half);
        glVertex3f(-half, floorY + 0.001f, p);
        glVertex3f( half, floorY + 0.001f, p);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

/* ============================================================
 *  DRAW: Ceiling with soft panels
 * ============================================================ */
static void drawCeiling(void) {
    glDisable(GL_LIGHTING);
    float ceilY = 7.0f;

    /* Ceiling base */
    glColor4f(0.03f, 0.04f, 0.08f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, ceilY, -10.0f);
    glVertex3f( 10.0f, ceilY, -10.0f);
    glVertex3f( 10.0f, ceilY,  10.0f);
    glVertex3f(-10.0f, ceilY,  10.0f);
    glEnd();

    /* Ceiling light panels (soft rectangles) */
    float panelY = ceilY - 0.01f;
    float panelPositions[4][2] = {
        {-3.0f, -3.0f}, {3.0f, -3.0f},
        {-3.0f,  3.0f}, {3.0f,  3.0f}
    };
    float pulse = 0.75f + 0.25f * sinf(gTime * 2.5f);

    for (int i = 0; i < 4; i++) {
        float px = panelPositions[i][0];
        float pz = panelPositions[i][1];
        glColor4f(0.6f * pulse, 0.75f * pulse, 1.0f * pulse, 0.35f);
        glBegin(GL_QUADS);
        glVertex3f(px - 1.0f, panelY, pz - 0.3f);
        glVertex3f(px + 1.0f, panelY, pz - 0.3f);
        glVertex3f(px + 1.0f, panelY, pz + 0.3f);
        glVertex3f(px - 1.0f, panelY, pz + 0.3f);
        glEnd();

        /* Halo around panel */
        glColor4f(0.3f * pulse, 0.5f * pulse, 1.0f * pulse, 0.08f);
        glBegin(GL_QUADS);
        glVertex3f(px - 2.0f, panelY + 0.01f, pz - 1.0f);
        glVertex3f(px + 2.0f, panelY + 0.01f, pz - 1.0f);
        glVertex3f(px + 2.0f, panelY + 0.01f, pz + 1.0f);
        glVertex3f(px - 2.0f, panelY + 0.01f, pz + 1.0f);
        glEnd();
    }
    glEnable(GL_LIGHTING);
}

/* ============================================================
 *  DRAW: Walls
 * ============================================================ */
static void drawWalls(void) {
    glDisable(GL_LIGHTING);
    float wallColor[4] = {0.04f, 0.06f, 0.12f, 1.0f};
    glColor4fv(wallColor);

    /* Back wall */
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, -1.0f, -8.0f);
    glVertex3f( 10.0f, -1.0f, -8.0f);
    glVertex3f( 10.0f,  7.0f, -8.0f);
    glVertex3f(-10.0f,  7.0f, -8.0f);
    glEnd();

    /* Side walls */
    glBegin(GL_QUADS);
    glVertex3f(-9.0f, -1.0f, -8.0f);
    glVertex3f(-9.0f, -1.0f,  8.0f);
    glVertex3f(-9.0f,  7.0f,  8.0f);
    glVertex3f(-9.0f,  7.0f, -8.0f);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f( 9.0f, -1.0f,  8.0f);
    glVertex3f( 9.0f, -1.0f, -8.0f);
    glVertex3f( 9.0f,  7.0f, -8.0f);
    glVertex3f( 9.0f,  7.0f,  8.0f);
    glEnd();

    /* Ambient glow on back wall */
    float glow = 0.5f + 0.5f * sinf(gTime * 2.5f);
    glColor4f(0.0f, 0.3f * glow, 0.5f * glow, 0.15f);
    glBegin(GL_QUADS);
    glVertex3f(-6.0f, -1.0f, -7.95f);
    glVertex3f( 6.0f, -1.0f, -7.95f);
    glVertex3f( 6.0f,  7.0f, -7.95f);
    glVertex3f(-6.0f,  7.0f, -7.95f);
    glEnd();

    glEnable(GL_LIGHTING);
}

/* ============================================================
 *  DRAW: A single data screen panel with scrolling text
 * ============================================================ */
static void drawScreen(Screen *sc) {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(sc->x, sc->y, sc->z);
    glRotatef(sc->rotY, 0, 1, 0);

    float sw = 2.8f, sh = 2.0f;
    float hw = sw * 0.5f, hh = sh * 0.5f;

    /* Screen bezel */
    glColor4f(0.05f, 0.07f, 0.15f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-hw - 0.08f, -hh - 0.08f, 0.0f);
    glVertex3f( hw + 0.08f, -hh - 0.08f, 0.0f);
    glVertex3f( hw + 0.08f,  hh + 0.08f, 0.0f);
    glVertex3f(-hw - 0.08f,  hh + 0.08f, 0.0f);
    glEnd();

    /* Screen background */
    glColor4f(0.02f, 0.06f, 0.12f, 0.95f);
    glBegin(GL_QUADS);
    glVertex3f(-hw, -hh, 0.01f);
    glVertex3f( hw, -hh, 0.01f);
    glVertex3f( hw,  hh, 0.01f);
    glVertex3f(-hw,  hh, 0.01f);
    glEnd();

    /* Scan-line overlay (subtle) */
    glColor4f(0.0f, 0.5f, 1.0f, 0.05f);
    glLineWidth(0.5f);
    int scanLines = (int)(sh / 0.04f);
    glBegin(GL_LINES);
    for (int s = 0; s < scanLines; s++) {
        float sy = -hh + s * 0.04f;
        glVertex3f(-hw, sy, 0.02f);
        glVertex3f( hw, sy, 0.02f);
    }
    glEnd();

    /* Border glow */
    float bGlow = 0.6f + 0.4f * sinf(gTime * 1.2f + sc->x);
    glColor4f(0.0f, bGlow * 0.8f, bGlow, 0.8f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-hw, -hh, 0.02f);
    glVertex3f( hw, -hh, 0.02f);
    glVertex3f( hw,  hh, 0.02f);
    glVertex3f(-hw,  hh, 0.02f);
    glEnd();

    /* Text rendering — use larger stroke scale for clarity */
    float scroll   = sc->scrollOffset;
    float lineH    = 0.30f;               /* more vertical spacing */
    float textScale = 0.00110f;           /* 57% bigger than before */

    for (int li = 0; li < sc->numLines; li++) {
        float ty = hh - 0.32f - li * lineH + fmodf(scroll, sc->numLines * lineH);
        if (ty < -hh + 0.05f || ty > hh - 0.05f) continue;

        /* High-contrast bright colors */
        if (li % 3 == 0)      glColor4f(0.0f, 1.0f, 1.0f, 1.0f);   /* pure cyan */
        else if (li % 3 == 1) glColor4f(0.3f, 1.0f, 0.5f, 1.0f);   /* bright green */
        else                  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);   /* white */

        glLineWidth(1.8f);   /* thicker stroke lines for readability */
        glPushMatrix();
        glTranslatef(-hw + 0.10f, ty, 0.03f);
        glScalef(textScale, textScale, textScale);
        const char *s = sc->lines[li];
        while (*s) { glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *s++); }
        glPopMatrix();
    }

    glPopMatrix();
    glEnable(GL_LIGHTING);
}

/* ============================================================
 *  DRAW: Holographic Central Table
 * ============================================================ */
static void drawHoloTable(void) {
    glDisable(GL_LIGHTING);

    /* Table base - solid dark cylinder approximation using quads */
    float tableR = 2.0f, tableY = -0.95f, tableTop = -0.5f;
    int segs = 40;

    /* Table side */
    glColor4f(0.05f, 0.08f, 0.18f, 1.0f);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; i++) {
        float a = TWO_PI * i / segs;
        glVertex3f(tableR * cosf(a), tableY,   tableR * sinf(a));
        glVertex3f(tableR * cosf(a), tableTop, tableR * sinf(a));
    }
    glEnd();

    /* Table top surface */
    float topGlow = 0.5f + 0.5f * sinf(gTime * 3.5f);
    glColor4f(0.0f, 0.5f * topGlow, 0.9f * topGlow, 0.7f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, tableTop, 0.0f);
    for (int i = 0; i <= segs; i++) {
        float a = TWO_PI * i / segs;
        glVertex3f(tableR * cosf(a), tableTop, tableR * sinf(a));
    }
    glEnd();

    /* Table top edge ring glow */
    glColor4f(0.0f, topGlow, 1.0f, 0.9f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segs; i++) {
        float a = TWO_PI * i / segs;
        glVertex3f(tableR * cosf(a), tableTop + 0.01f, tableR * sinf(a));
    }
    glEnd();

    /* Outer ring (decorative) */
    glColor4f(0.0f, 0.6f * topGlow, 1.0f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segs; i++) {
        float a = TWO_PI * i / segs;
        glVertex3f((tableR + 0.3f) * cosf(a), tableTop, (tableR + 0.3f) * sinf(a));
    }
    glEnd();

    /* ---- HOLOGRAM ABOVE TABLE ---- */

    /* Update light0 (cyan from table) */
    float lPulse = 0.6f + 0.4f * sinf(gTime * 4.5f);
    GLfloat light0Pos[] = {0.0f, 1.5f, 0.0f, 1.0f};
    GLfloat light0Diff[] = {0.0f, lPulse * 0.6f, lPulse * 1.0f, 1.0f};
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diff);
    glDisable(GL_LIGHTING);

    /* ---- Rotating Wireframe EARTH ---- */
    glPushMatrix();
    glTranslatef(-0.7f, 0.8f, 0.0f);
    /* Earth: no spin rotation */
    float earthAlpha = 0.6f + 0.4f * sinf(gTime * 2.5f);
    drawWireframeSphere(0.45f, 12, 16, 0.0f, 0.8f, 1.0f, earthAlpha);

    /* Earth axis ring */
    glColor4f(0.0f, 1.0f, 1.0f, 0.3f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 36; i++) {
        float a = TWO_PI * i / 36.0f;
        glVertex3f(0.55f * cosf(a), 0.0f, 0.55f * sinf(a));
    }
    glEnd();
    glPopMatrix();

    /* ---- Rotating Wireframe MARS ---- */
    glPushMatrix();
    glTranslatef(0.9f, 0.8f, 0.3f);
    /* Mars: no spin rotation */
    float marsAlpha = 0.5f + 0.5f * sinf(gTime * 2.0f + 1.0f);
    drawWireframeSphere(0.32f, 10, 14, 1.0f, 0.4f, 0.1f, marsAlpha);
    glPopMatrix();

    /* ---- TRAJECTORY LINE: Earth -> Mars ---- */
    /* Bezier-style arc from Earth pos to Mars pos above table */
    float ex = -0.7f, ey = 0.8f, ez = 0.0f;
    float mx =  0.9f, my = 0.8f, mz = 0.3f;
    float cx = 0.1f,  cy = 1.8f, cz = 0.15f;  /* control point (arc top) */

    int steps = 60;
    gTrajPulse  = 0.5f + 0.5f * sinf(gTime * 6.0f);
    gTrajOffset = fmodf(gTime * 0.6f, 1.0f);

    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        /* Quadratic Bezier */
        float bx = (1-t)*(1-t)*ex + 2*(1-t)*t*cx + t*t*mx;
        float by = (1-t)*(1-t)*ey + 2*(1-t)*t*cy + t*t*my;
        float bz = (1-t)*(1-t)*ez + 2*(1-t)*t*cz + t*t*mz;

        /* Pulse along trajectory */
        float dist = fabsf(t - gTrajOffset);
        if (dist > 0.5f) dist = 1.0f - dist;
        float localBright = gTrajPulse * (0.4f + 1.2f * (1.0f - dist * 4.0f));
        localBright = localBright < 0.2f ? 0.2f : localBright;
        localBright = localBright > 1.0f ? 1.0f : localBright;

        glColor4f(1.0f, 0.8f * localBright, 0.0f, localBright);
        glVertex3f(bx, by, bz);
    }
    glEnd();

    /* Trajectory HEAD sparkle dot */
    float hx = (1-gTrajOffset)*(1-gTrajOffset)*ex + 2*(1-gTrajOffset)*gTrajOffset*cx + gTrajOffset*gTrajOffset*mx;
    float hy = (1-gTrajOffset)*(1-gTrajOffset)*ey + 2*(1-gTrajOffset)*gTrajOffset*cy + gTrajOffset*gTrajOffset*my;
    float hz = (1-gTrajOffset)*(1-gTrajOffset)*ez + 2*(1-gTrajOffset)*gTrajOffset*cz + gTrajOffset*gTrajOffset*mz;
    glPointSize(8.0f * gTrajPulse);
    glColor4f(1.0f, 0.9f, 0.2f, 1.0f);
    glBegin(GL_POINTS);
    glVertex3f(hx, hy, hz);
    glEnd();

    /* ---- Hologram base glow ring ---- */
    float hGlow = 0.4f + 0.6f * sinf(gTime * 4.5f);
    for (int ring = 0; ring < 3; ring++) {
        float rr = 0.6f + ring * 0.4f;
        float ra = hGlow * (1.0f - ring * 0.3f);
        glColor4f(0.0f, ra * 0.8f, ra, ra * 0.5f);
        glLineWidth(1.0f - ring * 0.2f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 48; i++) {
            float a = TWO_PI * i / 48.0f;
            glVertex3f(rr * cosf(a), tableTop + 0.02f + ring * 0.02f, rr * sinf(a));
        }
        glEnd();
    }

    /* ---- "MOM TRAJECTORY" label floating above table ---- */
    float labelGlow = 0.7f + 0.3f * sinf(gTime * 4.0f);
    glColor4f(0.1f, labelGlow, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glPushMatrix();
    glTranslatef(-1.35f, 1.65f, 0.0f);
    glRotatef(-18.0f, 1, 0, 0);  /* tilt toward viewer */
    float lscale = 0.00130f;     /* was 0.0009 — 44% larger */
    glScalef(lscale, lscale, lscale);
    const char *label = "MOM TRAJECTORY: EARTH TO MARS";
    while (*label) { glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *label++); }
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

/* ============================================================
 *  DRAW: A static humanoid scientist (no animation)
 *  poseType 0 = standing at attention
 *  poseType 1 = arms slightly forward (gesturing at screen)
 *  poseType 2 = one arm raised (holding tablet up)
 * ============================================================ */
static void drawScientist(Scientist *sc) {
    glPushMatrix();
    glTranslatef(sc->x, -1.0f, sc->z);
    glRotatef(sc->rotY, 0, 1, 0);
    glScalef(sc->scale, sc->scale, sc->scale);
    /* Static fixed lean — no animation */
    glRotatef(sc->leanAngle, 0, 0, 1);

    glDisable(GL_LIGHTING);

    /* ---- SHOES ---- */
    glColor3f(0.08f, 0.08f, 0.10f);
    glPushMatrix();
    glTranslatef(-0.14f, 0.03f, 0.05f);
    glScalef(0.22f, 0.10f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.14f, 0.03f, 0.05f);
    glScalef(0.22f, 0.10f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();

    /* ---- LEGS (trousers) ---- */
    glColor3f(0.12f, 0.16f, 0.28f);
    glPushMatrix();
    glTranslatef(-0.14f, 0.42f, 0.0f);
    glScalef(0.24f, 0.72f, 0.24f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef( 0.14f, 0.42f, 0.0f);
    glScalef(0.24f, 0.72f, 0.24f);
    glutSolidCube(1.0f);
    glPopMatrix();

    /* ---- BODY (white lab coat) ---- */
    glColor3f(0.90f, 0.92f, 0.96f);
    glPushMatrix();
    glTranslatef(0.0f, 0.92f, 0.0f);
    glScalef(0.58f, 0.80f, 0.34f);
    glutSolidCube(1.0f);
    glPopMatrix();

    /* Coat lapel lines */
    glColor3f(0.70f, 0.72f, 0.76f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex3f(-0.05f, 1.25f, 0.18f);  glVertex3f(-0.18f, 0.65f, 0.18f);
    glVertex3f( 0.05f, 1.25f, 0.18f);  glVertex3f( 0.18f, 0.65f, 0.18f);
    glEnd();

    /* Inner shirt / saree color */
    if (sc->isFemale) {
        glColor3f(0.72f, 0.28f, 0.48f);  /* magenta/pink saree hint */
    } else {
        glColor3f(0.22f, 0.38f, 0.68f);  /* blue shirt */
    }
    glPushMatrix();
    glTranslatef(0.0f, 0.88f, 0.175f);
    glScalef(0.22f, 0.52f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();

    /* ---- ARMS ---- */
    glColor3f(0.90f, 0.92f, 0.96f);  /* coat sleeves */
    if (sc->poseType == 0) {
        /* Standing at attention — arms straight down */
        glPushMatrix();
        glTranslatef(-0.37f, 0.82f, 0.0f);
        glRotatef(8.0f, 0, 0, 1);
        glScalef(0.20f, 0.68f, 0.20f);
        glutSolidCube(1.0f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef( 0.37f, 0.82f, 0.0f);
        glRotatef(-8.0f, 0, 0, 1);
        glScalef(0.20f, 0.68f, 0.20f);
        glutSolidCube(1.0f);
        glPopMatrix();
    } else if (sc->poseType == 1) {
        /* Gesturing forward — left arm extended toward screen */
        glPushMatrix();
        glTranslatef(-0.37f, 0.90f, 0.0f);
        glRotatef(-35.0f, 1, 0, 0);
        glScalef(0.20f, 0.68f, 0.20f);
        glutSolidCube(1.0f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef( 0.37f, 0.82f, 0.0f);
        glRotatef(-10.0f, 1, 0, 0);
        glScalef(0.20f, 0.68f, 0.20f);
        glutSolidCube(1.0f);
        glPopMatrix();
    } else {
        /* Tablet raised — right arm up at 45 deg */
        glPushMatrix();
        glTranslatef(-0.37f, 0.82f, 0.0f);
        glRotatef(-15.0f, 1, 0, 0);
        glScalef(0.20f, 0.68f, 0.20f);
        glutSolidCube(1.0f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef( 0.37f, 0.95f, 0.0f);
        glRotatef(-55.0f, 1, 0, 0);
        glScalef(0.20f, 0.68f, 0.20f);
        glutSolidCube(1.0f);
        glPopMatrix();
        /* Tablet in raised hand */
        glColor3f(0.10f, 0.15f, 0.25f);
        glPushMatrix();
        glTranslatef(0.37f, 1.40f, -0.32f);
        glRotatef(-55.0f, 1, 0, 0);
        glScalef(0.28f, 0.04f, 0.20f);
        glutSolidCube(1.0f);
        glPopMatrix();
        /* Tablet screen glow */
        glColor3f(0.0f, 0.7f, 1.0f);
        glPushMatrix();
        glTranslatef(0.37f, 1.40f, -0.31f);
        glRotatef(-55.0f, 1, 0, 0);
        glScalef(0.24f, 0.02f, 0.16f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    /* ---- NECK ---- */
    glColor3f(0.78f, 0.62f, 0.52f);
    glPushMatrix();
    glTranslatef(0.0f, 1.36f, 0.0f);
    glScalef(0.17f, 0.22f, 0.17f);
    glutSolidCube(1.0f);
    glPopMatrix();

    /* ---- HEAD ---- */
    glPushMatrix();
    glTranslatef(0.0f, 1.62f, 0.0f);
    /* Skin — vary tone slightly by index */
    float skinR = 0.74f + (sc->breathPhase * 0.03f);
    float skinG = 0.58f + (sc->breathPhase * 0.02f);
    float skinB = 0.48f;
    glColor3f(skinR, skinG, skinB);
    glScalef(0.42f, 0.48f, 0.40f);
    glutSolidSphere(0.5f, 14, 10);

    /* Hair */
    if (sc->isFemale) {
        /* Long hair bun */
        glColor3f(0.12f, 0.08f, 0.06f);
        glPushMatrix();
        glScalef(1.08f, 0.55f, 1.12f);
        glTranslatef(0.0f, 0.62f, -0.18f);
        glutSolidSphere(0.5f, 10, 8);
        glPopMatrix();
        /* Bun knot */
        glPushMatrix();
        glScalef(0.5f, 0.5f, 0.5f);
        glTranslatef(0.0f, 1.5f, -0.5f);
        glutSolidSphere(0.5f, 8, 6);
        glPopMatrix();
    } else {
        /* Short hair cap */
        glColor3f(0.22f, 0.18f, 0.14f);
        glPushMatrix();
        glScalef(1.06f, 0.38f, 1.06f);
        glTranslatef(0.0f, 0.95f, 0.0f);
        glutSolidSphere(0.5f, 10, 8);
        glPopMatrix();
    }
    glPopMatrix(); /* end head */

    /* ---- ID BADGE (ISRO yellow) ---- */
    glColor3f(0.85f, 0.78f, 0.05f);
    glPushMatrix();
    glTranslatef(-0.06f, 1.02f, 0.20f);
    glScalef(0.09f, 0.12f, 0.02f);
    glutSolidCube(1.0f);
    glPopMatrix();
    /* Badge lanyard line */
    glColor3f(0.6f, 0.5f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.06f, 1.14f, 0.19f);
    glVertex3f(-0.06f, 1.36f, 0.18f);
    glEnd();

    glEnable(GL_LIGHTING);
    glPopMatrix();
}

/* ============================================================
 *  DRAW: Particle system (data dust)
 * ============================================================ */
static void drawParticles(void) {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);  /* additive for glow */
    glDepthMask(GL_FALSE);

    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle *p = &gParticles[i];
        float alpha = (p->life / p->maxLife);
        if (alpha > 0.5f) alpha = 1.0f - alpha;
        alpha *= 2.0f;  /* fade in/out */

        glPointSize(p->size * alpha);
        glColor4f(p->r, p->g, p->b, alpha * 0.8f);
        glBegin(GL_POINTS);
        glVertex3f(p->x, p->y, p->z);
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
}

/* ============================================================
 *  DRAW: Main large screen (back wall center top)
 * ============================================================ */
static void drawMainScreen(void) {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(0.0f, 4.8f, -7.7f);

    float sw = 7.0f, sh = 3.0f;
    float hw = sw * 0.5f, hh = sh * 0.5f;

    /* Bezel */
    glColor4f(0.03f, 0.05f, 0.12f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-hw - 0.15f, -hh - 0.15f, 0.0f);
    glVertex3f( hw + 0.15f, -hh - 0.15f, 0.0f);
    glVertex3f( hw + 0.15f,  hh + 0.15f, 0.0f);
    glVertex3f(-hw - 0.15f,  hh + 0.15f, 0.0f);
    glEnd();

    /* Screen bg - deep space gradient effect */
    glBegin(GL_QUADS);
    glColor4f(0.01f, 0.03f, 0.10f, 1.0f);
    glVertex3f(-hw, -hh, 0.01f);
    glVertex3f( hw, -hh, 0.01f);
    glColor4f(0.02f, 0.06f, 0.18f, 1.0f);
    glVertex3f( hw,  hh, 0.01f);
    glVertex3f(-hw,  hh, 0.01f);
    glEnd();

    /* Stars (static dots) */
    glPointSize(1.5f);
    glColor4f(0.8f, 0.9f, 1.0f, 0.6f);
    glBegin(GL_POINTS);
    /* Use deterministic "stars" */
    srand(42);
    for (int s = 0; s < 120; s++) {
        float sx = -hw + (float)(rand() % 1000) / 1000.0f * sw;
        float sy = -hh + (float)(rand() % 1000) / 1000.0f * sh;
        glVertex3f(sx, sy, 0.02f);
    }
    srand((unsigned int)time(NULL));
    glEnd();

    /* Large Earth on screen */
    glPushMatrix();
    glTranslatef(-1.5f, 0.0f, 0.03f);
    /* Earth on screen: no spin */
    drawWireframeSphere(0.7f, 14, 18, 0.0f, 0.7f, 1.0f, 0.7f);
    glPopMatrix();

    /* Large Mars on screen */
    glPushMatrix();
    glTranslatef(2.2f, 0.1f, 0.03f);
    /* Mars on screen: no spin */
    drawWireframeSphere(0.5f, 12, 16, 1.0f, 0.35f, 0.1f, 0.7f);
    glPopMatrix();

    /* Trajectory arc on main screen */
    float sEx=-1.5f, sEy=0.0f;
    float sMx=2.2f,  sMy=0.1f;
    float sCx=0.35f, sCy=1.1f;
    float pulse = gTrajPulse;

    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 50; i++) {
        float t = (float)i / 50.0f;
        float bx = (1-t)*(1-t)*sEx + 2*(1-t)*t*sCx + t*t*sMx;
        float by = (1-t)*(1-t)*sEy + 2*(1-t)*t*sCy + t*t*sMy;
        float dist = fabsf(t - gTrajOffset);
        if (dist > 0.5f) dist = 1.0f - dist;
        float lb = pulse * (0.3f + 1.5f * (1.0f - dist * 5.0f));
        lb = lb < 0.15f ? 0.15f : (lb > 1.0f ? 1.0f : lb);
        glColor4f(1.0f, 0.7f * lb, 0.0f, lb);
        glVertex3f(bx, by, 0.05f);
    }
    glEnd();

    /* Title text — large, bold, always readable */
    float tGlow = 0.8f + 0.2f * sinf(gTime * 3.5f);
    glColor4f(0.2f, tGlow, 1.0f, 1.0f);
    glLineWidth(2.5f);
    glPushMatrix();
    glTranslatef(-3.1f, hh - 0.52f, 0.06f);
    float tscale = 0.00175f;   /* was 0.0012 — 46% larger */
    glScalef(tscale, tscale, tscale);
    const char *title = "MOM TRAJECTORY: EARTH TO MARS";
    while (*title) { glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *title++); }
    glPopMatrix();

    /* ISRO bottom label — larger and fully opaque */
    glColor4f(1.0f, 0.85f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    glPushMatrix();
    glTranslatef(-3.1f, -hh + 0.18f, 0.06f);
    float iscale = 0.00130f;   /* was 0.0009 — 44% larger */
    glScalef(iscale, iscale, iscale);
    const char *isro = "INDIAN SPACE RESEARCH ORGANISATION  |  MISSION: MOM";
    while (*isro) { glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *isro++); }
    glPopMatrix();

    /* Border */
    float bG = 0.6f + 0.4f * sinf(gTime * 3.0f);
    glColor4f(0.0f, bG, 1.0f, 0.9f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-hw, -hh, 0.06f);
    glVertex3f( hw, -hh, 0.06f);
    glVertex3f( hw,  hh, 0.06f);
    glVertex3f(-hw,  hh, 0.06f);
    glEnd();

    glPopMatrix();
    glEnable(GL_LIGHTING);
}

/* ============================================================
 *  DRAW: HUD overlay (2D)
 * ============================================================ */
static void drawHUD(void) {
    /* Switch to 2D orthographic */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, WIN_W, 0.0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    /* Corner glow bars */
    float hGlow = 0.5f + 0.5f * sinf(gTime * 1.2f);

    /* Top bar */
    glColor4f(0.0f, hGlow * 0.5f, hGlow, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, WIN_H - 2.0f);
    glVertex2f(WIN_W, WIN_H - 2.0f);
    glVertex2f(WIN_W, WIN_H);
    glVertex2f(0.0f, WIN_H);
    glEnd();

    /* Bottom bar */
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(WIN_W, 0.0f);
    glVertex2f(WIN_W, 2.0f);
    glVertex2f(0.0f, 2.0f);
    glEnd();

    /* Mission status text — large bright font */
    glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
    drawString2D(12.0f, WIN_H - 22.0f,
                 "ISRO MISSION CONTROL  |  MOM  |  TEAM FORMATION STAGE",
                 GLUT_BITMAP_HELVETICA_18);

    /* Time display — bright green, larger */
    char timeBuf[64];
    int elapsed = (int)gTime;
    snprintf(timeBuf, sizeof(timeBuf),
             "T+%02d:%02d:%02d  |  MISSION: ACTIVE  |  STATUS: BRIEFING IN PROGRESS",
             elapsed / 3600, (elapsed % 3600) / 60, elapsed % 60);
    glColor4f(0.2f, 1.0f, 0.4f, 1.0f);
    drawString2D(12.0f, 12.0f, timeBuf, GLUT_BITMAP_HELVETICA_18);

    /* Scanning corner marks */
    glColor4f(0.0f, hGlow, 1.0f, 0.7f);
    glLineWidth(2.0f);
    float cs = 25.0f;  /* corner size */
    /* Top-left */
    glBegin(GL_LINES);
    glVertex2f(5, WIN_H - 5);  glVertex2f(5 + cs, WIN_H - 5);
    glVertex2f(5, WIN_H - 5);  glVertex2f(5, WIN_H - 5 - cs);
    /* Top-right */
    glVertex2f(WIN_W - 5, WIN_H - 5);  glVertex2f(WIN_W - 5 - cs, WIN_H - 5);
    glVertex2f(WIN_W - 5, WIN_H - 5);  glVertex2f(WIN_W - 5, WIN_H - 5 - cs);
    /* Bot-left */
    glVertex2f(5, 5);  glVertex2f(5 + cs, 5);
    glVertex2f(5, 5);  glVertex2f(5, 5 + cs);
    /* Bot-right */
    glVertex2f(WIN_W - 5, 5);  glVertex2f(WIN_W - 5 - cs, 5);
    glVertex2f(WIN_W - 5, 5);  glVertex2f(WIN_W - 5, 5 + cs);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/* ============================================================
 *  UPDATE: Animate particles
 * ============================================================ */
static void updateParticles(float dt) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle *p = &gParticles[i];
        p->life += dt;
        if (p->life > p->maxLife) {
            initParticle(p);
            p->life = 0.0f;
        }
        p->x += p->vx;
        p->y += p->vy;
        p->z += p->vz;
        /* Wrap vertically */
        if (p->y > 6.5f) p->y = -1.0f;
    }
}

/* ============================================================
 *  UPDATE: Scroll screen text
 * ============================================================ */
static void updateScreens(float dt) {
    for (int i = 0; i < NUM_SCREENS; i++) {
        gScreens[i].scrollOffset += dt * 0.36f;
        float maxScroll = gScreens[i].numLines * 0.22f;
        if (gScreens[i].scrollOffset > maxScroll)
            gScreens[i].scrollOffset -= maxScroll;
    }
}

/* ============================================================
 *  DRAW: Workstation desks for side and back scientists
 * ============================================================ */
static void drawWorkstations(void) {
    glDisable(GL_LIGHTING);

    /* Helper to draw one desk at (wx, wy, wz) rotated by rotY */
    /* deskW = width, deskD = depth, deskH = height */
    #define DRAW_DESK(wx, wy, wz, ry, dW, dD, dH) \
    do { \
        glPushMatrix(); \
        glTranslatef(wx, wy, wz); \
        glRotatef(ry, 0, 1, 0); \
        /* Desk surface */ \
        glColor4f(0.08f, 0.11f, 0.20f, 1.0f); \
        glPushMatrix(); \
        glTranslatef(0.0f, dH, 0.0f); \
        glScalef(dW, 0.06f, dD); \
        glutSolidCube(1.0f); \
        glPopMatrix(); \
        /* Desk legs */ \
        glColor4f(0.05f, 0.07f, 0.14f, 1.0f); \
        float lx[2] = {-dW*0.45f, dW*0.45f}; \
        float lz[2] = {-dD*0.45f, dD*0.45f}; \
        for(int li=0;li<2;li++) for(int lj=0;lj<2;lj++) { \
            glPushMatrix(); \
            glTranslatef(lx[li], dH*0.5f, lz[lj]); \
            glScalef(0.06f, dH, 0.06f); \
            glutSolidCube(1.0f); \
            glPopMatrix(); \
        } \
        /* Desk-top monitor glow strip */ \
        float mGlow = 0.5f + 0.5f * sinf(gTime * 2.5f + wx); \
        glColor4f(0.0f, mGlow * 0.7f, mGlow, 0.85f); \
        glPushMatrix(); \
        glTranslatef(0.0f, dH + 0.45f, -dD*0.3f); \
        glScalef(dW * 0.80f, 0.55f, 0.04f); \
        glutSolidCube(1.0f); \
        glPopMatrix(); \
        /* Monitor screen bright inner */ \
        glColor4f(0.02f, 0.06f, 0.15f, 1.0f); \
        glPushMatrix(); \
        glTranslatef(0.0f, dH + 0.45f, -dD*0.3f + 0.025f); \
        glScalef(dW * 0.72f, 0.45f, 0.02f); \
        glutSolidCube(1.0f); \
        glPopMatrix(); \
        /* Screen glow text line (decorative) */ \
        glColor4f(0.0f, 1.0f, 1.0f, 0.9f); \
        glLineWidth(1.5f); \
        glPushMatrix(); \
        glTranslatef(-dW*0.30f, dH + 0.50f, -dD*0.3f + 0.036f); \
        float ms = 0.00065f; \
        glScalef(ms, ms, ms); \
        const char *dl = "SYSTEM: ONLINE"; \
        while(*dl){ glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,*dl++); } \
        glPopMatrix(); \
        glPopMatrix(); \
    } while(0)

    /* ---- Left wall workstations (3 desks) ---- */
    DRAW_DESK(-6.8f, -1.0f, -4.0f,  90.0f, 1.4f, 0.7f, 0.85f);
    DRAW_DESK(-6.8f, -1.0f, -1.5f,  90.0f, 1.4f, 0.7f, 0.85f);
    DRAW_DESK(-6.8f, -1.0f,  1.0f,  90.0f, 1.4f, 0.7f, 0.85f);

    /* ---- Right wall workstations (3 desks) ---- */
    DRAW_DESK( 6.8f, -1.0f, -4.5f, -90.0f, 1.4f, 0.7f, 0.85f);
    DRAW_DESK( 6.8f, -1.0f, -2.0f, -90.0f, 1.4f, 0.7f, 0.85f);
    DRAW_DESK( 6.8f, -1.0f,  0.5f, -90.0f, 1.4f, 0.7f, 0.85f);

    /* ---- Back row standing desks (5 stations) ---- */
    float bx[5] = { -5.5f, -2.8f, 0.0f, 2.8f, 5.5f };
    for (int i = 0; i < 5; i++) {
        DRAW_DESK(bx[i], -1.0f, -5.2f, 0.0f, 1.2f, 0.6f, 1.10f); /* taller standing desk */
    }

    #undef DRAW_DESK
    glEnable(GL_LIGHTING);
}

/* ============================================================
 *  DISPLAY CALLBACK
 * ============================================================ */
static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* ---- Camera setup ---- */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55.0, (double)WIN_W / (double)WIN_H, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Fixed front camera (no rotation) */
    gluLookAt(0.0, gCameraHeight, gCameraZoom,  /* eye: straight front */
              0.0,  0.5,  0.0,                  /* center (table area) */
              0.0,  1.0,  0.0);                 /* up */

    /* ---- Draw scene elements ---- */
    drawWalls();
    drawCeiling();
    drawFloor();
    drawMainScreen();

    /* Data screens on walls */
    for (int i = 0; i < NUM_SCREENS; i++)
        drawScreen(&gScreens[i]);

    /* Workstation desks (side and back) */
    drawWorkstations();

    /* Central holographic table */
    drawHoloTable();

    /* Scientists */
    for (int i = 0; i < NUM_SCIENTISTS; i++)
        drawScientist(&gSci[i]);

    /* Particles */
    drawParticles();

    /* 2D HUD overlay */
    drawHUD();

    glutSwapBuffers();
}

/* ============================================================
 *  TIMER CALLBACK: Animation update
 * ============================================================ */
static void timer(int value) {
    float dt = TIMER_MS / 1000.0f;
    gTime += dt;

    /* Camera is FIXED (no orbit rotation) */
    /* Gentle zoom pulse only */
    gCameraZoom = 12.5f + 0.5f * sinf(gTime * 0.4f);
    gCameraHeight = 5.0f + 0.3f * sinf(gTime * 0.25f);

    /* Scientists are STATIC — no position updates */

    updateParticles(dt);
    updateScreens(dt);

    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, timer, 0);
}

/* ============================================================
 *  RESHAPE CALLBACK
 * ============================================================ */
static void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
}

/* ============================================================
 *  KEYBOARD CALLBACK
 * ============================================================ */
static void keyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;
    switch (key) {
        case 27: exit(0); break;      /* ESC */
        case 'r': case 'R':
            gCameraAngle = 0.0f;
            gCameraZoom  = 14.0f;
            break;
        case '+': gCameraZoom -= 0.5f; break;
        case '-': gCameraZoom += 0.5f; break;
    }
}

/* ============================================================
 *  MAIN
 * ============================================================ */
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("ISRO Mission Control - MOM: Earth to Mars");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(TIMER_MS, timer, 0);

    printf("\n");
    printf("=========================================\n");
    printf("  ISRO Mission Control Animation\n");
    printf("  MOM Trajectory: Earth to Mars\n");
    printf("=========================================\n");
    printf("  Controls:\n");
    printf("    ESC  - Quit\n");
    printf("    R    - Reset camera\n");
    printf("    +/-  - Zoom in/out\n");
    printf("=========================================\n\n");

    glutMainLoop();
    return 0;
}