/*
 * Scene 3: Rocket at Shoreline
 * Duration: 5 seconds
 * Extracted from rocket_shoreline.c - EXACT VERSION
 */

#ifndef SCENE_3_SHORELINE_H
#define SCENE_3_SHORELINE_H

#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

/* Static rocket on shore scene */

static const float kRocketX = 0.0f;
static const float kRocketZ = -12.0f;
static float g_time = 0.0f;

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
static void drawSkyAndClouds(void) {
    glDisable(GL_LIGHTING);

    /* Five off-white clouds in flat, clean style */
    glColor3f(0.97f, 0.94f, 0.88f);
    drawCloud(-28.0f, 13.8f, -34.0f, 1.5f);
    drawCloud(-6.0f, 12.6f, -32.0f, 1.1f);
    drawCloud(10.0f, 14.4f, -36.0f, 1.3f);
    drawCloud(26.0f, 13.2f, -38.0f, 1.0f);
    drawCloud(48.0f, 13.9f, -40.0f, 1.1f);

    glEnable(GL_LIGHTING);
}

static void drawSeaSurface(void) {
    int i;
    const float y = -2.0f;
    const float zNear = 6.0f;
    const float zFar = 40.0f;
    const float xMin = -80.0f;
    const float xMax = 80.0f;

    glDisable(GL_LIGHTING);

    /* Flat water base (darker than sky for clear separation) */
    glBegin(GL_QUADS);
    glColor3f(0.072f, 0.42f, 0.52f);
    glVertex3f(xMin, y, zNear);
    glVertex3f(xMax, y, zNear);
    glVertex3f(xMax, y, zFar);
    glVertex3f(xMin, y, zFar);
    glEnd();

    /* Thin horizon divider to separate sea and sky */
    glBegin(GL_QUADS);
    glColor3f(0.06f, 0.36f, 0.46f);
    glVertex3f(xMin, y + 0.01f, zNear - 0.2f);
    glVertex3f(xMax, y + 0.01f, zNear - 0.2f);
    glVertex3f(xMax, y + 0.01f, zNear - 0.6f);
    glVertex3f(xMin, y + 0.01f, zNear - 0.6f);
    glEnd();

    /* Stylized geometric wave bands moving toward shore */
    for (i = 0; i < 7; ++i) {
        float speed = 2.6f;
        float spacing = 4.6f;
        float offset = fmodf(g_time * speed, spacing);
        float z0 = zFar - 1.5f - (float)i * spacing + offset;
        float z1 = z0 + 0.7f;
        float y0 = y + 0.02f + 0.02f * sinf(g_time * 2.0f + (float)i);
        float y1 = y + 0.02f + 0.02f * cosf(g_time * 1.8f + (float)i);

        if (z1 < zNear) {
            z0 += spacing * 7.0f;
            z1 += spacing * 7.0f;
        }

        glBegin(GL_QUADS);
        glColor3f(0.10f, 0.50f, 0.60f);
        glVertex3f(xMin, y0, z0);
        glVertex3f(xMax, y1, z0);
        glVertex3f(xMax, y1, z1);
        glVertex3f(xMin, y0, z1);
        glEnd();
    }

    /* Shoreward ripple accents for visible movement */
    for (i = 0; i < 5; ++i) {
        float travel = (zFar - zNear - 2.0f);
        float z = zFar - fmodf(g_time * 3.0f + (float)i * 3.0f, travel);
        float w = 9.0f + 2.5f * sinf(g_time * 2.4f + (float)i);
        glBegin(GL_QUADS);
        glColor3f(0.12f, 0.54f, 0.64f);
        glVertex3f(-w, y + 0.04f, z);
        glVertex3f(w, y + 0.04f, z);
        glVertex3f(w, y + 0.04f, z + 0.28f);
        glVertex3f(-w, y + 0.04f, z + 0.28f);
        glEnd();
    }

    /* Simple reflections of the rocket and gantry */
    glBegin(GL_QUADS);
    glColor3f(0.10f, 0.50f, 0.60f);
    glVertex3f(-2.0f, y + 0.01f, 10.0f);
    glVertex3f(2.0f, y + 0.01f, 10.0f);
    glVertex3f(2.0f, y + 0.01f, 30.0f);
    glVertex3f(-2.0f, y + 0.01f, 30.0f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.09f, 0.48f, 0.58f);
    glVertex3f(8.0f, y + 0.01f, 12.0f);
    glVertex3f(14.0f, y + 0.01f, 12.0f);
    glVertex3f(14.0f, y + 0.01f, 30.0f);
    glVertex3f(8.0f, y + 0.01f, 30.0f);
    glEnd();

    glEnable(GL_LIGHTING);
}

static void drawShoreline(void) {
    int i;
    const float zStart = -2.0f;
    const float zEnd = -26.0f;
    const int bands = 6;
    const float bandDepth = (zStart - zEnd) / (float)bands;

    glNormal3f(0.0f, 1.0f, 0.0f);
    glDisable(GL_LIGHTING);

    glBegin(GL_QUADS);
    for (i = 0; i < bands; ++i) {
        float z0 = zStart - bandDepth * (float)i;
        float z1 = z0 - bandDepth;
        if (i % 2 == 0) {
            glColor3f(0.44f, 0.50f, 0.34f);
        } else {
            glColor3f(0.33f, 0.30f, 0.20f);
        }
        glVertex3f(-60.0f, -1.99f, z0);
        glVertex3f(60.0f, -1.99f, z0);
        glVertex3f(60.0f, -1.99f, z1);
        glVertex3f(-60.0f, -1.99f, z1);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

static void drawForegroundPlatform(void) {
    glDisable(GL_LIGHTING);
    setColor3(0.62f, 0.47f, 0.28f);

    glBegin(GL_QUADS);
    glVertex3f(-6.0f, -1.98f, 6.0f);
    glVertex3f(6.0f, -1.98f, 6.0f);
    glVertex3f(14.0f, -1.98f, 22.0f);
    glVertex3f(-14.0f, -1.98f, 22.0f);
    glEnd();

    glEnable(GL_LIGHTING);
}

static void drawTreeClump(float x, float z, float h) {
    int i;
    glDisable(GL_LIGHTING);
    setColor3(0.08f, 0.15f, 0.06f);
    
    glPushMatrix();
    glTranslatef(x, -1.8f, z);
    
    glPushMatrix();
    glTranslatef(0.0f, h * 0.3f, 0.0f);
    drawUnitBox(0.22f, h * 0.6f, 0.22f);
    glPopMatrix();
    
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
    int i;
    const float trees[][2] = {
        {-26.0f, -8.0f}, {-22.0f, -8.5f}, {-18.0f, -8.2f},
        {-14.0f, -8.7f}, {-10.0f, -8.3f}
    };

    glDisable(GL_LIGHTING);
    glColor3f(0.12f, 0.20f, 0.10f);

    for (i = 0; i < 5; ++i) {
        glPushMatrix();
        glTranslatef(trees[i][0], -1.72f, trees[i][1]);
        glutSolidSphere(0.55f, 10, 10);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
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

    for (i = 0; i < 8; ++i) {
        float h = (float)i / 7.0f * height;
        glPushMatrix();
        glTranslatef(0.0f, h, -base);
        drawUnitBox(base * 2.2f, 0.12f, 0.12f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, h, base);
        drawUnitBox(base * 2.2f, 0.12f, 0.12f);
        glPopMatrix();
    }

    glPopMatrix();
}
static void drawGroundAndPaths(void) {
    setColor3(0.42f, 0.48f, 0.36f);

    glBegin(GL_QUADS);
    glVertex3f(-30.0f, -1.99f, -30.0f);
    glVertex3f(30.0f, -1.99f, -30.0f);
    glVertex3f(30.0f, -1.99f, 5.0f);
    glVertex3f(-30.0f, -1.99f, 5.0f);
    glEnd();
}

static void drawWaterPonds(void) {
    /* Reflection pools near launch complex - like image_1.png aerials */
    glDisable(GL_LIGHTING);
    glColor4f(0.15f, 0.35f, 0.5f, 0.8f);
    
    /* Pond 1 */
    glBegin(GL_QUADS);
    glVertex3f(-25.0f, -1.98f, 5.0f);
    glVertex3f(-18.0f, -1.98f, 5.0f);
    glVertex3f(-18.0f, -1.98f, 12.0f);
    glVertex3f(-25.0f, -1.98f, 12.0f);
    glEnd();
    
    /* Pond 2 */
    glBegin(GL_QUADS);
    glVertex3f(12.0f, -1.98f, 8.0f);
    glVertex3f(22.0f, -1.98f, 8.0f);
    glVertex3f(22.0f, -1.98f, 15.0f);
    glVertex3f(12.0f, -1.98f, 15.0f);
    glEnd();
    
    glEnable(GL_LIGHTING);
}

static void drawSeagull(float x, float y, float z, float wingFlap) {
    glDisable(GL_LIGHTING);
    glColor3f(0.95f, 0.93f, 0.90f);
    
    glPushMatrix();
    glTranslatef(x, y, z);
    
    /* Body */
    setColor3(0.92f, 0.90f, 0.88f);
    glPushMatrix();
    glScalef(0.12f, 0.08f, 0.10f);
    glutSolidSphere(0.5, 12, 12);
    glPopMatrix();
    
    /* Head */
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.08f);
    glScalef(0.07f, 0.07f, 0.08f);
    glutSolidSphere(0.5, 10, 10);
    glPopMatrix();
    
    /* Left wing */
    glPushMatrix();
    glTranslatef(-0.12f, 0.02f, 0.0f);
    glRotatef(wingFlap, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.25f, 0.08f, -0.02f);
    glVertex3f(-0.20f, -0.02f, 0.01f);
    glEnd();
    glPopMatrix();
    
    /* Right wing */
    glPushMatrix();
    glTranslatef(0.12f, 0.02f, 0.0f);
    glRotatef(-wingFlap, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.25f, 0.08f, -0.02f);
    glVertex3f(0.20f, -0.02f, 0.01f);
    glEnd();
    glPopMatrix();
    
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

static void drawSeagullsOnShore(void) {
    float wingFlap = sinf(g_time * 6.0f) * 25.0f;
    
    /* Seagulls on sand/beach */
    drawSeagull(-12.0f, -1.85f, 1.0f, 0.0f);
    drawSeagull(-8.0f, -1.85f, 0.5f, 0.0f);
    drawSeagull(-4.0f, -1.85f, 2.0f, 0.0f);
    drawSeagull(8.0f, -1.85f, 1.5f, 0.0f);
    
    /* Seagulls flying near shore */
    drawSeagull(-20.0f, 1.5f, -1.0f, wingFlap);
    drawSeagull(-10.0f, 2.0f, 1.0f, wingFlap * 0.7f);
    drawSeagull(5.0f, 1.8f, 0.5f, wingFlap * 1.3f);
    drawSeagull(18.0f, 2.2f, 2.0f, wingFlap);
}

static void drawLaunchPad(void) {
    setColor3(0.44f, 0.44f, 0.44f);

    glBegin(GL_QUADS);
    glVertex3f(-4.5f, -1.98f, -15.0f);
    glVertex3f(4.5f, -1.98f, -15.0f);
    glVertex3f(4.5f, -1.98f, -9.0f);
    glVertex3f(-4.5f, -1.98f, -9.0f);
    glEnd();

    setColor3(0.62f, 0.57f, 0.46f);
    glBegin(GL_QUADS);
    glVertex3f(-5.2f, -1.96f, -15.8f);
    glVertex3f(5.2f, -1.96f, -15.8f);
    glVertex3f(5.2f, -1.96f, -8.2f);
    glVertex3f(-5.2f, -1.96f, -8.2f);
    glEnd();
}


static void drawServiceGantry(void) {
    GLUquadric *quad = gluNewQuadric();
    
    setColor3(0.35f, 0.35f, 0.38f);

    glPushMatrix();
    glTranslatef(12.0f, -1.0f, kRocketZ + 2.0f);

    int i;
    for (i = 0; i < 4; ++i) {
        float sx = (i & 1) ? 2.4f : -2.4f;
        float sz = (i & 2) ? 2.2f : -2.2f;
        glPushMatrix();
        glTranslatef(sx, 8.0f, sz);
        drawUnitBox(0.28f, 16.0f, 0.28f);
        glPopMatrix();
    }

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

static void drawStaticRocket(void) {
    GLUquadric *quad = gluNewQuadric();

    glPushMatrix();
    glTranslatef(kRocketX, -1.0f, kRocketZ);
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
    glTranslatef(0.0f, 0.0f, 8.3f);
    gluDisk(quad, 0.0, 0.44, 34, 8);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

static void drawPerson(float x, float z) {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(x, -1.95f, z);

    setColor3(0.82f, 0.82f, 0.84f);
    drawUnitBox(0.18f, 0.48f, 0.12f);

    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    glutSolidSphere(0.09f, 10, 10);
    glPopMatrix();

    glPopMatrix();
    glEnable(GL_LIGHTING);
}

static void setupLighting(void) {
    /* Flat style: lighting kept minimal; colors drive the look */
}

static void shoreline_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.66f, 0.84f, 0.91f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Low perspective at water level */
    gluLookAt(0.0f, 3.2f, 28.0f,
              kRocketX, 2.6f, kRocketZ,
              0.0f, 1.0f, 0.0f);

    glDisable(GL_LIGHTING);
    drawSkyAndClouds();
    drawSeaSurface();
    drawForegroundPlatform();
    drawShoreline();
    drawForegroundVegetation();

    drawGroundAndPaths();
    drawLaunchTower(18.0f, kRocketZ + 4.0f, 16.0f, 1.6f);
    drawServiceGantry();
    drawLaunchPad();
    drawStaticRocket();

    /* Crowd standing around */
    drawPerson(-20.0f, -10.0f);
    drawPerson(-16.0f, -10.0f);
    drawPerson(-12.0f, -10.0f);
    drawPerson(-8.0f, -10.0f);
    drawPerson(-4.0f, -10.0f);
    
    /* Seagulls on shore and flying */
    drawSeagullsOnShore();

    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

static void shoreline_update(float dt) {
    g_time += dt;
}

#endif
