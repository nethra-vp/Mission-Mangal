/*
 * Scene 11: Poori Scene
 * Embedded from scenes_C_files/poori_scene.c
 */

#ifndef SCENE_11_POORI_SCENE_H
#define SCENE_11_POORI_SCENE_H

#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

/* Prevent the embedded scene from scheduling its own GLUT timers.
   The orchestrator drives updates via scene11_update(). */
#define s11_glutTimerFunc(ms, func, val) ((void)0)
#define glutTimerFunc s11_glutTimerFunc

/* Rename embedded symbols that would otherwise be too generic in this translation unit. */
#define update   s11_poori_update_impl
#define display  s11_poori_display_impl
#define reshape  s11_poori_reshape_impl
#define keyboard s11_poori_keyboard_impl
#define drawHUD  s11_poori_drawHUD_impl
#define main     s11_poori_main_disabled

#include "scenes_C_files/poori_scene.c"

#undef main
#undef keyboard
#undef reshape
#undef display
#undef update
#undef drawHUD
#undef glutTimerFunc
#undef s11_glutTimerFunc

/* Clean up commonly defined macros from embedded file to reduce cross-header leakage. */
#ifdef PI
#undef PI
#endif
#ifdef TAU
#undef TAU
#endif
#ifdef NS
#undef NS
#endif
#ifdef MAX_OIL
#undef MAX_OIL
#endif
#ifdef MAX_BURN
#undef MAX_BURN
#endif
#ifdef MAX_SMOKE
#undef MAX_SMOKE
#endif

static int s11_poori_initialized = 0;

static void scene11_display(void) {
	if (!s11_poori_initialized) {
		/* Match original deterministic randomness */
		srand(12345);
		initScene();
		s11_poori_initialized = 1;
	}

	/* Keep the embedded scene's expected 2D projection in sync with the current window size. */
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	s11_poori_reshape_impl(w, h);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);

	glClearColor(0.04f, 0.06f, 0.14f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	s11_poori_display_impl();
}

static void scene11_update(float deltaTime) {
	( void)deltaTime;
	if (!s11_poori_initialized) {
		srand(12345);
		initScene();
		s11_poori_initialized = 1;
	}

	/* Embedded update uses a fixed 60 FPS step, which matches the original.
	   Orchestrator tick is already ~0.016s, so we keep original behavior. */
	s11_poori_update_impl(0);
}

#endif /* SCENE_11_POORI_SCENE_H */
