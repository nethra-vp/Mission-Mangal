#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static float g_launchTime = 0.0f;
static float g_rocketY = 0.0f;
static float g_crashStartTime = -1.0f;
static float g_crashOriginY = 0.0f;

static const float kRocketX = 0.0f;
static const float kRocketZ = -12.0f;

static void drawUnitBox(float sx, float sy, float sz);

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float size;
    float life;
    float maxLife;
} Debris;

#define MAX_DEBRIS 200
static Debris g_debris[MAX_DEBRIS];
static int g_debrisCount = 0;

typedef struct {
    float x, y, z;
    float radius;
    float life;
    float maxLife;
    float r, g, b;
} Explosion;

#define MAX_EXPLOSIONS 50
static Explosion g_explosions[MAX_EXPLOSIONS];
static int g_explosionCount = 0;

typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float rx, ry, rz;
    float avx, avy, avz;
    float length;
    float radius;
    float r, g, b;
} CrashChunk;

#define MAX_CHUNKS 10
static CrashChunk g_chunks[MAX_CHUNKS];
static int g_chunkCount = 0;
static int g_chunksInitialized = 0;

static float randf(float a, float b) {
    return a + (b - a) * ((float)rand() / (float)RAND_MAX);
}

static void initCrashChunks(void) {
    /* Initialize a few big rocket pieces that tumble and fall. */
    float yBase = -1.0f + g_crashOriginY;
    int i;

    g_chunkCount = 6;
    if (g_chunkCount > MAX_CHUNKS) {
        g_chunkCount = MAX_CHUNKS;
    }

    /* Segment centers along the rocket body (approx 10 units tall). */
    const float centers[6] = {1.2f, 3.0f, 4.8f, 6.4f, 8.0f, 9.3f};
    const float lengths[6] = {2.0f, 1.9f, 1.8f, 1.7f, 1.6f, 1.2f};
    const float radii[6] =   {0.55f, 0.50f, 0.50f, 0.48f, 0.45f, 0.35f};

    for (i = 0; i < g_chunkCount; ++i) {
        g_chunks[i].x = kRocketX;
        g_chunks[i].y = yBase + centers[i];
        g_chunks[i].z = kRocketZ;

        /* Scatter outward; keep upward impulse so pieces are visible */
        g_chunks[i].vx = randf(-8.0f, 8.0f);
        g_chunks[i].vy = randf(6.0f, 16.0f);
        g_chunks[i].vz = randf(-8.0f, 8.0f);

        g_chunks[i].rx = randf(0.0f, 360.0f);
        g_chunks[i].ry = randf(0.0f, 360.0f);
        g_chunks[i].rz = randf(0.0f, 360.0f);
        g_chunks[i].avx = randf(-240.0f, 240.0f);
        g_chunks[i].avy = randf(-300.0f, 300.0f);
        g_chunks[i].avz = randf(-240.0f, 240.0f);

        g_chunks[i].length = lengths[i];
        g_chunks[i].radius = radii[i];

        /* Slightly charred colors after breakup */
        g_chunks[i].r = 0.78f;
        g_chunks[i].g = 0.76f;
        g_chunks[i].b = 0.74f;
    }

    g_chunksInitialized = 1;
}

static void updateCrashChunks(float dt) {
    int i;
    for (i = 0; i < g_chunkCount; ++i) {
        /* Gravity */
        g_chunks[i].vy -= 18.0f * dt;

        g_chunks[i].x += g_chunks[i].vx * dt;
        g_chunks[i].y += g_chunks[i].vy * dt;
        g_chunks[i].z += g_chunks[i].vz * dt;

        g_chunks[i].rx += g_chunks[i].avx * dt;
        g_chunks[i].ry += g_chunks[i].avy * dt;
        g_chunks[i].rz += g_chunks[i].avz * dt;

        /* Keep from going far below ground */
        if (g_chunks[i].y < -2.0f) {
            g_chunks[i].y = -2.0f;
            g_chunks[i].vx *= 0.6f;
            g_chunks[i].vz *= 0.6f;
            g_chunks[i].vy = 0.0f;
        }
    }
}

static void drawCrashChunks(void) {
    int i;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (i = 0; i < g_chunkCount; ++i) {
        float glow = 0.25f + 0.15f * sinf(g_launchTime * 15.0f + (float)i);
        glColor4f(g_chunks[i].r + glow, g_chunks[i].g + glow * 0.5f, g_chunks[i].b, 0.95f);

        glPushMatrix();
        glTranslatef(g_chunks[i].x, g_chunks[i].y, g_chunks[i].z);
        glRotatef(g_chunks[i].rx, 1.0f, 0.0f, 0.0f);
        glRotatef(g_chunks[i].ry, 0.0f, 1.0f, 0.0f);
        glRotatef(g_chunks[i].rz, 0.0f, 0.0f, 1.0f);

        /* Represent each chunk as a thick capsule-like box segment */
        drawUnitBox(g_chunks[i].radius * 2.0f, g_chunks[i].length, g_chunks[i].radius * 2.0f);

        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

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

static void addDebris(float x, float y, float z, float vx, float vy, float vz, float size) {
    if (g_debrisCount < MAX_DEBRIS) {
        g_debris[g_debrisCount].x = x;
        g_debris[g_debrisCount].y = y;
        g_debris[g_debrisCount].z = z;
        g_debris[g_debrisCount].vx = vx;
        g_debris[g_debrisCount].vy = vy;
        g_debris[g_debrisCount].vz = vz;
        g_debris[g_debrisCount].size = size;
        g_debris[g_debrisCount].life = 4.0f;
        g_debris[g_debrisCount].maxLife = 4.0f;
        g_debrisCount++;
    }
}

static void addExplosion(float x, float y, float z, float r, float g, float b, float radius) {
    if (g_explosionCount < MAX_EXPLOSIONS) {
        g_explosions[g_explosionCount].x = x;
        g_explosions[g_explosionCount].y = y;
        g_explosions[g_explosionCount].z = z;
        g_explosions[g_explosionCount].radius = radius;
        g_explosions[g_explosionCount].life = 1.6f;
        g_explosions[g_explosionCount].maxLife = 1.6f;
        g_explosions[g_explosionCount].r = r;
        g_explosions[g_explosionCount].g = g;
        g_explosions[g_explosionCount].b = b;
        g_explosionCount++;
    }
}

static void updateDebris(void) {
    int i;
    for (i = 0; i < g_debrisCount; ++i) {
        g_debris[i].vy -= 0.098f * 0.016f;
        g_debris[i].x += g_debris[i].vx * 0.016f;
        g_debris[i].y += g_debris[i].vy * 0.016f;
        g_debris[i].z += g_debris[i].vz * 0.016f;
        g_debris[i].life -= 0.016f;
    }
}

static void drawDebris(void) {
    int i;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    for (i = 0; i < g_debrisCount; ++i) {
        float alpha = g_debris[i].life / g_debris[i].maxLife;
        
        /* Burning debris with glow - orange/yellow fire */
        glColor4f(1.0f, 0.7f, 0.2f, alpha * 0.9f);
        glPushMatrix();
        glTranslatef(g_debris[i].x, g_debris[i].y, g_debris[i].z);
        glRotatef(g_launchTime * 200.0f, 0.3f, 0.7f, 0.4f);
        
        /* Draw glowing core */
        glutSolidSphere(g_debris[i].size * 0.8f, 12, 12);
        
        /* Draw bright inner glow */
        glColor4f(1.0f, 0.9f, 0.4f, alpha * 0.5f);
        glutSolidSphere(g_debris[i].size * 0.5f, 10, 10);
        
        glPopMatrix();
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void drawExplosions(void) {
    int i;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    
    for (i = 0; i < g_explosionCount; ++i) {
        float alpha = g_explosions[i].life / g_explosions[i].maxLife;
        
        /* Main explosion sphere */
        glColor4f(g_explosions[i].r, g_explosions[i].g, g_explosions[i].b, alpha * 0.8f);
        glPushMatrix();
        glTranslatef(g_explosions[i].x, g_explosions[i].y, g_explosions[i].z);
        glutSolidSphere(g_explosions[i].radius, 24, 24);
        glPopMatrix();
        
        /* Inner bright core for intensity */
        glColor4f(1.0f, 1.0f, 0.8f, alpha * 0.4f);
        glPushMatrix();
        glTranslatef(g_explosions[i].x, g_explosions[i].y, g_explosions[i].z);
        glutSolidSphere(g_explosions[i].radius * 0.6f, 20, 20);
        glPopMatrix();
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void triggerExplosion(float x, float y, float z, float crashTime) {
    int i, j;
    /* Create massive debris fragments with strong outward and downward velocities - match reference image */
    for (i = 0; i < 100; ++i) {
        float angle1 = (float)i / 100.0f * 6.2831853f;
        float angle2 = (float)(i % 12) / 12.0f * 6.2831853f;
        float angle3 = sinf(crashTime * 15.0f + i);
        float radius = 1.5f + angle3 * 3.0f;
        
        /* Strong horizontal spread for pieces to fall into water, downward velocity */
        float vx = cosf(angle1) * cosf(angle2) * radius * 25.0f;
        float vy = -3.5f - fabsf(angle3) * 12.0f;  /* Strongly downward */
        float vz = sinf(angle1) * cosf(angle2) * radius * 25.0f;
        
        addDebris(x, y, z, vx, vy, vz, 0.2f + (float)(i % 6) * 0.10f);
    }
    
    /* MASSIVE explosion blooms - orange, red, and grey like reference image */
    /* Center explosion - intense orange/red core */
    for (j = 0; j < 6; ++j) {
        float offsetX = sinf(crashTime + j) * 1.2f;
        float offsetY = cosf(crashTime * 0.5f + j) * 1.5f;
        float offsetZ = sinf(crashTime * 1.2f + j) * 1.0f;
        addExplosion(x + offsetX, y + offsetY, z + offsetZ, 1.0f, 0.6f, 0.1f, 3.5f + j * 0.8f);
    }
    
    /* Red/orange mid explosions */
    for (j = 0; j < 5; ++j) {
        float offsetX = cosf(crashTime * 2.0f + j) * 1.8f;
        float offsetY = sinf(crashTime * 1.5f + j) * 2.0f;
        float offsetZ = cosf(crashTime * 1.8f + j) * 1.5f;
        addExplosion(x + offsetX, y + offsetY + 1.0f, z + offsetZ, 0.98f, 0.45f, 0.1f, 2.8f + j * 0.6f);
    }
    
    /* Large grey/white outer explosions - main smoke cloud */
    for (j = 0; j < 6; ++j) {
        float offsetX = sinf(crashTime * 3.0f + j) * 2.0f;
        float offsetY = cosf(crashTime * 2.2f + j) * 1.2f;
        float offsetZ = sinf(crashTime * 2.5f + j) * 1.8f;
        addExplosion(x + offsetX, y + offsetY + 0.5f, z + offsetZ, 0.8f, 0.75f, 0.65f, 4.0f + j * 0.9f);
    }
}

static void drawLaunchTower(float x, float z, float height, float base) {
    int i;
    setColor3(0.30f, 0.30f, 0.32f);

    glPushMatrix();
    glTranslatef(x, -2.0f, z);

    /* Four legs */
    for (i = 0; i < 4; ++i) {
        float sx = (i & 1) ? base : -base;
        float sz = (i & 2) ? base : -base;
        glPushMatrix();
        glTranslatef(sx, height * 0.5f, sz);
        drawUnitBox(0.18f, height, 0.18f);
        glPopMatrix();
    }

    /* Cross beams */
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

static void drawServiceGantry(void) {
    /* A simple gantry near the rocket. */
    setColor3(0.55f, 0.43f, 0.20f);
    glPushMatrix();
    glTranslatef(kRocketX + 4.0f, -2.0f, kRocketZ + 1.2f);
    drawUnitBox(1.8f, 11.0f, 1.8f);
    glTranslatef(-0.9f, 4.5f, 0.0f);
    setColor3(0.65f, 0.50f, 0.24f);
    drawUnitBox(3.0f, 0.25f, 1.4f);
    glTranslatef(0.0f, 2.4f, 0.0f);
    drawUnitBox(3.2f, 0.25f, 1.4f);
    glPopMatrix();
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

static void drawRoundedCanopy(float radius, int slices, float flattenY) {
    int i, j;
    for (i = 0; i < slices; ++i) {
        float lat0 = 3.1415926f * (-0.5f + (float)i / slices);
        float z0 = sinf(lat0);
        float zr0 = cosf(lat0);
        float lat1 = 3.1415926f * (-0.5f + (float)(i + 1) / slices);
        float z1 = sinf(lat1);
        float zr1 = cosf(lat1);

        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= slices * 2; ++j) {
            float lng = 2.0f * 3.1415926f * (float)j / (slices * 2);
            float x = cosf(lng);
            float y = sinf(lng);
            glNormal3f(x * zr0, y * zr0 * flattenY, z0);
            glVertex3f(radius * x * zr0, radius * y * zr0 * flattenY, radius * z0);
            glNormal3f(x * zr1, y * zr1 * flattenY, z1);
            glVertex3f(radius * x * zr1, radius * y * zr1 * flattenY, radius * z1);
        }
        glEnd();
    }
}

static void drawMangoTree(float x, float z, float h) {
    int i;
    glPushMatrix();
    glTranslatef(x, -2.0f, z);

    setColor3(0.44f, 0.27f, 0.15f);
    glPushMatrix();
    glTranslatef(0.0f, h * 0.5f, 0.0f);
    drawUnitBox(0.62f, h, 0.62f);
    glPopMatrix();

    setColor3(0.17f, 0.42f, 0.16f);
    glTranslatef(0.0f, h + 0.45f, 0.0f);
    for (i = 0; i < 7; ++i) {
        glPushMatrix();
        glRotatef(i * 51.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.6f, (i % 2) ? 0.18f : -0.06f, 0.0f);
        drawRoundedCanopy(1.05f, 8, 0.72f);
        glPopMatrix();
    }

    glPopMatrix();
}

static void drawFlowerBush(float x, float z, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, -1.8f, z);
    setColor3(0.18f, 0.50f, 0.21f);
    glutSolidSphere(0.45, 20, 20);

    setColor3(r, g, b);
    glTranslatef(0.15f, 0.2f, 0.1f);
    glutSolidSphere(0.10, 14, 14);
    glTranslatef(-0.28f, 0.0f, -0.1f);
    glutSolidSphere(0.10, 14, 14);
    glTranslatef(0.13f, 0.05f, 0.24f);
    glutSolidSphere(0.10, 14, 14);
    glPopMatrix();
}

static void drawFlowerBed(float x, float z, float width, float depth) {
    int i;
    for (i = 0; i < 9; ++i) {
        float t = (float)i / 8.0f;
        float px = x - width * 0.5f + t * width;
        float pz = z + sinf(t * 6.2831853f) * depth;

        drawFlowerBush(px, pz, 0.73f, 0.10f, 0.16f);
        drawFlowerBush(px + 0.35f, pz - 0.45f, 0.95f, 0.35f, 0.60f);
        drawFlowerBush(px - 0.32f, pz + 0.36f, 0.96f, 0.76f, 0.13f);
    }
}

static void drawWindowGrid(float startX, float startY, float z, int cols, int rows) {
    int c, r;
    for (r = 0; r < rows; ++r) {
        for (c = 0; c < cols; ++c) {
            glPushMatrix();
            glTranslatef(startX + c * 1.2f, startY + r * 0.95f, z);
            drawUnitBox(0.85f, 0.58f, 0.06f);
            glPopMatrix();
        }
    }
}

static void drawFacadePattern(void) {
    int i;
    setColor3(0.72f, 0.57f, 0.40f);
    for (i = 0; i < 7; ++i) {
        float x = -7.2f + i * 2.4f;
        glBegin(GL_LINE_LOOP);
        glVertex3f(x - 0.6f, 2.4f, 2.03f);
        glVertex3f(x, 3.0f, 2.03f);
        glVertex3f(x + 0.6f, 2.4f, 2.03f);
        glVertex3f(x, 1.8f, 2.03f);
        glEnd();
    }
}

static void drawBuilding(void) {
    setColor3(0.89f, 0.76f, 0.59f);

    glPushMatrix();
    glTranslatef(0.0f, 1.0f, -2.5f);
    drawUnitBox(17.0f, 6.0f, 4.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10.5f, 0.6f, -2.7f);
    drawUnitBox(4.8f, 5.2f, 3.5f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10.5f, 0.6f, -2.7f);
    drawUnitBox(4.8f, 5.2f, 3.5f);
    glPopMatrix();

    setColor3(0.27f, 0.41f, 0.58f);
    drawWindowGrid(-7.0f, -0.2f, -0.42f, 13, 5);
    drawWindowGrid(-12.0f, -0.4f, -0.9f, 3, 4);
    drawWindowGrid(8.5f, -0.4f, -0.9f, 3, 4);

    setColor3(0.78f, 0.62f, 0.42f);
    glPushMatrix();
    glTranslatef(0.0f, -0.9f, 0.15f);
    drawUnitBox(4.2f, 2.3f, 1.0f);
    glPopMatrix();

    drawFacadePattern();
}

static void drawLogo(void) {
    int i;
    glDisable(GL_LIGHTING);

    glColor3f(0.0f, 0.3f, 0.75f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < 80; ++i) {
        float a = (float)i * 6.2831853f / 80.0f;
        glVertex3f(cosf(a) * 0.52f, sinf(a) * 0.52f, 0.0f);
    }
    glEnd();

    glColor3f(0.96f, 0.43f, 0.14f);
    glBegin(GL_TRIANGLES);
    glVertex3f(-0.1f, -0.15f, 0.0f);
    glVertex3f(0.40f, 0.20f, 0.0f);
    glVertex3f(-0.4f, 0.15f, 0.0f);
    glEnd();

    glEnable(GL_LIGHTING);
}

static void drawNameboard(void) {
    glPushMatrix();
    glTranslatef(0.0f, -0.9f, 8.3f);

    setColor3(0.56f, 0.56f, 0.57f);
    drawUnitBox(15.0f, 0.35f, 1.15f);

    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 0.0f);
    setColor3(0.87f, 0.87f, 0.88f);
    drawUnitBox(14.2f, 1.65f, 0.75f);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(0.08f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef(-6.55f, 0.73f, 0.43f);
    drawStrokeText("INDIAN SPACE RESEARCH ORGANISATION (ISRO)", 0.0022f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-6.2f, 0.36f, 0.43f);
    drawStrokeText("HEADQUARTERS, BENGALURU", 0.00195f);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(6.4f, 0.42f, 0.48f);
    drawLogo();
    glPopMatrix();

    glPopMatrix();
}

static void drawFlag(void) {
    int i;
    const int strips = 28;
    const float width = 3.0f;

    glPushMatrix();
    glTranslatef(13.0f, -2.0f, 8.0f);

    setColor3(0.70f, 0.70f, 0.72f);
    glPushMatrix();
    glTranslatef(0.0f, 5.3f, 0.0f);
    drawUnitBox(0.12f, 10.6f, 0.12f);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    for (i = 0; i < strips; ++i) {
        float x0 = ((float)i / strips) * width;
        float x1 = ((float)(i + 1) / strips) * width;
        float wave0 = sinf(g_launchTime * 3.2f + x0 * 3.4f) * 0.16f;
        float wave1 = sinf(g_launchTime * 3.2f + x1 * 3.4f) * 0.16f;

        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.60f, 0.20f);
        glVertex3f(x0, 6.9f + wave0, 0.0f);
        glVertex3f(x1, 6.9f + wave1, 0.0f);
        glColor3f(1.0f, 0.92f, 0.85f);
        glVertex3f(x1, 6.4f + wave1, 0.0f);
        glVertex3f(x0, 6.4f + wave0, 0.0f);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.92f, 0.85f);
        glVertex3f(x0, 6.4f + wave0, 0.0f);
        glVertex3f(x1, 6.4f + wave1, 0.0f);
        glVertex3f(x1, 5.9f + wave1, 0.0f);
        glVertex3f(x0, 5.9f + wave0, 0.0f);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.08f, 0.56f, 0.28f);
        glVertex3f(x0, 5.9f + wave0, 0.0f);
        glVertex3f(x1, 5.9f + wave1, 0.0f);
        glVertex3f(x1, 5.4f + wave1, 0.0f);
        glVertex3f(x0, 5.4f + wave0, 0.0f);
        glEnd();
    }

    glColor3f(0.05f, 0.2f, 0.55f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < 32; ++i) {
        float a = (float)i * 6.2831853f / 32.0f;
        glVertex3f(1.5f + cosf(a) * 0.14f, 6.15f + sinf(a) * 0.14f, 0.01f);
    }
    glEnd();

    glEnable(GL_LIGHTING);
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
    if (g_crashStartTime >= 0.0f) {
        float crashTime = g_launchTime - g_crashStartTime;
        /* After the breakup moment, draw the rocket as falling chunks instead of an intact body. */
        if (crashTime >= 0.12f && g_chunksInitialized && g_chunkCount > 0) {
            drawCrashChunks();
            return;
        }
    }

    GLUquadric *quad = gluNewQuadric();
    if (quad == NULL) {
        return;
    }

    float rocketX = kRocketX;
    float rocketZ = kRocketZ;
    float rocketY = g_rocketY;
    float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

    if (g_crashStartTime >= 0.0f) {
        float crashTime = g_launchTime - g_crashStartTime;
        rocketX = sinf(crashTime * 1.5f) * 4.0f;
        rocketZ = kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
        rocketY = g_crashOriginY - crashTime * crashTime * 8.0f;
        if (rocketY < -2.0f) rocketY = -2.0f;

        rotX = sinf(crashTime * 0.8f) * 45.0f;
        rotZ = cosf(crashTime * 1.2f) * 35.0f;
        rotY = crashTime * 120.0f;
    }

    glPushMatrix();
    glTranslatef(rocketX, -1.0f + rocketY, rocketZ);

    if (g_crashStartTime >= 0.0f) {
        glRotatef(rotX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
        glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    }

    /* Thruster flame (only during launch phase) */
    if (g_crashStartTime < 0.0f && g_rocketY > 0.1f) {
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

    if (g_crashStartTime >= 0.0f) {
        setColor3(0.72f, 0.72f, 0.72f);
    } else {
        setColor3(0.97f, 0.97f, 0.97f);
    }
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

static void drawCrashSmokePlumes(void) {
    if (g_crashStartTime < 0.0f) return;
    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float crashTime = g_launchTime - g_crashStartTime;
    
    int i;
    /* Large dark smoke plume that rises and expands - creates shadow effect */
    for (i = 0; i < 25; ++i) {
        float angle = (float)i / 25.0f * 6.2831853f;
        float drift = sinf(crashTime * 0.5f + i) * 1.5f;
        float rocketX = sinf(crashTime * 1.5f) * 4.0f;
        float rocketZ = kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
        
        float smokeX = rocketX + cosf(angle) * (2.0f + crashTime * 1.5f) + drift;
        float smokeY = 8.0f + crashTime * 5.0f;
        float smokeZ = rocketZ + sinf(angle) * (2.0f + crashTime * 1.5f);
        
        /* Grey smoke - primary component */
        float greyLevel = 0.45f - crashTime * 0.1f;
        float alpha = fmaxf(0.0f, 0.8f - crashTime * 0.3f);
        glColor4f(greyLevel, greyLevel, greyLevel, alpha);
        
        float smokeSize = 1.2f + crashTime * 0.8f + sinf(crashTime * 3.0f + i) * 0.3f;
        glPushMatrix();
        glTranslatef(smokeX, smokeY, smokeZ);
        glutSolidSphere(smokeSize, 14, 14);
        glPopMatrix();
    }
    
    /* Red/dark smoke inner layer for drama */
    for (i = 0; i < 15; ++i) {
        float angle = (float)i / 15.0f * 6.2831853f;
        float drift = sinf(crashTime * 0.7f + i) * 0.9f;
        float rocketX = sinf(crashTime * 1.5f) * 4.0f;
        float rocketZ = kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
        
        float smokeX = rocketX + cosf(angle) * (1.5f + crashTime * 1.2f) + drift;
        float smokeY = 10.0f + crashTime * 5.5f;
        float smokeZ = rocketZ + sinf(angle) * (1.5f + crashTime * 1.2f);
        
        /* Darker red-grey for inner turbulence */
        float alpha = fmaxf(0.0f, 0.6f - crashTime * 0.25f);
        glColor4f(0.35f, 0.25f, 0.20f, alpha);
        
        float smokeSize = 0.8f + crashTime * 0.6f;
        glPushMatrix();
        glTranslatef(smokeX, smokeY, smokeZ);
        glutSolidSphere(smokeSize, 12, 12);
        glPopMatrix();
    }
    
    /* Shadow effect - dark area under smoke plume on ground/sea */
    if (crashTime < 2.0f) {
        float shadowAlpha = fmaxf(0.0f, 0.3f - crashTime * 0.15f);
        glColor4f(0.1f, 0.15f, 0.2f, shadowAlpha);
        float shadowSize = 4.0f + crashTime * 2.0f;
        glPushMatrix();
        glTranslatef(sinf(crashTime * 1.5f) * 4.0f, -1.95f, kRocketZ + cosf(crashTime * 1.8f) * 5.0f);
        glScalef(shadowSize, 0.1f, shadowSize * 0.8f);
        glutSolidSphere(1.0, 16, 16);
        glPopMatrix();
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void drawCrashImpactSite(void) {
    if (g_crashStartTime < 0.0f) return;
    
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float crashTime = g_launchTime - g_crashStartTime;
    
    /* Water splashes - debris hitting water creates ripples and splashes */
    if (crashTime > 0.3f && crashTime < 4.0f) {
        int i;
        for (i = 0; i < 12; ++i) {
            float angle = (float)i / 12.0f * 6.2831853f;
            float splashX = kRocketX + cosf(angle) * (3.0f + crashTime * 4.5f);
            float splashZ = kRocketZ + sinf(angle) * (3.0f + crashTime * 4.5f);
            float splashAlpha = fmaxf(0.0f, 1.0f - crashTime * 0.35f);
            
            /* Water spray - pieces hitting water */
            glColor4f(0.6f, 0.8f, 1.0f, splashAlpha * 0.5f);
            glPushMatrix();
            glTranslatef(splashX, -1.6f, splashZ);
            glScalef(0.4f, 1.5f, 0.4f);
            glutSolidSphere(0.5, 12, 12);
            glPopMatrix();
            
            /* Water foam/mist */
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Camera: multi-shot sequence with crash sequence. */
    {
        float t = g_launchTime;
        float rocketY = g_rocketY;

        if (g_crashStartTime >= 0.0f) {
            /* Crash phase: camera follows tumbling rocket */
            float crashTime = g_launchTime - g_crashStartTime;
            float rocketX = sinf(crashTime * 1.5f) * 4.0f;
            float rocketZ = kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
            float rocketYCrash = g_crashOriginY - crashTime * crashTime * 8.0f;
            if (rocketYCrash < -2.0f) rocketYCrash = -2.0f;

            /* After breakup, track the chunk cluster so pieces stay visible. */
            if (crashTime >= 0.12f && g_chunksInitialized && g_chunkCount > 0) {
                float cx = 0.0f, cy = 0.0f, cz = 0.0f;
                int i;
                for (i = 0; i < g_chunkCount; ++i) {
                    cx += g_chunks[i].x;
                    cy += g_chunks[i].y;
                    cz += g_chunks[i].z;
                }
                cx /= (float)g_chunkCount;
                cy /= (float)g_chunkCount;
                cz /= (float)g_chunkCount;

                gluLookAt(cx - 18.0f, cy + 9.0f, cz + 16.0f,
                          cx, cy + 1.5f, cz,
                          0.0f, 1.0f, 0.0f);
            } else {
                /* Pre-breakup: follow the tumbling rocket (world Y includes base offset). */
                gluLookAt(rocketX - 16.0f, (-1.0f + rocketYCrash) + 7.0f, rocketZ + 14.0f,
                          rocketX, (-1.0f + rocketYCrash) + 2.5f, rocketZ,
                          0.0f, 1.0f, 0.0f);
            }
        } else if (t < 2.0f) {
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

    drawSkyAndClouds();
    drawBirds();

    drawSeaSurface();
    drawShoreline();
    drawForegroundVegetation();

    setupLighting();

    /* Launch site on the coast */
    drawGroundAndPaths();
    drawLaunchTower(18.0f, kRocketZ + 4.0f, 16.0f, 1.6f);
    drawServiceGantry();

    drawLaunchSmoke();
    drawLaunchPad();
    drawAnimatedRocket();

    /* Crash effects */
    drawCrashSmokePlumes();
    drawExplosions();
    drawDebris();
    drawCrashImpactSite();

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

        /* Launch ascent only before crash */
        if (g_crashStartTime < 0.0f && g_launchTime > 2.0f) {
            g_rocketY += (g_launchTime - 2.0f) * 0.02f;
            if (g_rocketY > 45.0f) g_rocketY = 45.0f;
        }

        /* Start crash at mid-ascent so it stays visible */
        if (g_launchTime > 8.0f && g_crashStartTime < 0.0f) {
            g_crashStartTime = g_launchTime;
            g_crashOriginY = g_rocketY;
            g_chunksInitialized = 0;
            initCrashChunks();

            /* Explosion centered around upper body (world Y includes base offset). */
            triggerExplosion(kRocketX, (-1.0f + g_crashOriginY) + 7.0f, kRocketZ, 0.0f);
        }

        if (g_crashStartTime >= 0.0f) {
            float crashTime = g_launchTime - g_crashStartTime;
            float rocketX = sinf(crashTime * 1.5f) * 4.0f;
            float rocketZ = kRocketZ + cosf(crashTime * 1.8f) * 5.0f;
            float rocketYCrash = g_crashOriginY - crashTime * crashTime * 8.0f;
            if (rocketYCrash < -2.0f) rocketYCrash = -2.0f;

            /* Update big rocket chunks after breakup. */
            if (crashTime >= 0.0f) {
                updateCrashChunks(dt);
            }

            /* Extra bursts early in the failure like the reference */
            if (crashTime < 1.2f && fmod(crashTime, 0.08f) < dt) {
                float rx = sinf(crashTime * 5.0f) * 0.7f;
                float ry = cosf(crashTime * 4.0f) * 1.4f;
                float rz = sinf(crashTime * 3.5f) * 0.7f;
                triggerExplosion(rocketX + rx, (-1.0f + rocketYCrash) + 2.5f + ry, rocketZ + rz, crashTime);
            }
        }

        /* Update debris */
        updateDebris();

        /* Decay explosions and remove expired */
        {
            int i = 0;
            while (i < g_explosionCount) {
                g_explosions[i].life -= dt;
                if (g_explosions[i].life <= 0.0f) {
                    g_explosions[i] = g_explosions[g_explosionCount - 1];
                    g_explosionCount--;
                } else {
                    i++;
                }
            }
        }
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
    gluPerspective(62.0, (double)w / (double)h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

// You can add a main function for testing this scene independently

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 800);
    glutCreateWindow("Rocket Launch");

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.99f, 0.61f, 0.32f, 1.0f);

    glutDisplayFunc(display_launch);
    glutReshapeFunc(reshape_launch);
    glutTimerFunc(16, timer_launch, 0);

    glutMainLoop();
    return 0;
}

