/*
 * Scene 17: Success (Server Room)
 * Embedded from scenes_C_files/success_server_room.c
 */

#ifndef SCENE_17_SUCCESS_SERVER_ROOM_H
#define SCENE_17_SUCCESS_SERVER_ROOM_H

#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

/* Prevent the embedded scene from scheduling its own GLUT timers.
   The orchestrator drives updates via scene17_update(). */
#define s17_glutTimerFunc(ms, func, val) ((void)0)
#define glutTimerFunc s17_glutTimerFunc

/* Rename embedded globals/helpers/entrypoints to avoid collisions across embedded scenes. */
#define gTime s17_gTime

#define setColor3 s17_setColor3_impl
#define drawBox s17_drawBox_impl
#define drawBitmapText s17_drawBitmapText_impl
#define drawStrokeText s17_drawStrokeText_impl
#define getStrokeTextWidth s17_getStrokeTextWidth_impl
#define drawVideoWallMainContent s17_drawVideoWallMainContent_impl
#define drawVideoWall s17_drawVideoWall_impl
#define drawScientist s17_drawScientist_impl
#define drawHuman s17_drawHuman_impl
#define drawDeskStation s17_drawDeskStation_impl
#define drawTieredControlGallery s17_drawTieredControlGallery_impl
#define drawServerRack s17_drawServerRack_impl
#define drawServerGallery s17_drawServerGallery_impl
#define drawArchitecture s17_drawArchitecture_impl
#define setupLights s17_setupLights_impl

#define display s17_display_impl
#define reshape s17_reshape_impl
#define timer s17_timer_impl
#define main s17_main_disabled

#include "scenes_C_files/success_server_room.c"

#undef main
#undef timer
#undef reshape
#undef display

#undef setupLights
#undef drawArchitecture
#undef drawServerGallery
#undef drawServerRack
#undef drawTieredControlGallery
#undef drawDeskStation
#undef drawHuman
#undef drawScientist
#undef drawVideoWall
#undef drawVideoWallMainContent
#undef getStrokeTextWidth
#undef drawStrokeText
#undef drawBitmapText
#undef drawBox
#undef setColor3

#undef gTime

#undef glutTimerFunc
#undef s17_glutTimerFunc

static int s17_initialized = 0;

static void s17_ensureInit(void) {
	if (s17_initialized) {
		return;
	}

	/* Match embedded main() setup */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.03f, 0.04f, 0.06f, 1.0f);

	s17_initialized = 1;
}

static void scene17_display(void) {
	s17_ensureInit();

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	s17_reshape_impl(w, h);

	glDisable(GL_FOG);
	s17_display_impl();
}

static void scene17_update(float deltaTime) {
	( void)deltaTime;
	s17_ensureInit();

	/* Advances embedded time and requests redraw; timer rescheduling is disabled above. */
	s17_timer_impl(0);
}

#endif /* SCENE_17_SUCCESS_SERVER_ROOM_H */
