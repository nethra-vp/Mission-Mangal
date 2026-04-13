/*
 * Scene 14: Design Satellite
 * Embedded from scenes_C_files/design_satellite.c
 */

#ifndef SCENE_14_DESIGN_SATELLITE_H
#define SCENE_14_DESIGN_SATELLITE_H

#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

/* We need scene-local time (start at 0 when this scene begins), but the original
   code uses glutGet(GLUT_ELAPSED_TIME). We proxy glutGet for the embedded file
   so elapsed time becomes (global elapsed - base). */
static int s14_elapsedBaseMs = 0;
static int s14_glutGet_proxy(GLenum what) {
	if (what == GLUT_ELAPSED_TIME) {
		return glutGet(what) - s14_elapsedBaseMs;
	}
	return glutGet(what);
}

/* Prevent the embedded scene from scheduling its own GLUT timers.
   The orchestrator drives updates via scene14_update(). */
#define s14_glutTimerFunc(ms, func, val) ((void)0)
#define glutTimerFunc s14_glutTimerFunc

/* Rename embedded entrypoints and common helpers to avoid collisions. */
#define glutGet  s14_glutGet_proxy
#define drawScene     s14_sat_drawScene_impl
#define reshapeScene  s14_sat_reshapeScene_impl
#define timerTick     s14_sat_timerTick_impl
#define initScene     s14_sat_initScene_impl
#define clamp01       s14_sat_clamp01_impl
#define smooth01      s14_sat_smooth01_impl
#define stageProgress s14_sat_stageProgress_impl
#define lerp          s14_sat_lerp_impl
#define main          s14_sat_main_disabled

#include "scenes_C_files/design_satellite.c"

#undef main
#undef lerp
#undef stageProgress
#undef smooth01
#undef clamp01
#undef initScene
#undef timerTick
#undef reshapeScene
#undef drawScene
#undef glutGet
#undef glutTimerFunc
#undef s14_glutTimerFunc

/* Clean up macro names from embedded file. */
#ifdef STAR_COUNT
#undef STAR_COUNT
#endif
#ifdef PI
#undef PI
#endif

static int s14_sat_initialized = 0;

static void s14_sat_ensureInit(void) {
	if (s14_sat_initialized) {
		return;
	}
	/* Reset scene-local elapsed time origin */
	s14_elapsedBaseMs = glutGet(GLUT_ELAPSED_TIME);
	/* Run original init (build textures, starfield, lights, etc.) */
	s14_sat_initScene_impl();
	s14_sat_initialized = 1;
}

static void scene14_display(void) {
	s14_sat_ensureInit();

	/* Keep projection aspect correct for current window. */
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	s14_sat_reshapeScene_impl(w, h);

	glDisable(GL_FOG);
	s14_sat_drawScene_impl();
}

static void scene14_update(float deltaTime) {
	( void)deltaTime;
	s14_sat_ensureInit();

	/* Original timer only posts redisplay + schedules itself.
	   Scheduling is disabled, so calling once is enough. */
	s14_sat_timerTick_impl(0);
}

#endif /* SCENE_14_DESIGN_SATELLITE_H */
