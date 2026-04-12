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

        float wave0 = sinf(g_time * 0.3f + z0 * 0.08f) * 0.02f;
        float wave1 = sinf(g_time * 0.3f + z1 * 0.08f) * 0.02f;

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

    float refl = 0.3f;
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

    glEnable(GL_LIGHTING);
}

static void drawShoreline(void) {
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
    glDisable(GL_LIGHTING);
    glColor3f(0.06f, 0.12f, 0.04f);
    
    drawTreeClump(-28.0f, -1.0f, 3.2f);
    drawTreeClump(-25.5f, -0.8f, 2.8f);
    drawTreeClump(-22.0f, -1.2f, 3.5f);
    drawTreeClump(-18.5f, -0.5f, 2.6f);
    drawTreeClump(-15.0f, -1.0f, 3.0f);
    
    setColor3(0.15f, 0.15f, 0.16f);
    glPushMatrix();
    glTranslatef(26.0f, -1.8f, 3.0f);
    drawUnitBox(0.35f, 14.0f, 0.35f);
    glPopMatrix();
    
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
    glTranslatef(-0.15f, 0.40f, 0.0f);
    drawUnitBox(0.08f, 0.35f, 0.08f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.15f, 0.40f, 0.0f);
    drawUnitBox(0.08f, 0.35f, 0.08f);
    glPopMatrix();
    
    glPopMatrix();
}

static void setupLighting(void) {
    /* Golden hour lighting - early morning from left and low angle */
    GLfloat lightPos[] = {-25.0f, 12.0f, 8.0f, 0.0f};
    GLfloat lightAmbient[] = {0.5f, 0.45f, 0.35f, 1.0f};  /* Warm ambient */
    GLfloat lightDiffuse[] = {1.0f, 0.85f, 0.60f, 1.0f};  /* Golden diffuse */
    GLfloat lightSpec[] = {1.0f, 0.90f, 0.70f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
}

void display_shoreline(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Camera: front view from the sea looking at rocket on shore */
    gluLookAt(0.0f, 5.0f, 40.0f,
              kRocketX, 3.0f, kRocketZ,
              0.0f, 1.0f, 0.0f);

    drawSkyAndClouds();
    drawSeaSurface();
    drawShoreline();
    drawForegroundVegetation();

    setupLighting();

    drawGroundAndPaths();
    drawWaterPonds();
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

    glutSwapBuffers();
}

void timer_shoreline(int value) {
    (void)value;
    g_time += 0.016f;
    glutPostRedisplay();
    glutTimerFunc(16, timer_shoreline, 0);
}

void reshape_cb(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1f, 500.0f);
}

void keyboard_cb(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27) exit(0);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("Rocket on Shore - ISRO");

    glClearColor(0.68f, 0.85f, 0.90f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1200.0f / 800.0f, 0.1f, 500.0f);

    glutDisplayFunc(display_shoreline);
    glutReshapeFunc(reshape_cb);
    glutKeyboardFunc(keyboard_cb);
    glutTimerFunc(16, timer_shoreline, 0);

    glutMainLoop();
    return 0;
}
