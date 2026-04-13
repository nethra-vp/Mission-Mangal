/*
 * Scene 13: Ideating Orbit
 * Embedded from scenes_C_files/ideating_orbit.c
 */

#ifndef SCENE_13_IDEATING_ORBIT_H
#define SCENE_13_IDEATING_ORBIT_H

#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

/* Prevent the embedded scene from scheduling its own GLUT timers.
   The orchestrator drives updates via scene13_update(). */
#define s13_glutTimerFunc(ms, func, val) ((void)0)
#define glutTimerFunc s13_glutTimerFunc

/* Rename embedded entrypoints and common helpers to avoid collisions in the orchestrator translation unit. */
#define display  s13_orbit_display_impl
#define reshape  s13_orbit_reshape_impl
#define keyboard s13_orbit_keyboard_impl
#define timer    s13_orbit_timer_impl
#define drawHUD  s13_orbit_drawHUD_impl
#define drawText s13_orbit_drawText_impl
#define drawText2D s13_orbit_drawText2D_impl
#define drawText2DSmall s13_orbit_drawText2DSmall_impl
#define drawScreen s13_orbit_drawScreen_impl
#define initStars s13_orbit_initStars_impl
#define setupLighting s13_orbit_setupLighting_impl
#define starX s13_orbit_starX
#define starY s13_orbit_starY
#define starZ s13_orbit_starZ
#define lerp     s13_orbit_lerp_impl
#define main     s13_orbit_main_disabled

#include "scenes_C_files/ideating_orbit.c"

#undef main
#undef lerp
#undef starZ
#undef starY
#undef starX
#undef setupLighting
#undef initStars
#undef drawScreen
#undef drawText2DSmall
#undef drawText2D
#undef drawText
#undef drawHUD
#undef timer
#undef keyboard
#undef reshape
#undef display
#undef glutTimerFunc
#undef s13_glutTimerFunc

/* Clean up common macros from embedded file to reduce cross-header leakage. */
#ifdef M_PI
#undef M_PI
#endif
#ifdef fminf
#undef fminf
#endif

static int s13_orbit_initialized = 0;

static void scene13_display(void) {
	if (!s13_orbit_initialized) {
		/* Match the embedded main() setup */
		srand(12345);
		s13_orbit_initStars_impl();
		s13_orbit_setupLighting_impl();
		/* Reset to scene start state */
		s13_orbit_keyboard_impl('r', 0, 0);
		s13_orbit_initialized = 1;
	}

	/* Keep the embedded scene's window sizing in sync with current window. */
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	s13_orbit_reshape_impl(w, h);

	glDisable(GL_FOG);
	s13_orbit_display_impl();
}

static void scene13_update(float deltaTime) {
	( void)deltaTime;
	if (!s13_orbit_initialized) {
		srand(12345);
		s13_orbit_initStars_impl();
		s13_orbit_setupLighting_impl();
		s13_orbit_keyboard_impl('r', 0, 0);
		s13_orbit_initialized = 1;
	}

	/* Run one simulation step (dt is fixed to 0.016f inside the embedded timer). */
	s13_orbit_timer_impl(0);
}

#endif /* SCENE_13_IDEATING_ORBIT_H */
