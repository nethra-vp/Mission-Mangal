#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define STAR_COUNT 520
#define PI 3.1415926535f

static GLuint gBodyTex = 0;
static GLuint gPanelTex = 0;
static float gStarPos[STAR_COUNT][3];
static float gStarBase[STAR_COUNT];
static float gStarPhase[STAR_COUNT];
static float gStarSize[STAR_COUNT];

static float clamp01(float v) {
    if (v < 0.0f) {
        return 0.0f;
    }
    if (v > 1.0f) {
        return 1.0f;
    }
    return v;
}

static float smooth01(float v) {
    float t = clamp01(v);
    return t * t * (3.0f - 2.0f * t);
}

static float stageProgress(float timeSec, float beginSec, float endSec) {
    if (timeSec <= beginSec) {
        return 0.0f;
    }
    if (timeSec >= endSec) {
        return 1.0f;
    }
    return smooth01((timeSec - beginSec) / (endSec - beginSec));
}

static float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

/* GLUT-only equivalent for glfwGetTime(): elapsed seconds since program start. */
static float getTimeSeconds(void) {
    return 0.001f * (float)glutGet(GLUT_ELAPSED_TIME);
}

static void setMaterial(const GLfloat *amb, const GLfloat *diff, const GLfloat *spec, GLfloat shininess) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

static void buildBodyTexture(void) {
    unsigned char tex[128][128][3];
    int y;
    int x;

    for (y = 0; y < 128; ++y) {
        for (x = 0; x < 128; ++x) {
            float stripe = 0.92f + 0.08f * sinf((float)x * 0.42f);
            float wrinkle = 0.93f + 0.07f * sinf((float)(x + y) * 0.19f);
            float micro = 0.96f + 0.04f * sinf((float)(x * 3 + y * 2) * 0.11f);
            float g = stripe * wrinkle * micro;

            float r = 0.80f * g + 0.10f;
            float gg = 0.70f * g + 0.08f;
            float b = 0.30f * g + 0.03f;

            if (((x / 16) + (y / 16)) % 2 == 0) {
                r *= 1.03f;
                gg *= 1.02f;
            }

            tex[y][x][0] = (unsigned char)(255.0f * clamp01(r));
            tex[y][x][1] = (unsigned char)(255.0f * clamp01(gg));
            tex[y][x][2] = (unsigned char)(255.0f * clamp01(b));
        }
    }

    glGenTextures(1, &gBodyTex);
    glBindTexture(GL_TEXTURE_2D, gBodyTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
}

static void buildPanelTexture(void) {
    unsigned char tex[128][128][3];
    int y;
    int x;

    for (y = 0; y < 128; ++y) {
        for (x = 0; x < 128; ++x) {
            int gx = x % 16;
            int gy = y % 16;
            int grid = (gx == 0 || gy == 0 || gx == 15 || gy == 15);

            float base = 0.20f + 0.06f * sinf((float)(x + y) * 0.16f);
            float cell = 0.35f + 0.08f * sinf((float)x * 0.25f);
            float r = 0.10f + 0.03f * base;
            float g = 0.20f + 0.05f * base;
            float b = 0.50f + 0.22f * cell;

            if (grid) {
                r = 0.07f;
                g = 0.11f;
                b = 0.23f;
            }

            tex[y][x][0] = (unsigned char)(255.0f * clamp01(r));
            tex[y][x][1] = (unsigned char)(255.0f * clamp01(g));
            tex[y][x][2] = (unsigned char)(255.0f * clamp01(b));
        }
    }

    glGenTextures(1, &gPanelTex);
    glBindTexture(GL_TEXTURE_2D, gPanelTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
}

static void initStarfield(void) {
    int i;
    srand(7);
    for (i = 0; i < STAR_COUNT; ++i) {
        float a = ((float)rand() / (float)RAND_MAX) * 2.0f * PI;
        float b = ((float)rand() / (float)RAND_MAX) * PI;
        float r = 25.0f + ((float)rand() / (float)RAND_MAX) * 65.0f;

        gStarPos[i][0] = cosf(a) * sinf(b) * r;
        gStarPos[i][1] = cosf(b) * r;
        gStarPos[i][2] = -fabsf(sinf(a) * sinf(b) * r) - 12.0f;
        gStarBase[i] = 0.45f + ((float)rand() / (float)RAND_MAX) * 0.55f;
        gStarPhase[i] = ((float)rand() / (float)RAND_MAX) * 6.28318f;
        gStarSize[i] = 1.0f + ((float)rand() / (float)RAND_MAX) * 2.2f;
    }
}

static void drawStarfield(float t) {
    int i;

    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);

    for (i = 0; i < STAR_COUNT; ++i) {
        float tw = 0.60f + 0.40f * sinf(t * 0.9f + gStarPhase[i]);
        float b = clamp01(gStarBase[i] * tw);
        glPointSize(gStarSize[i]);
        glColor3f(b, b, b);
        glBegin(GL_POINTS);
        glVertex3f(gStarPos[i][0], gStarPos[i][1], gStarPos[i][2]);
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
}

static void drawTexturedCuboid(float sx, float sy, float sz, float cornerDarken) {
    float x = sx * 0.5f;
    float y = sy * 0.5f;
    float z = sz * 0.5f;
    float c = 1.0f - cornerDarken;

    glBegin(GL_QUADS);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, z);
    glColor3f(1.0f, 1.0f, 1.0f); glTexCoord2f(2.0f, 0.0f); glVertex3f(x, -y, z);
    glColor3f(0.96f, 0.96f, 0.96f); glTexCoord2f(2.0f, 2.0f); glVertex3f(x, y, z);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 2.0f); glVertex3f(-x, y, z);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 0.0f); glVertex3f(x, -y, -z);
    glColor3f(1.0f, 1.0f, 1.0f); glTexCoord2f(2.0f, 0.0f); glVertex3f(-x, -y, -z);
    glColor3f(0.95f, 0.95f, 0.95f); glTexCoord2f(2.0f, 2.0f); glVertex3f(-x, y, -z);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 2.0f); glVertex3f(x, y, -z);

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, -z);
    glColor3f(0.92f, 0.92f, 0.92f); glTexCoord2f(2.0f, 0.0f); glVertex3f(-x, -y, z);
    glColor3f(0.85f, 0.85f, 0.85f); glTexCoord2f(2.0f, 2.0f); glVertex3f(-x, y, z);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 2.0f); glVertex3f(-x, y, -z);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 0.0f); glVertex3f(x, -y, z);
    glColor3f(0.92f, 0.92f, 0.92f); glTexCoord2f(2.0f, 0.0f); glVertex3f(x, -y, -z);
    glColor3f(0.85f, 0.85f, 0.85f); glTexCoord2f(2.0f, 2.0f); glVertex3f(x, y, -z);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 2.0f); glVertex3f(x, y, z);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, y, z);
    glColor3f(0.90f, 0.90f, 0.90f); glTexCoord2f(2.0f, 0.0f); glVertex3f(x, y, z);
    glColor3f(0.88f, 0.88f, 0.88f); glTexCoord2f(2.0f, 2.0f); glVertex3f(x, y, -z);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 2.0f); glVertex3f(-x, y, -z);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, -z);
    glColor3f(0.84f, 0.84f, 0.84f); glTexCoord2f(2.0f, 0.0f); glVertex3f(x, -y, -z);
    glColor3f(0.80f, 0.80f, 0.80f); glTexCoord2f(2.0f, 2.0f); glVertex3f(x, -y, z);
    glColor3f(c, c, c); glTexCoord2f(0.0f, 2.0f); glVertex3f(-x, -y, z);

    glEnd();
}

static void drawBody(float bodyScale) {
    static const GLfloat amb[] = { 0.35f, 0.30f, 0.14f, 1.0f };
    static const GLfloat diff[] = { 0.80f, 0.70f, 0.30f, 1.0f };
    static const GLfloat spec[] = { 1.0f, 0.92f, 0.55f, 1.0f };

    setMaterial(amb, diff, spec, 80.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gBodyTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();
    glScalef(bodyScale, bodyScale, bodyScale);
    drawTexturedCuboid(1.0f, 1.0f, 1.0f, 0.22f);

    glPushMatrix();
    glTranslatef(0.0f, 0.57f, 0.0f);
    drawTexturedCuboid(0.72f, 0.10f, 0.72f, 0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.57f, 0.0f);
    drawTexturedCuboid(0.72f, 0.10f, 0.72f, 0.18f);
    glPopMatrix();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

static void drawPanelSurface(void) {
    static const GLfloat amb[] = { 0.08f, 0.12f, 0.24f, 1.0f };
    static const GLfloat diff[] = { 0.10f, 0.20f, 0.50f, 1.0f };
    static const GLfloat spec[] = { 0.70f, 0.80f, 0.95f, 1.0f };

    setMaterial(amb, diff, spec, 110.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gPanelTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    drawTexturedCuboid(2.0f, 0.8f, 0.1f, 0.12f);
    glDisable(GL_TEXTURE_2D);
}

static void drawPanelStrut(float side) {
    static const GLfloat amb[] = { 0.16f, 0.16f, 0.18f, 1.0f };
    static const GLfloat diff[] = { 0.28f, 0.30f, 0.34f, 1.0f };
    static const GLfloat spec[] = { 0.22f, 0.24f, 0.28f, 1.0f };

    setMaterial(amb, diff, spec, 25.0f);

    glPushMatrix();
    glTranslatef(side * 0.65f, 0.22f, 0.0f);
    glScalef(0.14f, 0.12f, 0.18f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(side * 0.80f, 0.02f, 0.0f);
    glScalef(0.30f, 0.08f, 0.10f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(side * 0.65f, -0.22f, 0.0f);
    glScalef(0.14f, 0.12f, 0.18f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

static void drawThrusters(void) {
    static const GLfloat amb[] = { 0.05f, 0.05f, 0.06f, 1.0f };
    static const GLfloat diff[] = { 0.12f, 0.12f, 0.14f, 1.0f };
    static const GLfloat spec[] = { 0.18f, 0.18f, 0.20f, 1.0f };
    int sx;
    int sy;

    setMaterial(amb, diff, spec, 18.0f);

    for (sx = -1; sx <= 1; sx += 2) {
        for (sy = -1; sy <= 1; sy += 2) {
            glPushMatrix();
            glTranslatef(0.42f * (float)sx, 0.42f * (float)sy, -0.47f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            glutSolidCone(0.08f, 0.14f, 12, 8);
            glPopMatrix();
        }
    }
}

static void drawAntenna(float y, float scale) {
    static const GLfloat amb[] = { 0.28f, 0.28f, 0.30f, 1.0f };
    static const GLfloat diff[] = { 0.72f, 0.72f, 0.76f, 1.0f };
    static const GLfloat spec[] = { 0.95f, 0.95f, 0.98f, 1.0f };

    setMaterial(amb, diff, spec, 45.0f);

    glPushMatrix();
    glTranslatef(0.0f, y, 0.62f);
    glScalef(scale, scale, scale);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.09f);
    glScalef(0.09f, 0.09f, 0.28f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glutSolidCone(0.40f, 0.30f, 20, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.11f);
    glScalef(0.11f, 0.11f, 0.11f);
    glutSolidSphere(1.0f, 16, 16);
    glPopMatrix();

    glPopMatrix();
}

static void drawAntennaBodyShadow(float antennaP) {
    float alpha = 0.24f * antennaP;

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.02f, 0.02f, 0.02f, alpha);
    glBegin(GL_POLYGON);
    {
        int i;
        for (i = 0; i < 24; ++i) {
            float a = (2.0f * PI * (float)i) / 24.0f;
            glVertex3f(cosf(a) * 0.22f, sinf(a) * 0.16f, 0.49f);
        }
    }
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

static void drawLeftPanelHinged(float progress) {
    float p = smooth01(progress);
    float angle = lerp(180.0f, 0.0f, p);

    drawPanelStrut(-1.0f);

    glPushMatrix();
    glTranslatef(-0.55f, 0.0f, 0.0f);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(-1.0f, 0.0f, 0.0f);
    drawPanelSurface();
    glPopMatrix();
}

static void drawRightPanelHinged(float progress) {
    float p = smooth01(progress);
    float angle = lerp(-180.0f, 0.0f, p);

    drawPanelStrut(1.0f);

    glPushMatrix();
    glTranslatef(0.55f, 0.0f, 0.0f);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(1.0f, 0.0f, 0.0f);
    drawPanelSurface();
    glPopMatrix();
}

static void drawAssemblyByTime(float t) {
    float bodyP = stageProgress(t, 2.0f, 4.0f);
    float leftP = stageProgress(t, 4.0f, 6.0f);
    float rightP = stageProgress(t, 6.0f, 8.0f);
    float antennaP = stageProgress(t, 8.0f, 10.0f);

    if (t < 2.0f) {
        return;
    }

    drawBody(bodyP);

    if (t >= 4.0f) {
        drawLeftPanelHinged(leftP);
    }

    if (t >= 6.0f) {
        drawRightPanelHinged(rightP);
    }

    if (t >= 8.0f) {
        float ay = lerp(1.4f, 0.0f, antennaP);
        drawAntennaBodyShadow(antennaP);
        drawAntenna(ay, antennaP);
    }

    drawThrusters();
}

static void drawScene(void) {
    float t = getTimeSeconds();
    float camOrbit = 0.12f * sinf(t * 0.18f);
    float camShakeX = 0.02f * sinf(t * 1.5f);
    float camShakeY = 0.02f * cosf(t * 1.2f);
    float camZoom = 8.2f + 0.35f * sinf(t * 0.14f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(camOrbit + camShakeX, 2.55f + camShakeY, camZoom,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    drawStarfield(t);

    if (t > 10.0f) {
        glPushMatrix();
        glRotatef((t - 10.0f) * 16.0f, 0.0f, 1.0f, 0.0f);
        drawAssemblyByTime(10.0f);
        glPopMatrix();
    } else {
        drawAssemblyByTime(t);
    }

    glutSwapBuffers();
}

static void timerTick(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timerTick, 0);
}

static void reshapeScene(int w, int h) {
    if (h == 0) {
        h = 1;
    }
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 120.0);
    glMatrixMode(GL_MODELVIEW);
}

static void initScene(void) {
    GLfloat sunDirPos[] = { 0.35f, 0.85f, 0.35f, 0.0f };
    GLfloat sunDiff[] = { 1.0f, 1.0f, 0.9f, 1.0f };
    GLfloat sunSpec[] = { 1.0f, 1.0f, 0.92f, 1.0f };
    GLfloat sunAmb[] = { 0.01f, 0.01f, 0.02f, 1.0f };

    GLfloat rimDirPos[] = { -0.2f, 0.1f, -1.0f, 0.0f };
    GLfloat rimDiff[] = { 0.20f, 0.26f, 0.44f, 1.0f };
    GLfloat rimSpec[] = { 0.32f, 0.40f, 0.55f, 1.0f };
    GLfloat rimAmb[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT0, GL_POSITION, sunDirPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpec);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmb);

    glLightfv(GL_LIGHT1, GL_POSITION, rimDirPos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, rimDiff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, rimSpec);
    glLightfv(GL_LIGHT1, GL_AMBIENT, rimAmb);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    buildBodyTexture();
    buildPanelTexture();
    initStarfield();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Satellite Assembly - Cinematic");

    initScene();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(reshapeScene);
    glutTimerFunc(0, timerTick, 0);

    glutMainLoop();
    return 0;
}
