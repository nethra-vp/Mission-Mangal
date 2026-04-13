/*
 * Scene 3: Timer Countdown
 * Duration: 12 seconds
 * Extracted from timer.c - COMPLETE VERSION
 */

#ifndef SCENE_3_TIMER_H
#define SCENE_3_TIMER_H

#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static float s3_gTime = 0.0f;

static void s3_setColor3(float r, float g, float b) {
    GLfloat mat[4] = {r, g, b, 1.0f};
    glColor3f(r, g, b);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
}

static void s3_drawBox(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

static void s3_drawBitmapText(const char *txt, void *font) {
    int i;
    for (i = 0; txt[i] != '\0'; ++i) {
        glutBitmapCharacter(font, txt[i]);
    }
}

static void s3_drawSevenSegDigitGlyph(float x0, float yBase, float dW, float dH, float thick, int digit, float pulse, float alpha) {
    unsigned char segMask[10] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66,
        0x6D, 0x7D, 0x07, 0x7F, 0x6F
    };
    unsigned char m;
    float sr, sg, sb;
    float dr = 0.12f;
    float dg = 0.02f;
    float db = 0.02f;

    if (digit < 0 || digit > 9) {
        return;
    }

    m = segMask[digit];
    sr = 0.88f + pulse * 0.12f;
    sg = 0.08f + pulse * 0.04f;
    sb = 0.08f + pulse * 0.03f;

    /* A */
    glColor4f((m & 0x01) ? sr : dr, (m & 0x01) ? sg : dg, (m & 0x01) ? sb : db, alpha);
    glBegin(GL_QUADS);
    glVertex3f(x0 + thick, yBase + dH - thick, 0.04f);
    glVertex3f(x0 + dW - thick, yBase + dH - thick, 0.04f);
    glVertex3f(x0 + dW - thick, yBase + dH, 0.04f);
    glVertex3f(x0 + thick, yBase + dH, 0.04f);
    glEnd();

    /* B */
    glColor4f((m & 0x02) ? sr : dr, (m & 0x02) ? sg : dg, (m & 0x02) ? sb : db, alpha);
    glBegin(GL_QUADS);
    glVertex3f(x0 + dW - thick, yBase + dH * 0.5f + 0.03f, 0.04f);
    glVertex3f(x0 + dW, yBase + dH * 0.5f + 0.03f, 0.04f);
    glVertex3f(x0 + dW, yBase + dH - thick, 0.04f);
    glVertex3f(x0 + dW - thick, yBase + dH - thick, 0.04f);
    glEnd();

    /* C */
    glColor4f((m & 0x04) ? sr : dr, (m & 0x04) ? sg : dg, (m & 0x04) ? sb : db, alpha);
    glBegin(GL_QUADS);
    glVertex3f(x0 + dW - thick, yBase + thick, 0.04f);
    glVertex3f(x0 + dW, yBase + thick, 0.04f);
    glVertex3f(x0 + dW, yBase + dH * 0.5f - 0.03f, 0.04f);
    glVertex3f(x0 + dW - thick, yBase + dH * 0.5f - 0.03f, 0.04f);
    glEnd();

    /* D */
    glColor4f((m & 0x08) ? sr : dr, (m & 0x08) ? sg : dg, (m & 0x08) ? sb : db, alpha);
    glBegin(GL_QUADS);
    glVertex3f(x0 + thick, yBase, 0.04f);
    glVertex3f(x0 + dW - thick, yBase, 0.04f);
    glVertex3f(x0 + dW - thick, yBase + thick, 0.04f);
    glVertex3f(x0 + thick, yBase + thick, 0.04f);
    glEnd();

    /* E */
    glColor4f((m & 0x10) ? sr : dr, (m & 0x10) ? sg : dg, (m & 0x10) ? sb : db, alpha);
    glBegin(GL_QUADS);
    glVertex3f(x0, yBase + thick, 0.04f);
    glVertex3f(x0 + thick, yBase + thick, 0.04f);
    glVertex3f(x0 + thick, yBase + dH * 0.5f - 0.03f, 0.04f);
    glVertex3f(x0, yBase + dH * 0.5f - 0.03f, 0.04f);
    glEnd();

    /* F */
    glColor4f((m & 0x20) ? sr : dr, (m & 0x20) ? sg : dg, (m & 0x20) ? sb : db, alpha);
    glBegin(GL_QUADS);
    glVertex3f(x0, yBase + dH * 0.5f + 0.03f, 0.04f);
    glVertex3f(x0 + thick, yBase + dH * 0.5f + 0.03f, 0.04f);
    glVertex3f(x0 + thick, yBase + dH - thick, 0.04f);
    glVertex3f(x0, yBase + dH - thick, 0.04f);
    glEnd();

    /* G */
    glColor4f((m & 0x40) ? sr : dr, (m & 0x40) ? sg : dg, (m & 0x40) ? sb : db, alpha);
    glBegin(GL_QUADS);
    glVertex3f(x0 + thick, yBase + dH * 0.5f - thick * 0.5f, 0.04f);
    glVertex3f(x0 + dW - thick, yBase + dH * 0.5f - thick * 0.5f, 0.04f);
    glVertex3f(x0 + dW - thick, yBase + dH * 0.5f + thick * 0.5f, 0.04f);
    glVertex3f(x0 + thick, yBase + dH * 0.5f + thick * 0.5f, 0.04f);
    glEnd();
}

static void s3_drawVideoWallMainContent(void) {
    int i;
    int totalSec;
    int hours;
    int minutes;
    int seconds;
    int prevSeconds;
    int nextSeconds;
    int secTens;
    int secOnes;
    int prevTens;
    int prevOnes;
    int nextTens;
    int nextOnes;
    float loopCountdown;
    float secPhase;
    float approach;
    float pulse = 0.5f + 0.5f * sinf(s3_gTime * 7.0f);
    int digits[8];
    const float startX = -4.15f;
    const float yBase = -0.3f;
    const float dW = 0.82f;
    const float dH = 1.42f;
    const float thick = 0.12f;

    loopCountdown = 83.0f - fmodf(s3_gTime, 84.0f);
    if (loopCountdown < 0.0f) {
        loopCountdown = 0.0f;
    }
    totalSec = (int)floorf(loopCountdown);
    secPhase = loopCountdown - (float)totalSec;
    approach = 1.0f - secPhase;

    hours = totalSec / 3600;
    minutes = (totalSec % 3600) / 60;
    seconds = totalSec % 60;

    digits[0] = (hours / 10) % 10;
    digits[1] = hours % 10;
    digits[2] = -1;
    digits[3] = (minutes / 10) % 10;
    digits[4] = minutes % 10;
    digits[5] = -1;
    digits[6] = (seconds / 10) % 10;
    digits[7] = seconds % 10;

    prevSeconds = (seconds + 1) % 60;
    nextSeconds = (seconds + 59) % 60;
    secTens = digits[6];
    secOnes = digits[7];
    prevTens = prevSeconds / 10;
    prevOnes = prevSeconds % 10;
    nextTens = nextSeconds / 10;
    nextOnes = nextSeconds % 10;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor3f(0.015f, 0.015f, 0.018f);
    glBegin(GL_QUADS);
    glVertex3f(-5.5f, -2.5f, 0.01f);
    glVertex3f(5.5f, -2.5f, 0.01f);
    glVertex3f(5.5f, 2.5f, 0.01f);
    glVertex3f(-5.5f, 2.5f, 0.01f);
    glEnd();

    glColor3f(0.04f, 0.22f, 0.16f);
    for (i = 0; i < 10; ++i) {
        float x = -5.2f + i * 1.15f;
        glBegin(GL_LINES);
        glVertex3f(x, -2.3f, 0.015f);
        glVertex3f(x, 2.3f, 0.015f);
        glEnd();
    }
    for (i = 0; i < 6; ++i) {
        float y = -2.1f + i * 0.84f;
        glBegin(GL_LINES);
        glVertex3f(-5.2f, y, 0.015f);
        glVertex3f(5.2f, y, 0.015f);
        glEnd();
    }

    glColor3f(0.16f, 0.68f, 0.95f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < 64; ++i) {
        float a = (float)i * 6.2831853f / 64.0f;
        glVertex3f(-3.95f + cosf(a) * 0.55f, 1.45f + sinf(a) * 0.38f, 0.02f);
    }
    glEnd();

    glColor3f(0.95f, 0.45f, 0.2f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < 64; ++i) {
        float a = (float)i * 6.2831853f / 64.0f;
        glVertex3f(-2.65f + cosf(a) * 0.75f, 1.45f + sinf(a) * 0.5f, 0.02f);
    }
    glEnd();

    glColor3f(0.3f, 1.0f, 0.75f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= 40; ++i) {
        float t = (float)i / 40.0f;
        float x = -3.95f + t * 1.3f;
        float y = 1.45f + sinf(t * 4.8f + s3_gTime * 0.7f) * 0.18f;
        glVertex3f(x, y, 0.025f);
    }
    glEnd();

    for (i = 0; i < 9; ++i) {
        float level = 0.2f + (0.5f + 0.5f * sinf(s3_gTime * 4.5f + i * 0.8f)) * 1.1f;
        float y = -1.65f + i * 0.34f;
        glColor3f(0.12f, 0.95f, 0.35f);
        glBegin(GL_QUADS);
        glVertex3f(5.9f, y, 0.02f);
        glVertex3f(5.9f + level, y, 0.02f);
        glVertex3f(5.9f + level, y + 0.17f, 0.02f);
        glVertex3f(5.9f, y + 0.17f, 0.02f);
        glEnd();
    }

    for (i = 0; i < 8; ++i) {
        float x0 = startX + i * 1.1f;
        if (digits[i] == -1) {
            glColor4f(1.0f, 0.08f + pulse * 0.05f, 0.08f, 1.0f);
            glBegin(GL_QUADS);
            glVertex3f(x0 + 0.25f, yBase + 0.95f, 0.04f);
            glVertex3f(x0 + 0.38f, yBase + 0.95f, 0.04f);
            glVertex3f(x0 + 0.38f, yBase + 1.08f, 0.04f);
            glVertex3f(x0 + 0.25f, yBase + 1.08f, 0.04f);
            glVertex3f(x0 + 0.25f, yBase + 0.35f, 0.04f);
            glVertex3f(x0 + 0.38f, yBase + 0.35f, 0.04f);
            glVertex3f(x0 + 0.38f, yBase + 0.48f, 0.04f);
            glVertex3f(x0 + 0.25f, yBase + 0.48f, 0.04f);
            glEnd();
            continue;
        }

        s3_drawSevenSegDigitGlyph(x0, yBase, dW, dH, thick, digits[i], pulse, 1.0f);

        if (i == 6 || i == 7) {
            float ghostIn = 0.08f + 0.32f * approach;
            float ghostOut = 0.08f + 0.28f * (1.0f - approach);
            int dPrev = (i == 6) ? prevTens : prevOnes;
            int dNow = (i == 6) ? secTens : secOnes;
            int dNext = (i == 6) ? nextTens : nextOnes;

            s3_drawSevenSegDigitGlyph(x0 - 0.06f * (1.0f - approach), yBase, dW, dH, thick, dPrev, pulse, ghostOut);
            s3_drawSevenSegDigitGlyph(x0 + 0.02f * sinf(s3_gTime * 40.0f), yBase, dW, dH, thick, dNow, pulse, 0.78f);
            s3_drawSevenSegDigitGlyph(x0 + 0.06f * approach, yBase, dW, dH, thick, dNext, pulse, ghostIn);
        }

        glColor4f(1.0f, 0.09f, 0.09f, 0.1f + pulse * 0.09f);
        glBegin(GL_QUADS);
        glVertex3f(x0 - 0.08f, yBase - 0.08f, 0.03f);
        glVertex3f(x0 + dW + 0.08f, yBase - 0.08f, 0.03f);
        glVertex3f(x0 + dW + 0.08f, yBase + dH + 0.08f, 0.03f);
        glVertex3f(x0 - 0.08f, yBase + dH + 0.08f, 0.03f);
        glEnd();
    }
    glDisable(GL_BLEND);

    glColor3f(0.16f, 0.98f, 0.36f);
    glRasterPos3f(-1.85f, -1.3f, 0.5f);
    s3_drawBitmapText("T-MINUS COUNTDOWN", GLUT_BITMAP_HELVETICA_18);
    glRasterPos3f(-2.1f, -1.72f, 0.5f);
    s3_drawBitmapText("STATUS: GO FOR LAUNCH", GLUT_BITMAP_HELVETICA_18);

    glEnable(GL_LIGHTING);
}

static void s3_drawVideoWall(void) {
    s3_setColor3(0.12f, 0.12f, 0.16f);
    glPushMatrix();
    glTranslatef(0.0f, 5.5f, -31.0f);
    s3_drawBox(42.0f, 14.0f, 0.8f);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.42f);
    glScalef(2.0f, 2.0f, 1.0f);
    s3_drawVideoWallMainContent();
    glPopMatrix();

    glPopMatrix();
}

static void s3_drawScientist(float x, float y, float z, float r, float g, float b, int gesture) {
    float arm = 0.35f + 0.15f * sinf(s3_gTime * 2.0f + x);
    glPushMatrix();
    glTranslatef(x, y, z);

    s3_setColor3(r, g, b);
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    s3_drawBox(0.32f, 0.72f, 0.2f);
    glPopMatrix();

    s3_setColor3(0.92f, 0.76f, 0.62f);
    glPushMatrix();
    glTranslatef(0.0f, 1.02f, 0.0f);
    glutSolidSphere(0.12f, 14, 14);
    glPopMatrix();

    s3_setColor3(0.12f, 0.12f, 0.14f);
    glPushMatrix();
    glTranslatef(0.0f, 1.14f, 0.0f);
    s3_drawBox(0.22f, 0.08f, 0.22f);
    glPopMatrix();

    s3_setColor3(r * 0.8f, g * 0.8f, b * 0.8f);
    glPushMatrix();
    glTranslatef(-0.18f, 0.65f + (gesture ? arm : 0.0f), 0.0f);
    s3_drawBox(0.07f, 0.45f, 0.07f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.18f, 0.65f + (gesture ? -arm * 0.5f : 0.0f), 0.0f);
    s3_drawBox(0.07f, 0.45f, 0.07f);
    glPopMatrix();

    glPopMatrix();
}

static void s3_drawDeskStation(float x, float y, float z, float rotY, const char *dept) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    s3_setColor3(0.18f, 0.19f, 0.22f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    s3_drawBox(1.3f, 0.08f, 0.6f);
    glPopMatrix();

    s3_setColor3(0.14f, 0.14f, 0.18f);
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);
    s3_drawBox(1.25f, 0.25f, 0.54f);
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
    s3_drawBitmapText(dept, GLUT_BITMAP_8_BY_13);
    glEnable(GL_LIGHTING);

    s3_setColor3(0.08f, 0.08f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, -0.06f, -0.18f);
    s3_drawBox(0.6f, 0.28f, 0.5f);
    glPopMatrix();

    glPopMatrix();
}

static void s3_drawTieredControlGallery(void) {
    int row, seat;
    float baseZ = -4.0f;
    float deskSpacing = 2.2f;
    const char *deptNames[3] = {"PROPULSION", "GUIDANCE", "TELEMETRY"};

    for (row = 0; row < 4; ++row) {
        float y = -0.6f + row * 0.75f;
        float z = baseZ - row * 3.5f;

        s3_setColor3(0.16f, 0.17f, 0.2f);
        glPushMatrix();
        glTranslatef(0.0f, y - 0.2f, z - 0.5f);
        s3_drawBox(22.0f, 0.35f, 3.0f);
        glPopMatrix();

        glDisable(GL_LIGHTING);
        glColor3f(0.0f, 0.8f, 1.0f);
        glRasterPos3f(-10.5f, y + 1.0f, z); 
        s3_drawBitmapText(deptNames[row % 3], GLUT_BITMAP_HELVETICA_18);
        glEnable(GL_LIGHTING);

        for (seat = -4; seat <= 4; ++seat) {
            float x = seat * deskSpacing;
            float zz = z;
            float rot = 0.0f;
            s3_drawDeskStation(x, y, zz, rot, "");
        }
    }

    s3_drawScientist(-0.6f, 0.2f, -4.5f, 0.20f, 0.33f, 0.67f, 1);
    s3_drawScientist(7.2f, 0.2f, -4.5f, 0.1f, 0.5f, 0.2f, 1);

    s3_drawScientist(-5.0f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);
    s3_drawScientist(4.0f, 0.95f, -8.0f, 0.65f, 0.40f, 0.20f, 0);
    s3_drawScientist(-6.5f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);

    s3_drawScientist(2.5f, 1.70f, -11.5f, 0.8f, 0.2f, 0.1f, 0);
    s3_drawScientist(-8.0f, 1.7f, -11.5f, 0.9f, 0.9f, 0.9f, 0);
    s3_drawScientist(-3.5f, 1.7f, -11.5f, 0.2f, 0.6f, 0.3f, 1);
    s3_drawScientist(6.0f, 1.7f, -11.5f, 0.3f, 0.3f, 0.8f, 0);

    s3_drawScientist(9.0f, 2.45f, -15.0f, 0.15f, 0.15f, 0.15f, 0);
    s3_drawScientist(-4.8f, 2.45f, -15.0f, 0.88f, 0.88f, 0.9f, 0);
    s3_drawScientist(8.5f, 2.45f, -15.0f, 0.7f, 0.4f, 0.2f, 0);
}

static void s3_drawServerRack(float x, float y, float z, float phase) {
    int i;
    glPushMatrix();
    glTranslatef(x, y, z);

    s3_setColor3(0.14f, 0.18f, 0.22f);
    s3_drawBox(1.35f, 5.8f, 1.15f);

    glDisable(GL_LIGHTING);
    for (i = 0; i < 14; ++i) {
        float led = 0.35f + 0.65f * (0.5f + 0.5f * sinf(s3_gTime * 4.5f + i + phase));
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

static void s3_drawServerGallery(void) {
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
            s3_drawServerRack(x, 1.8f, z, (float)(c + r * 2));
        }
    }
}

static void s3_drawArchitecture(void) {
    s3_setColor3(0.09f, 0.1f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, -1.5f, -14.0f);
    s3_drawBox(44.0f, 0.3f, 48.0f);
    glPopMatrix();

    s3_setColor3(0.08f, 0.09f, 0.11f);
    glPushMatrix();
    glTranslatef(-22.0f, 3.25f, -14.0f);
    s3_drawBox(0.4f, 10.0f, 48.0f);
    glPopMatrix();

    s3_setColor3(0.08f, 0.09f, 0.11f);
    glPushMatrix();
    glTranslatef(-22.0f, 5.2f, -14.0f);
    s3_drawBox(0.4f, 14.0f, 48.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(22.0f, 5.2f, -14.0f);
    s3_drawBox(0.4f, 14.0f, 48.0f);
    glPopMatrix();
}

static void s3_setupLights(void) {
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

static void scene3_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.0, 9.5, 14.8,
              0.0, 0.0, -24.0,
              0.0, 1.0, 0.0);

    s3_setupLights();
    s3_drawArchitecture();
    s3_drawVideoWall();
    s3_drawTieredControlGallery();
    s3_drawServerGallery();

    glutSwapBuffers();
}

static void scene3_update(float dt) {
    s3_gTime += dt;
}

#endif
