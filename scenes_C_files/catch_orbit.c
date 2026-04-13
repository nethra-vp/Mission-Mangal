#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

#define STAR_COUNT 900
#define TRAIL_MAX 420

typedef struct Vec3 {
	float x;
	float y;
	float z;
} Vec3;

static float gTime = 0.0f;
static Vec3 gStars[STAR_COUNT];
static float gStarSeed[STAR_COUNT];
static Vec3 gTrail[TRAIL_MAX];
static int gTrailCount = 0;
static int gWidth = 1366;
static int gHeight = 768;

static float clampf(float v, float lo, float hi) {
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

static float lerpf(float a, float b, float t) {
	return a + (b - a) * clampf(t, 0.0f, 1.0f);
}

static Vec3 vec3(float x, float y, float z) {
	Vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

static void drawCylinder(float radius, float height, int slices) {
	int i;
	float step = 6.2831853f / (float)slices;

	glBegin(GL_QUAD_STRIP);
	for (i = 0; i <= slices; ++i) {
		float a = step * (float)i;
		float x = cosf(a) * radius;
		float z = sinf(a) * radius;
		glNormal3f(cosf(a), 0.0f, sinf(a));
		glVertex3f(x, 0.0f, z);
		glVertex3f(x, height, z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (i = 0; i <= slices; ++i) {
		float a = step * (float)i;
		glVertex3f(cosf(a) * radius, 0.0f, sinf(a) * radius);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, height, 0.0f);
	for (i = 0; i <= slices; ++i) {
		float a = -step * (float)i;
		glVertex3f(cosf(a) * radius, height, sinf(a) * radius);
	}
	glEnd();
}

static Vec3 orbitPoint(float angle) {
	/* Elliptical insertion orbit around Mars. */
	float a = 10.3f;
	float b = 6.7f;
	return vec3(a * cosf(angle), b * sinf(angle), 0.25f * sinf(angle * 1.7f));
}

static void pushTrail(Vec3 p) {
	int i;
	if (gTrailCount < TRAIL_MAX) {
		gTrail[gTrailCount++] = p;
		return;
	}
	for (i = 1; i < TRAIL_MAX; ++i) {
		gTrail[i - 1] = gTrail[i];
	}
	gTrail[TRAIL_MAX - 1] = p;
}

static void drawText3D(float x, float y, float z, const char *text, void *font, float r, float g, float b) {
	int i;
	glDisable(GL_LIGHTING);
	glColor3f(r, g, b);
	glRasterPos3f(x, y, z);
	for (i = 0; text[i] != '\0'; ++i) {
		glutBitmapCharacter(font, text[i]);
	}
	glEnable(GL_LIGHTING);
}

static void initStarfield(void) {
	int i;
	for (i = 0; i < STAR_COUNT; ++i) {
		float u = (float)rand() / (float)RAND_MAX;
		float v = (float)rand() / (float)RAND_MAX;
		float d = 45.0f + 130.0f * ((float)rand() / (float)RAND_MAX);
		float theta = 6.2831853f * u;
		float phi = acosf(2.0f * v - 1.0f);
		gStars[i].x = d * sinf(phi) * cosf(theta);
		gStars[i].y = d * cosf(phi);
		gStars[i].z = d * sinf(phi) * sinf(theta) - 25.0f;
		gStarSeed[i] = (float)rand() / (float)RAND_MAX;
	}
}

static void drawMilkyWayBand(void) {
	int i;
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (i = 0; i < 30; ++i) {
		float y = 16.0f - i * 1.1f;
		float alpha = 0.05f + 0.06f * sinf(gTime * 0.15f + i * 0.2f);
		glColor4f(0.45f, 0.58f, 0.95f, alpha);
		glBegin(GL_QUADS);
		glVertex3f(-75.0f, y, -90.0f);
		glVertex3f(75.0f, y + 0.9f, -90.0f);
		glVertex3f(75.0f, y + 0.9f, -140.0f);
		glVertex3f(-75.0f, y, -140.0f);
		glEnd();
	}
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

static void drawStarfield(void) {
	int i;
	glDisable(GL_LIGHTING);
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	for (i = 0; i < STAR_COUNT; ++i) {
		float tw = 0.55f + 0.45f * sinf(gTime * 1.4f + gStarSeed[i] * 10.0f);
		glColor3f(0.7f * tw + 0.2f, 0.78f * tw + 0.2f, 0.95f * tw + 0.2f);
		glVertex3f(gStars[i].x, gStars[i].y, gStars[i].z);
	}
	glEnd();
	glEnable(GL_LIGHTING);

	drawMilkyWayBand();
}

static void drawMarsSurfaceDetail(float radius) {
	int i;
	/* Valles Marineris inspired canyon strip. */
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.32f, 0.16f, 0.1f, 0.35f);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= 70; ++i) {
		float a = -1.2f + i * (2.4f / 70.0f);
		float x = radius * cosf(a) * 0.72f;
		float y = radius * sinf(a) * 0.2f + radius * 0.08f;
		glVertex3f(x, y, radius * 0.72f);
	}
	glEnd();

	/* Crater hints as soft spots. */
	for (i = 0; i < 22; ++i) {
		float a = i * 0.47f;
		float bx = radius * 0.6f * sinf(a * 1.2f);
		float by = radius * 0.52f * cosf(a * 0.9f);
		float r = 0.22f + 0.14f * sinf(i * 1.3f);
		glColor4f(0.2f, 0.1f, 0.07f, 0.18f);
		glPushMatrix();
		glTranslatef(bx, by, radius * 0.7f);
		glScalef(1.0f, 0.8f, 1.0f);
		glutSolidSphere(r, 12, 12);
		glPopMatrix();
	}
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

static void drawMars(void) {
	GLfloat matSpec[] = {0.2f, 0.18f, 0.14f, 1.0f};
	GLfloat matAmb[] = {0.35f, 0.18f, 0.1f, 1.0f};
	float radius = 5.65f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0f);

	/* Base body */
	glPushMatrix();
	glRotatef(-18.0f, 0.0f, 0.0f, 1.0f);
	glColor3f(0.74f, 0.32f, 0.18f);
	glutSolidSphere(radius, 64, 64);

	/* Fine surface patches */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.55f, 0.22f, 0.12f, 0.32f);
	{
		int i;
		for (i = 0; i < 36; ++i) {
			float a = i * 0.37f;
			float b = i * 0.23f;
			float px = radius * 0.8f * sinf(a);
			float py = radius * 0.78f * cosf(b);
			float pz = radius * 0.82f * cosf(a * 0.7f);
			glPushMatrix();
			glTranslatef(px * 0.18f, py * 0.18f, pz * 0.18f + radius * 0.85f);
			glScalef(1.4f, 0.7f, 1.0f);
			glutSolidSphere(0.26f + 0.08f * sinf(i), 10, 10);
			glPopMatrix();
		}
	}
	glDisable(GL_BLEND);

	drawMarsSurfaceDetail(radius);

	/* Atmospheric haze */
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.98f, 0.52f, 0.22f, 0.14f);
	glutSolidSphere(radius * 1.04f, 40, 40);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

static void drawTrajectoryGuide(void) {
	int i;
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	/* Predicted insertion trajectory line (light blue glow). */
	glLineWidth(2.0f);
	glColor4f(0.45f, 0.78f, 1.0f, 0.45f);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= 230; ++i) {
		float a = 0.15f + i * (4.95f / 230.0f);
		Vec3 p = orbitPoint(a);
		glVertex3f(p.x, p.y, p.z);
	}
	glEnd();

	/* Additional faint ellipse ring to imply stable capture orbit. */
	glColor4f(0.25f, 0.62f, 1.0f, 0.18f);
	glBegin(GL_LINE_LOOP);
	for (i = 0; i < 260; ++i) {
		float a = i * (6.2831853f / 260.0f);
		Vec3 p = orbitPoint(a);
		glVertex3f(p.x, p.y, p.z);
	}
	glEnd();

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

static void drawSatelliteModel(Vec3 p, float yaw, int thrusterOn, float plumePulse) {
	GLfloat satSpec[] = {0.95f, 0.88f, 0.38f, 1.0f};
	GLfloat satAmb[] = {0.35f, 0.3f, 0.12f, 1.0f};

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, satSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, satAmb);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 54.0f);

	glPushMatrix();
	glTranslatef(p.x, p.y, p.z);
	glRotatef(yaw, 0.0f, 1.0f, 0.0f);
	glRotatef(13.0f * sinf(gTime * 0.7f), 0.0f, 0.0f, 1.0f);

	/* Gold bus body */
	glColor3f(0.84f, 0.68f, 0.2f);
	glPushMatrix();
	glScalef(0.9f, 0.7f, 0.7f);
	glutSolidCube(1.0f);
	glPopMatrix();

	/* Blue solar panels */
	glColor3f(0.18f, 0.42f, 0.84f);
	glPushMatrix();
	glTranslatef(-0.95f, 0.0f, 0.0f);
	glScalef(1.2f, 0.36f, 0.05f);
	glutSolidCube(1.0f);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.95f, 0.0f, 0.0f);
	glScalef(1.2f, 0.36f, 0.05f);
	glutSolidCube(1.0f);
	glPopMatrix();

	/* Dish and antenna mast */
	glColor3f(0.84f, 0.84f, 0.9f);
	glPushMatrix();
	glTranslatef(0.0f, 0.34f, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	drawCylinder(0.06f, 0.28f, 14);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0f, 0.55f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.17f, 0.12f, 20, 12);
	glPopMatrix();

	if (thrusterOn) {
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glPushMatrix();
		glTranslatef(0.0f, -0.38f, 0.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glColor4f(1.0f, 0.58f + 0.3f * plumePulse, 0.18f, 0.9f);
		glutSolidCone(0.11f, 0.75f + 0.25f * plumePulse, 18, 12);
		glColor4f(1.0f, 0.85f, 0.35f, 0.5f);
		glutSolidCone(0.16f, 0.98f + 0.3f * plumePulse, 16, 10);
		glPopMatrix();
		glDisable(GL_BLEND);
		glEnable(GL_LIGHTING);
	}

	glPopMatrix();
}

static void drawOrbitalTrail(void) {
	int i;
	if (gTrailCount < 3) {
		return;
	}
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glLineWidth(2.2f);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < gTrailCount; ++i) {
		float t = (float)i / (float)gTrailCount;
		float a = 0.08f + 0.75f * t;
		glColor4f(0.25f, 0.68f, 1.0f, a);
		glVertex3f(gTrail[i].x, gTrail[i].y, gTrail[i].z);
	}
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

static void renderScene(void) {
	GLfloat lightPos[] = {40.0f, 16.0f, 10.0f, 1.0f};
	GLfloat lightDiff[] = {1.0f, 0.94f, 0.85f, 1.0f};
	GLfloat lightAmb[] = {0.06f, 0.07f, 0.11f, 1.0f};
	float timeline = fmodf(gTime, 36.0f);
	Vec3 satPos;
	float satYaw;
	int thrusterOn;
	float pulse = 0.5f + 0.5f * sinf(gTime * 16.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 2.7, 26.0,
			  0.0, 0.5, 0.0,
			  0.0, 1.0, 0.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);

	drawStarfield();
	drawTrajectoryGuide();
	drawMars();

	if (timeline < 8.0f) {
		float t = timeline / 15.0f;
		satPos = vec3(17.0f - 11.8f * t, 2.4f - 2.1f * t, 0.7f - 0.5f * t);
		satYaw = -62.0f;
		thrusterOn = 1;
		gTrailCount = 0;
	} else {
		float t = (timeline - 8.0f) / 28.0f;
		float angle = lerpf(0.16f, 6.34f, t);
		satPos = orbitPoint(angle);
		satYaw = 22.0f + angle * 57.2958f;
		/* Thruster visible near right limb burn, then off in shadow side. */
		thrusterOn = (satPos.x > 0.4f && angle < 2.9f) ? 1 : 0;
		pushTrail(satPos);
	}

	drawOrbitalTrail();
	drawSatelliteModel(satPos, satYaw, thrusterOn, pulse);

	drawText3D(-10.2f, 8.2f, -2.0f,
		"MARS ORBIT INSERTION - MANGALYAAN", GLUT_BITMAP_HELVETICA_18,
		0.85f, 0.92f, 1.0f);

	glutSwapBuffers();
}

static void reshape(int w, int h) {
	if (h == 0) h = 1;
	gWidth = w;
	gHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(48.0, (double)w / (double)h, 0.1, 260.0);
	glMatrixMode(GL_MODELVIEW);
}

static void timer(int value) {
	(void)value;
	gTime += 0.016f;
	glutPostRedisplay();
	glutTimerFunc(16, timer, 0);
}

int main(int argc, char **argv) {
	srand(22);
	initStarfield();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(gWidth, gHeight);
	glutCreateWindow("Mangalyaan Mars Orbit Insertion");

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.01f, 0.01f, 0.03f, 1.0f);

	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
	glutTimerFunc(16, timer, 0);

	glutMainLoop();
	return 0;
}
