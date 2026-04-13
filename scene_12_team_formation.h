/*
 * Scene 12: Team Formation
 * Embedded from scenes_C_files/team_formation.c
 */

#ifndef SCENE_12_TEAM_FORMATION_H
#define SCENE_12_TEAM_FORMATION_H

#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

/* Prevent the embedded scene from scheduling its own GLUT timers.
   The orchestrator drives updates via scene12_update(). */
#define s12_glutTimerFunc(ms, func, val) ((void)0)
#define glutTimerFunc s12_glutTimerFunc

/* Rename embedded entrypoints to avoid collisions in the orchestrator translation unit. */
#define init     s12_team_init_impl
#define display  s12_team_display_impl
#define reshape  s12_team_reshape_impl
#define timer    s12_team_timer_impl
#define keyboard s12_team_keyboard_impl
#define drawHUD  s12_team_drawHUD_impl
#define main     s12_team_main_disabled

#include "scenes_C_files/team_formation.c"

#undef main
#undef keyboard
#undef timer
#undef reshape
#undef display
#undef init
#undef drawHUD
#undef glutTimerFunc
#undef s12_glutTimerFunc

/* Clean up common macros from embedded file to reduce cross-header leakage. */
#ifdef WIN_W
#undef WIN_W
#endif
#ifdef WIN_H
#undef WIN_H
#endif
#ifdef PI
#undef PI
#endif
#ifdef TWO_PI
#undef TWO_PI
#endif
#ifdef NUM_SCIENTISTS
#undef NUM_SCIENTISTS
#endif
#ifdef NUM_PARTICLES
#undef NUM_PARTICLES
#endif
#ifdef NUM_SCREENS
#undef NUM_SCREENS
#endif
#ifdef TIMER_MS
#undef TIMER_MS
#endif

static int s12_team_initialized = 0;

static void scene12_display(void) {
	if (!s12_team_initialized) {
		s12_team_init_impl();
		s12_team_initialized = 1;
	}

	/* Ensure viewport tracks current window size. */
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	s12_team_reshape_impl(w, h);

	glDisable(GL_FOG);

	s12_team_display_impl();
}

static void scene12_update(float deltaTime) {
	( void)deltaTime;
	if (!s12_team_initialized) {
		s12_team_init_impl();
		s12_team_initialized = 1;
	}

	/* Run the embedded timer update once per orchestrator tick.
	   Its internal dt is TIMER_MS/1000, matching the original scene pacing. */
	s12_team_timer_impl(0);
}

#endif /* SCENE_12_TEAM_FORMATION_H */
