/*
 * Scene 6: Rocket Launch
 * Duration: 8 seconds
 * Extracted from rocket_launch.c - COMPLETE VERSION
 */

#ifndef SCENE_6_ROCKET_LAUNCH_H
#define SCENE_6_ROCKET_LAUNCH_H

#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static float s6_launchTime = 0.0f;
static float s6_rocketY = 0.0f;
static float s6_crashStartTime = -1.0f;
static float s6_crashOriginY = 0.0f;

static const float s6_kRocketX = 0.0f;
static const float s6_kRocketZ = -12.0f;

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float size;
    float life;
    float maxLife;
} s6_Debris;

#define S6_MAX_DEBRIS 200
static s6_Debris s6_debris[S6_MAX_DEBRIS];
static int s6_debrisCount = 0;

typedef struct {
    float x, y, z;
    float radius;
    float life;
    float maxLife;
    float r, g, b;
} s6_Explosion;

#define S6_MAX_EXPLOSIONS 50
static s6_Explosion s6_explosions[S6_MAX_EXPLOSIONS];
static int s6_explosionCount = 0;

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float rx, ry, rz;
    float avx, avy, avz;
    float length;
    float radius;
    float r, g, b;
} s6_CrashChunk;

#define S6_MAX_CHUNKS 10
static s6_CrashChunk s6_chunks[S6_MAX_CHUNKS];
static int s6_chunkCount = 0;
static int s6_chunksInitialized = 0;

static float s6_randf(float a, float b) {
    return a + (b - a) * ((float)rand() / (float)RAND_MAX);
}

static void s6_initCrashChunks(void) {
    float yBase = -1.0f + s6_crashOriginY;
    int i;

    s6_chunkCount = 6;
    if (s6_chunkCount > S6_MAX_CHUNKS) {
        s6_chunkCount = S6_MAX_CHUNKS;
    }

    const float centers[6] = {1.2f, 3.0f, 4.8f, 6.4f, 8.0f, 9.3f};
    const float lengths[6] = {2.0f, 1.9f, 1.8f, 1.7f, 1.6f, 1.2f};
    const float radii[6] =   {0.55f, 0.50f, 0.50f, 0.48f, 0.45f, 0.35f};

    for (i = 0; i < s6_chunkCount; ++i) {
        s6_chunks[i].x = s6_kRocketX;
        s6_chunks[i].y = yBase + centers[i];
        s6_chunks[i].z = s6_kRocketZ;

        s6_chunks[i].vx = s6_randf(-8.0f, 8.0f);
        s6_chunks[i].vy = s6_randf(6.0f, 16.0f);
        s6_chunks[i].vz = s6_randf(-8.0f, 8.0f);

        s6_chunks[i].rx = s6_randf(0.0f, 360.0f);
        s6_chunks[i].ry = s6_randf(0.0f, 360.0f);
        s6_chunks[i].rz = s6_randf(0.0f, 360.0f);
        s6_chunks[i].avx = s6_randf(-240.0f, 240.0f);
        s6_chunks[i].avy = s6_randf(-300.0f, 300.0f);
        s6_chunks[i].avz = s6_randf(-240.0f, 240.0f);

        s6_chunks[i].length = lengths[i];
        s6_chunks[i].radius = radii[i];

        s6_chunks[i].r = 0.78f;
        s6_chunks[i].g = 0.76f;
        s6_chunks[i].b = 0.74f;
    }

    s6_chunksInitialized = 1;
}

static void s6_updateCrashChunks(float dt) {
    int i;
    for (i = 0; i < s6_chunkCount; ++i) {
        s6_chunks[i].vy -= 18.0f * dt;
        s6_chunks[i].x += s6_chunks[i].vx * dt;
        s6_chunks[i].y += s6_chunks[i].vy * dt;
        s6_chunks[i].z += s6_chunks[i].vz * dt;

        s6_chunks[i].rx += s6_chunks[i].avx * dt;
        s6_chunks[i].ry += s6_chunks[i].avy * dt;
        s6_chunks[i].rz += s6_chunks[i].avz * dt;

        if (s6_chunks[i].y < -2.0f) {
            s6_chunks[i].y = -2.0f;
            s6_chunks[i].vx *= 0.6f;
            s6_chunks[i].vz *= 0.6f;
            s6_chunks[i].vy = 0.0f;
        }
    }
}

static void s6_drawCrashChunks(void) {
    int i;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (i = 0; i < s6_chunkCount; ++i) {
        float glow = 0.25f + 0.15f * sinf(s6_launchTime * 15.0f + (float)i);
        glColor4f(s6_chunks[i].r + glow, s6_chunks[i].g + glow * 0.5f, s6_chunks[i].b, 0.95f);

        glPushMatrix();
        glTranslatef(s6_chunks[i].x, s6_chunks[i].y, s6_chunks[i].z);
        glRotatef(s6_chunks[i].rx, 1.0f, 0.0f, 0.0f);
        glRotatef(s6_chunks[i].ry, 0.0f, 1.0f, 0.0f);
        glRotatef(s6_chunks[i].rz, 0.0f, 0.0f, 1.0f);

        glPushMatrix();
        glScalef(s6_chunks[i].radius * 2.0f, s6_chunks[i].length, s6_chunks[i].radius * 2.0f);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void s6_setColor3(float r, float g, float b) {
    glColor3f(r, g, b);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, (GLfloat[]){r, g, b, 1.0f});
}

static void s6_drawUnitBox(float sx, float sy, float sz) {
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

static void s6_drawStrokeText(const char *txt, float scale) {
    int i;
    glPushMatrix();
    glScalef(scale, scale, scale);
    for (i = 0; txt[i] != '\0'; ++i) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, txt[i]);
    }
    glPopMatrix();
}

static void s6_drawCloud(float x, float y, float z, float s) {
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

static void s6_drawSkyAndClouds(void) {
    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 0.93f, 0.85f);
    s6_drawCloud(-18.0f, 14.0f, -35.0f, 1.7f);
    s6_drawCloud(-5.0f, 12.2f, -31.0f, 1.2f);
    s6_drawCloud(14.0f, 15.0f, -37.0f, 1.5f);
    s6_drawCloud(24.0f, 13.0f, -39.0f, 1.1f);
    s6_drawCloud(-57.0f, 15.0f, -57.0f, 1.5f);
    s6_drawCloud(-38.0f, 13.0f, -14.0f, 1.1f);
    s6_drawCloud(57.0f, 15.0f, -57.0f, 1.5f);
    s6_drawCloud(38.0f, 13.0f, -14.0f, 1.1f);

    glEnable(GL_LIGHTING);
}

static void s6_drawSeaSurface(void) {
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

        float wave0 = sinf(s6_launchTime * 0.3f + z0 * 0.08f) * 0.02f;
        float wave1 = sinf(s6_launchTime * 0.3f + z1 * 0.08f) * 0.02f;

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

    {
        float refl = fminf(fmaxf((s6_rocketY * 0.12f), 0.0f), 1.0f);
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

static void s6_drawShoreline(void) {
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

static void s6_drawTreeClump(float x, float z, float h) {
    int i;
    glDisable(GL_LIGHTING);
    s6_setColor3(0.08f, 0.15f, 0.06f);
    
    glPushMatrix();
    glTranslatef(x, -1.8f, z);
    
    glPushMatrix();
    glTranslatef(0.0f, h * 0.3f, 0.0f);
    s6_drawUnitBox(0.22f, h * 0.6f, 0.22f);
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

static void s6_drawForegroundVegetation(void) {
    glDisable(GL_LIGHTING);
    glColor3f(0.06f, 0.12f, 0.04f);
    
    s6_drawTreeClump(-28.0f, -1.0f, 3.2f);
    s6_drawTreeClump(-25.5f, -0.8f, 2.8f);
    s6_drawTreeClump(-22.0f, -1.2f, 3.5f);
    s6_drawTreeClump(-18.5f, -0.5f, 2.6f);
    s6_drawTreeClump(-15.0f, -1.0f, 3.0f);
    
    s6_setColor3(0.15f, 0.15f, 0.16f);
    glPushMatrix();
    glTranslatef(26.0f, -1.8f, 3.0f);
    s6_drawUnitBox(0.35f, 14.0f, 0.35f);
    glPopMatrix();
    
    glEnable(GL_LIGHTING);
}

static void s6_addDebris(float x, float y, float z, float vx, float vy, float vz, float size) {
    if (s6_debrisCount < S6_MAX_DEBRIS) {
        s6_debris[s6_debrisCount].x = x;
        s6_debris[s6_debrisCount].y = y;
        s6_debris[s6_debrisCount].z = z;
        s6_debris[s6_debrisCount].vx = vx;
        s6_debris[s6_debrisCount].vy = vy;
        s6_debris[s6_debrisCount].vz = vz;
        s6_debris[s6_debrisCount].size = size;
        s6_debris[s6_debrisCount].life = 4.0f;
        s6_debris[s6_debrisCount].maxLife = 4.0f;
        s6_debrisCount++;
    }
}

static void s6_addExplosion(float x, float y, float z, float r, float g, float b, float radius) {
    if (s6_explosionCount < S6_MAX_EXPLOSIONS) {
        s6_explosions[s6_explosionCount].x = x;
        s6_explosions[s6_explosionCount].y = y;
        s6_explosions[s6_explosionCount].z = z;
        s6_explosions[s6_explosionCount].radius = radius;
        s6_explosions[s6_explosionCount].life = 1.6f;
        s6_explosions[s6_explosionCount].maxLife = 1.6f;
        s6_explosions[s6_explosionCount].r = r;
        s6_explosions[s6_explosionCount].g = g;
        s6_explosions[s6_explosionCount].b = b;
        s6_explosionCount++;
    }
}

static void s6_updateDebris(void) {
    int i;
    for (i = 0; i < s6_debrisCount; ++i) {
        s6_debris[i].vy -= 0.098f * 0.016f;
        s6_debris[i].x += s6_debris[i].vx * 0.016f;
        s6_debris[i].y += s6_debris[i].vy * 0.016f;
        s6_debris[i].z += s6_debris[i].vz * 0.016f;
        s6_debris[i].life -= 0.016f;
    }
}

static void s6_drawDebris(void) {
    int i;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    for (i = 0; i < s6_debrisCount; ++i) {
        float alpha = s6_debris[i].life / s6_debris[i].maxLife;
        
        glColor4f(1.0f, 0.7f, 0.2f, alpha * 0.9f);
        glPushMatrix();
        glTranslatef(s6_debris[i].x, s6_debris[i].y, s6_debris[i].z);
        glRotatef(s6_launchTime * 200.0f, 0.3f, 0.7f, 0.4f);
        
        glutSolidSphere(s6_debris[i].size * 0.8f, 12, 12);
        
        glColor4f(1.0f, 0.9f, 0.4f, alpha * 0.5f);
        glutSolidSphere(s6_debris[i].size * 0.5f, 10, 10);
        
        glPopMatrix();
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void s6_drawExplosions(void) {
    int i;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    for (i = 0; i < s6_explosionCount; ++i) {
        float alpha = s6_explosions[i].life / s6_explosions[i].maxLife;
        
        glColor4f(s6_explosions[i].r, s6_explosions[i].g, s6_explosions[i].b, alpha * 0.8f);
        glPushMatrix();
        glTranslatef(s6_explosions[i].x, s6_explosions[i].y, s6_explosions[i].z);
        glutSolidSphere(s6_explosions[i].radius, 24, 24);
        glPopMatrix();
        
        glColor4f(1.0f, 1.0f, 0.8f, alpha * 0.4f);
        glPushMatrix();
        glTranslatef(s6_explosions[i].x, s6_explosions[i].y, s6_explosions[i].z);
        glutSolidSphere(s6_explosions[i].radius * 0.6f, 20, 20);
        glPopMatrix();
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void s6_triggerExplosion(float x, float y, float z, float crashTime) {
    int i, j;
    for (i = 0; i < 100; ++i) {
        float angle1 = (float)i / 100.0f * 6.2831853f;
        float angle2 = (float)(i % 12) / 12.0f * 6.2831853f;
        float angle3 = sinf(crashTime * 15.0f + i);
        float radius = 1.5f + angle3 * 3.0f;
        
        float vx = cosf(angle1) * cosf(angle2) * radius * 25.0f;
        float vy = -3.5f - fabsf(angle3) * 12.0f;
        float vz = sinf(angle1) * cosf(angle2) * radius * 25.0f;
        
        s6_addDebris(x, y, z, vx, vy, vz, 0.2f + (float)(i % 6) * 0.10f);
    }
    
    for (j = 0; j < 6; ++j) {
        float offsetX = sinf(crashTime + j) * 1.2f;
        float offsetY = cosf(crashTime * 0.5f + j) * 1.5f;
        float offsetZ = sinf(crashTime * 1.2f + j) * 1.0f;
        s6_addExplosion(x + offsetX, y + offsetY, z + offsetZ, 1.0f, 0.6f, 0.1f, 3.5f + j * 0.8f);
    }
    
    for (j = 0; j < 5; ++j) {
        float offsetX = cosf(crashTime * 2.0f + j) * 1.8f;
        float offsetY = sinf(crashTime * 1.5f + j) * 2.0f;
        float offsetZ = cosf(crashTime * 1.8f + j) * 1.5f;
        s6_addExplosion(x + offsetX, y + offsetY + 1.0f, z + offsetZ, 0.98f, 0.45f, 0.1f, 2.8f + j * 0.6f);
    }
    
    for (j = 0; j < 6; ++j) {
        float offsetX = sinf(crashTime * 3.0f + j) * 2.0f;
        float offsetY = cosf(crashTime * 2.2f + j) * 1.2f;
        float offsetZ = sinf(crashTime * 2.5f + j) * 1.8f;
        s6_addExplosion(x + offsetX, y + offsetY + 0.5f, z + offsetZ, 0.8f, 0.75f, 0.65f, 4.0f + j * 0.9f);
    }
}

static void s6_drawLaunchTower(float x, float z, float height, float base) {
    int i;
    s6_setColor3(0.30f, 0.30f, 0.32f);

    glPushMatrix();
    glTranslatef(x, -2.0f, z);

    for (i = 0; i < 4; ++i) {
        float sx = (i & 1) ? base : -base;
        float sz = (i & 2) ? base : -base;
        glPushMatrix();
        glTranslatef(sx, height * 0.5f, sz);
        s6_drawUnitBox(0.18f, height, 0.18f);
        glPopMatrix();
    }

    s6_setColor3(0.24f, 0.24f, 0.25f);
    for (i = 0; i < 8; ++i) {
        float y = (float)i / 7.0f * height;
        glPushMatrix();
        glTranslatef(0.0f, y, 0.0f);
        s6_drawUnitBox(base * 2.2f, 0.06f, 0.06f);
        s6_drawUnitBox(0.06f, 0.06f, base * 2.2f);
        glPopMatrix();
    }

    glPopMatrix();
}

static void s6_drawServiceGantry(void) {
    s6_setColor3(0.55f, 0.43f, 0.20f);
    glPushMatrix();
    glTranslatef(s6_kRocketX + 4.0f, -2.0f, s6_kRocketZ + 1.2f);
    s6_drawUnitBox(1.8f, 11.0f, 1.8f);
    glTranslatef(-0.9f, 4.5f, 0.0f);
    s6_setColor3(0.65f, 0.50f, 0.24f);
    s6_drawUnitBox(3.0f, 0.25f, 1.4f);
    glTranslatef(0.0f, 2.4f, 0.0f);
    s6_drawUnitBox(3.2f, 0.25f, 1.4f);
    glPopMatrix();
}

static void s6_drawBird(float x, float y, float z, float s, float phase) {
    float flap = sinf(s6_launchTime * 4.0f + phase) * 0.22f;
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

static void s6_drawBirds(void) {
    s6_drawBird(-7.0f, 10.5f, -25.0f, 0.7f, 0.0f);
    s6_drawBird(-2.0f, 12.0f, -30.0f, 0.55f, 1.0f);
    s6_drawBird(6.0f, 11.0f, -22.0f, 0.65f, 2.0f);
    s6_drawBird(14.0f, 9.5f, -28.0f, 0.5f, 2.7f);
    s6_drawBird(19.0f, 11.3f, -33.0f, 0.45f, 3.2f);
}

static void s6_drawGroundAndPaths(void) {
    glNormal3f(0.0f, 1.0f, 0.0f);

    s6_setColor3(0.35f, 0.61f, 0.34f);
    glBegin(GL_QUADS);
    glVertex3f(-35.0f, -2.0f, -35.0f);
    glVertex3f(35.0f, -2.0f, -35.0f);
    glVertex3f(35.0f, -2.0f, 20.0f);
    glVertex3f(-35.0f, -2.0f, 20.0f);
    glEnd();

    s6_setColor3(0.76f, 0.74f, 0.68f);
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

static void s6_drawLaunchPad(void) {
    s6_setColor3(0.56f, 0.56f, 0.6f);
    glPushMatrix();
    glTranslatef(s6_kRocketX, -1.85f, s6_kRocketZ);
    s6_drawUnitBox(3.2f, 0.3f, 3.2f);
    glPopMatrix();
}

static void s6_drawAnimatedRocket(void) {
    if (s6_crashStartTime >= 0.0f) {
        float crashTime = s6_launchTime - s6_crashStartTime;
        if (crashTime >= 0.12f && s6_chunksInitialized && s6_chunkCount > 0) {
            s6_drawCrashChunks();
            return;
        }
    }

    GLUquadric *quad = gluNewQuadric();
    if (quad == NULL) {
        return;
    }

    float rocketX = s6_kRocketX;
    float rocketZ = s6_kRocketZ;
    float rocketY = s6_rocketY;
    float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

    if (s6_crashStartTime >= 0.0f) {
        float crashTime = s6_launchTime - s6_crashStartTime;
        rocketX = sinf(crashTime * 1.5f) * 4.0f;
        rocketZ = s6_kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
        rocketY = s6_crashOriginY - crashTime * crashTime * 8.0f;
        if (rocketY < -2.0f) rocketY = -2.0f;

        rotX = sinf(crashTime * 0.8f) * 45.0f;
        rotZ = cosf(crashTime * 1.2f) * 35.0f;
        rotY = crashTime * 120.0f;
    }

    glPushMatrix();
    glTranslatef(rocketX, -1.0f + rocketY, rocketZ);

    if (s6_crashStartTime >= 0.0f) {
        glRotatef(rotX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
        glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    }

    if (s6_crashStartTime < 0.0f && s6_rocketY > 0.1f) {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glTranslatef(0.0f, -0.5f, 0.0f);
        float flameHeight = fminf(s6_rocketY * 0.6f, 3.4f) + sinf(s6_launchTime * 50.0f) * 0.2f;
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

    if (s6_crashStartTime >= 0.0f) {
        s6_setColor3(0.72f, 0.72f, 0.72f);
    } else {
        s6_setColor3(0.97f, 0.97f, 0.97f);
    }
    gluCylinder(quad, 0.45, 0.45, 9.0, 34, 8);

    s6_setColor3(0.95f, 0.45f, 0.20f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 2.2f);
    gluCylinder(quad, 0.46, 0.46, 1.0, 34, 3);
    glPopMatrix();

    s6_setColor3(0.20f, 0.43f, 0.86f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 5.2f);
    gluCylinder(quad, 0.46, 0.46, 1.0, 34, 3);
    glPopMatrix();

    s6_setColor3(0.97f, 0.97f, 0.97f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 9.0f);
    gluCylinder(quad, 0.45, 0.0, 1.25, 34, 8);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(0.07f, 0.07f, 0.07f);

    glPushMatrix();
    glTranslatef(-0.12f, 0.54f, 4.4f);
    glScalef(0.0018f, 0.0018f, 0.0018f);
    s6_drawStrokeText("ISRO", 1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.18f, -0.56f, 4.4f);
    glScalef(0.0018f, 0.0018f, 0.0018f);
    s6_drawStrokeText("INDIA", 1.0f);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glPopMatrix();

    gluDeleteQuadric(quad);
}

static void s6_drawCelebratePerson(float x, float z, float offset) {
    float clap = sinf(s6_launchTime * 3.0f + offset) * 0.3f;
    
    glPushMatrix();
    glTranslatef(x, -1.95f, z);
    
    s6_setColor3(0.16f, 0.20f, 0.28f);
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    s6_drawUnitBox(0.28f, 0.62f, 0.16f);
    glPopMatrix();
    
    s6_setColor3(0.94f, 0.78f, 0.62f);
    glPushMatrix();
    glTranslatef(0.0f, 0.98f, 0.0f);
    glutSolidSphere(0.11, 14, 14);
    glPopMatrix();
    
    s6_setColor3(0.06f, 0.08f, 0.1f);
    
    glPushMatrix();
    glTranslatef(-0.15f, 0.35f + clap * 0.15f, 0.0f);
    glRotatef(clap * 20.0f, 0.0f, 0.0f, 1.0f);
    s6_drawUnitBox(0.08f, 0.35f, 0.08f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.15f, 0.35f + clap * 0.15f, 0.0f);
    glRotatef(-clap * 20.0f, 0.0f, 0.0f, 1.0f);
    s6_drawUnitBox(0.08f, 0.35f, 0.08f);
    glPopMatrix();
    
    glPopMatrix();
}

static void s6_drawCrashSmokePlumes(void) {
    if (s6_crashStartTime < 0.0f) return;
    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float crashTime = s6_launchTime - s6_crashStartTime;
    
    int i;
    for (i = 0; i < 25; ++i) {
        float angle = (float)i / 25.0f * 6.2831853f;
        float drift = sinf(crashTime * 0.5f + i) * 1.5f;
        float rocketX = sinf(crashTime * 1.5f) * 4.0f;
        float rocketZ = s6_kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
        
        float smokeX = rocketX + cosf(angle) * (2.0f + crashTime * 1.5f) + drift;
        float smokeY = 8.0f + crashTime * 5.0f;
        float smokeZ = rocketZ + sinf(angle) * (2.0f + crashTime * 1.5f);
        
        float greyLevel = 0.45f - crashTime * 0.1f;
        float alpha = fmaxf(0.0f, 0.8f - crashTime * 0.3f);
        glColor4f(greyLevel, greyLevel, greyLevel, alpha);
        
        float smokeSize = 1.2f + crashTime * 0.8f + sinf(crashTime * 3.0f + i) * 0.3f;
        glPushMatrix();
        glTranslatef(smokeX, smokeY, smokeZ);
        glutSolidSphere(smokeSize, 14, 14);
        glPopMatrix();
    }
    
    for (i = 0; i < 15; ++i) {
        float angle = (float)i / 15.0f * 6.2831853f;
        float drift = sinf(crashTime * 0.7f + i) * 0.9f;
        float rocketX = sinf(crashTime * 1.5f) * 4.0f;
        float rocketZ = s6_kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
        
        float smokeX = rocketX + cosf(angle) * (1.5f + crashTime * 1.2f) + drift;
        float smokeY = 10.0f + crashTime * 5.5f;
        float smokeZ = rocketZ + sinf(angle) * (1.5f + crashTime * 1.2f);
        
        float alpha = fmaxf(0.0f, 0.6f - crashTime * 0.25f);
        glColor4f(0.35f, 0.25f, 0.20f, alpha);
        
        float smokeSize = 0.8f + crashTime * 0.6f;
        glPushMatrix();
        glTranslatef(smokeX, smokeY, smokeZ);
        glutSolidSphere(smokeSize, 12, 12);
        glPopMatrix();
    }
    
    if (crashTime < 2.0f) {
        float shadowAlpha = fmaxf(0.0f, 0.3f - crashTime * 0.15f);
        glColor4f(0.1f, 0.15f, 0.2f, shadowAlpha);
        float shadowSize = 4.0f + crashTime * 2.0f;
        glPushMatrix();
        glTranslatef(sinf(crashTime * 1.5f) * 4.0f, -1.95f, s6_kRocketZ + cosf(crashTime * 1.8f) * 5.0f);
        glScalef(shadowSize, 0.1f, shadowSize * 0.8f);
        glutSolidSphere(1.0, 16, 16);
        glPopMatrix();
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void s6_drawCrashImpactSite(void) {
    if (s6_crashStartTime < 0.0f) return;
    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float crashTime = s6_launchTime - s6_crashStartTime;
    
    if (crashTime > 0.3f && crashTime < 4.0f) {
        int i;
        for (i = 0; i < 12; ++i) {
            float angle = (float)i / 12.0f * 6.2831853f;
            float splashX = s6_kRocketX + cosf(angle) * (3.0f + crashTime * 4.5f);
            float splashZ = s6_kRocketZ + sinf(angle) * (3.0f + crashTime * 4.5f);
            float splashAlpha = fmaxf(0.0f, 1.0f - crashTime * 0.35f);
            
            glColor4f(0.6f, 0.8f, 1.0f, splashAlpha * 0.5f);
            glPushMatrix();
            glTranslatef(splashX, -1.6f, splashZ);
            glScalef(0.4f, 1.5f, 0.4f);
            glutSolidSphere(0.5, 12, 12);
            glPopMatrix();
            
            glColor4f(0.8f, 0.9f, 1.0f, splashAlpha * 0.3f);
            glPushMatrix();
            glTranslatef(splashX, -1.5f, splashZ);
            glutSolidSphere(0.6f + crashTime * 0.3f, 10, 10);
            glPopMatrix();
        }
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void s6_setupLighting(void) {
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

static void scene6_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.99f, 0.61f, 0.32f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    {
        float t = s6_launchTime;
        float rocketY = s6_rocketY;

        if (s6_crashStartTime >= 0.0f) {
            float crashTime = s6_launchTime - s6_crashStartTime;
            float rocketX = sinf(crashTime * 1.5f) * 4.0f;
            float rocketZ = s6_kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
            float rocketYCrash = s6_crashOriginY - crashTime * crashTime * 8.0f;
            if (rocketYCrash < -2.0f) rocketYCrash = -2.0f;

            if (crashTime >= 0.12f && s6_chunksInitialized && s6_chunkCount > 0) {
                float cx = 0.0f, cy = 0.0f, cz = 0.0f;
                int i;
                for (i = 0; i < s6_chunkCount; ++i) {
                    cx += s6_chunks[i].x;
                    cy += s6_chunks[i].y;
                    cz += s6_chunks[i].z;
                }
                cx /= (float)s6_chunkCount;
                cy /= (float)s6_chunkCount;
                cz /= (float)s6_chunkCount;

                gluLookAt(cx - 18.0f, cy + 9.0f, cz + 16.0f,
                          cx, cy + 1.5f, cz,
                          0.0f, 1.0f, 0.0f);
            } else {
                gluLookAt(rocketX - 16.0f, (-1.0f + rocketYCrash) + 7.0f, rocketZ + 14.0f,
                          rocketX, (-1.0f + rocketYCrash) + 2.5f, rocketZ,
                          0.0f, 1.0f, 0.0f);
            }
        } else if (t < 2.0f) {
            gluLookAt(0.0f, 2.4f, 45.0f,
                      s6_kRocketX, 2.0f, s6_kRocketZ,
                      0.0f, 1.0f, 0.0f);
        } else if (t < 6.0f) {
            gluLookAt(10.0f, 3.0f + rocketY * 0.15f, 28.0f,
                      s6_kRocketX, 1.5f + rocketY * 0.55f, s6_kRocketZ,
                      0.0f, 1.0f, 0.0f);
        } else {
            gluLookAt(12.0f, 6.0f + rocketY * 0.85f, 30.0f + rocketY * 0.10f,
                      s6_kRocketX, 6.0f + rocketY * 1.15f, s6_kRocketZ,
                      0.0f, 1.0f, 0.0f);
        }
    }

    s6_drawSkyAndClouds();
    s6_drawBirds();

    s6_drawSeaSurface();
    s6_drawShoreline();
    s6_drawForegroundVegetation();

    s6_setupLighting();

    s6_drawGroundAndPaths();
    s6_drawLaunchTower(18.0f, s6_kRocketZ + 4.0f, 16.0f, 1.6f);
    s6_drawServiceGantry();

    s6_drawLaunchPad();
    s6_drawAnimatedRocket();

    s6_drawCrashSmokePlumes();
    s6_drawExplosions();
    s6_drawDebris();
    s6_drawCrashImpactSite();

    s6_drawCelebratePerson(-20.0f, -20.0f, 0.0f);
    s6_drawCelebratePerson(-16.0f, -20.0f, 0.8f);
    s6_drawCelebratePerson(-12.0f, -20.0f, 1.6f);
    s6_drawCelebratePerson(-8.0f, -20.0f, 2.4f);
    s6_drawCelebratePerson(-4.0f, -20.0f, 3.2f);

    glutSwapBuffers();
}

static void scene6_update(float dt) {
    s6_launchTime += dt;

    if (s6_crashStartTime < 0.0f && s6_launchTime > 2.0f) {
        s6_rocketY += (s6_launchTime - 2.0f) * 0.02f;
        if (s6_rocketY > 45.0f) s6_rocketY = 45.0f;
    }

    if (s6_launchTime > 8.0f && s6_crashStartTime < 0.0f) {
        s6_crashStartTime = s6_launchTime;
        s6_crashOriginY = s6_rocketY;
        s6_chunksInitialized = 0;
        s6_initCrashChunks();

        s6_triggerExplosion(s6_kRocketX, (-1.0f + s6_crashOriginY) + 7.0f, s6_kRocketZ, 0.0f);
    }

    if (s6_crashStartTime >= 0.0f) {
        float crashTime = s6_launchTime - s6_crashStartTime;
        float rocketX = sinf(crashTime * 1.5f) * 4.0f;
        float rocketZ = s6_kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
        float rocketYCrash = s6_crashOriginY - crashTime * crashTime * 8.0f;
        if (rocketYCrash < -2.0f) rocketYCrash = -2.0f;

        if (crashTime >= 0.0f) {
            s6_updateCrashChunks(dt);
        }

        if (crashTime < 1.2f && fmodf(crashTime, 0.08f) < dt) {
            float rx = sinf(crashTime * 5.0f) * 0.7f;
            float ry = cosf(crashTime * 4.0f) * 1.4f;
            float rz = sinf(crashTime * 3.5f) * 0.7f;
            s6_triggerExplosion(rocketX + rx, (-1.0f + rocketYCrash) + 2.5f + ry, rocketZ + rz, crashTime);
        }
    }

    s6_updateDebris();

    {
        int i = 0;
        while (i < s6_explosionCount) {
            s6_explosions[i].life -= dt;
            if (s6_explosions[i].life <= 0.0f) {
                s6_explosions[i] = s6_explosions[s6_explosionCount - 1];
                s6_explosionCount--;
            } else {
                i++;
            }
        }
    }
}

#endif
