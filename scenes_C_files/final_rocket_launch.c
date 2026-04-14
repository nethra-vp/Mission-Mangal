#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

static float g_launchTime = 0.0f;
static float g_rocketY = 0.0f;

static const float kRocketX = 0.0f;
static const float kRocketZ = -12.0f;

#define STAR_COUNT 650
static float gStarPos[STAR_COUNT][3];
static float gStarSize[STAR_COUNT];
static float gStarPhase[STAR_COUNT];
static const float kStarWrapX = 260.0f;
static const float kStarWrapY = 180.0f;
static const float kStarWrapZ = 900.0f;

static float clamp01(float v) {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

static float smooth01(float v) {
    float t = clamp01(v);
    return t * t * (3.0f - 2.0f * t);
}

static void drawUnitBox(float sx, float sy, float sz);

static void setColor3(float r, float g, float b) {
    glColor3f(r, g, b);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, (GLfloat[]){r, g, b, 1.0f});
}

static void drawUnitBox(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

static void drawStrokeText(const char *txt, float scale) {
    int i;
    glPushMatrix();
    glScalef(scale, scale, scale);
    for (i = 0; txt[i] != '\0'; ++i) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, txt[i]);
    }
    glPopMatrix();
}

static void drawCloud(float x, float y, float z, float s) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(s, s, s);
    glutSolidSphere(0.7, 24, 24);
    glTranslatef(0.8f, 0.2f, 0.0f);
    glutSolidSphere(0.55, 22, 22);
    glTranslatef(-1.5f, -0.1f, 0.0f);
    glutSolidSphere(0.5, 20, 20);
    glPopMatrix();
}

static void initStarfield(void) {
    int i;
    srand(19);
    for (i = 0; i < STAR_COUNT; ++i) {
        float az = ((float)rand() / (float)RAND_MAX) * 6.2831853f;
        float el = 0.20f + ((float)rand() / (float)RAND_MAX) * 1.15f;
        float dist = 220.0f + ((float)rand() / (float)RAND_MAX) * 520.0f;

        gStarPos[i][0] = cosf(az) * sinf(el) * dist;
        gStarPos[i][1] = cosf(el) * dist * 0.55f;
        gStarPos[i][2] = -fabsf(sinf(az) * sinf(el) * dist) - 40.0f;
        gStarSize[i] = 0.7f + ((float)rand() / (float)RAND_MAX) * 1.7f;
        gStarPhase[i] = ((float)rand() / (float)RAND_MAX) * 6.2831853f;
    }
}

static void drawStarfield(void) {
    int i;
    float darkBlend;
    float driftY;
    float driftX;

    if (g_launchTime <= 6.0f) {
        darkBlend = 0.0f;
    } else if (g_launchTime >= 18.0f) {
        darkBlend = 1.0f;
    } else {
        darkBlend = smooth01((g_launchTime - 6.0f) / 12.0f);
    }

    /* Create apparent upward vehicle motion by moving stars downward over time. */
    driftY = (g_launchTime - 12.0f) * 1.9f;
    if (driftY < 0.0f) {
        driftY = 0.0f;
    }
    driftX = sinf(g_launchTime * 0.22f) * 0.75f;

    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (i = 0; i < STAR_COUNT; ++i) {
        float sx = gStarPos[i][0] + driftX;
        float sy = gStarPos[i][1] - driftY;
        float sz = gStarPos[i][2];
        float tw = 0.65f + 0.35f * sinf(g_launchTime * 0.8f + gStarPhase[i]);
        float brightness = clamp01((0.25f + 0.75f * darkBlend) * tw);

        while (sx < -kStarWrapX) {
            sx += kStarWrapX * 2.0f;
        }
        while (sx > kStarWrapX) {
            sx -= kStarWrapX * 2.0f;
        }
        while (sy < -140.0f) {
            sy += kStarWrapY * 2.0f;
        }
        while (sy > kStarWrapY) {
            sy -= kStarWrapY * 2.0f;
        }
        while (sz < -kStarWrapZ) {
            sz += kStarWrapZ;
        }
        while (sz > 0.0f) {
            sz -= kStarWrapZ;
        }

        glPointSize(gStarSize[i]);
        glBegin(GL_POINTS);
        glColor3f(brightness, brightness, brightness + 0.05f * brightness);
        glVertex3f(sx, sy, sz);
        glEnd();
    }

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}

static void drawSkyAndClouds(void) {
    glDisable(GL_LIGHTING);

    /* Keep the orange clearColor like isro2.c; only draw clouds here. */

    glColor3f(1.0f, 0.93f, 0.85f);
    drawCloud(-18.0f, 14.0f, -35.0f, 1.7f);
    drawCloud(-5.0f, 12.2f, -31.0f, 1.2f);
    drawCloud(14.0f, 15.0f, -37.0f, 1.5f);
    drawCloud(24.0f, 13.0f, -39.0f, 1.1f);
    drawCloud(-57.0f, 15.0f, -57.0f, 1.5f);
    drawCloud(-38.0f, 13.0f, -14.0f, 1.1f);
    drawCloud(57.0f, 15.0f, -57.0f, 1.5f);
    drawCloud(38.0f, 13.0f, -14.0f, 1.1f);

    glEnable(GL_LIGHTING);
}

static void updateSkyColor(void) {
    float fade = 0.0f;
    float r;
    float g;
    float b;

    /* Start with the existing orange launch sky, then blend toward dark space. */
    if (g_launchTime <= 6.0f) {
        fade = 0.0f;
    } else if (g_launchTime >= 18.0f) {
        fade = 1.0f;
    } else {
        fade = (g_launchTime - 6.0f) / 12.0f;
    }

    fade = fade * fade;

    r = 0.99f * (1.0f - fade) + 0.005f * fade;
    g = 0.61f * (1.0f - fade) + 0.01f * fade;
    b = 0.32f * (1.0f - fade) + 0.03f * fade;

    glClearColor(r, g, b, 1.0f);
}

static void drawSeaSurface(void) {
    int i;
    const float y = -2.0f;
    const float zNear = -2.0f;
    const float zFar = 60.0f;
    const float xMin = -60.0f;
    const float xMax = 60.0f;
    const int strips = 50;

    glDisable(GL_LIGHTING);
    for (i = 0; i < strips; ++i) {
        float t0 = (float)i / (float)strips;
        float t1 = (float)(i + 1) / (float)strips;
        float z0 = zNear + (zFar - zNear) * t0;
        float z1 = zNear + (zFar - zNear) * t1;

        /* Subtle gentle waves only, static overall appearance */
        float wave0 = sinf(g_launchTime * 0.3f + z0 * 0.08f) * 0.02f;
        float wave1 = sinf(g_launchTime * 0.3f + z1 * 0.08f) * 0.02f;

        float shade0 = 0.18f + 0.22f * (1.0f - t0);
        float shade1 = 0.18f + 0.22f * (1.0f - t1);

        glBegin(GL_QUADS);
        glColor3f(0.10f, 0.17f + shade0, 0.28f + shade0);
        glVertex3f(xMin, y + wave0, z0);
        glVertex3f(xMax, y + wave0, z0);
        glColor3f(0.10f, 0.17f + shade1, 0.28f + shade1);
        glVertex3f(xMax, y + wave1, z1);
        glVertex3f(xMin, y + wave1, z1);
        glEnd();
    }

    /* A bright reflection column under the rocket (simple, stylized). */
    {
        float refl = fminf(fmaxf((g_rocketY * 0.12f), 0.0f), 1.0f);
        float w = 3.5f + refl * 1.8f;
        float bright = 0.60f + refl * 0.25f;
        glBegin(GL_QUADS);
        glColor4f(bright, bright * 0.80f, 0.25f, 1.0f);
        glVertex3f(-w, y + 0.02f, 2.0f);
        glVertex3f(w, y + 0.02f, 2.0f);
        glColor4f(bright * 0.25f, bright * 0.15f, 0.05f, 1.0f);
        glVertex3f(w, y + 0.02f, 55.0f);
        glVertex3f(-w, y + 0.02f, 55.0f);
        glEnd();
    }

    glEnable(GL_LIGHTING);
}

static void drawShoreline(void) {
    /* A thin land strip separating sea and launch site. */
    glNormal3f(0.0f, 1.0f, 0.0f);
    glDisable(GL_LIGHTING);

    glBegin(GL_QUADS);
    glColor3f(0.26f, 0.20f, 0.12f);
    glVertex3f(-60.0f, -1.99f, -2.0f);
    glVertex3f(60.0f, -1.99f, -2.0f);
    glColor3f(0.14f, 0.11f, 0.07f);
    glVertex3f(60.0f, -1.99f, -25.0f);
    glVertex3f(-60.0f, -1.99f, -25.0f);
    glEnd();

    glEnable(GL_LIGHTING);
}

static void drawTreeClump(float x, float z, float h) {
    /* Simple vegetation silhouette. */
    int i;
    glDisable(GL_LIGHTING);
    setColor3(0.08f, 0.15f, 0.06f);
    
    glPushMatrix();
    glTranslatef(x, -1.8f, z);
    
    /* Trunk */
    glPushMatrix();
    glTranslatef(0.0f, h * 0.3f, 0.0f);
    drawUnitBox(0.22f, h * 0.6f, 0.22f);
    glPopMatrix();
    
    /* Foliage (round billboards) */
    for (i = 0; i < 5; ++i) {
        float angle = (float)i / 5.0f * 6.2831853f;
        float ox = cosf(angle) * 0.8f;
        float oz = sinf(angle) * 0.6f;
        glPushMatrix();
        glTranslatef(ox, h * 0.7f, oz);
        glutSolidSphere(0.65f, 12, 12);
        glPopMatrix();
    }
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
}

static void drawForegroundVegetation(void) {
    /* Silhouettes on left side of shore, framing the launch. */
    glDisable(GL_LIGHTING);
    glColor3f(0.06f, 0.12f, 0.04f);
    
    /* Left side bushes/trees */
    drawTreeClump(-28.0f, -1.0f, 3.2f);
    drawTreeClump(-25.5f, -0.8f, 2.8f);
    drawTreeClump(-22.0f, -1.2f, 3.5f);
    drawTreeClump(-18.5f, -0.5f, 2.6f);
    drawTreeClump(-15.0f, -1.0f, 3.0f);
    
    /* Right side single tall element (like gantry silhouette). */
    setColor3(0.15f, 0.15f, 0.16f);
    glPushMatrix();
    glTranslatef(26.0f, -1.8f, 3.0f);
    drawUnitBox(0.35f, 14.0f, 0.35f);
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
}

static void drawBird(float x, float y, float z, float s, float phase) {
    float flap = sinf(g_launchTime * 4.0f + phase) * 0.22f;

    glDisable(GL_LIGHTING);
    glColor3f(0.12f, 0.12f, 0.12f);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(s, s, s);
    glBegin(GL_LINES);
    glVertex3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, flap, 0.0f);
    glVertex3f(0.0f, flap, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

static void drawBirds(void) {
    drawBird(-7.0f, 10.5f, -25.0f, 0.7f, 0.0f);
    drawBird(-2.0f, 12.0f, -30.0f, 0.55f, 1.0f);
    drawBird(6.0f, 11.0f, -22.0f, 0.65f, 2.0f);
    drawBird(14.0f, 9.5f, -28.0f, 0.5f, 2.7f);
    drawBird(19.0f, 11.3f, -33.0f, 0.45f, 3.2f);
}

static void drawGroundAndPaths(void) {
    glNormal3f(0.0f, 1.0f, 0.0f);

    setColor3(0.35f, 0.61f, 0.34f);
    glBegin(GL_QUADS);
    glVertex3f(-35.0f, -2.0f, -35.0f);
    glVertex3f(35.0f, -2.0f, -35.0f);
    glVertex3f(35.0f, -2.0f, 20.0f);
    glVertex3f(-35.0f, -2.0f, 20.0f);
    glEnd();

    setColor3(0.76f, 0.74f, 0.68f);
    glBegin(GL_QUADS);
    glVertex3f(-15.5f, -1.98f, 7.0f);
    glVertex3f(15.5f, -1.98f, 7.0f);
    glVertex3f(15.5f, -1.98f, 11.5f);
    glVertex3f(-15.5f, -1.98f, 11.5f);

    glVertex3f(-2.8f, -1.98f, -11.0f);
    glVertex3f(2.8f, -1.98f, -11.0f);
    glVertex3f(2.8f, -1.98f, 11.5f);
    glVertex3f(-2.8f, -1.98f, 11.5f);
    glEnd();
}

static void drawLaunchTower(float x, float z, float height, float base) {
    int i;

    setColor3(0.30f, 0.30f, 0.32f);
    glPushMatrix();
    glTranslatef(x, -2.0f, z);

    for (i = 0; i < 4; ++i) {
        float sx = (i & 1) ? base : -base;
        float sz = (i & 2) ? base : -base;
        glPushMatrix();
        glTranslatef(sx, height * 0.5f, sz);
        drawUnitBox(0.18f, height, 0.18f);
        glPopMatrix();
    }

    setColor3(0.24f, 0.24f, 0.25f);
    for (i = 0; i < 8; ++i) {
        float y = (float)i / 7.0f * height;
        glPushMatrix();
        glTranslatef(0.0f, y, 0.0f);
        drawUnitBox(base * 2.2f, 0.06f, 0.06f);
        drawUnitBox(0.06f, 0.06f, base * 2.2f);
        glPopMatrix();
    }

    glPopMatrix();
}

static void drawLaunchPad(void) {
    setColor3(0.56f, 0.56f, 0.6f);
    glPushMatrix();
    glTranslatef(kRocketX, -1.85f, kRocketZ);
    drawUnitBox(3.2f, 0.3f, 3.2f);
    glPopMatrix();
}

static void drawAnimatedRocket(void) {
    GLUquadric *quad = gluNewQuadric();
    if (quad == NULL) {
        return;
    }

    float rocketX = kRocketX;
    float rocketZ = kRocketZ;
    float rocketY = g_rocketY;

    glPushMatrix();
    glTranslatef(rocketX, -1.0f + rocketY, rocketZ);

    /* Thruster flame (only during launch phase) */
    if (g_rocketY > 0.1f) {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glTranslatef(0.0f, -0.5f, 0.0f);
        float flameHeight = fminf(g_rocketY * 0.6f, 3.4f) + sinf(g_launchTime * 50.0f) * 0.2f;
        glColor3f(1.0f, 0.8f, 0.2f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, -flameHeight, 0.0f);
        for (int i = 0; i <= 16; ++i) {
            float angle = (float)i / 16.0f * 2.0f * 3.1415926f;
            glColor3f(1.0f, 0.4f, 0.1f);
            glVertex3f(cosf(angle) * 0.4f, 0.0f, sinf(angle) * 0.4f);
        }
        glEnd();
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    setColor3(0.97f, 0.97f, 0.97f);
    gluCylinder(quad, 0.45, 0.45, 9.0, 34, 8);

    setColor3(0.95f, 0.45f, 0.20f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 2.2f);
    gluCylinder(quad, 0.46, 0.46, 1.0, 34, 3);
    glPopMatrix();

    setColor3(0.20f, 0.43f, 0.86f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 5.2f);
    gluCylinder(quad, 0.46, 0.46, 1.0, 34, 3);
    glPopMatrix();

    setColor3(0.97f, 0.97f, 0.97f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 9.0f);
    gluCylinder(quad, 0.45, 0.0, 1.25, 34, 8);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(0.07f, 0.07f, 0.07f);

    glPushMatrix();
    glTranslatef(-0.12f, 0.54f, 4.4f);
    glScalef(0.0018f, 0.0018f, 0.0018f);
    drawStrokeText("ISRO", 1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.18f, -0.56f, 4.4f);
    glScalef(0.0018f, 0.0018f, 0.0018f);
    drawStrokeText("INDIA", 1.0f);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

static void drawCelebratePerson(float x, float z, float offset) {
    float clap = sinf(g_launchTime * 3.0f + offset) * 0.3f;
    
    glPushMatrix();
    glTranslatef(x, -1.95f, z);
    
    setColor3(0.16f, 0.20f, 0.28f);
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    drawUnitBox(0.28f, 0.62f, 0.16f);
    glPopMatrix();
    
    setColor3(0.94f, 0.78f, 0.62f);
    glPushMatrix();
    glTranslatef(0.0f, 0.98f, 0.0f);
    glutSolidSphere(0.11, 14, 14);
    glPopMatrix();
    
    setColor3(0.06f, 0.08f, 0.1f);
    
    glPushMatrix();
    glTranslatef(-0.15f, 0.35f + clap * 0.15f, 0.0f);
    glRotatef(clap * 20.0f, 0.0f, 0.0f, 1.0f);
    drawUnitBox(0.08f, 0.35f, 0.08f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.15f, 0.35f + clap * 0.15f, 0.0f);
    glRotatef(-clap * 20.0f, 0.0f, 0.0f, 1.0f);
    drawUnitBox(0.08f, 0.35f, 0.08f);
    glPopMatrix();
    
    glPopMatrix();
}

static void drawSmokeBillow(float x, float y, float z, float s, float phase) {
    glDisable(GL_LIGHTING);
    {
        float a = fminf(fmaxf((g_launchTime - 2.0f) * 0.18f, 0.0f), 1.0f);
        float puff = s * (1.0f + a * 2.4f);
        float drift = sinf(g_launchTime * 0.6f + phase) * 0.9f * a;
        glColor3f(0.76f, 0.64f, 0.50f);
        glPushMatrix();
        glTranslatef(x + drift, y, z);
        glutSolidSphere(puff, 18, 18);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
}

static void drawLaunchSmoke(void) {
    if (g_launchTime < 2.0f) {
        return;
    }
    drawSmokeBillow(kRocketX - 3.0f, -1.6f, kRocketZ + 4.0f, 0.9f, 0.0f);
    drawSmokeBillow(kRocketX + 1.5f, -1.6f, kRocketZ + 3.0f, 0.8f, 1.2f);
    drawSmokeBillow(kRocketX + 4.0f, -1.6f, kRocketZ + 4.5f, 1.0f, 2.1f);
    drawSmokeBillow(kRocketX - 5.0f, -1.6f, kRocketZ + 6.5f, 1.1f, 2.8f);
    drawSmokeBillow(kRocketX + 6.0f, -1.6f, kRocketZ + 6.0f, 1.15f, 3.6f);
}

static void setupLighting(void) {
    GLfloat lightPos[] = {-22.0f, 13.0f, 5.0f, 1.0f};
    GLfloat lightDiffuse[] = {1.0f, 0.72f, 0.42f, 1.0f};
    GLfloat lightAmbient[] = {0.26f, 0.20f, 0.19f, 1.0f};
    GLfloat lightSpec[] = {0.65f, 0.48f, 0.34f, 1.0f};

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
}

void display_launch(void) {
    updateSkyColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Camera: multi-shot sequence with crash sequence. */
    {
        float t = g_launchTime;
        float rocketY = g_rocketY;

        if (t < 2.0f) {
            /* Establishing shot across the sea. */
            gluLookAt(0.0f, 2.4f, 45.0f,
                      kRocketX, 2.0f, kRocketZ,
                      0.0f, 1.0f, 0.0f);
        } else if (t < 6.0f) {
            /* Closer, slightly off-center like the reference. */
            gluLookAt(10.0f, 3.0f + rocketY * 0.15f, 28.0f,
                      kRocketX, 1.5f + rocketY * 0.55f, kRocketZ,
                      0.0f, 1.0f, 0.0f);
        } else {
            /* Tracking up into the sky. */
            gluLookAt(12.0f, 6.0f + rocketY * 0.85f, 30.0f + rocketY * 0.10f,
                      kRocketX, 6.0f + rocketY * 1.15f, kRocketZ,
                      0.0f, 1.0f, 0.0f);
        }
    }

    if (g_launchTime < 12.0f) {
        drawSkyAndClouds();
        drawBirds();
    } else {
        drawStarfield();
    }

    drawSeaSurface();
    drawShoreline();
    drawForegroundVegetation();

    setupLighting();

    /* Launch site on the coast */
    drawGroundAndPaths();
    drawLaunchTower(18.0f, kRocketZ + 4.0f, 16.0f, 1.6f);

    drawLaunchSmoke();
    drawLaunchPad();
    drawAnimatedRocket();

    /* Crowd silhouettes far away on the shoreline. */
    drawCelebratePerson(-20.0f, -20.0f, 0.0f);
    drawCelebratePerson(-16.0f, -20.0f, 0.8f);
    drawCelebratePerson(-12.0f, -20.0f, 1.6f);
    drawCelebratePerson(-8.0f, -20.0f, 2.4f);
    drawCelebratePerson(-4.0f, -20.0f, 3.2f);

    glutSwapBuffers();
}

void timer_launch(int value) {
    (void)value;
    {
        const float dt = 0.016f;
        g_launchTime += dt;

        /* Continuous successful launch ascent. */
        if (g_launchTime > 2.0f) {
            g_rocketY += (g_launchTime - 2.0f) * 0.02f;
            if (g_rocketY > 45.0f) g_rocketY = 45.0f;
        }

        (void)dt;
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, timer_launch, 0);
}

void reshape_launch(int w, int h) {
    if (h == 0) {
        h = 1;
    }

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(62.0, (double)w / (double)h, 0.1, 1200.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 800);
    glutCreateWindow("Rocket Launch - Successful Mission");

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.99f, 0.61f, 0.32f, 1.0f);
    initStarfield();

    glutDisplayFunc(display_launch);
    glutReshapeFunc(reshape_launch);
    glutTimerFunc(16, timer_launch, 0);

    glutMainLoop();
    return 0;
}