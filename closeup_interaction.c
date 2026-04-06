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
		float a = step * (float)i;
		float x = cosf(a) * radius;
		float y = sinf(a) * radius;
		glNormal3f(cosf(a), sinf(a), 0.0f);
		glVertex3f(x, y, 0.0f);
		glVertex3f(x, y, height);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (i = 0; i <= slices; ++i) {
		float a = step * (float)i;
		glVertex3f(cosf(a) * radius, sinf(a) * radius, 0.0f);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, height);
	for (i = 0; i <= slices; ++i) {
		float a = -step * (float)i;
		glVertex3f(cosf(a) * radius, sinf(a) * radius, height);
	}
	glEnd();
}

static void drawText(const char *txt, void *font) {
	int i;
	for (i = 0; txt[i] != '\0'; ++i) {
		glutBitmapCharacter(font, txt[i]);
	}
}

static void applyMatteMaterial(void) {
	GLfloat spec[] = {0.08f, 0.08f, 0.1f, 1.0f};
	GLfloat amb[] = {0.22f, 0.22f, 0.24f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 12.0f);
}

static void applyShinyMaterial(void) {
	GLfloat spec[] = {0.92f, 0.95f, 1.0f, 1.0f};
	GLfloat amb[] = {0.09f, 0.1f, 0.14f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 96.0f);
}

static void drawMissionPatch(void) {
	setColor3(0.92f, 0.36f, 0.14f);
	glutSolidSphere(0.05f, 14, 14);
	setColor3(0.95f, 0.82f, 0.2f);
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.028f);
	drawBox(0.012f, 0.09f, 0.01f);
	glPopMatrix();
}

static void drawIDLanyard(void) {
	setColor3(0.15f, 0.16f, 0.2f);
	glPushMatrix();
	glTranslatef(-0.04f, 1.48f, 0.105f);
	drawBox(0.02f, 0.15f, 0.01f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.04f, 1.48f, 0.105f);
	drawBox(0.02f, 0.15f, 0.01f);
	glPopMatrix();
	setColor3(0.2f, 0.45f, 0.85f);
	glPushMatrix();
	glTranslatef(0.0f, 1.3f, 0.105f);
	drawBox(0.08f, 0.11f, 0.012f);
	glPopMatrix();
}

static void drawFaceDetails(int glasses) {
	setColor3(0.05f, 0.05f, 0.06f);
	glPushMatrix();
	glTranslatef(-0.04f, 1.63f, 0.105f);
	glutSolidSphere(0.012f, 10, 10);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.04f, 1.63f, 0.105f);
	glutSolidSphere(0.012f, 10, 10);
	glPopMatrix();

	if (glasses) {
		glDisable(GL_LIGHTING);
		glColor3f(0.08f, 0.08f, 0.1f);
		glBegin(GL_LINE_LOOP);
		glVertex3f(-0.06f, 1.64f, 0.108f);
		glVertex3f(-0.02f, 1.64f, 0.108f);
		glVertex3f(-0.02f, 1.61f, 0.108f);
		glVertex3f(-0.06f, 1.61f, 0.108f);
		glEnd();
		glBegin(GL_LINE_LOOP);
		glVertex3f(0.02f, 1.64f, 0.108f);
		glVertex3f(0.06f, 1.64f, 0.108f);
		glVertex3f(0.06f, 1.61f, 0.108f);
		glVertex3f(0.02f, 1.61f, 0.108f);
		glEnd();
		glBegin(GL_LINES);
		glVertex3f(-0.02f, 1.625f, 0.108f);
		glVertex3f(0.02f, 1.625f, 0.108f);
		glEnd();
		glEnable(GL_LIGHTING);
	}
}

static void drawScientistWoman(float x, float z) {
	float sway = sinf(gTime * 0.5f) * 0.02f;
	float headTilt = sinf(gTime * 2.0f) * 5.0f;
	float nod = sinf(gTime * 1.9f) * 3.2f;

	glPushMatrix();
	glTranslatef(x, sway, z);
	glRotatef(15.0f, 0.0f, 1.0f, 0.0f);

	applyMatteMaterial();
	setColor3(0.95f, 0.95f, 0.95f);
	glPushMatrix();
	glTranslatef(0.0f, 1.1f, 0.0f);
	drawBox(0.42f, 0.78f, 0.24f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.16f, 1.06f, 0.02f);
	drawBox(0.1f, 0.56f, 0.1f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.16f, 1.06f, 0.02f);
	drawBox(0.1f, 0.56f, 0.1f);
	glPopMatrix();

	setColor3(0.22f, 0.28f, 0.5f);
	glPushMatrix();
	glTranslatef(0.0f, 0.95f, 0.02f);
	drawBox(0.26f, 0.38f, 0.18f);
	glPopMatrix();
	setColor3(0.2f, 0.2f, 0.24f);
	glPushMatrix();
	glTranslatef(-0.08f, 0.42f, 0.0f);
	drawBox(0.12f, 0.54f, 0.12f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.08f, 0.42f, 0.0f);
	drawBox(0.12f, 0.54f, 0.12f);
	glPopMatrix();

	/* Head with active listening tilt. */
	setColor3(0.86f, 0.69f, 0.57f);
	glPushMatrix();
	glTranslatef(0.0f, 1.62f, 0.0f);
	glRotatef(headTilt + nod, 1.0f, 0.0f, 0.0f);
	glutSolidSphere(0.13f, 18, 18);
	glPopMatrix();

	setColor3(0.16f, 0.1f, 0.08f);
	glPushMatrix();
	glTranslatef(0.0f, 1.72f, -0.02f);
	glScalef(1.0f, 0.8f, 1.0f);
	glutSolidSphere(0.12f, 14, 14);
	glPopMatrix();

	drawFaceDetails(1);
	drawIDLanyard();

	glPushMatrix();
	glTranslatef(-0.19f, 1.24f, 0.1f);
	drawMissionPatch();
	glPopMatrix();

	glPopMatrix();
}

static void drawScientistMan(float x, float z) {
	float sway = sinf(gTime * 0.5f + 0.8f) * 0.02f;
	float explain = (sinf(gTime * 1.7f) > 0.0f) ? (18.0f + 14.0f * sinf(gTime * 3.2f)) : 4.0f;

	glPushMatrix();
	glTranslatef(x, sway, z);
	glRotatef(-18.0f, 0.0f, 1.0f, 0.0f);

	applyMatteMaterial();
	setColor3(0.95f, 0.95f, 0.95f);
	glPushMatrix();
	glTranslatef(0.0f, 1.1f, 0.0f);
	drawBox(0.44f, 0.8f, 0.24f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.16f, 1.06f, 0.02f);
	drawBox(0.1f, 0.56f, 0.1f);
	glPopMatrix();

	/* Explaining right arm motion. */
	glPushMatrix();
	glTranslatef(0.2f, 1.1f, 0.0f);
	glRotatef(-explain, 1.0f, 0.0f, 0.0f);
	drawBox(0.1f, 0.5f, 0.1f);
	glPopMatrix();

	setColor3(0.24f, 0.3f, 0.55f);
	glPushMatrix();
	glTranslatef(0.0f, 0.95f, 0.02f);
	drawBox(0.26f, 0.38f, 0.18f);
	glPopMatrix();
	setColor3(0.2f, 0.2f, 0.24f);
	glPushMatrix();
	glTranslatef(-0.08f, 0.42f, 0.0f);
	drawBox(0.12f, 0.54f, 0.12f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.08f, 0.42f, 0.0f);
	drawBox(0.12f, 0.54f, 0.12f);
	glPopMatrix();

	setColor3(0.87f, 0.7f, 0.58f);
	glPushMatrix();
	glTranslatef(0.0f, 1.62f, 0.0f);
	glutSolidSphere(0.13f, 18, 18);
	glPopMatrix();

	setColor3(0.08f, 0.07f, 0.06f);
	glPushMatrix();
	glTranslatef(0.0f, 1.71f, -0.02f);
	glScalef(1.0f, 0.7f, 1.0f);
	glutSolidSphere(0.12f, 14, 14);
	glPopMatrix();

	drawFaceDetails(0);
	drawIDLanyard();

	glPushMatrix();
	glTranslatef(0.19f, 1.24f, 0.1f);
	drawMissionPatch();
	glPopMatrix();

	glPopMatrix();
}

static void drawSoftBackground(void) {
	/* Defocused corridor impression with muted colors for bokeh-like depth. */
	applyShinyMaterial();
	setColor3(0.08f, 0.12f, 0.2f);
	glBegin(GL_QUADS);
	glVertex3f(-4.6f, 0.0f, 3.0f);
	glVertex3f(4.6f, 0.0f, 3.0f);
	glVertex3f(4.6f, 0.0f, -16.0f);
	glVertex3f(-4.6f, 0.0f, -16.0f);
	glEnd();

	setColor3(0.12f, 0.2f, 0.38f);
	glBegin(GL_QUADS);
	glVertex3f(-4.6f, 0.0f, -16.0f);
	glVertex3f(-4.6f, 3.4f, -16.0f);
	glVertex3f(-4.6f, 3.4f, 3.0f);
	glVertex3f(-4.6f, 0.0f, 3.0f);
	glEnd();
	setColor3(0.16f, 0.24f, 0.42f);
	glBegin(GL_QUADS);
	glVertex3f(4.6f, 0.0f, -16.0f);
	glVertex3f(4.6f, 3.4f, -16.0f);
	glVertex3f(4.6f, 3.4f, 3.0f);
	glVertex3f(4.6f, 0.0f, 3.0f);
	glEnd();

	setColor3(0.64f, 0.68f, 0.74f);
	glBegin(GL_QUADS);
	glVertex3f(-4.6f, 3.4f, 3.0f);
	glVertex3f(4.6f, 3.4f, 3.0f);
	glVertex3f(4.6f, 3.4f, -16.0f);
	glVertex3f(-4.6f, 3.4f, -16.0f);
	glEnd();

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	setColor3(0.95f, 0.96f, 1.0f);
	glColor4f(0.95f, 0.96f, 1.0f, 0.14f);
	{
		int i;
		for (i = 0; i < 5; ++i) {
			float z = -2.5f - i * 2.9f;
			float w = 1.4f + 0.3f * sinf(gTime * 0.7f + i);
			glBegin(GL_QUADS);
			glVertex3f(-w, 2.5f, z);
			glVertex3f(w, 2.5f, z);
			glVertex3f(w, 2.9f, z);
			glVertex3f(-w, 2.9f, z);
			glEnd();
		}
	}
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

static void drawPeople(void) {
	/* Tight conversational staging around frame center. */
	drawScientistWoman(-0.72f, -2.25f);
	drawScientistMan(0.72f, -2.2f);

	glDisable(GL_LIGHTING);
	glColor3f(0.92f, 0.95f, 0.99f);
	glRasterPos3f(-1.42f, 2.32f, -2.0f);
	drawText("Mission Mangal: Trajectory Review", GLUT_BITMAP_HELVETICA_18);
	glColor3f(0.86f, 0.9f, 1.0f);
	glRasterPos3f(-1.2f, 2.08f, -2.0f);
	drawText("Scientist Discussion", GLUT_BITMAP_HELVETICA_12);
	glEnable(GL_LIGHTING);
}

static void setupLights(void) {
	GLfloat lightPos0[] = {2.0f, 3.2f, 3.0f, 1.0f};
	GLfloat lightPos1[] = {-2.5f, 2.2f, 1.5f, 1.0f};
	GLfloat lightPos2[] = {0.0f, 1.9f, 0.8f, 1.0f};
	GLfloat diffuse0[] = {0.66f, 0.74f, 0.92f, 1.0f};
	GLfloat diffuse1[] = {0.4f, 0.5f, 0.72f, 1.0f};
	GLfloat diffuse2[] = {1.0f, 0.9f, 0.8f, 1.0f};
	GLfloat ambient[] = {0.14f, 0.18f, 0.28f, 1.0f};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
}

static void setupFog(void) {
	GLfloat fogColor[] = {0.1f, 0.14f, 0.23f, 1.0f};
	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_START, 2.8f);
	glFogf(GL_FOG_END, 11.5f);
	glHint(GL_FOG_HINT, GL_NICEST);
}

static void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* Medium close-up camera at shoulder height, focused between scientists. */
	gluLookAt(0.0, 1.62, 1.9,
			  0.0, 1.45, -2.25,
			  0.0, 1.0, 0.0);

	setupLights();
	setupFog();
	drawSoftBackground();
	drawPeople();

	glutSwapBuffers();
}

static void reshape(int w, int h) {
	if (h == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (double)w / (double)h, 0.1, 80.0);
	glMatrixMode(GL_MODELVIEW);
}

static void timer(int value) {
	(void)value;
	gTime += 0.016f;
	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1366, 768);
	glutCreateWindow("ISRO Scientists Close-Up Conversation");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.08f, 0.11f, 0.18f, 1.0f);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(16, timer, 0);

	glutMainLoop();
	return 0;
}
