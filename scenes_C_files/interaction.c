#include <GL/glut.h>
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

static void applyShinyFloorMaterial(void) {
	GLfloat spec[] = {0.92f, 0.95f, 1.0f, 1.0f};
	GLfloat amb[] = {0.08f, 0.1f, 0.14f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 108.0f);
}

static void applyMatteCoatMaterial(void) {
	GLfloat spec[] = {0.08f, 0.08f, 0.1f, 1.0f};
	GLfloat amb[] = {0.22f, 0.22f, 0.24f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 12.0f);
}

static void applyDefaultMaterial(void) {
	GLfloat spec[] = {0.3f, 0.3f, 0.32f, 1.0f};
	GLfloat amb[] = {0.25f, 0.25f, 0.28f, 1.0f};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 28.0f);
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

static void drawDoor(float x, float y, float z, float w, float h, const char *label) {
	glPushMatrix();
	glTranslatef(x, y, z);

	setColor3(0.88f, 0.89f, 0.9f);
	glPushMatrix();
	glTranslatef(0.0f, h * 0.5f, 0.0f);
	drawBox(w, h, 0.08f);
	glPopMatrix();

	setColor3(0.72f, 0.74f, 0.77f);
	glPushMatrix();
	glTranslatef(0.02f, h * 0.5f, 0.045f);
	drawBox(w - 0.14f, h - 0.12f, 0.01f);
	glPopMatrix();

	setColor3(0.35f, 0.37f, 0.4f);
	glPushMatrix();
	glTranslatef(w * 0.18f, h * 0.5f, 0.06f);
	drawBox(0.04f, 0.16f, 0.02f);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glColor3f(0.18f, 0.2f, 0.22f);
	glRasterPos3f(-w * 0.14f, h * 0.82f, 0.065f);
	drawText(label, GLUT_BITMAP_8_BY_13);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

static void drawPlant(float x, float y, float z, float scale) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(scale, scale, scale);

	setColor3(0.24f, 0.14f, 0.08f);
	glPushMatrix();
	glTranslatef(0.0f, 0.18f, 0.0f);
	drawBox(0.26f, 0.34f, 0.26f);
	glPopMatrix();

	setColor3(0.28f, 0.56f, 0.22f);
	glPushMatrix();
	glTranslatef(0.0f, 0.66f, 0.0f);
	glutSolidSphere(0.22f, 14, 14);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.13f, 0.76f, 0.0f);
	glutSolidSphere(0.16f, 14, 14);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.12f, 0.82f, 0.0f);
	glutSolidSphere(0.17f, 14, 14);
	glPopMatrix();

	glPopMatrix();
}

static void drawBlurredOfficePerson(float x, float y, float z, float tint, float height) {
	glPushMatrix();
	glTranslatef(x, y, z);

	setColor3(0.55f * tint, 0.58f * tint, 0.62f * tint);
	glPushMatrix();
	glTranslatef(0.0f, height * 0.55f, 0.0f);
	drawBox(0.18f, height * 0.62f, 0.14f);
	glPopMatrix();

	setColor3(0.82f, 0.72f, 0.6f);
	glPushMatrix();
	glTranslatef(0.0f, height * 0.98f, 0.0f);
	glutSolidSphere(0.08f, 10, 10);
	glPopMatrix();

	setColor3(0.38f * tint, 0.4f * tint, 0.43f * tint);
	glPushMatrix();
	glTranslatef(-0.06f, height * 0.15f, 0.0f);
	drawBox(0.06f, height * 0.42f, 0.06f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.06f, height * 0.15f, 0.0f);
	drawBox(0.06f, height * 0.42f, 0.06f);
	glPopMatrix();

	glPopMatrix();
}

static void drawColleagueWoman(float x, float y, float z, float walkPhase) {
	float stride = sinf(walkPhase) * 0.22f;
	float arm = sinf(walkPhase + 1.1f) * 0.18f;

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

	/* Hair */
	setColor3(0.24f, 0.16f, 0.12f);
	glPushMatrix();
	glTranslatef(0.02f, 1.8f, 0.0f);
	glScalef(0.18f, 0.2f, 0.16f);
	glutSolidSphere(1.0f, 14, 14);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.1f, 1.73f, 0.0f);
	glScalef(0.14f, 0.24f, 0.12f);
	glutSolidSphere(1.0f, 14, 14);
	glPopMatrix();

	setColor3(0.85f, 0.67f, 0.56f);
	glPushMatrix();
	glTranslatef(0.02f, 1.62f, 0.02f);
	glutSolidSphere(0.12f, 14, 14);
	glPopMatrix();

	setColor3(0.42f, 0.45f, 0.32f);
	glPushMatrix();
	glTranslatef(0.0f, 1.1f, 0.0f);
	drawBox(0.44f, 0.68f, 0.22f);
	glPopMatrix();

	setColor3(0.18f, 0.18f, 0.2f);
	glPushMatrix();
	glTranslatef(0.0f, 0.38f, 0.0f);
	drawBox(0.28f, 0.42f, 0.18f);
	glPopMatrix();

	setColor3(0.16f, 0.16f, 0.18f);
	glPushMatrix();
	glTranslatef(-0.11f, 0.0f + stride * 0.05f, 0.0f);
	glRotatef(stride * 28.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.46f, 0.08f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.11f, 0.0f - stride * 0.05f, 0.0f);
	glRotatef(-stride * 28.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.46f, 0.08f);
	glPopMatrix();

	setColor3(0.52f, 0.56f, 0.6f);
	glPushMatrix();
	glTranslatef(-0.24f, 1.08f + arm * 0.2f, 0.0f);
	glRotatef(-12.0f + arm * 34.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.07f, 0.4f, 0.07f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.22f, 1.06f - arm * 0.18f, 0.0f);
	glRotatef(8.0f - arm * 18.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.07f, 0.38f, 0.07f);
	glPopMatrix();

	/* Tote bag */
	setColor3(0.72f, 0.6f, 0.42f);
	glPushMatrix();
	glTranslatef(-0.34f, 0.95f, 0.12f);
	drawBox(0.16f, 0.18f, 0.06f);
	glPopMatrix();

	glPopMatrix();
}

static void drawColleagueMan(float x, float y, float z, float walkPhase) {
	float stride = sinf(walkPhase + 0.8f) * 0.2f;
	float arm = sinf(walkPhase) * 0.16f;

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

	setColor3(0.85f, 0.88f, 0.9f);
	glPushMatrix();
	glTranslatef(0.0f, 1.62f, 0.0f);
	glutSolidSphere(0.12f, 14, 14);
	glPopMatrix();

	setColor3(0.68f, 0.78f, 0.92f);
	glPushMatrix();
	glTranslatef(0.0f, 1.1f, 0.0f);
	drawBox(0.42f, 0.7f, 0.22f);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glColor3f(0.08f, 0.18f, 0.55f);
	glRasterPos3f(-0.17f, 1.16f, 0.16f);
	drawText("ISRO", GLUT_BITMAP_HELVETICA_12);
	glEnable(GL_LIGHTING);

	setColor3(0.22f, 0.23f, 0.26f);
	glPushMatrix();
	glTranslatef(-0.1f, 0.4f, 0.0f);
	drawBox(0.14f, 0.44f, 0.1f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.1f, 0.4f, 0.0f);
	drawBox(0.14f, 0.44f, 0.1f);
	glPopMatrix();

	setColor3(0.28f, 0.28f, 0.31f);
	glPushMatrix();
	glTranslatef(-0.12f, 0.0f + stride * 0.05f, 0.0f);
	glRotatef(stride * 26.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.48f, 0.08f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.12f, 0.0f - stride * 0.05f, 0.0f);
	glRotatef(-stride * 26.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.48f, 0.08f);
	glPopMatrix();

	setColor3(0.85f, 0.67f, 0.56f);
	glPushMatrix();
	glTranslatef(0.24f, 0.94f, 0.16f);
	glRotatef(-8.0f, 0.0f, 0.0f, 1.0f);
	drawCylinder(0.04f, 0.14f, 10);
	glPopMatrix();

	setColor3(0.34f, 0.24f, 0.16f);
	glPushMatrix();
	glTranslatef(0.34f, 0.98f, 0.12f);
	drawBox(0.18f, 0.18f, 0.05f);
	glPopMatrix();

	glPopMatrix();
}

static void drawFireExtinguisherCabinet(float x, float y, float z) {
	glPushMatrix();
	glTranslatef(x, y, z);

	setColor3(0.72f, 0.08f, 0.08f);
	drawBox(0.42f, 0.88f, 0.08f);

	setColor3(0.92f, 0.15f, 0.12f);
	glDisable(GL_LIGHTING);
	glColor3f(0.98f, 0.98f, 0.98f);
	glRasterPos3f(-0.12f, 0.05f, 0.05f);
	drawText("FIRE", GLUT_BITMAP_HELVETICA_12);
	glRasterPos3f(-0.19f, -0.14f, 0.05f);
	drawText("EXTINGUISHER", GLUT_BITMAP_HELVETICA_12);
	glEnable(GL_LIGHTING);

	setColor3(0.96f, 0.8f, 0.2f);
	glPushMatrix();
	glTranslatef(0.0f, 0.02f, 0.06f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	drawCylinder(0.09f, 0.42f, 18);
	glPopMatrix();

	glPopMatrix();
}

static void drawHumanBase(float shirtR, float shirtG, float shirtB, float pantsR, float pantsG, float pantsB, float skinR, float skinG, float skinB) {
	setColor3(shirtR, shirtG, shirtB);
	glPushMatrix();
	glTranslatef(0.0f, 1.05f, 0.0f);
	drawBox(0.36f, 0.62f, 0.22f);
	glPopMatrix();

	/* Lab coat layer over formal shirt. */
	applyMatteCoatMaterial();
	setColor3(0.95f, 0.96f, 0.97f);
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, 0.07f);
	drawBox(0.4f, 0.72f, 0.06f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-0.14f, 1.03f, 0.03f);
	drawBox(0.1f, 0.56f, 0.08f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.14f, 1.03f, 0.03f);
	drawBox(0.1f, 0.56f, 0.08f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0f, 0.78f, 0.1f);
	drawBox(0.08f, 0.22f, 0.04f);
	glPopMatrix();
	applyDefaultMaterial();

	setColor3(pantsR, pantsG, pantsB);
	glPushMatrix();
	glTranslatef(-0.09f, 0.48f, 0.0f);
	drawBox(0.14f, 0.52f, 0.16f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.09f, 0.48f, 0.0f);
	drawBox(0.14f, 0.52f, 0.16f);
	glPopMatrix();

	setColor3(skinR, skinG, skinB);
	glPushMatrix();
	glTranslatef(0.0f, 1.58f, 0.0f);
	glutSolidSphere(0.13f, 18, 18);
	glPopMatrix();

	setColor3(0.16f, 0.17f, 0.18f);
	glPushMatrix();
	glTranslatef(0.0f, 1.72f, 0.0f);
	drawBox(0.24f, 0.08f, 0.22f);
	glPopMatrix();
}

static void drawWoman(float x, float y, float z, float walkPhase, float headingY, float nodAngle, float heightScale, float hairR, float hairG, float hairB) {
	float armSwing = sinf(walkPhase) * 0.18f;
	float legSwing = sinf(walkPhase + 3.14159f) * 0.22f;

	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(1.0f, heightScale, 1.0f);
	glRotatef(headingY, 0.0f, 1.0f, 0.0f);

	drawHumanBase(0.72f, 0.84f, 0.95f, 0.82f, 0.75f, 0.55f, 0.85f, 0.67f, 0.55f);

	setColor3(hairR, hairG, hairB);
	glPushMatrix();
	glTranslatef(-0.08f, 1.74f, 0.02f);
	glRotatef(nodAngle, 1.0f, 0.0f, 0.0f);
	glScalef(0.12f, 0.28f, 0.12f);
	glutSolidSphere(1.0f, 14, 14);
	glPopMatrix();

	setColor3(hairR, hairG, hairB);
	glPushMatrix();
	glTranslatef(0.06f, 1.76f, 0.02f);
	glRotatef(nodAngle, 1.0f, 0.0f, 0.0f);
	glScalef(0.12f, 0.30f, 0.12f);
	glutSolidSphere(1.0f, 14, 14);
	glPopMatrix();

	setColor3(0.78f, 0.82f, 0.86f);
	glPushMatrix();
	glTranslatef(-0.2f, 1.0f + armSwing * 0.3f, 0.0f);
	glRotatef(-18.0f + armSwing * 40.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.07f, 0.42f, 0.07f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.2f, 1.0f - armSwing * 0.2f, 0.0f);
	glRotatef(10.0f - armSwing * 30.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.07f, 0.42f, 0.07f);
	glPopMatrix();

	setColor3(0.55f, 0.52f, 0.45f);
	glPushMatrix();
	glTranslatef(-0.09f, 0.12f + legSwing * 0.08f, 0.0f);
	glRotatef(legSwing * 30.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.48f, 0.08f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.09f, 0.12f - legSwing * 0.08f, 0.0f);
	glRotatef(-legSwing * 30.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.48f, 0.08f);
	glPopMatrix();

	setColor3(0.92f, 0.92f, 0.93f);
	glPushMatrix();
	glTranslatef(-0.18f, 1.05f, 0.15f);
	drawBox(0.18f, 0.24f, 0.03f);
	glPopMatrix();

	glPopMatrix();
}

static void drawMan(float x, float y, float z, float walkPhase, float headingY, float nodAngle, float heightScale, float hairR, float hairG, float hairB) {
	float armSwing = sinf(walkPhase + 3.14159f) * 0.17f;
	float legSwing = sinf(walkPhase) * 0.2f;

	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(1.0f, heightScale, 1.0f);
	glRotatef(headingY, 0.0f, 1.0f, 0.0f);

	drawHumanBase(0.95f, 0.97f, 0.98f, 0.3f, 0.32f, 0.36f, 0.86f, 0.69f, 0.55f);

	glDisable(GL_LIGHTING);
	glColor3f(0.08f, 0.18f, 0.55f);
	glRasterPos3f(-0.16f, 1.15f, 0.16f);
	drawText("ISRO", GLUT_BITMAP_HELVETICA_12);
	glEnable(GL_LIGHTING);

	setColor3(hairR, hairG, hairB);
	glPushMatrix();
	glTranslatef(0.0f, 1.67f, 0.02f);
	glRotatef(nodAngle, 1.0f, 0.0f, 0.0f);
	glScalef(0.11f, 0.24f, 0.11f);
	glutSolidSphere(1.0f, 14, 14);
	glPopMatrix();

	setColor3(0.84f, 0.86f, 0.9f);
	glPushMatrix();
	glTranslatef(-0.2f, 1.03f, 0.0f);
	glRotatef(-10.0f + armSwing * 30.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.07f, 0.42f, 0.07f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.2f, 1.03f, 0.0f);
	glRotatef(16.0f - armSwing * 26.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.07f, 0.42f, 0.07f);
	glPopMatrix();

	setColor3(0.22f, 0.24f, 0.28f);
	glPushMatrix();
	glTranslatef(-0.09f, 0.12f + legSwing * 0.08f, 0.0f);
	glRotatef(legSwing * 28.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.48f, 0.08f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.09f, 0.12f - legSwing * 0.08f, 0.0f);
	glRotatef(-legSwing * 28.0f, 1.0f, 0.0f, 0.0f);
	drawBox(0.08f, 0.48f, 0.08f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.18f, 0.98f, 0.16f);
	glRotatef(8.0f, 0.0f, 0.0f, 1.0f);
	setColor3(0.94f, 0.82f, 0.66f);
	glutSolidSphere(0.08f, 12, 12);
	glPopMatrix();

	glPopMatrix();
}

static void drawCorridor(void) {
	int i;

	/* Polished epoxy floor: deep charcoal/navy with strong highlights. */
	applyShinyFloorMaterial();
	setColor3(0.11f, 0.14f, 0.2f);
	glBegin(GL_QUADS);
	glVertex3f(-7.5f, 0.0f, 2.0f);
	glVertex3f(7.5f, 0.0f, 2.0f);
	glVertex3f(12.5f, 0.0f, -46.0f);
	glVertex3f(-12.5f, 0.0f, -46.0f);
	glEnd();
	applyDefaultMaterial();

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.66f, 0.75f, 0.9f, 0.09f);
	glBegin(GL_QUADS);
	glVertex3f(-3.6f, 0.02f, 1.2f);
	glVertex3f(5.8f, 0.02f, 1.2f);
	glVertex3f(8.4f, 0.02f, -44.8f);
	glVertex3f(-5.4f, 0.02f, -44.8f);
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	/* Two-tone ISRO wall: space-blue lower panel and cream upper section. */
	setColor3(0.08f, 0.22f, 0.46f);
	glBegin(GL_QUADS);
	glVertex3f(-7.6f, 0.0f, 2.0f);
	glVertex3f(-7.6f, 2.35f, 2.0f);
	glVertex3f(-12.6f, 2.35f, -46.0f);
	glVertex3f(-12.6f, 0.0f, -46.0f);
	glEnd();

	setColor3(0.95f, 0.93f, 0.88f);
	glBegin(GL_QUADS);
	glVertex3f(-7.2f, 2.3f, 2.0f);
	glVertex3f(-7.2f, 7.4f, 2.0f);
	glVertex3f(-11.6f, 7.4f, -46.0f);
	glVertex3f(-11.6f, 2.3f, -46.0f);
	glEnd();

	/* Right wall glass panels */
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.75f, 0.87f, 0.96f, 0.42f);
	glBegin(GL_QUADS);
	glVertex3f(7.0f, 0.2f, 1.6f);
	glVertex3f(7.0f, 7.0f, 1.6f);
	glVertex3f(12.0f, 7.0f, -45.0f);
	glVertex3f(12.0f, 0.2f, -45.0f);
	glEnd();

	glColor4f(0.76f, 0.88f, 0.98f, 0.14f);
	for (i = 0; i < 5; ++i) {
		float z = -6.0f - i * 8.0f;
		glBegin(GL_LINE_LOOP);
		glVertex3f(7.1f, 0.2f, z);
		glVertex3f(12.0f, 0.2f, z - 0.6f);
		glVertex3f(12.0f, 6.9f, z - 0.6f);
		glVertex3f(7.1f, 6.9f, z);
		glEnd();
	}
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	/* Metallic ceiling grid. */
	setColor3(0.72f, 0.76f, 0.82f);
	glBegin(GL_QUADS);
	glVertex3f(-7.6f, 7.4f, 2.0f);
	glVertex3f(-7.2f, 7.4f, 2.0f);
	glVertex3f(-11.6f, 7.4f, -46.0f);
	glVertex3f(-12.6f, 7.4f, -46.0f);
	glEnd();
	glDisable(GL_LIGHTING);
	glColor4f(0.84f, 0.88f, 0.94f, 0.34f);
	for (i = 0; i < 11; ++i) {
		float z = 1.0f - i * 4.5f;
		glBegin(GL_LINES);
		glVertex3f(-9.6f, 7.38f, z);
		glVertex3f(9.0f, 7.38f, z - 0.4f);
		glEnd();
	}
	glEnable(GL_LIGHTING);

	/* Ceiling lights */
	for (i = 0; i < 12; ++i) {
		float z = 0.5f - i * 4.2f;
		float pulse = 0.95f + 0.03f * sinf(gTime * 1.2f + i * 0.35f);
		setColor3(0.98f * pulse, 0.98f * pulse, 0.98f * pulse);
		glPushMatrix();
		glTranslatef(0.0f, 7.28f, z);
		drawBox(8.8f, 0.08f, 0.16f);
		glPopMatrix();

		glDisable(GL_LIGHTING);
		glColor4f(1.0f, 0.98f, 0.94f, 0.3f * pulse);
		glBegin(GL_QUADS);
		glVertex3f(-3.7f, 7.38f, z - 0.08f);
		glVertex3f(3.7f, 7.38f, z - 0.08f);
		glVertex3f(3.7f, 7.38f, z + 0.08f);
		glVertex3f(-3.7f, 7.38f, z + 0.08f);
		glEnd();
		glEnable(GL_LIGHTING);
	}

	/* Department doors */
	drawDoor(-7.48f, 0.0f, -7.0f, 1.35f, 3.8f, "SAT INTEG");
	drawDoor(-7.48f, 0.0f, -15.0f, 1.35f, 3.8f, "PROP LAB");
	drawDoor(-7.48f, 0.0f, -23.0f, 1.35f, 3.8f, "MISSION CTRL");
	drawDoor(-7.48f, 0.0f, -31.0f, 1.35f, 3.8f, "PAYLOAD OPS");

	/* Pulse warning beacon near Satellite Integration. */
	glPushMatrix();
	glTranslatef(-6.95f, 2.45f, -7.2f);
	setColor3(0.35f, 0.04f, 0.05f);
	drawBox(0.16f, 0.12f, 0.16f);
	glTranslatef(0.0f, 0.09f, 0.0f);
	setColor3(0.92f, 0.07f + 0.22f * (0.5f + 0.5f * sinf(gTime * 3.5f)), 0.08f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	drawCylinder(0.06f, 0.12f, 20);
	glPopMatrix();

	/* Wall panels and seam lines */
	glDisable(GL_LIGHTING);
	glColor3f(0.93f, 0.9f, 0.82f);
	for (i = 0; i < 10; ++i) {
		float z = 0.5f - i * 4.2f;
		glBegin(GL_LINES);
		glVertex3f(-7.55f, 0.1f, z);
		glVertex3f(-7.55f, 7.2f, z);
		glEnd();
	}
	for (i = 0; i < 6; ++i) {
		float z = -1.5f - i * 7.0f;
		glBegin(GL_LINES);
		glVertex3f(-12.6f, 0.2f, z);
		glVertex3f(-7.6f, 0.2f, z);
		glVertex3f(7.0f, 0.2f, z);
		glVertex3f(12.3f, 0.2f, z - 0.5f);
		glEnd();
	}
	glEnable(GL_LIGHTING);

	/* Safety posters and digital info panels. */
	setColor3(0.22f, 0.24f, 0.28f);
	glPushMatrix();
	glTranslatef(-7.24f, 4.85f, -12.5f);
	drawBox(1.05f, 0.78f, 0.05f);
	glPopMatrix();
	setColor3(0.88f, 0.9f, 0.92f);
	glPushMatrix();
	glTranslatef(-7.22f, 4.86f, -12.5f);
	drawBox(0.95f, 0.68f, 0.02f);
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(0.84f, 0.08f, 0.08f);
	glRasterPos3f(-7.6f, 5.05f, -12.46f);
	drawText("SAFETY FIRST", GLUT_BITMAP_HELVETICA_12);
	glColor3f(0.14f, 0.2f, 0.46f);
	glRasterPos3f(-7.61f, 4.83f, -12.46f);
	drawText("PPE MANDATORY", GLUT_BITMAP_HELVETICA_12);
	glEnable(GL_LIGHTING);

	setColor3(0.08f, 0.12f, 0.2f);
	glPushMatrix();
	glTranslatef(-7.24f, 4.85f, -27.5f);
	drawBox(1.12f, 0.82f, 0.06f);
	glPopMatrix();
	setColor3(0.06f, 0.55f, 0.9f);
	glPushMatrix();
	glTranslatef(-7.22f, 4.85f, -27.5f);
	drawBox(0.98f, 0.68f, 0.02f);
	glPopMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(0.92f, 0.98f, 1.0f);
	glRasterPos3f(-7.58f, 4.98f, -27.46f);
	drawText("SYSTEM HEALTH 97%", GLUT_BITMAP_HELVETICA_12);
	glColor3f(0.92f, 0.98f, 1.0f);
	glRasterPos3f(-7.58f, 4.78f, -27.46f);
	drawText("NETWORK STABLE", GLUT_BITMAP_HELVETICA_12);
	glEnable(GL_LIGHTING);

	/* Glass side view with blurred skyline and atrium */
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.72f, 0.82f, 0.96f, 0.22f);
	glBegin(GL_QUADS);
	glVertex3f(7.2f, 0.2f, 1.5f);
	glVertex3f(12.1f, 0.2f, -45.3f);
	glVertex3f(12.1f, 6.85f, -45.3f);
	glVertex3f(7.2f, 6.85f, 1.5f);
	glEnd();

	for (i = 0; i < 9; ++i) {
		float x = 8.0f + i * 0.42f;
		float h = 0.25f + 0.22f * sinf((float)i * 1.7f + 0.8f);
		glColor4f(0.62f + 0.03f * i, 0.68f + 0.02f * i, 0.76f + 0.02f * i, 0.18f);
		glBegin(GL_QUADS);
		glVertex3f(x, 2.0f, -30.0f - i * 1.8f);
		glVertex3f(x + 0.2f, 2.0f, -30.0f - i * 1.8f);
		glVertex3f(x + 0.2f, 2.0f + h, -30.0f - i * 1.8f);
		glVertex3f(x, 2.0f + h, -30.0f - i * 1.8f);
		glEnd();
	}
	glColor4f(0.78f, 0.84f, 0.9f, 0.12f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(9.3f, 0.8f, -23.0f);
	for (i = 0; i <= 18; ++i) {
		float a = 6.2831853f * (float)i / 18.0f;
		glVertex3f(9.3f + cosf(a) * 1.6f, 0.8f + sinf(a) * 0.95f, -23.0f);
	}
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	/* Potted greenery */
	drawPlant(1.8f, 0.0f, -10.0f, 1.0f);
	drawPlant(2.2f, 0.0f, -25.0f, 0.92f);
	drawPlant(3.0f, 0.0f, -34.0f, 0.84f);

	/* Blurred figures in glass-walled offices */
	drawBlurredOfficePerson(8.8f, 0.0f, -16.0f, 0.95f, 1.55f);
	drawBlurredOfficePerson(9.9f, 0.0f, -27.0f, 0.88f, 1.62f);
	drawBlurredOfficePerson(10.5f, 0.0f, -37.0f, 0.8f, 1.58f);

	/* Far end wall with simplified 3D ISRO emblem. */
	setColor3(0.86f, 0.9f, 0.95f);
	glBegin(GL_QUADS);
	glVertex3f(-5.6f, 0.0f, -45.8f);
	glVertex3f(5.8f, 0.0f, -45.8f);
	glVertex3f(5.8f, 7.1f, -45.8f);
	glVertex3f(-5.6f, 7.1f, -45.8f);
	glEnd();

	glPushMatrix();
	glTranslatef(0.25f, 4.35f, -45.45f);
	setColor3(0.1f, 0.22f, 0.6f);
	glutSolidSphere(0.68f, 30, 30);
	setColor3(0.98f, 0.58f, 0.12f);
	glTranslatef(0.0f, -0.1f, 0.35f);
	glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
	drawBox(0.14f, 1.18f, 0.08f);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glColor3f(0.08f, 0.2f, 0.52f);
	glRasterPos3f(-1.05f, 3.1f, -45.42f);
	drawText("ISRO", GLUT_BITMAP_HELVETICA_18);
	glEnable(GL_LIGHTING);

	drawFireExtinguisherCabinet(-7.35f, 0.88f, -20.4f);
}

static void drawPeople(void) {
	float cycle = fmodf(gTime, 14.0f);
	float womanX = 2.9f;
	float manX = 4.25f;
	float womanZ = -8.0f;
	float manZ = -24.0f;
	float womanHeading = 176.0f;
	float manHeading = 184.0f;
	float nod = 0.0f;
	float handGesture = 0.0f;
	float walkScale = 1.0f;

	if (cycle < 5.0f) {
		float p = cycle / 5.0f;
		womanZ = -8.0f - p * 8.0f;
		manZ = -24.0f + p * 8.0f;
	} else if (cycle < 8.0f) {			//Modified 8 to 10
		float p = cycle - 5.0f;
		womanZ = -16.1f;
		manZ = -15.1f;
		womanHeading = 205.0f;
		manHeading = 155.0f;
		nod = sinf(p * 7.0f) * 8.0f;
		handGesture = sinf(p * 5.6f) * 20.0f;
		walkScale = 0.25f;
	} else {
		float p = (cycle - 8.0f) / 6.0f;		//Modified 6 to 4
		womanZ = -16.1f - p * 10.0f;
		manZ = -15.1f + p * 10.0f;
		womanHeading = 170.0f;
		manHeading = 190.0f;
	}

	drawWoman(womanX, 0.0f, womanZ, gTime * 4.8f * walkScale, womanHeading, nod, 1.02f, 0.19f, 0.12f, 0.1f);
	drawMan(manX, 0.0f, manZ, gTime * 4.5f * walkScale + 0.8f, manHeading, -nod * 0.6f, 0.96f, 0.08f, 0.07f, 0.06f);

	/* Foreground motion blur streaks */
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.8f, 0.86f, 0.95f, 0.07f * walkScale);
	glBegin(GL_QUADS);
	glVertex3f(1.1f, 0.05f, womanZ - 0.8f);
	glVertex3f(5.2f, 0.05f, womanZ - 0.8f);
	glVertex3f(5.6f, 0.05f, manZ + 0.35f);
	glVertex3f(0.8f, 0.05f, manZ + 0.35f);
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	/* Tablet in the woman's hand with gesture during conversation. */
	setColor3(0.82f, 0.77f, 0.58f);
	glPushMatrix();
	glTranslatef(womanX - 0.2f, 1.0f, womanZ + 0.14f);
	glRotatef(10.0f + handGesture, 0.0f, 0.0f, 1.0f);
	drawBox(0.2f, 0.28f, 0.02f);
	glPopMatrix();

	/* Messenger bag across the man's body */
	setColor3(0.36f, 0.24f, 0.16f);
	glPushMatrix();
	glTranslatef(manX + 0.18f, 0.86f, manZ + 0.12f);
	glRotatef(-18.0f, 0.0f, 0.0f, 1.0f);
	drawBox(0.3f, 0.2f, 0.05f);
	glPopMatrix();

	/* Subtle shadow under the pair */
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, 0.12f);
	glBegin(GL_QUADS);
	glVertex3f(1.7f, 0.02f, womanZ - 0.65f);
	glVertex3f(5.0f, 0.02f, womanZ - 0.65f);
	glVertex3f(5.3f, 0.02f, manZ + 0.45f);
	glVertex3f(1.4f, 0.02f, manZ + 0.45f);
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

static void setupLights(void) {
	GLfloat lightPos0[] = {15.0f, 8.0f, -10.0f, 1.0f};
	GLfloat lightPos1[] = {-5.0f, 7.0f, 5.0f, 1.0f};
	GLfloat diffuse0[] = {1.0f, 0.95f, 0.85f, 1.0f};
	GLfloat diffuse1[] = {0.34f, 0.5f, 0.72f, 1.0f};
	GLfloat ambient[] = {0.16f, 0.2f, 0.28f, 1.0f};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_NORMALIZE);
	applyDefaultMaterial();

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
}

static void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.8, 1.95, 8.8,
			  1.6, 1.2, -20.0,
			  0.0, 1.0, 0.0);

	setupLights();
	drawCorridor();
	drawPeople();

	glDisable(GL_LIGHTING);
	glColor3f(0.08f, 0.2f, 0.48f);
	glRasterPos3f(-6.7f, 6.95f, 0.8f);
	drawText("ISRO RESEARCH FACILITY - CORRIDOR", GLUT_BITMAP_HELVETICA_18);
	glEnable(GL_LIGHTING);

	glutSwapBuffers();
}

static void reshape(int w, int h) {
	if (h == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(38.0, (double)w / (double)h, 0.1, 180.0);
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
	glutCreateWindow("ISRO Corridor Interaction");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.9f, 0.92f, 0.94f, 1.0f);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(16, timer, 0);

	glutMainLoop();
	return 0;
}
