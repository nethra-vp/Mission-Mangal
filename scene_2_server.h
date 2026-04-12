/*
 * Scene 2: Server Room
 * Duration: 5 seconds
 * Extracted from server_room.c - COMPLETE VERSION
 */

#ifndef SCENE_2_SERVER_H
#define SCENE_2_SERVER_H

#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static float s2_gTime = 0.0f;

static void s2_setColor3(float r, float g, float b) {
    GLfloat mat[4] = {r, g, b, 1.0f};
    glColor3f(r, g, b);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
}

static void s2_drawBox(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

static void s2_drawBitmapText(const char *txt, void *font) {
    int i;
    for (i = 0; txt[i] != '\0'; ++i) {
        glutBitmapCharacter(font, txt[i]);
    }
}

static void s2_drawEmissivePanel(float w, float h, float r, float g, float b) {
    glDisable(GL_LIGHTING);
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex3f(-w * 0.5f, -h * 0.5f, 0.0f);
    glVertex3f(w * 0.5f, -h * 0.5f, 0.0f);
    glVertex3f(w * 0.5f, h * 0.5f, 0.0f);
    glVertex3f(-w * 0.5f, h * 0.5f, 0.0f);
    glEnd();
    glEnable(GL_LIGHTING);
}

static void s2_drawScientist(float x, float y, float z, float r, float g, float b, int gesture) {
    float arm = 0.35f + 0.15f * sinf(s2_gTime * 2.0f + x);
    glPushMatrix();
    glTranslatef(x, y, z);

    s2_setColor3(r, g, b);
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    s2_drawBox(0.32f, 0.72f, 0.2f);
    glPopMatrix();

    s2_setColor3(0.92f, 0.76f, 0.62f);
    glPushMatrix();
    glTranslatef(0.0f, 1.02f, 0.0f);
    glutSolidSphere(0.12f, 14, 14);
    glPopMatrix();

    s2_setColor3(0.12f, 0.12f, 0.14f);
    glPushMatrix();
    glTranslatef(0.0f, 1.14f, 0.0f);
    s2_drawBox(0.22f, 0.08f, 0.22f);
    glPopMatrix();

    s2_setColor3(r * 0.8f, g * 0.8f, b * 0.8f);
    glPushMatrix();
    glTranslatef(-0.18f, 0.65f + (gesture ? arm : 0.0f), 0.0f);
    s2_drawBox(0.07f, 0.45f, 0.07f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.18f, 0.65f + (gesture ? -arm * 0.5f : 0.0f), 0.0f);
    s2_drawBox(0.07f, 0.45f, 0.07f);
    glPopMatrix();

    glPopMatrix();
}

static void s2_drawDeskStation(float x, float y, float z, float rotY, const char *dept) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    s2_setColor3(0.18f, 0.19f, 0.22f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    s2_drawBox(1.3f, 0.08f, 0.6f);
    glPopMatrix();

    s2_setColor3(0.14f, 0.14f, 0.18f);
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);
    s2_drawBox(1.25f, 0.25f, 0.54f);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.35f, 0.72f);
    glBegin(GL_QUADS);
    glVertex3f(-0.45f, 0.40f, 0.18f);
    glVertex3f(-0.05f, 0.40f, 0.18f);
    glVertex3f(-0.05f, 0.72f, 0.18f);
    glVertex3f(-0.45f, 0.72f, 0.18f);

    glVertex3f(0.05f, 0.40f, 0.18f);
    glVertex3f(0.45f, 0.40f, 0.18f);
    glVertex3f(0.45f, 0.72f, 0.18f);
    glVertex3f(0.05f, 0.72f, 0.18f);
    glEnd();

    glColor3f(0.9f, 0.9f, 1.0f);
    glRasterPos3f(-0.5f, 0.82f, 0.19f);
    s2_drawBitmapText(dept, GLUT_BITMAP_8_BY_13);
    glEnable(GL_LIGHTING);

    s2_setColor3(0.08f, 0.08f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, -0.06f, -0.18f);
    s2_drawBox(0.6f, 0.28f, 0.5f);
    glPopMatrix();

    glPopMatrix();
}

static void s2_drawTieredControlGallery(void) {
    int row, seat;
    float baseZ = -4.0f;
    float deskSpacing = 2.2f;
    const char *deptNames[3] = {"PROPULSION", "GUIDANCE", "TELEMETRY"};

    for (row = 0; row < 4; ++row) {
        float y = -0.6f + row * 0.75f;
        float z = baseZ - row * 3.5f;

        s2_setColor3(0.16f, 0.17f, 0.2f);
        glPushMatrix();
        glTranslatef(0.0f, y - 0.2f, z - 0.5f);
        s2_drawBox(22.0f, 0.35f, 3.0f);
        glPopMatrix();

        glDisable(GL_LIGHTING);
        glColor3f(0.0f, 0.8f, 1.0f);
        glRasterPos3f(-10.5f, y + 1.0f, z); 
        s2_drawBitmapText(deptNames[row % 3], GLUT_BITMAP_HELVETICA_18);
        glEnable(GL_LIGHTING);

        for (seat = -4; seat <= 4; ++seat) {
            float x = seat * deskSpacing;
            float zz = z;
            float rot = 0.0f;
            s2_drawDeskStation(x, y, zz, rot, "");
        }
    }

    s2_drawScientist(-0.6f, 0.2f, -4.5f, 0.20f, 0.33f, 0.67f, 1);
    s2_drawScientist(7.2f, 0.2f, -4.5f, 0.1f, 0.5f, 0.2f, 1);

    s2_drawScientist(-5.0f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);
    s2_drawScientist(4.0f, 0.95f, -8.0f, 0.65f, 0.40f, 0.20f, 0);
    s2_drawScientist(-6.5f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);

    s2_drawScientist(2.5f, 1.70f, -11.5f, 0.8f, 0.2f, 0.1f, 0);
    s2_drawScientist(-8.0f, 1.7f, -11.5f, 0.9f, 0.9f, 0.9f, 0);
    s2_drawScientist(-3.5f, 1.7f, -11.5f, 0.2f, 0.6f, 0.3f, 1);
    s2_drawScientist(6.0f, 1.7f, -11.5f, 0.3f, 0.3f, 0.8f, 0);

    s2_drawScientist(9.0f, 2.45f, -15.0f, 0.15f, 0.15f, 0.15f, 0);
    s2_drawScientist(-4.8f, 2.45f, -15.0f, 0.88f, 0.88f, 0.9f, 0);
    s2_drawScientist(8.5f, 2.45f, -15.0f, 0.7f, 0.4f, 0.2f, 0);
}

static void s2_drawVideoWallMainContent(void) {
    int i;
    float pulse = 0.5f + 0.5f * sinf(s2_gTime * 2.0f);

    glDisable(GL_LIGHTING);

    glColor3f(0.06f, 0.09f, 0.16f);
    glBegin(GL_QUADS);
    glVertex3f(-5.5f, -2.5f, 0.01f);
    glVertex3f(5.5f, -2.5f, 0.01f);
    glVertex3f(5.5f, 2.5f, 0.01f);
    glVertex3f(-5.5f, 2.5f, 0.01f);
    glEnd();

    glColor3f(0.22f, 0.85f, 0.28f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= 48; ++i) {
        float t = (float)i / 48.0f;
        float x = -4.1f + t * 2.6f;
        float y = -1.0f + t * 2.7f + sinf(s2_gTime * 3.0f + t * 10.0f) * 0.08f;
        glVertex3f(x, y, 0.02f);
    }
    glEnd();

    for (i = 0; i < 10; ++i) {
        float y0 = -1.5f + i * 0.32f;
        float bar = 0.3f + (sinf(s2_gTime * 2.2f + i) * 0.5f + 0.5f) * 0.9f;
        glColor3f(0.12f, 0.95f, 0.32f);
        glBegin(GL_QUADS);
        glVertex3f(-3.95f, y0, 0.02f);
        glVertex3f(-3.95f + bar, y0, 0.02f);
        glVertex3f(-3.95f + bar, y0 + 0.18f, 0.02f);
        glVertex3f(-3.95f, y0 + 0.18f, 0.02f);
        glEnd();
    }

    glColor3f(0.1f, 0.55f, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < 80; ++i) {
        float a = (float)i * 6.2831853f / 80.0f;
        glVertex3f(cosf(a) * 1.05f, sinf(a) * 1.05f, 0.02f);
    }
    glEnd();

    glColor3f(0.0f, 0.95f, 0.85f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= 64; ++i) {
        float t = (float)i / 64.0f;
        float a = t * 4.9f + s2_gTime * 0.6f;
        glVertex3f(cosf(a) * 0.9f, sinf(a * 1.3f) * 0.6f, 0.03f);
    }
    glEnd();

    glColor3f(1.0f, 0.0f, 0.5f);
    glRasterPos3f(-1.0f, -2.2f, 0.5f);
    s2_drawBitmapText("INDIA", GLUT_BITMAP_HELVETICA_18);
    glRasterPos3f(2.0f, 1.9f, 0.5f);
    s2_drawBitmapText("MOM", GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.08f, 0.08f, 0.1f);
    glBegin(GL_QUADS);
    glVertex3f(1.55f, -1.5f, 0.02f);
    glVertex3f(4.1f, -1.5f, 0.02f);
    glVertex3f(4.1f, 1.5f, 0.02f);
    glVertex3f(1.55f, 1.5f, 0.02f);
    glEnd();

    glColor3f(0.95f, 0.95f, 0.95f);
    glBegin(GL_QUADS);
    glVertex3f(2.65f, -0.9f, 0.03f);
    glVertex3f(2.95f, -0.9f, 0.03f);
    glVertex3f(2.95f, 0.85f, 0.03f);
    glVertex3f(2.65f, 0.85f, 0.03f);
    glEnd();

    glColor3f(1.0f, 0.52f, 0.18f);
    glBegin(GL_QUADS);
    glVertex3f(2.62f, -0.35f, 0.03f);
    glVertex3f(2.98f, -0.35f, 0.03f);
    glVertex3f(2.98f, -0.12f, 0.03f);
    glVertex3f(2.62f, -0.12f, 0.03f);
    glEnd();

    glColor3f(0.2f, 0.45f, 0.95f);
    glBegin(GL_QUADS);
    glVertex3f(2.62f, 0.18f, 0.03f);
    glVertex3f(2.98f, 0.18f, 0.03f);
    glVertex3f(2.98f, 0.42f, 0.03f);
    glVertex3f(2.62f, 0.42f, 0.03f);
    glEnd();

    glColor3f(0.18f + pulse * 0.5f, 0.55f + pulse * 0.35f, 0.95f);
    glBegin(GL_LINES);
    glVertex3f(1.75f, -1.2f, 0.03f);
    glVertex3f(3.9f, 1.2f, 0.03f);
    glVertex3f(1.75f, 1.2f, 0.03f);
    glVertex3f(3.9f, -1.2f, 0.03f);
    glEnd();

    glEnable(GL_LIGHTING);
}

static void s2_drawMetricScreen(const char *label, float phase, int mode) {
    int i;
    glPushMatrix();
    s2_drawEmissivePanel(1.9f, 2.6f, 0.07f, 0.11f, 0.19f);

    glDisable(GL_LIGHTING);

    glColor3f(0.02f, 0.04f, 0.08f);
    glBegin(GL_QUADS);
    glVertex3f(-0.6f, 1.0f, 0.04f); 
    glVertex3f(0.6f, 1.0f, 0.04f);
    glVertex3f(0.6f, 1.3f, 0.04f);
    glVertex3f(-0.6f, 1.3f, 0.04f);
    glEnd();

    if (mode == 0) {
        for (i = 0; i < 6; ++i) {
            float y = -0.8f + i * 0.32f;
            float w = 0.2f + 0.7f * (0.5f + 0.5f * sinf(s2_gTime * 2.1f + i + phase));
            glColor3f(0.2f, 0.95f, 0.4f);
            glBegin(GL_QUADS);
            glVertex3f(-0.5f, y, 0.01f); 
            glVertex3f(-0.5f + w, y, 0.01f);
            glVertex3f(-0.5f + w, y + 0.15f, 0.01f); 
            glVertex3f(-0.5f, y + 0.15f, 0.01f);
            glEnd();
        }
    }

    glColor3f(0.0f, 1.0f, 1.0f);
    glRasterPos3f(-1.4f, 1.5f, 0.09f);
    s2_drawBitmapText(label, GLUT_BITMAP_HELVETICA_12);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

static void s2_drawVideoWall(void) {
    int i;
    const char *labels[6] = {
        "PROPULSION", "GUIDANCE", "COMMS", "ENV CTRL", "TELEMETRY", "NAV DATA"
    };
    int modes[6] = { 0, 0, 0, 0, 0, 0 };

    s2_setColor3(0.12f, 0.12f, 0.16f);
    glPushMatrix();
    glTranslatef(0.0f, 5.5f, -31.0f);
    s2_drawBox(42.0f, 14.0f, 0.8f);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.42f);
    glScalef(2.0f, 2.0f, 1.0f);
    s2_drawVideoWallMainContent();
    glPopMatrix();

    for (i = 0; i < 3; ++i) {
        float yOffset = 3.8f - (i * 3.8f);

        glPushMatrix();
        glTranslatef(-15.5f, yOffset, 0.55f);
        s2_drawMetricScreen(labels[i], (float)i, modes[i]);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(15.5f, yOffset, 0.55f);
        s2_drawMetricScreen(labels[i + 3], (float)(i + 3), modes[i + 3]);
        glPopMatrix();
    }

    glPopMatrix();
}

static void s2_drawServerRack(float x, float y, float z, float phase) {
    int i;
    glPushMatrix();
    glTranslatef(x, y, z);

    s2_setColor3(0.14f, 0.18f, 0.22f);
    s2_drawBox(1.35f, 5.8f, 1.15f);

    glDisable(GL_LIGHTING);
    for (i = 0; i < 14; ++i) {
        float led = 0.35f + 0.65f * (0.5f + 0.5f * sinf(s2_gTime * 4.5f + i + phase));
        float y0 = -2.4f + i * 0.34f;
        glColor3f(0.08f, 0.16f + led * 0.45f, 0.85f + led * 0.15f);
        glBegin(GL_QUADS);
        glVertex3f(-0.52f, y0, 0.58f);
        glVertex3f(-0.22f, y0, 0.58f);
        glVertex3f(-0.22f, y0 + 0.12f, 0.58f);
        glVertex3f(-0.52f, y0 + 0.12f, 0.58f);
        glEnd();
    }
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

static void s2_drawServerGallery(void) {
    int c, r;

    glDisable(GL_LIGHTING);
    glColor4f(0.55f, 0.72f, 0.9f, 0.18f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glVertex3f(-20.0f, -1.0f, -25.2f);
    glVertex3f(20.0f, -1.0f, -25.2f);
    glVertex3f(20.0f, 12.0f, -25.2f);
    glVertex3f(-20.0f, 12.0f, -25.2f);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    for (r = 0; r < 3; ++r) {
        for (c = 0; c < 9; ++c) {
            float x = -13.0f + c * 3.2f;
            float z = -30.0f - r * 4.0f;
            s2_drawServerRack(x, 1.8f, z, (float)(c + r * 2));
        }
    }
}

static void s2_drawArchitecture(void) {
    s2_setColor3(0.09f, 0.1f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, -1.5f, -14.0f);
    s2_drawBox(44.0f, 0.3f, 48.0f);
    glPopMatrix();

    s2_setColor3(0.08f, 0.09f, 0.11f);
    glPushMatrix();
    glTranslatef(-22.0f, 3.25f, -14.0f);
    s2_drawBox(0.4f, 10.0f, 48.0f);
    glPopMatrix();

    s2_setColor3(0.08f, 0.09f, 0.11f);
    glPushMatrix();
    glTranslatef(-22.0f, 5.2f, -14.0f);
    s2_drawBox(0.4f, 14.0f, 48.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22.0f, 5.2f, -14.0f);
    s2_drawBox(0.4f, 14.0f, 48.0f);
    glPopMatrix();
}

static void s2_setupLights(void) {
    GLfloat coolPos[] = {-16.0f, 11.0f, 8.0f, 1.0f};
    GLfloat coolDiff[] = {0.35f, 0.58f, 0.95f, 1.0f};
    GLfloat coolAmb[] = {0.1f, 0.14f, 0.22f, 1.0f};

    GLfloat warmPos[] = {14.0f, 9.0f, -5.0f, 1.0f};
    GLfloat warmDiff[] = {0.95f, 0.62f, 0.32f, 1.0f};
    GLfloat warmAmb[] = {0.16f, 0.11f, 0.08f, 1.0f};

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT0, GL_POSITION, coolPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, coolDiff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, coolAmb);

    glLightfv(GL_LIGHT1, GL_POSITION, warmPos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, warmDiff);
    glLightfv(GL_LIGHT1, GL_AMBIENT, warmAmb);
}

static void scene2_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.0, 9.5, 14.8,
              0.0, 0.0, -24.0,
              0.0, 1.0, 0.0);

    s2_setupLights();
    s2_drawArchitecture();
    s2_drawVideoWall();
    s2_drawTieredControlGallery();
    s2_drawServerGallery();

    glutSwapBuffers();
}

static void scene2_update(float dt) {
    s2_gTime += dt;
}

#endif
