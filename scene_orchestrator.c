/*
 * MISSION MANGAL - Complete Animation Sequence
 * Master Orchestrator Program
 * 
 * This program sequences all 16 scenes from the Mission Mangal story:
 * 1. ISRO Building Zoom (8 sec)
 * 2. Server Room (5 sec)
 * 3. Scientists Close-Up (5 sec)
 * 4. Rocket at Shoreline (5 sec)
 * 5. Timer Countdown (12 sec)
 * 6. Rocket Launch (8 sec)
 * 7. Rocket Crash (8 sec)
 * 8. Server Room Tension (5 sec)
 * 9. Conference Room (5 sec)
 * 10. Ideating Orbit (8 sec)
 * 11. Team Formation (8 sec)
 * 12. Corridor Interaction (5 sec)
 * 13. Rocket Launch II (8 sec)
 * 14. Catching Orbit (10 sec)
 * 15. Success (5 sec)
 * 16. The End (3 sec)
 * 
 * Total: 133 seconds (~2.2 minutes)
 * 
 * Usage: Compile each original scene file separately, then link with this orchestrator
 * OR: Merge all scene code here with scene-specific function names
 * 
 * TO EXTEND THIS PROGRAM:
 * 1. Create scene_N_<name>.h headers for each scene (see scene_1_isro.h as example)
 * 2. Include the headers at the top
 * 3. Register scene functions in initializeScenes()
 * 4. Test each scene individually before adding to the sequence
 */

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* OpenGL includes - compatible with FreeGLUT on Windows */
#ifdef _WIN32
    #include <GL/freeglut.h>
#else
    #include <GL/glut.h>
#endif

/* Include scene header files */
#include "scene_1_isro.h"
#include "scene_2_server.h"
#include "scene_3_timer.h"
#include "scene_3_shoreline.h"
#include "scene_4_timer.h"
/* TODO: #include etc for scenes 4-16 */

/* ===== GLOBAL ANIMATION CONTROL ===== */
static float g_masterTime = 0.0f;       /* Total elapsed time across all scenes */
static int g_currentScene = 0;          /* Current scene (1-16) */
static float g_sceneStartTime = 0.0f;   /* When this scene started */
static float g_sceneTime = 0.0f;        /* Time within current scene */
static int g_previousScene = 0;         /* Previous scene for fade transitions */
static float g_transitionDuration = 0.5f; /* Fade transition duration (seconds) */

/* Scene timing configuration (total duration per scene in seconds) */
static const struct {
    int sceneNum;
    float duration;
    const char *name;
} g_sceneConfig[] = {
    {1,  8.0f, "ISRO Building Zoom"},
    {2,  5.0f, "Server Room"},
    {3,  5.0f, "Scientists Close-Up"},
    {4,  5.0f, "Rocket at Shoreline"},
    {5,  12.0f, "Timer Countdown"},
    {6,  8.0f, "Rocket Launch"},
    {7,  8.0f, "Rocket Crash"},
    {8,  5.0f, "Server Room Tension"},
    {9,  5.0f, "Conference Room"},
    {10, 8.0f, "Ideating Orbit"},
    {11, 8.0f, "Team Formation"},
    {12, 5.0f, "Corridor Interaction"},
    {13, 8.0f, "Rocket Launch II"},
    {14, 10.0f, "Catching Orbit"},
    {15, 5.0f, "Success"},
    {16, 3.0f, "The End"}
};

static const int NUM_SCENES = sizeof(g_sceneConfig) / sizeof(g_sceneConfig[0]);
static float g_totalDuration = 0.0f;

/* ===== SCENE STATE VARIABLES ===== */
/* 
 * Note: Scene-specific variables are now defined in scene_#_<name>.h headers
 * Examples: s1_gTime, s1_gCamBack, s2_gTime, etc.
 * This avoids duplication and keeps scene code modular
 */

/* ===== FUNCTION TYPE DEFINITIONS ===== */
/* Scene display and update functions */
typedef void (*SceneDisplayFunc)(void);
typedef void (*SceneUpdateFunc)(float deltaTime);

/* Array of scene display functions - will be populated at initialization */
static SceneDisplayFunc g_sceneDisplay[17] = {NULL};
static SceneUpdateFunc g_sceneUpdate[17] = {NULL};

/* ===== PLACEHOLDER SCENE IMPLEMENTATIONS ===== */
/* Scene 1 is now included from scene_1_isro.h header file */
/* Scenes 2-3: Placeholder implementations shown below */
/* These will be replaced with full implementations from scene headers */

/* SCENE 2: Server Room - PLACEHOLDER (will be replaced with scene_2_server.h) */
/* SCENE 3: Scientists - PLACEHOLDER (will be replaced with scene_3_scientists.h) */
/* Placeholder implementations removed - use scene_N_<name>.h headers instead */

/* Generic placeholder for any unimplemented scene */
static void scene_placeholder_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    glDisable(GL_LIGHTING);
    glColor3f(0.8f, 0.8f, 1.0f);
    glBegin(GL_QUADS);
    glVertex3f(-5.0f, -3.0f, -10.0f);
    glVertex3f(5.0f, -3.0f, -10.0f);
    glVertex3f(5.0f, 3.0f, -10.0f);
    glVertex3f(-5.0f, 3.0f, -10.0f);
    glEnd();
    
    glEnable(GL_LIGHTING);
    glutSwapBuffers();
}

static void scene_placeholder_update(float dt) {
    /* Placeholder */
}

/* ===== SCENE ROUTING LOGIC ===== */
static void updateScene(void) {
    float timeSinceStart = 0.0f;
    int newScene = 0;
    
    /* Determine which scene based on master time */
    for (int i = 0; i < NUM_SCENES; i++) {
        float nextTime = timeSinceStart + g_sceneConfig[i].duration;
        if (g_masterTime < nextTime) {
            newScene = i + 1;
            g_sceneTime = g_masterTime - timeSinceStart;
            break;
        }
        timeSinceStart = nextTime;
    }
    
    /* Handle scene transition */
    if (newScene != g_currentScene) {
        printf("[SCENE TRANSITION] %d -> %d: %s\n", 
               g_currentScene, newScene, 
               newScene <= NUM_SCENES ? g_sceneConfig[newScene-1].name : "END");
        g_previousScene = g_currentScene;
        g_currentScene = newScene;
        g_sceneStartTime = g_masterTime;
    }
    
    /* Update current scene */
    if (g_currentScene > 0 && g_currentScene <= NUM_SCENES && g_sceneUpdate[g_currentScene]) {
        g_sceneUpdate[g_currentScene](0.016f);
    }
    
    /* Check if animation is complete */
    if (g_currentScene > NUM_SCENES) {
        printf("\n=== MISSION COMPLETE ===\n");
        g_currentScene = NUM_SCENES;
    }
}

static void masterDisplay(void) {
    /* Calculate transition alpha for smooth fade effect */
    float transitionAlpha = 1.0f;
    int displayScene = g_currentScene;
    
    /* Check if we're in a transition (first part of new scene) */
    if (g_currentScene > 0 && g_currentScene <= NUM_SCENES && g_sceneTime < g_transitionDuration) {
        transitionAlpha = g_sceneTime / g_transitionDuration;  /* Fade in: 0->1 */
    }
    
    /* Render current scene */
    if (displayScene > 0 && displayScene <= NUM_SCENES && g_sceneDisplay[displayScene]) {
        g_sceneDisplay[displayScene]();
        
        /* Apply fade-in overlay during transition */
        if (transitionAlpha < 1.0f) {
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            
            /* Black fade-in overlay */
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f - transitionAlpha);
            glBegin(GL_QUADS);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glEnd();
            
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
            
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
        }
    } else if (displayScene > NUM_SCENES) {
        /* Final "The End" screen */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos3f(-1.0f, 0.0f, -5.0f);
        const char *end = "MISSION MANGAL: COMPLETE";
        for (int i = 0; end[i]; i++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, end[i]);
        }
        glEnable(GL_LIGHTING);
    }
}

static void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
}

static void timerCallback(int value) {
    (void)value;
    g_masterTime += 0.016f;
    updateScene();
    glutPostRedisplay();
    glutTimerFunc(16, timerCallback, 0);
}

static void keyboardCallback(unsigned char key, int x, int y) {
    (void)x; (void)y;
    if (key == 27) {  /* ESC */
        exit(0);
    } else if (key == ' ') {
        /* Pause/resume - not implemented for simplicity */
        printf("Current scene: %d/%d\n", g_currentScene, NUM_SCENES);
    }
}


static void initializeScenes(void) {
    /* Register all scenes */
    g_sceneDisplay[1] = scene1_display;
    g_sceneUpdate[1] = scene1_update;
    
    g_sceneDisplay[2] = scene2_display;
    g_sceneUpdate[2] = scene2_update;
    
    g_sceneDisplay[3] = scene3_display;
    g_sceneUpdate[3] = scene3_update;
    
    /* Scene 4: Shoreline (Rocket at Launch Site) */
    g_sceneDisplay[4] = shoreline_display;  /* From scene_3_shoreline.h */
    g_sceneUpdate[4] = shoreline_update;
    
    /* Scene 5: Mission Control Timer */
    g_sceneDisplay[5] = scene4_display;  /* From scene_4_timer.h */
    g_sceneUpdate[5] = scene4_update;
    
    /* Scenes 6-16: Placeholder (replace with actual implementations from headers) */
    for (int i = 6; i <= 16; i++) {
        g_sceneDisplay[i] = scene_placeholder_display;
        g_sceneUpdate[i] = scene_placeholder_update;
    }
    
    /* Calculate total duration */
    g_totalDuration = 0.0f;
    for (int i = 0; i < NUM_SCENES; i++) {
        g_totalDuration += g_sceneConfig[i].duration;
    }
    
    printf("\n=== MISSION MANGAL - COMPLETE ANIMATION ===\n");
    printf("Total Sequence: %.1f seconds (%.1f minutes)\n", g_totalDuration, g_totalDuration / 60.0f);
    printf("Number of Scenes: %d\n", NUM_SCENES);
    printf("\nScene Configuration:\n");
    for (int i = 0; i < NUM_SCENES; i++) {
        printf("  %2d. %-30s (%5.1f sec)\n", g_sceneConfig[i].sceneNum, g_sceneConfig[i].name, g_sceneConfig[i].duration);
    }
    printf("\nStarting animation...\n\n");
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1366, 768);
    glutCreateWindow("MISSION MANGAL - Complete Animation Sequence");
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    initializeScenes();
    
    glutDisplayFunc(masterDisplay);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardCallback);
    glutTimerFunc(16, timerCallback, 0);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    
    GLfloat lightPos[] = {5.0f, 10.0f, 7.0f, 1.0f};
    GLfloat lightDiff[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightAmb[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    
    glutMainLoop();
    return 0;
}
