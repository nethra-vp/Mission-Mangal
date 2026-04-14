/*
 * Scene 16: Catching Orbit
 * Embedded from scenes_C_files/catch_orbit.c
 */

#ifndef SCENE_16_CATCH_ORBIT_H
#define SCENE_16_CATCH_ORBIT_H

#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

/* Prevent the embedded scene from scheduling its own GLUT timers.
   The orchestrator drives updates via scene16_update(). */
#define s16_glutTimerFunc(ms, func, val) ((void)0)
#define glutTimerFunc s16_glutTimerFunc

/* Prevent macro redefinition warnings across embedded scenes. */
#ifdef STAR_COUNT
#undef STAR_COUNT
#endif
#ifdef TRAIL_MAX
#undef TRAIL_MAX
#endif

/* Rename type names to avoid collisions across embedded scenes. */
#define Vec3       s16_Vec3

/* Rename embedded globals/helpers/entrypoints. */
#define gTime       s16_gTime
#define gStars      s16_gStars
#define gStarSeed   s16_gStarSeed
#define gTrail      s16_gTrail
#define gTrailCount s16_gTrailCount
#define gWidth      s16_gWidth
#define gHeight     s16_gHeight

#define clampf      s16_clampf_impl
#define lerpf       s16_lerpf_impl
#define vec3        s16_vec3_impl
#define drawCylinder s16_drawCylinder_impl
#define orbitPoint  s16_orbitPoint_impl
#define pushTrail   s16_pushTrail_impl
#define drawText3D  s16_drawText3D_impl
#define initStarfield s16_initStarfield_impl
#define drawMilkyWayBand s16_drawMilkyWayBand_impl
#define drawStarfield s16_drawStarfield_impl
#define drawMarsSurfaceDetail s16_drawMarsSurfaceDetail_impl
#define drawMars    s16_drawMars_impl
#define drawTrajectoryGuide s16_drawTrajectoryGuide_impl
#define drawSatelliteModel s16_drawSatelliteModel_impl
#define drawOrbitalTrail s16_drawOrbitalTrail_impl

#define renderScene s16_renderScene_impl
#define reshape     s16_reshape_impl
#define timer       s16_timer_impl
#define main        s16_main_disabled

#include "scenes_C_files/catch_orbit.c"

#undef main
#undef timer
#undef reshape
#undef renderScene

#undef drawOrbitalTrail
#undef drawSatelliteModel
#undef drawTrajectoryGuide
#undef drawMars
#undef drawMarsSurfaceDetail
#undef drawStarfield
#undef drawMilkyWayBand
#undef initStarfield
#undef drawText3D
#undef pushTrail
#undef orbitPoint
#undef drawCylinder
#undef vec3
#undef lerpf
#undef clampf

#undef gHeight
#undef gWidth
#undef gTrailCount
#undef gTrail
#undef gStarSeed
#undef gStars
#undef gTime

#undef Vec3

#ifdef TRAIL_MAX
#undef TRAIL_MAX
#endif
#ifdef STAR_COUNT
#undef STAR_COUNT
#endif

#undef glutTimerFunc
#undef s16_glutTimerFunc

static int s16_initialized = 0;

static void s16_ensureInit(void) {
	if (s16_initialized) {
		return;
	}

	/* Match embedded main() setup */
	srand(22);
	s16_initStarfield_impl();

	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.01f, 0.01f, 0.03f, 1.0f);

	s16_initialized = 1;
}

static void scene16_display(void) {
	s16_ensureInit();

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	s16_reshape_impl(w, h);

	glDisable(GL_FOG);
	s16_renderScene_impl();
}

static void scene16_update(float deltaTime) {
	( void)deltaTime;
	s16_ensureInit();

	s16_timer_impl(0);
}

#endif /* SCENE_16_CATCH_ORBIT_H */
