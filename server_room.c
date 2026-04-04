#include "C:/OpenGL/include/GL/glut.h"
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

static void drawEmissivePanel(float w, float h, float r, float g, float b) {
	glDisable(GL_LIGHTING);
	glColor3f(r, g, b);
	glBegin(GL_QUADS);
	glVertex3f(-w * 0.5f, -h * 0.5f, 0.0f);
	glVertex3f(w * 0.5f, -h * 0.5f, 0.0f);
	glVertex3f(w * 0.5f, h * 0.5f, 0.0f);
	glVertex3f(-w * 0.5f, h * 0.5f, 0.0f);
	glEnd();
	glEnable(GL_LIGHTING);
}

static void drawVideoWallMainContent(void) {
	int i;
	float pulse = 0.5f + 0.5f * sinf(gTime * 2.0f);

	glDisable(GL_LIGHTING);

	glColor3f(0.06f, 0.09f, 0.16f);
	glBegin(GL_QUADS);
	glVertex3f(-5.5f, -2.5f, 0.01f);
	glVertex3f(5.5f, -2.5f, 0.01f);
	glVertex3f(5.5f, 2.5f, 0.01f);
	glVertex3f(-5.5f, 2.5f, 0.01f);
	glEnd();

	glColor3f(0.22f, 0.85f, 0.28f);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= 48; ++i) {
		float t = (float)i / 48.0f;
		float x = -4.1f + t * 2.6f;
		float y = -1.0f + t * 2.7f + sinf(gTime * 3.0f + t * 10.0f) * 0.08f;
		glVertex3f(x, y, 0.02f);
	}
	glEnd();

	for (i = 0; i < 10; ++i) {
		float y0 = -1.5f + i * 0.32f;
		float bar = 0.3f + (sinf(gTime * 2.2f + i) * 0.5f + 0.5f) * 0.9f;
		glColor3f(0.12f, 0.95f, 0.32f);
		glBegin(GL_QUADS);
		glVertex3f(-3.95f, y0, 0.02f);
		glVertex3f(-3.95f + bar, y0, 0.02f);
		glVertex3f(-3.95f + bar, y0 + 0.18f, 0.02f);
		glVertex3f(-3.95f, y0 + 0.18f, 0.02f);
		glEnd();
	}

	glColor3f(0.1f, 0.55f, 1.0f);
	glBegin(GL_LINE_LOOP);
	for (i = 0; i < 80; ++i) {
		float a = (float)i * 6.2831853f / 80.0f;
		glVertex3f(cosf(a) * 1.05f, sinf(a) * 1.05f, 0.02f);
	}
	glEnd();

	glColor3f(0.0f, 0.95f, 0.85f);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= 64; ++i) {
		float t = (float)i / 64.0f;
		float a = t * 4.9f + gTime * 0.6f;
		glVertex3f(cosf(a) * 0.9f, sinf(a * 1.3f) * 0.6f, 0.03f);
	}
	glEnd();

	glColor3f(1.0f, 0.0f, 0.5f);
	glRasterPos3f(-1.0f, -2.2f, 0.5f);
	drawBitmapText("INDIA", GLUT_BITMAP_HELVETICA_18);
	glRasterPos3f(2.0f, 1.9f, 0.5f);
	drawBitmapText("MOM", GLUT_BITMAP_HELVETICA_18);

	glColor3f(0.08f, 0.08f, 0.1f);
	glBegin(GL_QUADS);
	glVertex3f(1.55f, -1.5f, 0.02f);
	glVertex3f(4.1f, -1.5f, 0.02f);
	glVertex3f(4.1f, 1.5f, 0.02f);
	glVertex3f(1.55f, 1.5f, 0.02f);
	glEnd();

	glColor3f(0.95f, 0.95f, 0.95f);
	glBegin(GL_QUADS);
	glVertex3f(2.65f, -0.9f, 0.03f);
	glVertex3f(2.95f, -0.9f, 0.03f);
	glVertex3f(2.95f, 0.85f, 0.03f);
	glVertex3f(2.65f, 0.85f, 0.03f);
	glEnd();

	glColor3f(1.0f, 0.52f, 0.18f);
	glBegin(GL_QUADS);
	glVertex3f(2.62f, -0.35f, 0.03f);
	glVertex3f(2.98f, -0.35f, 0.03f);
	glVertex3f(2.98f, -0.12f, 0.03f);
	glVertex3f(2.62f, -0.12f, 0.03f);
	glEnd();

	glColor3f(0.2f, 0.45f, 0.95f);
	glBegin(GL_QUADS);
	glVertex3f(2.62f, 0.18f, 0.03f);
	glVertex3f(2.98f, 0.18f, 0.03f);
	glVertex3f(2.98f, 0.42f, 0.03f);
	glVertex3f(2.62f, 0.42f, 0.03f);
	glEnd();

	glColor3f(0.18f + pulse * 0.5f, 0.55f + pulse * 0.35f, 0.95f);
	glBegin(GL_LINES);
	glVertex3f(1.75f, -1.2f, 0.03f);
	glVertex3f(3.9f, 1.2f, 0.03f);
	glVertex3f(1.75f, 1.2f, 0.03f);
	glVertex3f(3.9f, -1.2f, 0.03f);
	glEnd();

	glEnable(GL_LIGHTING);
}

static void drawMetricScreen(const char *label, float phase, int mode) {
	int i;
	glPushMatrix();
	drawEmissivePanel(1.9f, 2.6f, 0.07f, 0.11f, 0.19f);

	glDisable(GL_LIGHTING);

	glColor3f(0.02f, 0.04f, 0.08f);
    glBegin(GL_QUADS);
        glVertex3f(-0.6f, 1.0f, 0.04f); 
        glVertex3f(0.6f, 1.0f, 0.04f);
        glVertex3f(0.6f, 1.3f, 0.04f);
        glVertex3f(-0.6f, 1.3f, 0.04f);
    glEnd();

	if (mode == 0) { // MODE 0: Standard Bars (Your original)
        for (i = 0; i < 6; ++i) {
            float y = -0.8f + i * 0.32f;
            float w = 0.2f + 0.7f * (0.5f + 0.5f * sinf(gTime * 2.1f + i + phase));
            glColor3f(0.2f, 0.95f, 0.4f); // Bright green bars
            glBegin(GL_QUADS);
                glVertex3f(-0.5f, y, 0.01f); 
                glVertex3f(-0.5f + w, y, 0.01f);
                glVertex3f(-0.5f + w, y + 0.15f, 0.01f); 
                glVertex3f(-0.5f, y + 0.15f, 0.01f);
            glEnd();
        }
    } 
    else if (mode == 1) { // MODE 1: Earth/Sphere representing target
        glColor3f(0.2f, 0.4f, 1.0f); // Blue Earth
        glBegin(GL_POLYGON);
        for(i=0; i<20; ++i) {
            float a = i * 6.283f / 20.0f;
            glVertex3f(cosf(a)*0.4f, sinf(a)*0.4f, 0.01f);
        }
        glEnd();
        // // Add a small "cloud" or detail
        glColor3f(0.0f, 1.0f, 0.0f);
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        glVertex3f(0.1f, 0.1f, 0.02f); 
		glVertex3f(-0.15f, -0.1f, 0.02f);
		glVertex3f(-0.0f, -0.1f, 0.02f);
        glEnd();
    }
    else if (mode == 2) { // MODE 2: Rocket with Fixed Visibility
		float rocketY = -0.6f + fmodf(gTime * 0.5f, 1.2f);
		float flicker = sinf(gTime * 45.0f) * 0.06f; // High-speed flicker
		float flameLen = 0.4f + flicker;

		// 1. DYNAMIC FIRE (Drawn first, slightly further back at 0.01f)
		// Outer Flame (Orange) - Fixed CCW Order
		glColor3f(1.0f, 0.4f, 0.0f);
		glBegin(GL_TRIANGLES);
			glVertex3f(0.1f, rocketY, 0.01f);     
			glVertex3f(-0.1f, rocketY, 0.01f);       
			glVertex3f(0.0f, rocketY - flameLen, 0.01f);
		glEnd();

		// Inner Flame (Yellow) - Fixed CCW Order
		glColor3f(1.0f, 1.0f, 0.2f);
		glBegin(GL_TRIANGLES);
			glVertex3f(0.06f, rocketY, 0.011f);              // Right
			glVertex3f(-0.06f, rocketY, 0.011f);             // Left
			glVertex3f(0.0f, rocketY - (flameLen * 0.6f), 0.011f); // Tip
		glEnd();

		// 2. ROCKET BODY (The "Square" part - Z=0.02f)
		glColor3f(0.8f, 0.8f, 0.85f);
		glBegin(GL_QUADS);
			glVertex3f(-0.12f, rocketY, 0.02f);         // Bottom Left
			glVertex3f(0.12f, rocketY, 0.02f);          // Bottom Right
			glVertex3f(0.12f, rocketY + 0.45f, 0.02f);  // Top Right
			glVertex3f(-0.12f, rocketY + 0.45f, 0.02f); // Top Left
		glEnd();

		// 3. NOSE CONE (Triangle - Z=0.02f)
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);
			glVertex3f(0.12f, rocketY + 0.45f, 0.02f);  // Right Base
			glVertex3f(0.0f, rocketY + 0.75f, 0.02f);   // Tip
			glVertex3f(-0.12f, rocketY + 0.45f, 0.02f); // Left Base
		glEnd();

		// 4. SIDE FINS (Corrected CCW Order)
		glColor3f(0.5f, 0.5f, 0.6f);
		glBegin(GL_TRIANGLES);
			// LEFT FIN
			glVertex3f(-0.12f, rocketY + 0.15f, 0.02f); // Top
			glVertex3f(-0.12f, rocketY, 0.02f);         // Bottom
			glVertex3f(-0.25f, rocketY - 0.1f, 0.02f);  // Outer Tip
			
			// RIGHT FIN
			glVertex3f(0.12f, rocketY, 0.02f);          // Bottom
			glVertex3f(0.12f, rocketY + 0.15f, 0.02f);  // Top
			glVertex3f(0.25f, rocketY - 0.1f, 0.02f);   // Outer Tip
		glEnd();
	}
    else if (mode == 3) { // MODE 3: Telemetry Numbers
        glColor3f(0.0f, 1.0f, 1.0f);
        for(i=0; i<4; ++i) {
            char val[10];
            sprintf(val, "%02.1f", 45.0f + sinf(gTime + i)*10.0f);
            glRasterPos3f(-0.4f, 0.4f - i*0.4f, 0.5f);
            drawBitmapText(val, GLUT_BITMAP_HELVETICA_12);
        }
    }

	glColor3f(0.0f, 1.0f, 1.0f);
	glRasterPos3f(-1.4f, 1.5f, 0.09f);
	drawBitmapText(label, GLUT_BITMAP_HELVETICA_12);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

static void drawVideoWall(void) {
	int i;
	const char *labels[6] = {
		"PROPULSION", "GUIDANCE", "COMMS", "ENV CTRL", "TELEMETRY", "NAV DATA"
	};
	int modes[6] = { 0, 2, 3, 0, 1, 3 };

	setColor3(0.12f, 0.12f, 0.16f);
	glPushMatrix();
	glTranslatef(0.0f, 5.5f, -31.0f);
	drawBox(42.0f, 14.0f, 0.8f);

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.42f);
	glScalef(2.0f, 2.0f, 1.0f);
	drawVideoWallMainContent();
	glPopMatrix();

	for (i = 0; i < 3; ++i) {
		float yOffset = 3.8f - (i * 3.8f);

		glPushMatrix();
		glTranslatef(-15.5f, yOffset, 0.55f);
		drawMetricScreen(labels[i], (float)i, modes[i]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(15.5f, yOffset, 0.55f);
		drawMetricScreen(labels[i + 3], (float)(i + 3), modes[i + 3]);
		glPopMatrix();
	}

	glPopMatrix();
}

static void drawScientist(float x, float y, float z, float r, float g, float b, int gesture) {
	float arm = 0.35f + 0.15f * sinf(gTime * 2.0f + x);
	glPushMatrix();
	glTranslatef(x, y, z);

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

	// For Z = -7.0f
	// drawScientist(-0.6f, 0.2f, -8.0f, 0.20f, 0.33f, 0.67f, 1);
	// drawScientist(-5.0f, 0.95f, -10.5f, 0.23f, 0.23f, 0.54f, 0);
    // drawScientist(4.0f, 1.7f, -14.0f, 0.65f, 0.40f, 0.20f, 0);

	// drawScientist(9.0f, 2.45f, -17.5f, 0.15f, 0.15f, 0.15f, 0);
	// drawScientist(7.2f, 0.95f, -10.5f, 0.1f, 0.5f, 0.2f, 1);
    // drawScientist(2.5f, 1.70f, -14.0f, 0.8f, 0.2f, 0.1f, 0);

	// drawScientist(-6.5f, 0.9f, -13.5f, 0.23f, 0.23f, 0.54f, 0);
	// drawScientist(5.8f, 1.4f, -16.0f, 0.30f, 0.44f, 0.70f, 0);
	// drawScientist(-4.8f, 2.0f, -19.0f, 0.88f, 0.88f, 0.9f, 0);

	// drawScientist(-8.0f, 1.7f, -14.0f, 0.9f, 0.9f, 0.9f, 0);
	// drawScientist(-3.5f, 1.7f, -14.0f, 0.2f, 0.6f, 0.3f, 1);
	// drawScientist(6.0f, 1.7f, -14.0f, 0.3f, 0.3f, 0.8f, 0); 
	// drawScientist(8.5f, 1.7f, -14.0f, 0.7f, 0.4f, 0.2f, 0);  

	// Row 0 (Z = -4.0)
    drawScientist(-0.6f, 0.2f, -4.5f, 0.20f, 0.33f, 0.67f, 1);
    drawScientist(7.2f, 0.2f, -4.5f, 0.1f, 0.5f, 0.2f, 1);

    // Row 1 (Z = -7.5)
    drawScientist(-5.0f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);
    drawScientist(4.0f, 0.95f, -8.0f, 0.65f, 0.40f, 0.20f, 0);
    drawScientist(-6.5f, 0.95f, -8.0f, 0.23f, 0.23f, 0.54f, 0);

    // Row 2 (Z = -11.0)
    drawScientist(2.5f, 1.70f, -11.5f, 0.8f, 0.2f, 0.1f, 0);
    drawScientist(-8.0f, 1.7f, -11.5f, 0.9f, 0.9f, 0.9f, 0);
    drawScientist(-3.5f, 1.7f, -11.5f, 0.2f, 0.6f, 0.3f, 1);
    drawScientist(6.0f, 1.7f, -11.5f, 0.3f, 0.3f, 0.8f, 0); 

    // Row 3 (Z = -14.5)
    drawScientist(9.0f, 2.45f, -15.0f, 0.15f, 0.15f, 0.15f, 0);
    drawScientist(-4.8f, 2.45f, -15.0f, 0.88f, 0.88f, 0.9f, 0);
    drawScientist(8.5f, 2.45f, -15.0f, 0.7f, 0.4f, 0.2f, 0);
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
	glutCreateWindow("ISRO Mission Control - Server Room Launch Monitoring");

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
