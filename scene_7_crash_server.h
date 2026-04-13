/*
 * Scene 7: Server Room Tension - Crash Announcement
 * Duration: 12 seconds
 * Extracted from crash_server_room.c - COMPLETE VERSION
 */

#ifndef SCENE_7_CRASH_SERVER_H
#define SCENE_7_CRASH_SERVER_H

#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

static float s7_gTime = 0.0f;

static void s7_setColor3(float r, float g, float b) {
	GLfloat mat[4] = {r, g, b, 1.0f};
	glColor3f(r, g, b);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat);
}

static void s7_drawBox(float sx, float sy, float sz) {
	glPushMatrix();
	glScalef(sx, sy, sz);
	glutSolidCube(1.0f);
	glPopMatrix();
}

static void s7_drawBitmapText(const char *txt, void *font) {
	int i;
	for (i = 0; txt[i] != '\0'; ++i) {
		glutBitmapCharacter(font, txt[i]);
	}
}

static void s7_drawStrokeText(const char *txt, void *font, float scale) {
	int i;
	glPushMatrix();
	glScalef(scale, scale, scale);
	for (i = 0; txt[i] != '\0'; ++i) {
		glutStrokeCharacter(font, txt[i]);
	}
	glPopMatrix();
}

static float s7_getStrokeTextWidth(const char *txt, void *font) {
	int i;
	float width = 0.0f;
	for (i = 0; txt[i] != '\0'; ++i) {
		width += (float)glutStrokeWidth(font, txt[i]);
	}
	return width;
}

static void s7_drawVideoWallMainContent(void) {
	int i;
	float pulse = 0.5f + 0.5f * sinf(s7_gTime * 8.0f);
	float crackle = 0.5f + 0.5f * sinf(s7_gTime * 21.0f);
	const char *crash = "CRASH.";
	float crashWidthUnits;
	float crashScale;
	float crashX;

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Deep matte black base panel. */
	glColor3f(0.015f, 0.015f, 0.018f);
	glBegin(GL_QUADS);
	glVertex3f(-5.5f, -2.5f, 0.01f);
	glVertex3f(5.5f, -2.5f, 0.01f);
	glVertex3f(5.5f, 2.5f, 0.01f);
	glVertex3f(-5.5f, 2.5f, 0.01f);
	glEnd();

	/* Peripheral telemetry frame lines. */
	glColor4f(0.08f, 0.7f + 0.2f * crackle, 0.22f, 0.75f);
	for (i = 0; i < 18; ++i) {
		float x = -5.2f + i * 0.62f;
		glBegin(GL_LINES);
		glVertex3f(x, 1.95f, 0.015f);
		glVertex3f(x + 0.22f, 2.3f, 0.015f);
		glVertex3f(x, -1.95f, 0.015f);
		glVertex3f(x + 0.22f, -2.3f, 0.015f);
		glEnd();
	}
	for (i = 0; i < 14; ++i) {
		float y = -2.25f + i * 0.34f;
		glBegin(GL_LINES);
		glVertex3f(-5.35f, y, 0.015f);
		glVertex3f(-4.3f + 0.22f * sinf(s7_gTime * 5.0f + i), y, 0.015f);
		glVertex3f(4.3f - 0.22f * cosf(s7_gTime * 4.0f + i), y, 0.015f);
		glVertex3f(5.35f, y, 0.015f);
		glEnd();
	}

	/* Flickering telemetry blocks around the border. */
	for (i = 0; i < 9; ++i) {
		float level = 0.2f + (0.5f + 0.5f * sinf(s7_gTime * 4.5f + i * 0.8f)) * 1.1f;
		float y = -1.65f + i * 0.34f;
		glColor3f(0.12f, 0.95f, 0.35f);
		glBegin(GL_QUADS);
		glVertex3f(5.9f, y, 0.02f);
		glVertex3f(5.9f + level, y, 0.02f);
		glVertex3f(5.9f + level, y + 0.17f, 0.02f);
		glVertex3f(5.9f, y + 0.17f, 0.02f);
		glVertex3f(-5.9f, y, 0.02f);
		glVertex3f(-5.9f - level, y, 0.02f);
		glVertex3f(-5.9f - level, y + 0.17f, 0.02f);
		glVertex3f(-5.9f, y + 0.17f, 0.02f);
		glEnd();
	}

	/* Massive plain-English CRASH title sized to the screen width. */
	crashWidthUnits = s7_getStrokeTextWidth(crash, GLUT_STROKE_ROMAN);
	crashScale = 4.2f / crashWidthUnits;
	crashX = -2.1f;
	glColor3f(1.0f, 0.12f + pulse * 0.1f, 0.12f + pulse * 0.08f);
	glPushMatrix();
	glTranslatef(crashX, 0.55f, 0.5f);
	s7_drawStrokeText(crash, GLUT_STROKE_ROMAN, crashScale);
	glPopMatrix();

	glColor3f(0.8f, 0.8f, 0.82f);
	glRasterPos3f(3.25f, 1.62f, 0.5f);
	s7_drawBitmapText("INDIA", GLUT_BITMAP_HELVETICA_18);
	glDisable(GL_BLEND);

	/* Top-left crash status marker. */
	glColor3f(0.95f, 0.12f, 0.12f);
	glRasterPos3f(-5.05f, 2.08f, 0.5f);
	s7_drawBitmapText("STATUS: CRASHED", GLUT_BITMAP_8_BY_13);

	/* Large neon secondary message. */
	glColor3f(1.0f, 0.09f + pulse * 0.12f, 0.09f + pulse * 0.1f);
	glPushMatrix();
	glTranslatef(-1.95f, -1.15f, 0.05f);
	s7_drawStrokeText("Mission Abort", GLUT_STROKE_ROMAN, 0.0048f);
	glPopMatrix();

	glColor4f(1.0f, 0.06f, 0.06f, 0.18f + 0.12f * pulse);
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);
	glVertex3f(-2.5f, -1.45f, 0.025f);
	glVertex3f(2.5f, -1.45f, 0.025f);
	glVertex3f(2.5f, -0.35f, 0.025f);
	glVertex3f(-2.5f, -0.35f, 0.025f);
	glEnd();
	glDisable(GL_BLEND);

	glEnable(GL_LIGHTING);
}

static void s7_drawVideoWall(void) {
	s7_setColor3(0.12f, 0.12f, 0.16f);
	glPushMatrix();
	glTranslatef(0.0f, 5.5f, -31.0f);
	s7_drawBox(42.0f, 14.0f, 0.8f);

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.42f);
	glScalef(2.0f, 2.0f, 1.0f);
	s7_drawVideoWallMainContent();
	glPopMatrix();

	glPopMatrix();
}

static void s7_drawScientist(float x, float y, float z, float r, float g, float b, int gesture) {
	float arm = 0.35f + 0.15f * sinf(s7_gTime * 2.0f + x);
	glPushMatrix();
	glTranslatef(x, y, z);

	s7_setColor3(r, g, b);
	glPushMatrix();
	glTranslatef(0.0f, 0.55f, 0.0f);
	s7_drawBox(0.32f, 0.72f, 0.2f);
	glPopMatrix();

	s7_setColor3(0.92f, 0.76f, 0.62f);
	glPushMatrix();
	glTranslatef(0.0f, 1.02f, 0.0f);
	glutSolidSphere(0.12f, 14, 14);
	glPopMatrix();

	s7_setColor3(0.12f, 0.12f, 0.14f);
	glPushMatrix();
	glTranslatef(0.0f, 1.14f, 0.0f);
	s7_drawBox(0.22f, 0.08f, 0.22f);
	glPopMatrix();

	s7_setColor3(r * 0.8f, g * 0.8f, b * 0.8f);
	glPushMatrix();
	glTranslatef(-0.18f, 0.65f + (gesture ? arm : 0.0f), 0.0f);
	s7_drawBox(0.07f, 0.45f, 0.07f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.18f, 0.65f + (gesture ? -arm * 0.5f : 0.0f), 0.0f);
	s7_drawBox(0.07f, 0.45f, 0.07f);
	glPopMatrix();

	glPopMatrix();
}

static void s7_drawDeskStation(float x, float y, float z, float rotY, const char *dept) {
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);

	s7_setColor3(0.18f, 0.19f, 0.22f);
	glPushMatrix();
	glTranslatef(0.0f, 0.35f, 0.0f);
	s7_drawBox(1.3f, 0.08f, 0.6f);
	glPopMatrix();

	s7_setColor3(0.14f, 0.14f, 0.18f);
	glPushMatrix();
	glTranslatef(0.0f, 0.15f, 0.0f);
	s7_drawBox(1.25f, 0.25f, 0.54f);
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
	s7_drawBitmapText(dept, GLUT_BITMAP_8_BY_13);
	glEnable(GL_LIGHTING);

	s7_setColor3(0.08f, 0.08f, 0.1f);
	glPushMatrix();
	glTranslatef(0.0f, -0.06f, -0.18f);
	s7_drawBox(0.6f, 0.28f, 0.5f);
	glPopMatrix();

	glPopMatrix();
}

static void s7_drawTieredControlGallery(void) {
	int row, seat;
	float baseZ = -4.0f;
	float deskSpacing = 2.2f;
	const char *deptNames[3] = {"PROPULSION", "GUIDANCE", "TELEMETRY"};

	for (row = 0; row < 4; ++row) {
		float y = -0.6f + row * 0.75f;
		float z = baseZ - row * 3.5f;

		s7_setColor3(0.16f, 0.17f, 0.2f);
		glPushMatrix();
		glTranslatef(0.0f, y - 0.2f, z - 0.5f);
		s7_drawBox(22.0f, 0.35f, 3.0f);
		glPopMatrix();

		glDisable(GL_LIGHTING);
        glColor3f(0.0f, 0.8f, 1.0f);
        glRasterPos3f(-10.5f, y + 1.0f, z); 
        s7_drawBitmapText(deptNames[row % 3], GLUT_BITMAP_HELVETICA_18);
        glEnable(GL_LIGHTING);

		for (seat = -4; seat <= 4; ++seat) {
			float x = seat * deskSpacing;
			float zz = z;
			float rot = 0.0f;
			s7_drawDeskStation(x, y, zz, rot, "");
		}
	}

    s7_drawScientist(-0.6f, 0.2f, -4.5f, 0.20f, 0.33f, 0.67f, 1);
    s7_drawScientist(7.2f, 0.2f, -4.5f, 0.1f, 0.5f, 0.2f, 1);
    s7_drawScientist(-5.0f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);
    s7_drawScientist(4.0f, 0.95f, -8.0f, 0.65f, 0.40f, 0.20f, 0);
    s7_drawScientist(-6.5f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);
    s7_drawScientist(2.5f, 1.70f, -11.5f, 0.8f, 0.2f, 0.1f, 0);
    s7_drawScientist(-8.0f, 1.7f, -11.5f, 0.9f, 0.9f, 0.9f, 0);
    s7_drawScientist(-3.5f, 1.7f, -11.5f, 0.2f, 0.6f, 0.3f, 1);
    s7_drawScientist(6.0f, 1.7f, -11.5f, 0.3f, 0.3f, 0.8f, 0);
    s7_drawScientist(9.0f, 2.45f, -15.0f, 0.15f, 0.15f, 0.15f, 0);
    s7_drawScientist(-4.8f, 2.45f, -15.0f, 0.88f, 0.88f, 0.9f, 0);
    s7_drawScientist(8.5f, 2.45f, -15.0f, 0.7f, 0.4f, 0.2f, 0);
}

static void s7_drawServerRack(float x, float y, float z, float phase) {
	int i;
	glPushMatrix();
	glTranslatef(x, y, z);

	s7_setColor3(0.14f, 0.18f, 0.22f);
	s7_drawBox(1.35f, 5.8f, 1.15f);

	glDisable(GL_LIGHTING);
	for (i = 0; i < 14; ++i) {
		float led = 0.35f + 0.65f * (0.5f + 0.5f * sinf(s7_gTime * 4.5f + i + phase));
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

static void s7_drawServerGallery(void) {
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
			s7_drawServerRack(x, 1.8f, z, (float)(c + r * 2));
		}
	}
}

static void s7_drawArchitecture(void) {
	s7_setColor3(0.09f, 0.1f, 0.12f);
	glPushMatrix();
	glTranslatef(0.0f, -1.5f, -14.0f);
	s7_drawBox(44.0f, 0.3f, 48.0f);
	glPopMatrix();

	s7_setColor3(0.08f, 0.09f, 0.11f);
	glPushMatrix();
	glTranslatef(-22.0f, 3.25f, -14.0f);
	s7_drawBox(0.4f, 10.0f, 48.0f);
	glPopMatrix();

	s7_setColor3(0.08f, 0.09f, 0.11f);
	glPushMatrix();
	glTranslatef(-22.0f, 5.2f, -14.0f);
	s7_drawBox(0.4f, 14.0f, 48.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(22.0f, 5.2f, -14.0f);
	s7_drawBox(0.4f, 14.0f, 48.0f);
	glPopMatrix();
}

static void s7_setupLights(void) {
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

static void scene7_display(void) {
	static int once = 0;
	if (!once) {
		printf("[DEBUG] Scene 7 display function called!\n");
		fflush(stdout);
		once = 1;
	}
	glClearColor(0.03f, 0.04f, 0.06f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 9.5, 14.8,
			  0.0, 0.0, -24.0,
			  0.0, 1.0, 0.0);

	s7_setupLights();
	s7_drawArchitecture();
	s7_drawVideoWall();
	s7_drawTieredControlGallery();
	s7_drawServerGallery();

	glutSwapBuffers();
}

static void scene7_update(float dt) {
	s7_gTime += dt;
}

#endif
