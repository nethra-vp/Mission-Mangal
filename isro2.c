#include "C:/OpenGL/include/GL/glut.h"
#include <math.h>
#include <stdio.h>

static float gTime = 0.0f;
static float gCamBack = 0.0f;

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

    // glBegin(GL_QUADS);
    // glColor3f(0.12f, 0.24f, 0.52f);
    // glVertex3f(-100.0f, 35.0f, -100.0f);
    // glVertex3f(100.0f, 35.0f, -100.0f);
    // glColor3f(1.0f, 0.56f, 0.24f);
    // glVertex3f(100.0f, 3.0f, -100.0f);
    // glVertex3f(-100.0f, 3.0f, -100.0f);
    // glEnd();

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

    glPushMatrix();
    glTranslatef(6.4f, 0.42f, 0.48f);
    drawLogo();
    glPopMatrix();

    glPopMatrix();
}

static void drawRocket(void) {
    GLUquadric *quad = gluNewQuadric();
    if (quad == NULL) {
        return;
    }

    glPushMatrix();
    glTranslatef(-9.5f, -1.0f, 2.0f);

    setColor3(0.56f, 0.56f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);
    drawUnitBox(2.8f, 0.3f, 2.8f);
    glPopMatrix();

    glTranslatef(0.0f, 0.35f, 0.0f);
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

static void drawPerson(float baseX, float baseZ, float offset) {
    float step = sinf(gTime * 2.2f + offset) * 0.24f;
    float x = baseX + step;

    glPushMatrix();
    glTranslatef(x, -1.95f, baseZ);

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
    glTranslatef(-0.08f, 0.2f + fabsf(step) * 0.2f, 0.0f);
    drawUnitBox(0.08f, 0.4f, 0.08f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.08f, 0.2f + (0.24f - fabsf(step)) * 0.2f, 0.0f);
    drawUnitBox(0.08f, 0.4f, 0.08f);
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
        float wave0 = sinf(gTime * 3.2f + x0 * 3.4f) * 0.16f;
        float wave1 = sinf(gTime * 3.2f + x1 * 3.4f) * 0.16f;

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

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.0, 2.4, 12.0 + gCamBack,
              0.0, 1.35, -1.8,
              0.0, 1.0, 0.0);

    drawSkyAndClouds();
    setupLighting();

    drawGroundAndPaths();
    drawBuilding();
    drawNameboard();
    drawRocket();

    drawMangoTree(14.5f, 4.0f, 4.4f);
    drawMangoTree(-17.5f, 8.2f, 4.4f);
    drawMangoTree(-20.2f, 2.2f, 3.8f);
    drawMangoTree(18.4f, 8.0f, 4.2f);
    drawMangoTree(21.0f, 2.6f, 3.7f);
    drawMangoTree(-14.2f, -1.5f, 3.2f);
    drawMangoTree(15.2f, -1.8f, 3.1f);

    drawFlowerBed(-9.8f, 6.3f, 7.2f, 0.7f);
    drawFlowerBed(9.8f, 6.1f, 7.0f, 0.8f);
    drawFlowerBed(-8.2f, 2.0f, 6.0f, 0.55f);
    drawFlowerBed(8.0f, 2.3f, 6.2f, 0.6f);

    drawPerson(-7.2f, 9.2f, 0.0f);
    drawPerson(-3.4f, 8.8f, 1.3f);
    drawPerson(2.6f, 9.0f, 2.2f);
    drawPerson(6.7f, 8.9f, 3.8f);
    drawPerson(0.6f, 4.2f, 4.7f);

    drawFlag();

    glutSwapBuffers();
}

static void reshape(int w, int h) {
    if (h == 0) {
        h = 1;
    }

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(62.0, (double)w / (double)h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

static void timer(int value) {
    (void)value;
    gTime += 0.016f;
    if (gCamBack < 9.0f) {
        gCamBack += 0.009f;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 800);
    glutCreateWindow("ISRO HQ - Bengaluru");

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.99f, 0.61f, 0.32f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
