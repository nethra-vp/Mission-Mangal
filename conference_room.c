#include "C:/OpenGL/include/GL/glut.h"
#include <math.h>

static float gTime = 0.0f;

static void setColor3(float r, float g, float b) {
	glColor3f(r, g, b);
}

static void drawBox(float sx, float sy, float sz) {
	glPushMatrix();
	glScalef(sx, sy, sz);
	glutSolidCube(1.0f);
	glPopMatrix();
}

static void drawCylinder(float radius, float height, int slices) {
	int i;
	float step = 6.2831853f / (float)slices;

	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= slices; ++i) {
		float a = i * step;
		float x = cosf(a) * radius;
		float z = sinf(a) * radius;
		glNormal3f(cosf(a), 0.0f, sinf(a));
		glVertex3f(x, 0.0f, z);
		glVertex3f(x, height, z);
	}
	glEnd();
}

static void drawText(const char *text, void *font) {
	int i;
	for (i = 0; text[i] != '\0'; ++i) {
		glutBitmapCharacter(font, text[i]);
	}
}

static void drawLaptop(float x, float y, float z, float hueShift) {
	glPushMatrix();
	glTranslatef(x, y, z);

	setColor3(0.22f, 0.22f, 0.24f);
	drawBox(0.38f, 0.02f, 0.28f);

	glPushMatrix();
	glTranslatef(0.0f, 0.14f, -0.13f);
	glRotatef(-105.0f, 1.0f, 0.0f, 0.0f);
	setColor3(0.15f, 0.16f, 0.18f);
	drawBox(0.38f, 0.24f, 0.02f);

	glDisable(GL_LIGHTING);
	glColor3f(0.14f + hueShift, 0.55f, 0.88f - hueShift * 0.6f);
	glBegin(GL_QUADS);
	glVertex3f(-0.16f, -0.08f, 0.012f);
	glVertex3f(0.16f, -0.08f, 0.012f);
	glVertex3f(0.16f, 0.08f, 0.012f);
	glVertex3f(-0.16f, 0.08f, 0.012f);
	glEnd();
	glColor3f(0.8f, 0.9f, 1.0f);
	glRasterPos3f(-0.13f, 0.0f, 0.013f);
	drawText("ORBIT DATA", GLUT_BITMAP_HELVETICA_10);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	glPopMatrix();
}

static void drawMug(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);
	setColor3(0.9f, 0.92f, 0.95f);
	drawCylinder(0.055f, 0.08f, 20);
	setColor3(0.86f, 0.87f, 0.9f);
	glPushMatrix();
	glTranslatef(0.065f, 0.04f, 0.0f);
	glutSolidTorus(0.01f, 0.02f, 8, 16);
	glPopMatrix();
	glPopMatrix();
}

static void drawNotebook(float x, float y, float z, float tint) {
	glPushMatrix();
	glTranslatef(x, y, z);
	setColor3(0.82f - tint, 0.82f - tint * 0.2f, 0.78f - tint * 0.3f);
	drawBox(0.30f, 0.012f, 0.22f);
	setColor3(0.2f, 0.2f, 0.24f);
	glPushMatrix();
	glTranslatef(-0.14f, 0.012f, 0.0f);
	drawBox(0.018f, 0.018f, 0.22f);
	glPopMatrix();
	glPopMatrix();
}

static void drawChair(float x, float z, float rotY) {
	glPushMatrix();
	glTranslatef(x, 0.0f, z);
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);

	setColor3(0.22f, 0.24f, 0.27f);
	glPushMatrix();
	glTranslatef(0.0f, 0.42f, 0.0f);
	drawBox(0.55f, 0.08f, 0.50f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.78f, -0.2f);
	drawBox(0.55f, 0.62f, 0.08f);
	glPopMatrix();

	setColor3(0.16f, 0.17f, 0.19f);
	glPushMatrix();
	glTranslatef(0.0f, 0.20f, 0.0f);
	drawCylinder(0.04f, 0.28f, 16);
	glPopMatrix();

	setColor3(0.14f, 0.14f, 0.16f);
	glPushMatrix();
	glTranslatef(0.0f, 0.03f, 0.0f);
	drawBox(0.58f, 0.04f, 0.58f);
	glPopMatrix();

	glPopMatrix();
}

static void drawScientistSeated(float x, float z, float faceYaw, float skinT, float hairR, float hairG, float hairB) {
	float lean = 0.06f + 0.01f * sinf(gTime * 1.2f + x);
	glPushMatrix();
	glTranslatef(x, 0.0f, z);

	setColor3(0.95f, 0.95f, 0.96f);
	glPushMatrix();
	glTranslatef(0.0f, 1.04f - lean, 0.0f);
	drawBox(0.48f, 0.72f, 0.28f);
	glPopMatrix();

	setColor3(0.2f, 0.24f, 0.34f);
	glPushMatrix();
	glTranslatef(0.0f, 0.84f - lean, 0.02f);
	drawBox(0.28f, 0.34f, 0.20f);
	glPopMatrix();

	setColor3(0.2f, 0.2f, 0.23f);
	glPushMatrix();
	glTranslatef(-0.09f, 0.46f, 0.06f);
	glRotatef(85.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.12f, 0.42f, 0.12f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.09f, 0.46f, 0.06f);
	glRotatef(85.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.12f, 0.42f, 0.12f);
	glPopMatrix();

	setColor3(0.84f - skinT, 0.68f - skinT * 0.2f, 0.56f - skinT * 0.2f);
	glPushMatrix();
	glTranslatef(0.0f, 1.50f - lean, 0.02f);
	glRotatef(faceYaw, 0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.13f, 18, 18);
	setColor3(hairR, hairG, hairB);
	glTranslatef(0.0f, 0.08f, -0.01f);
	glScalef(1.0f, 0.7f, 1.0f);
	glutSolidSphere(0.13f, 14, 14);
	glPopMatrix();

	setColor3(0.82f - skinT * 0.8f, 0.68f - skinT * 0.3f, 0.56f - skinT * 0.3f);
	glPushMatrix();
	glTranslatef(-0.23f, 1.02f - lean, 0.03f);
	glRotatef(12.0f + 6.0f * sinf(gTime * 1.5f + z), 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.35f, 0.08f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.23f, 1.02f - lean, 0.03f);
	glRotatef(-8.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.32f, 0.08f);
	glPopMatrix();

	glPopMatrix();
}

static void drawSeniorWoman(float x, float z) {
	glPushMatrix();
	glTranslatef(x, 0.0f, z);
	glRotatef(18.0f, 0.0f, 1.0f, 0.0f);

	setColor3(0.96f, 0.96f, 0.97f);
	glPushMatrix();
	glTranslatef(0.0f, 1.1f, 0.0f);
	drawBox(0.50f, 0.84f, 0.30f);
	glPopMatrix();

	setColor3(0.22f, 0.28f, 0.46f);
	glPushMatrix();
	glTranslatef(0.0f, 0.9f, 0.02f);
	drawBox(0.30f, 0.42f, 0.20f);
	glPopMatrix();

	/* Legs and shoes */
	setColor3(0.22f, 0.24f, 0.28f);
	glPushMatrix();
	glTranslatef(-0.10f, 0.45f, 0.03f);
	drawBox(0.12f, 0.62f, 0.12f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.10f, 0.45f, 0.03f);
	drawBox(0.12f, 0.62f, 0.12f);
	glPopMatrix();
	setColor3(0.1f, 0.1f, 0.12f);
	glPushMatrix();
	glTranslatef(-0.10f, 0.12f, 0.10f);
	drawBox(0.14f, 0.08f, 0.22f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.10f, 0.12f, 0.10f);
	drawBox(0.14f, 0.08f, 0.22f);
	glPopMatrix();

	setColor3(0.85f, 0.70f, 0.59f);
	glPushMatrix();
	glTranslatef(0.0f, 1.60f, 0.02f);
	glutSolidSphere(0.14f, 18, 18);
	setColor3(0.75f, 0.75f, 0.78f);
	glTranslatef(0.0f, 0.09f, -0.01f);
	glScalef(1.0f, 0.7f, 1.0f);
	glutSolidSphere(0.14f, 14, 14);
	glPopMatrix();

	setColor3(0.83f, 0.7f, 0.6f);
	glPushMatrix();
	glTranslatef(0.28f, 1.16f, 0.08f);
	glRotatef(-28.0f + 8.0f * sinf(gTime * 2.0f), 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.46f, 0.08f);
	glPopMatrix();

	/* Pointer stick */
	setColor3(0.6f, 0.4f, 0.2f);
	glPushMatrix();
	glTranslatef(0.36f, 1.06f, 0.14f);
	glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
	drawCylinder(0.01f, 0.65f, 10);
	glPopMatrix();

	glPopMatrix();
}

static void drawSeniorMan(float x, float z) {
	glPushMatrix();
	glTranslatef(x, 0.0f, z);
	glRotatef(-10.0f, 0.0f, 1.0f, 0.0f);

	setColor3(0.95f, 0.95f, 0.97f);
	glPushMatrix();
	glTranslatef(0.0f, 1.1f, 0.0f);
	drawBox(0.50f, 0.84f, 0.30f);
	glPopMatrix();

	setColor3(0.24f, 0.3f, 0.48f);
	glPushMatrix();
	glTranslatef(0.0f, 0.9f, 0.02f);
	drawBox(0.30f, 0.42f, 0.20f);
	glPopMatrix();

	/* Legs and shoes */
	setColor3(0.2f, 0.22f, 0.26f);
	glPushMatrix();
	glTranslatef(-0.10f, 0.45f, 0.03f);
	drawBox(0.12f, 0.62f, 0.12f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.10f, 0.45f, 0.03f);
	drawBox(0.12f, 0.62f, 0.12f);
	glPopMatrix();
	setColor3(0.1f, 0.1f, 0.12f);
	glPushMatrix();
	glTranslatef(-0.10f, 0.12f, 0.10f);
	drawBox(0.14f, 0.08f, 0.22f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.10f, 0.12f, 0.10f);
	drawBox(0.14f, 0.08f, 0.22f);
	glPopMatrix();

	setColor3(0.84f, 0.68f, 0.56f);
	glPushMatrix();
	glTranslatef(0.0f, 1.60f, 0.02f);
	glutSolidSphere(0.14f, 18, 18);

	/* Glasses */
	glDisable(GL_LIGHTING);
	glColor3f(0.08f, 0.08f, 0.1f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-0.08f, 0.03f, 0.12f);
	glVertex3f(-0.02f, 0.03f, 0.12f);
	glVertex3f(-0.02f, -0.02f, 0.12f);
	glVertex3f(-0.08f, -0.02f, 0.12f);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.02f, 0.03f, 0.12f);
	glVertex3f(0.08f, 0.03f, 0.12f);
	glVertex3f(0.08f, -0.02f, 0.12f);
	glVertex3f(0.02f, -0.02f, 0.12f);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(-0.02f, 0.005f, 0.12f);
	glVertex3f(0.02f, 0.005f, 0.12f);
	glEnd();
	glEnable(GL_LIGHTING);

	setColor3(0.16f, 0.14f, 0.12f);
	glTranslatef(0.0f, 0.09f, -0.01f);
	glScalef(1.0f, 0.72f, 1.0f);
	glutSolidSphere(0.14f, 14, 14);
	glPopMatrix();

	glPopMatrix();
}

static void drawSatelliteModel(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);

	setColor3(0.72f, 0.74f, 0.78f);
	drawBox(0.46f, 0.36f, 0.32f);
	setColor3(0.14f, 0.38f, 0.7f);
	glPushMatrix();
	glTranslatef(-0.38f, 0.0f, 0.0f);
	drawBox(0.30f, 0.20f, 0.02f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.38f, 0.0f, 0.0f);
	drawBox(0.30f, 0.20f, 0.02f);
	glPopMatrix();

	setColor3(0.9f, 0.55f, 0.14f);
	glPushMatrix();
	glTranslatef(0.0f, 0.24f, 0.0f);
	drawCylinder(0.06f, 0.16f, 16);
	glPopMatrix();

	glPopMatrix();
}

static void drawConferenceRoom(void) {
	int i;

	/* Floor and walls */
	setColor3(0.16f, 0.15f, 0.14f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-8.0f, 0.0f, 4.0f);
	glVertex3f(8.0f, 0.0f, 4.0f);
	glVertex3f(8.0f, 0.0f, -10.0f);
	glVertex3f(-8.0f, 0.0f, -10.0f);
	glEnd();

	setColor3(0.26f, 0.26f, 0.28f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-12.0f, 0.0f, -10.0f);
	glVertex3f(12.0f, 0.0f, -10.0f);
	glVertex3f(12.0f, 7.2f, -10.0f);
	glVertex3f(-12.0f, 7.2f, -10.0f);
	glEnd();

	setColor3(0.2f, 0.2f, 0.22f);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-8.0f, 5.6f, 4.0f);
	glVertex3f(8.0f, 5.6f, 4.0f);
	glVertex3f(8.0f, 5.6f, -10.0f);
	glVertex3f(-8.0f, 5.6f, -10.0f);
	glEnd();

	/* Chalkboard */
	setColor3(0.1f, 0.28f, 0.14f);
	glPushMatrix();
	glTranslatef(0.4f, 2.45f, -9.82f);
	drawBox(8.8f, 2.9f, 0.08f);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	setColor3(0.86f, 0.9f, 0.84f);
	glRasterPos3f(-3.5f, 3.6f, -9.75f);
	drawText("v = sqrt(mu/r)      DELTA-V = Isp * g0 * ln(m0/mf)", GLUT_BITMAP_HELVETICA_12);
	glRasterPos3f(-3.8f, 3.25f, -9.75f);
	drawText("Earth Transfer Orbit --> Mars Capture Burn", GLUT_BITMAP_HELVETICA_12);
	glRasterPos3f(-0.8f, 2.9f, -9.75f);
	drawText("T-MINUS 1:30:00", GLUT_BITMAP_HELVETICA_18);
	glRasterPos3f(2.4f, 2.65f, -9.75f);
	drawText("Earth", GLUT_BITMAP_HELVETICA_12);
	glRasterPos3f(4.3f, 2.55f, -9.75f);
	drawText("Mars", GLUT_BITMAP_HELVETICA_12);

	glBegin(GL_LINE_STRIP);
	glVertex3f(2.2f, 2.48f, -9.75f);
	glVertex3f(2.9f, 2.9f, -9.75f);
	glVertex3f(3.6f, 2.5f, -9.75f);
	glVertex3f(4.1f, 2.82f, -9.75f);
	glVertex3f(4.7f, 2.53f, -9.75f);
	glEnd();
	glEnable(GL_LIGHTING);

	/* Countdown screen */
	setColor3(0.16f, 0.16f, 0.19f);
	glPushMatrix();
	glTranslatef(4.8f, 4.55f, -9.78f);
	drawBox(2.8f, 0.9f, 0.06f);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	setColor3(0.08f, 0.82f, 0.95f);
	glRasterPos3f(3.9f, 4.5f, -9.74f);
	drawText("00:01:29:54", GLUT_BITMAP_TIMES_ROMAN_24);
	glEnable(GL_LIGHTING);

	/* Conference table */
	setColor3(0.18f, 0.1f, 0.06f);
	glPushMatrix();
	glTranslatef(-1.0f, 0.9f, -2.5f);
	drawBox(8.6f, 0.15f, 1.95f);
	glPopMatrix();

	setColor3(0.13f, 0.08f, 0.05f);
	for (i = 0; i < 4; ++i) {
		float lx = -4.6f + i * 2.4f;
		glPushMatrix();
		glTranslatef(lx, 0.44f, -3.2f);
		drawBox(0.22f, 0.88f, 0.22f);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(lx, 0.44f, -1.8f);
		drawBox(0.22f, 0.88f, 0.22f);
		glPopMatrix();
	}

	/* Chairs and seated team */
	drawChair(-4.3f, -1.15f, 170.0f);
	drawChair(-2.6f, -1.2f, 172.0f);
	drawChair(-0.9f, -1.18f, 175.0f);
	drawChair(-4.0f, -3.95f, -170.0f);
	drawChair(-2.2f, -3.9f, -172.0f);
	drawChair(-0.3f, -3.87f, -175.0f);

	drawScientistSeated(-4.3f, -1.28f, 12.0f, 0.02f, 0.14f, 0.1f, 0.08f);
	drawScientistSeated(-2.6f, -1.34f, 9.0f, 0.06f, 0.12f, 0.09f, 0.07f);
	drawScientistSeated(-0.9f, -1.30f, 16.0f, 0.00f, 0.16f, 0.12f, 0.09f);
	drawScientistSeated(-4.0f, -3.82f, -8.0f, 0.08f, 0.1f, 0.08f, 0.06f);
	drawScientistSeated(-2.2f, -3.78f, -12.0f, 0.04f, 0.18f, 0.13f, 0.1f);
	drawScientistSeated(-0.3f, -3.73f, -14.0f, 0.03f, 0.11f, 0.1f, 0.08f);

	/* Table props */
	drawLaptop(-3.5f, 0.98f, -2.0f, 0.0f);
	drawLaptop(-1.9f, 0.98f, -2.55f, 0.08f);
	drawLaptop(-0.1f, 0.98f, -2.2f, 0.12f);
	drawNotebook(-4.5f, 0.98f, -2.8f, 0.1f);
	drawNotebook(-2.9f, 0.98f, -1.9f, 0.02f);
	drawNotebook(-1.1f, 0.98f, -3.0f, 0.14f);
	drawMug(-4.0f, 0.98f, -2.1f);
	drawMug(-2.2f, 0.98f, -2.85f);
	drawMug(-0.45f, 0.98f, -2.4f);

	/* Podium and MOM model */
	setColor3(0.2f, 0.12f, 0.08f);
	glPushMatrix();
	glTranslatef(2.2f, 1.0f, -7.7f);
	drawBox(1.1f, 1.9f, 1.0f);
	glPopMatrix();
	drawSatelliteModel(2.2f, 2.0f, -7.65f);

	/* Senior presenters */
	drawSeniorWoman(1.1f, -7.8f);
	drawSeniorMan(3.2f, -7.75f);
}

static void setupLights(void) {
	GLfloat ambient[] = {0.12f, 0.12f, 0.14f, 1.0f};
	GLfloat warmPos0[] = {-2.2f, 5.1f, -1.8f, 1.0f};
	GLfloat warmPos1[] = {2.2f, 5.1f, -6.4f, 1.0f};
	GLfloat warmDiff[] = {1.0f, 0.86f, 0.72f, 1.0f};
	GLfloat coolFillPos[] = {6.0f, 4.5f, 1.0f, 1.0f};
	GLfloat coolFillDiff[] = {0.25f, 0.32f, 0.48f, 1.0f};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_POSITION, warmPos0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, warmDiff);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glLightfv(GL_LIGHT1, GL_POSITION, warmPos1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, warmDiff);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);

	glLightfv(GL_LIGHT2, GL_POSITION, coolFillPos);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, coolFillDiff);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
}

static void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* Centered middle-of-table shot toward board and presenters. */
	gluLookAt(-1.9, 3.0, 5.6,
			  -0.1, 1.55, -17.0,
			  0.0, 1.0, 0.0);

	setupLights();
	drawConferenceRoom();

	glutSwapBuffers();
}

static void reshape(int w, int h) {
	if (h == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(42.0, (double)w / (double)h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

static void timer(int value) {
	(void)value;
	gTime += 0.016f;
	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);
}

static void keyboard(unsigned char key, int x, int y) {
	(void)x;
	(void)y;
	if (key == 27 || key == 'q' || key == 'Q') {
		exit(0);
	}
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1366, 768);
	glutCreateWindow("ISRO Project Review - Conference Room");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.05f, 0.05f, 0.06f, 1.0f);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(16, timer, 0);

	glutMainLoop();
	return 0;
}
