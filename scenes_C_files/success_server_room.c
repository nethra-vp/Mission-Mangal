#include "GL/glut.h"
#include <math.h>
#include <stdio.h>

static float gTime = 0.0f;

static void setColor3(float r, float g, float b) {
	GLfloat mat[4] = {r, g, b, 1.0f};
	glColor3f(r, g, b);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
}

static void drawBox(float sx, float sy, float sz) {
	glPushMatrix();
	glScalef(sx, sy, sz);
	glutSolidCube(1.0f);
	glPopMatrix();
}

static void drawBitmapText(const char *txt, void *font) {
	int i;
	for (i = 0; txt[i] != '\0'; ++i) {
		glutBitmapCharacter(font, txt[i]);
	}
}

static void drawStrokeText(const char *txt, void *font, float scale) {
	int i;
	glPushMatrix();
	glScalef(scale, scale, scale);
	for (i = 0; txt[i] != '\0'; ++i) {
		glutStrokeCharacter(font, txt[i]);
	}
	glPopMatrix();
}

static float getStrokeTextWidth(const char *txt, void *font) {
	int i;
	float width = 0.0f;
	for (i = 0; txt[i] != '\0'; ++i) {
		width += (float)glutStrokeWidth(font, txt[i]);
	}
	return width;
}

static void drawVideoWallMainContent(void) {
	int i;
	const int marsSegments = 96;
	const float marsCx = -2.25f;
	const float marsCy = 0.05f;
	const float marsR = 1.85f;
	const float orbitRx = marsR * 1.34f;
	const float orbitRy = marsR * 0.88f;
	/* Increase orbitDepthContrast for a more dramatic hide-behind effect. */
	const float orbitDepthMid = 0.06f;
	const float orbitDepthContrast = 0.04f;
	float t = gTime;
	float pulse = 0.5f + 0.5f * sinf(t * 8.0f);
	float shipA = t * 0.75f;
	float shipX = marsCx + cosf(shipA) * orbitRx;
	float shipY = marsCy + sinf(shipA) * orbitRy;
	float shipZ = orbitDepthMid + orbitDepthContrast * cosf(shipA);
	float shipTangentDeg = atan2f(cosf(shipA) * orbitRy, -sinf(shipA) * orbitRx) * 57.2957795f;

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Deep-space panel base with subtle orange tension glow. */
	glBegin(GL_QUADS);
	glColor3f(0.01f, 0.012f, 0.02f);
	glVertex3f(-5.5f, -2.5f, 0.01f);
	glColor3f(0.02f, 0.015f, 0.03f);
	glVertex3f(5.5f, -2.5f, 0.01f);
	glColor3f(0.03f, 0.02f, 0.02f);
	glVertex3f(5.5f, 2.5f, 0.01f);
	glColor3f(0.012f, 0.012f, 0.022f);
	glVertex3f(-5.5f, 2.5f, 0.01f);
	glEnd();

	/* Sparse stars in the right-side field. */
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	for (i = 0; i < 90; ++i) {
		float sx = -0.2f + 5.4f * ((float)((i * 37) % 97) / 97.0f);
		float sy = -2.2f + 4.2f * ((float)((i * 61) % 89) / 89.0f);
		float tw = 0.45f + 0.55f * sinf(t * 2.5f + i * 0.37f);
		glColor4f(0.6f + 0.35f * tw, 0.72f + 0.22f * tw, 0.95f, 0.7f);
		glVertex3f(sx, sy, 0.012f);
	}
	glEnd();

	/* Mars atmospheric haze and planet body occupying left two-thirds. */
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.96f, 0.42f, 0.12f, 0.38f);
	glVertex3f(marsCx, marsCy, 0.03f);
	for (i = 0; i <= marsSegments; ++i) {
		float a = 6.2831853f * (float)i / (float)marsSegments;
		float rr = marsR * 1.12f;
		glColor4f(0.98f, 0.5f, 0.16f, 0.08f);
		glVertex3f(marsCx + cosf(a) * rr, marsCy + sinf(a) * rr, 0.03f);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.78f, 0.33f, 0.14f);
	glVertex3f(marsCx, marsCy, 0.04f);
	for (i = 0; i <= marsSegments; ++i) {
		float a = 6.2831853f * (float)i / (float)marsSegments;
		float l = 0.7f + 0.3f * cosf(a - 0.65f);
		glColor3f(0.62f + 0.2f * l, 0.24f + 0.13f * l, 0.11f + 0.07f * l);
		glVertex3f(marsCx + cosf(a) * marsR, marsCy + sinf(a) * marsR, 0.04f);
	}
	glEnd();

	/* Surface streaks and craters for a realistic rust texture. */
	glColor4f(0.42f, 0.16f, 0.08f, 0.55f);
	for (i = 0; i < 14; ++i) {
		float y = marsCy - 1.8f + i * 0.27f;
		float xLeft = marsCx - 2.05f + 0.15f * sinf(i * 0.9f + t * 0.3f);
		float xRight = marsCx + 1.85f + 0.15f * cosf(i * 0.8f + t * 0.26f);
		glBegin(GL_LINES);
		glVertex3f(xLeft, y, 0.05f);
		glVertex3f(xRight, y + 0.09f * sinf(i * 0.5f), 0.05f);
		glEnd();
	}
	for (i = 0; i < 18; ++i) {
		float cx = marsCx - 1.7f + (float)((i * 31) % 33) * 0.11f;
		float cy = marsCy - 1.5f + (float)((i * 19) % 29) * 0.1f;
		float rr = 0.05f + 0.06f * (float)((i * 7) % 5) / 4.0f;
		int k;
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(0.37f, 0.13f, 0.07f, 0.42f);
		glVertex3f(cx, cy, 0.051f);
		for (k = 0; k <= 20; ++k) {
			float a = 6.2831853f * (float)k / 20.0f;
			glColor4f(0.29f, 0.1f, 0.05f, 0.0f);
			glVertex3f(cx + cosf(a) * rr, cy + sinf(a) * rr, 0.051f);
		}
		glEnd();
	}

	/* Glowing blue elliptical insertion trajectory around Mars. */
	glLineWidth(2.8f);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= 120; ++i) {
		float a = 6.2831853f * (float)i / 120.0f;
		float ex = marsCx + cosf(a) * orbitRx;
		float ey = marsCy + sinf(a) * orbitRy;
		float vis = 0.25f + 0.75f * (0.5f + 0.5f * cosf(a - t * 0.45f));
		float orbZ = orbitDepthMid + orbitDepthContrast * cosf(a);
		glColor4f(0.16f, 0.72f + 0.25f * vis, 1.0f, 0.22f + 0.42f * vis);
		glVertex3f(ex, ey, orbZ);
	}
	glEnd();
	glLineWidth(1.0f);

	/* Mangalyaan body entering from right side close to Martian limb. */
	glPushMatrix();
	glTranslatef(shipX, shipY, shipZ);
	glRotatef(shipTangentDeg, 0.0f, 0.0f, 1.0f);

	/* Gold thermal blanket central bus. */
	glBegin(GL_QUADS);
	glColor3f(0.96f, 0.78f, 0.25f);
	glVertex3f(-0.18f, -0.12f, 0.0f);
	glVertex3f(0.2f, -0.12f, 0.0f);
	glColor3f(0.76f, 0.55f, 0.12f);
	glVertex3f(0.2f, 0.12f, 0.0f);
	glVertex3f(-0.18f, 0.12f, 0.0f);
	glEnd();

	/* Solar panels. */
	glBegin(GL_QUADS);
	glColor3f(0.15f, 0.35f, 0.78f);
	glVertex3f(-0.75f, -0.07f, 0.0f);
	glVertex3f(-0.22f, -0.07f, 0.0f);
	glVertex3f(-0.22f, 0.07f, 0.0f);
	glVertex3f(-0.75f, 0.07f, 0.0f);
	glVertex3f(0.24f, -0.07f, 0.0f);
	glVertex3f(0.77f, -0.07f, 0.0f);
	glVertex3f(0.77f, 0.07f, 0.0f);
	glVertex3f(0.24f, 0.07f, 0.0f);
	glEnd();

	/* Antenna dish. */
	glColor3f(0.85f, 0.88f, 0.92f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, 0.01f);
	for (i = 0; i <= 24; ++i) {
		float a = 6.2831853f * (float)i / 24.0f;
		glVertex3f(0.0f + cosf(a) * 0.09f, -0.18f + sinf(a) * 0.07f, 0.01f);
	}
	glEnd();

	/* Thruster plume with pulsating orange brightness. */
	glBegin(GL_TRIANGLES);
	glColor4f(1.0f, 0.8f, 0.25f, 0.95f);
	glVertex3f(0.21f, 0.0f, -0.01f);
	glColor4f(1.0f, 0.45f, 0.08f, 0.0f);
	glVertex3f(0.7f + 0.2f * pulse, 0.14f, -0.01f);
	glVertex3f(0.7f + 0.2f * pulse, -0.14f, -0.01f);
	glEnd();

	glPopMatrix();

	/* Tense mission captions on the wall feed. */
	glColor3f(0.87f, 0.9f, 0.95f);
	glRasterPos3f(-5.0f, 2.08f, 0.1f);
	drawBitmapText("MARS ORBIT INSERTION - LIVE VISUAL", GLUT_BITMAP_8_BY_13);

	glColor3f(1.28f, 0.78f, 0.0f);
	glRasterPos3f(1.5f, 1.65f, 1.0f);
	drawBitmapText("MISSION COMPLETE", GLUT_BITMAP_HELVETICA_18);

	glColor3f(0.28f, 0.78f, 1.0f);
	glRasterPos3f(1.5f, -2.08f, 0.1f);
	drawBitmapText("TRAJECTORY LOCK CONFIRMED", GLUT_BITMAP_HELVETICA_12);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

static void drawVideoWall(void) {
	setColor3(0.12f, 0.12f, 0.16f);
	glPushMatrix();
	glTranslatef(0.0f, 5.5f, -31.0f);
	drawBox(42.0f, 14.0f, 0.8f);

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.42f);
	glScalef(2.0f, 2.0f, 1.0f);
	drawVideoWallMainContent();
	glPopMatrix();

	glPopMatrix();
}

static void drawScientist(float x, float y, float z, float r, float g, float b, int gesture) {
	float arm = 0.35f + 0.15f * sinf(gTime * 2.0f + x);
	float jumpCycle = sinf(gTime * 6.0f + x * 0.7f + z * 0.25f);
	float jump = fabsf(jumpCycle) * 0.18f;
	glPushMatrix();
	glTranslatef(x, y + jump, z);

	setColor3(r, g, b);
	glPushMatrix();
	glTranslatef(0.0f, 0.55f, 0.0f);
	drawBox(0.32f, 0.72f, 0.2f);
	glPopMatrix();

	setColor3(0.92f, 0.76f, 0.62f);
	glPushMatrix();
	glTranslatef(0.0f, 1.02f, 0.0f);
	glutSolidSphere(0.12f, 14, 14);
	glPopMatrix();

	setColor3(0.12f, 0.12f, 0.14f);
	glPushMatrix();
	glTranslatef(0.0f, 1.14f, 0.0f);
	drawBox(0.22f, 0.08f, 0.22f);
	glPopMatrix();

	setColor3(r * 0.8f, g * 0.8f, b * 0.8f);
	glPushMatrix();
	glTranslatef(-0.18f, 0.65f + (gesture ? arm : 0.0f), 0.0f);
	drawBox(0.07f, 0.45f, 0.07f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.18f, 0.65f + (gesture ? -arm * 0.5f : 0.0f), 0.0f);
	drawBox(0.07f, 0.45f, 0.07f);
	glPopMatrix();

	glPopMatrix();
}

static void drawHuman(float x, float y, float z, float r, float g, float b, float scale, int cheering) {
	float bounce = fabsf(sinf(gTime * 5.8f + x * 0.5f + z * 0.12f)) * 0.14f;
	float arm = 0.25f + 0.14f * sinf(gTime * 4.0f + x);
	glPushMatrix();
	glTranslatef(x, y + bounce, z);
	glScalef(scale, scale, scale);

	setColor3(r, g, b);
	glPushMatrix();
	glTranslatef(0.0f, 0.5f, 0.0f);
	drawBox(0.28f, 0.65f, 0.18f);
	glPopMatrix();

	setColor3(0.26f, 0.26f, 0.3f);
	glPushMatrix();
	glTranslatef(-0.08f, 0.13f, 0.0f);
	drawBox(0.09f, 0.34f, 0.09f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.08f, 0.13f, 0.0f);
	drawBox(0.09f, 0.34f, 0.09f);
	glPopMatrix();

	setColor3(0.9f, 0.74f, 0.61f);
	glPushMatrix();
	glTranslatef(0.0f, 0.95f, 0.0f);
	glutSolidSphere(0.11f, 12, 12);
	glPopMatrix();

	setColor3(0.12f, 0.12f, 0.14f);
	glPushMatrix();
	glTranslatef(0.0f, 1.05f, 0.0f);
	drawBox(0.2f, 0.08f, 0.2f);
	glPopMatrix();

	setColor3(r * 0.8f, g * 0.8f, b * 0.8f);
	glPushMatrix();
	glTranslatef(-0.16f, 0.58f + (cheering ? arm : 0.0f), 0.0f);
	drawBox(0.06f, 0.36f, 0.06f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.16f, 0.58f + (cheering ? -arm * 0.55f : 0.0f), 0.0f);
	drawBox(0.06f, 0.36f, 0.06f);
	glPopMatrix();

	glPopMatrix();
}

static void drawDeskStation(float x, float y, float z, float rotY, const char *dept) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);

	setColor3(0.18f, 0.19f, 0.22f);
	glPushMatrix();
	glTranslatef(0.0f, 0.35f, 0.0f);
	drawBox(1.3f, 0.08f, 0.6f);
	glPopMatrix();

	setColor3(0.14f, 0.14f, 0.18f);
	glPushMatrix();
	glTranslatef(0.0f, 0.15f, 0.0f);
	drawBox(1.25f, 0.25f, 0.54f);
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
	drawBitmapText(dept, GLUT_BITMAP_8_BY_13);
	glEnable(GL_LIGHTING);

	setColor3(0.08f, 0.08f, 0.1f);
	glPushMatrix();
	glTranslatef(0.0f, -0.06f, -0.18f);
	drawBox(0.6f, 0.28f, 0.5f);
	glPopMatrix();

	glPopMatrix();
}

static void drawTieredControlGallery(void) {
	int row, seat;
	float baseZ = -4.0f;
	float deskSpacing = 2.2f;
	const char *deptNames[3] = {"PROPULSION", "GUIDANCE", "TELEMETRY"};

	for (row = 0; row < 4; ++row) {
		float y = -0.6f + row * 0.75f;
		float z = baseZ - row * 3.5f;

		setColor3(0.16f, 0.17f, 0.2f);
		glPushMatrix();
		glTranslatef(0.0f, y - 0.2f, z - 0.5f);
		drawBox(22.0f, 0.35f, 3.0f);
		glPopMatrix();

		glDisable(GL_LIGHTING);
        glColor3f(0.0f, 0.8f, 1.0f);
        glRasterPos3f(-10.5f, y + 1.0f, z); 
        drawBitmapText(deptNames[row % 3], GLUT_BITMAP_HELVETICA_18);
        glEnable(GL_LIGHTING);

		for (seat = -4; seat <= 4; ++seat) {
			float x = seat * deskSpacing;
			float zz = z;
			float rot = 0.0f;
			drawDeskStation(x, y, zz, rot, "");
		}
	}

	// Row 0 (Z = -4.0)
    drawScientist(-0.6f, 0.2f, -4.5f, 0.20f, 0.33f, 0.67f, 1);
    drawScientist(7.2f, 0.2f, -4.5f, 0.1f, 0.5f, 0.2f, 1);

    // Row 1 (Z = -7.5)
    drawScientist(-5.0f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);
    drawScientist(4.0f, 0.95f, -8.0f, 0.65f, 0.40f, 0.20f, 0);
    drawScientist(-6.5f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);
	drawScientist(8.8f, 0.95f, -8.0f, 0.25f, 0.45f, 0.65f, 1);
	drawScientist(-9.2f, 0.95f, -8.0f, 0.65f, 0.3f, 0.25f, 0);

    // Row 2 (Z = -11.0)
    drawScientist(2.5f, 1.70f, -11.5f, 0.8f, 0.2f, 0.1f, 0);
    drawScientist(-8.0f, 1.7f, -11.5f, 0.9f, 0.9f, 0.9f, 0);
    drawScientist(-3.5f, 1.7f, -11.5f, 0.2f, 0.6f, 0.3f, 1);
    drawScientist(6.0f, 1.7f, -11.5f, 0.3f, 0.3f, 0.8f, 0); 
	drawScientist(9.8f, 1.7f, -11.5f, 0.58f, 0.35f, 0.2f, 1);
	drawScientist(-10.3f, 1.7f, -11.5f, 0.22f, 0.48f, 0.68f, 0);

    // Row 3 (Z = -14.5)
    drawScientist(9.0f, 2.45f, -15.0f, 0.15f, 0.15f, 0.15f, 0);
    drawScientist(-4.8f, 2.45f, -15.0f, 0.88f, 0.88f, 0.9f, 0);
    drawScientist(8.5f, 2.45f, -15.0f, 0.7f, 0.4f, 0.2f, 0);
	drawScientist(2.8f, 2.45f, -15.0f, 0.2f, 0.6f, 0.28f, 1);
	drawScientist(-9.5f, 2.45f, -15.0f, 0.48f, 0.28f, 0.68f, 0);

	/* Extra standing celebration crowd near aisles */
	drawHuman(-11.5f, 0.2f, -6.5f, 0.62f, 0.34f, 0.22f, 1.1f, 1);
	drawHuman(-11.5f, 0.2f, -9.0f, 0.24f, 0.52f, 0.78f, 1.0f, 1);
	drawHuman(-14.4f, 1.0f, -12.2f, 0.35f, 0.64f, 0.32f, 0.95f, 0);
	drawHuman(-12.6f, 0.2f, -6.2f, 0.82f, 0.45f, 0.25f, 1.05f, 1);
	drawHuman(-12.8f, 0.55f, -10.8f, 0.22f, 0.56f, 0.84f, 0.98f, 0);
	drawHuman(-15.2f, 1.25f, -14.3f, 0.78f, 0.78f, 0.84f, 0.9f, 1);
	drawHuman(-12.1f, 1.0f, -12.4f, 0.3f, 0.6f, 0.28f, 0.92f, 1);
	drawHuman(13.8f, 0.2f, -6.8f, 0.22f, 0.36f, 0.72f, 1.1f, 1);
	drawHuman(12.2f, 0.2f, -9.4f, 0.7f, 0.42f, 0.22f, 1.0f, 0);
	drawHuman(14.8f, 1.0f, -12.0f, 0.84f, 0.84f, 0.88f, 0.95f, 1);
	drawHuman(11.7f, 0.3f, -6.3f, 0.28f, 0.48f, 0.82f, 1.05f, 1);
	drawHuman(13.5f, 0.55f, -10.7f, 0.74f, 0.44f, 0.24f, 0.98f, 1);
	drawHuman(15.4f, 1.25f, -14.1f, 0.82f, 0.82f, 0.86f, 0.9f, 0);
	drawHuman(12.0f, 1.0f, -12.5f, 0.24f, 0.62f, 0.32f, 0.92f, 1);
}

static void drawServerRack(float x, float y, float z, float phase) {
	int i;
	glPushMatrix();
	glTranslatef(x, y, z);

	setColor3(0.14f, 0.18f, 0.22f);
	drawBox(1.35f, 5.8f, 1.15f);

	glDisable(GL_LIGHTING);
	for (i = 0; i < 14; ++i) {
		float led = 0.35f + 0.65f * (0.5f + 0.5f * sinf(gTime * 4.5f + i + phase));
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

static void drawServerGallery(void) {
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
			drawServerRack(x, 1.8f, z, (float)(c + r * 2));
		}
	}
}

static void drawArchitecture(void) {
	setColor3(0.09f, 0.1f, 0.12f);
	glPushMatrix();
	glTranslatef(0.0f, -1.5f, -14.0f);
	drawBox(44.0f, 0.3f, 48.0f);
	glPopMatrix();

	setColor3(0.08f, 0.09f, 0.11f);
	glPushMatrix();
	glTranslatef(-22.0f, 3.25f, -14.0f);
	drawBox(0.4f, 10.0f, 48.0f);
	glPopMatrix();

	setColor3(0.08f, 0.09f, 0.11f);
	glPushMatrix();
	glTranslatef(-22.0f, 5.2f, -14.0f);
	drawBox(0.4f, 14.0f, 48.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(22.0f, 5.2f, -14.0f);
	drawBox(0.4f, 14.0f, 48.0f);
	glPopMatrix();
}

static void setupLights(void) {
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

static void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 9.5, 14.8,
			  0.0, 0.0, -24.0,
			  0.0, 1.0, 0.0);

	setupLights();
	drawArchitecture();
	drawVideoWall();
	drawTieredControlGallery();
	drawServerGallery();

	glutSwapBuffers();
}

static void reshape(int w, int h) {
	if (h == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(35.0, (double)w / (double)h, 0.1, 220.0);
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
	glutCreateWindow("ISRO Mission Control - Success");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.03f, 0.04f, 0.06f, 1.0f);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(16, timer, 0);

	glutMainLoop();
	return 0;
}
