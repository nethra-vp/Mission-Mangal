/*
 * Scene 15: Final Rocket Launch (Successful Mission)
 * Embedded from scenes_C_files/final_rocket_launch.c
 */

#ifndef SCENE_15_FINAL_ROCKET_LAUNCH_H
#define SCENE_15_FINAL_ROCKET_LAUNCH_H

#ifdef _WIN32
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

/* Prevent the embedded scene from scheduling its own GLUT timers.
   The orchestrator drives updates via scene15_update(). */
#define s15_glutTimerFunc(ms, func, val) ((void)0)
#define glutTimerFunc s15_glutTimerFunc

/* Macro-rename everything that could collide in the single-TU orchestrator build. */
#define g_launchTime s15_g_launchTime
#define g_rocketY    s15_g_rocketY
#define kRocketX     s15_kRocketX
#define kRocketZ     s15_kRocketZ

/* Prevent macro redefinition warnings across embedded scenes. */
#ifdef STAR_COUNT
#undef STAR_COUNT
#endif

#define gStarPos     s15_gStarPos
#define gStarSize    s15_gStarSize
#define gStarPhase   s15_gStarPhase
#define kStarWrapX   s15_kStarWrapX
#define kStarWrapY   s15_kStarWrapY
#define kStarWrapZ   s15_kStarWrapZ

#define clamp01      s15_clamp01_impl
#define smooth01     s15_smooth01_impl
#define setColor3    s15_setColor3_impl
#define drawUnitBox  s15_drawUnitBox_impl
#define drawStrokeText s15_drawStrokeText_impl
#define drawCloud    s15_drawCloud_impl
#define initStarfield s15_initStarfield_impl
#define drawStarfield s15_drawStarfield_impl
#define drawSkyAndClouds s15_drawSkyAndClouds_impl
#define updateSkyColor s15_updateSkyColor_impl
#define drawSeaSurface s15_drawSeaSurface_impl
#define drawShoreline s15_drawShoreline_impl
#define drawTreeClump s15_drawTreeClump_impl
#define drawForegroundVegetation s15_drawForegroundVegetation_impl
#define drawBird      s15_drawBird_impl
#define drawBirds     s15_drawBirds_impl
#define drawGroundAndPaths s15_drawGroundAndPaths_impl
#define drawLaunchTower s15_drawLaunchTower_impl
#define drawLaunchPad s15_drawLaunchPad_impl
#define drawAnimatedRocket s15_drawAnimatedRocket_impl
#define drawCelebratePerson s15_drawCelebratePerson_impl
#define drawSmokeBillow s15_drawSmokeBillow_impl
#define drawLaunchSmoke s15_drawLaunchSmoke_impl
#define setupLighting s15_setupLighting_impl

#define display_launch s15_display_launch_impl
#define timer_launch   s15_timer_launch_impl
#define reshape_launch s15_reshape_launch_impl
#define main           s15_main_disabled

#include "scenes_C_files/final_rocket_launch.c"

#undef main
#undef reshape_launch
#undef timer_launch
#undef display_launch

#undef setupLighting
#undef drawLaunchSmoke
#undef drawSmokeBillow
#undef drawCelebratePerson
#undef drawAnimatedRocket
#undef drawLaunchPad
#undef drawLaunchTower
#undef drawGroundAndPaths
#undef drawBirds
#undef drawBird
#undef drawForegroundVegetation
#undef drawTreeClump
#undef drawShoreline
#undef drawSeaSurface
#undef updateSkyColor
#undef drawSkyAndClouds
#undef drawStarfield
#undef initStarfield
#undef drawCloud
#undef drawStrokeText
#undef drawUnitBox
#undef setColor3
#undef smooth01
#undef clamp01

#undef kStarWrapZ
#undef kStarWrapY
#undef kStarWrapX
#undef gStarPhase
#undef gStarSize
#undef gStarPos

#ifdef STAR_COUNT
#undef STAR_COUNT
#endif

#undef kRocketZ
#undef kRocketX
#undef g_rocketY
#undef g_launchTime

#undef glutTimerFunc
#undef s15_glutTimerFunc

static int s15_initialized = 0;

static void s15_ensureInit(void) {
	if (s15_initialized) {
		return;
	}

	/* Match embedded main() initialization */
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.99f, 0.61f, 0.32f, 1.0f);
	s15_initStarfield_impl();

	s15_initialized = 1;
}

static void scene15_display(void) {
	s15_ensureInit();

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	s15_reshape_launch_impl(w, h);

	glDisable(GL_FOG);
	s15_display_launch_impl();
}

static void scene15_update(float deltaTime) {
	( void)deltaTime;
	s15_ensureInit();

	/* Advances time and posts redisplay; timer rescheduling is disabled above. */
	s15_timer_launch_impl(0);
}

#endif /* SCENE_15_FINAL_ROCKET_LAUNCH_H */
