
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define PI  3.14159265358979323846f
#define TAU (2.0f * PI)

static int W = 1280, H = 760;
static float T = 0.0f;

/* animation */
static float pooriBobble=0,oilShimmer=0,orbiterAngle=0,engineGlow=0;
static float armSwing=0,flameDance=0;

/* stars */
#define NS 90
static float starX[NS],starY[NS],starB[NS];

/* particles */
#define MAX_OIL   70
#define MAX_BURN  50
#define MAX_SMOKE 30

typedef struct{float x,y,vx,vy,life,sz,r,g,b;}OilP;
typedef struct{float x,y,vx,vy,life,sz;}BurnP;
typedef struct{float x,y,vy,life,sz;}SmokeP;

static OilP   oilP[MAX_OIL];   static int nOil=0;
static BurnP  burnP[MAX_BURN];  static int nBurn=0;
static SmokeP smokeP[MAX_SMOKE];static int nSmoke=0;

/* ─── helpers ─── */
static float rnd(void){return(float)rand()/(float)RAND_MAX;}

static void col3(float r,float g,float b){glColor3f(r,g,b);}
static void col4(float r,float g,float b,float a){glColor4f(r,g,b,a);}

static void quad(float x,float y,float w,float h){
    glBegin(GL_QUADS);
    glVertex2f(x,y);glVertex2f(x+w,y);
    glVertex2f(x+w,y+h);glVertex2f(x,y+h);
    glEnd();
}
static void disk(float cx,float cy,float r,int s){
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx,cy);
    for(int i=0;i<=s;i++){float a=TAU*i/s;glVertex2f(cx+cosf(a)*r,cy+sinf(a)*r);}
    glEnd();
}
static void efill(float cx,float cy,float rx,float ry,int s){
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx,cy);
    for(int i=0;i<=s;i++){float a=TAU*i/s;glVertex2f(cx+cosf(a)*rx,cy+sinf(a)*ry);}
    glEnd();
}
static void ering(float cx,float cy,float rx,float ry,int s){
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<s;i++){float a=TAU*i/s;glVertex2f(cx+cosf(a)*rx,cy+sinf(a)*ry);}
    glEnd();
}
static void txt(float x,float y,const char*s,void*f){
    glRasterPos2f(x,y);
    for(const char*c=s;*c;c++)glutBitmapCharacter(f,*c);
}
static void blend(int on){
    if(on){glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);}
    else glDisable(GL_BLEND);
}

/* ─── init ─── */
static void initScene(void){
    for(int i=0;i<NS;i++){
        starX[i]=rnd();starY[i]=rnd()*0.82f;starB[i]=0.4f+rnd()*0.6f;
    }
}

/* ─── spawn ─── */
static void spawnOil(float cx,float cy){
    if(nOil>=MAX_OIL)return;
    OilP*p=&oilP[nOil++];
    p->x=cx+(rnd()-.5f)*78;p->y=cy+(rnd()-.5f)*20;
    p->vx=(rnd()-.5f)*.6f;p->vy=1.2f+rnd()*2.8f;
    p->life=1;p->sz=2+rnd()*5;
    float h=rnd();p->r=.9f+h*.1f;p->g=.55f+h*.25f;p->b=.05f;
}
static void spawnBurn(float bx,float by){
    if(nBurn>=MAX_BURN)return;
    BurnP*b=&burnP[nBurn++];
    b->x=bx;b->y=by;b->vx=(rnd()-.5f)*3.5f;b->vy=(rnd()-.5f)*3.5f;
    b->life=1;b->sz=3+rnd()*7;
}
static void spawnSmoke(float sx,float sy){
    if(nSmoke>=MAX_SMOKE)return;
    SmokeP*s=&smokeP[nSmoke++];
    s->x=sx+(rnd()-.5f)*18;s->y=sy;s->vy=.7f+rnd()*1.1f;s->life=1;s->sz=7+rnd()*12;
}

/* ─── update ─── */
static void updateP(void){
    for(int i=0;i<nOil;){OilP*p=&oilP[i];p->x+=p->vx;p->y+=p->vy;p->life-=.028f;if(p->life<=0)oilP[i]=oilP[--nOil];else i++;}
    for(int i=0;i<nBurn;){BurnP*b=&burnP[i];b->x+=b->vx;b->y+=b->vy;b->vx*=.90f;b->vy*=.90f;b->life-=.045f;if(b->life<=0)burnP[i]=burnP[--nBurn];else i++;}
    for(int i=0;i<nSmoke;){SmokeP*s=&smokeP[i];s->x+=sinf(T*1.5f+i)*.3f;s->y+=s->vy;s->life-=.018f;if(s->life<=0)smokeP[i]=smokeP[--nSmoke];else i++;}
}
static void update(int v){
    (void)v;T+=1.f/60.f;
    pooriBobble=sinf(T*2.8f)*3.5f;
    oilShimmer=.5f+.5f*sinf(T*9.f);
    flameDance=T;
    armSwing=sinf(T*1.6f)*7.f;
    engineGlow=.5f+.5f*sinf(T*14.f);
    float spd=1.f+.9f*(1.f-fabsf(sinf(orbiterAngle)));
    orbiterAngle+=.013f*spd;if(orbiterAngle>TAU)orbiterAngle-=TAU;

    /* stove centre x=240 */
    float panCX=232.f,panCY=428.f;
    if(rnd()<.30f)spawnOil(panCX,panCY);
    if(rnd()<.10f)spawnSmoke(panCX,panCY+28.f);

    float diagCX=830.f,diagCY=415.f,orx=162.f,ory=96.f;
    float bx=diagCX+cosf(orbiterAngle)*orx,by=diagCY+sinf(orbiterAngle)*ory;
    if(sinf(orbiterAngle)>.55f&&rnd()<.35f)spawnBurn(bx,by);

    updateP();
    glutPostRedisplay();
    glutTimerFunc(16,update,0);
}

/* ══════════════════════════════════════════
   BACKGROUND
══════════════════════════════════════════ */
static void drawBG(void){
    glBegin(GL_QUADS);
    col3(.04f,.06f,.14f);glVertex2f(0,0);glVertex2f(W,0);
    col3(.07f,.10f,.20f);glVertex2f(W,H);glVertex2f(0,H);
    glEnd();
    glBegin(GL_QUADS);
    col3(.06f,.08f,.14f);glVertex2f(0,0);glVertex2f(W,0);
    col3(.09f,.11f,.17f);glVertex2f(W,H*.28f);glVertex2f(0,H*.28f);
    glEnd();
    /* ceiling lights */
    for(int i=0;i<6;i++){
        float lx=W*(.08f+i*.17f),b=.6f+.4f*sinf(T*.4f+i);
        blend(1);col4(.5f,.75f,1.f,.14f*b);efill(lx,H,55,28,24);
        col4(.9f,.95f,1.f,.65f*b);efill(lx,H,9,4.5f,12);blend(0);
    }
}

/* ══════════════════════════════════════════
   MONITORS
══════════════════════════════════════════ */
static void drawMonitors(void){
    /* left 3 */
    float ls[3][4]={{18,H*.60f,165,125},{18,H*.77f,100,78},{125,H*.77f,100,78}};
    for(int i=0;i<3;i++){
        float x=ls[i][0],y=ls[i][1],sw=ls[i][2],sh=ls[i][3];
        col3(.14f,.16f,.20f);quad(x-4,y-4,sw+8,sh+8);
        col3(.02f,.06f,.16f);quad(x,y,sw,sh);
        glLineWidth(1.5f);col3(0,.75f,.35f);
        glBegin(GL_LINE_LOOP);
        for(int j=0;j<48;j++){float a=TAU*j/48;glVertex2f(x+sw*.5f+cosf(a)*sw*.28f,y+sh*.5f+sinf(a)*sh*.22f);}
        glEnd();
        float blink=.5f+.5f*sinf(T*3+i);col3(0,blink,.3f);
        disk(x+sw*.5f+cosf(T*1.4f+i)*sw*.28f,y+sh*.5f+sinf(T*1.4f+i)*sh*.22f,3.5f,8);
        col3(0,.65f,.28f);
        char buf[32];sprintf(buf,"ALT:%04.0fkm",400.f+sinf(T+i)*55);txt(x+4,y+sh-16,buf,GLUT_BITMAP_HELVETICA_10);
        sprintf(buf,"VEL:%4.2fkm/s",7.6f+cosf(T*.3f)*.4f);txt(x+4,y+sh-27,buf,GLUT_BITMAP_HELVETICA_10);
    }
    /* right 2 */
    float rs[2][4]={{W-232,H*.60f,218,148},{W-232,H*.77f,218,88}};
    for(int i=0;i<2;i++){
        float x=rs[i][0],y=rs[i][1],sw=rs[i][2],sh=rs[i][3];
        col3(.14f,.16f,.20f);quad(x-4,y-4,sw+8,sh+8);
        col3(.02f,.04f,.14f);quad(x,y,sw,sh);
        glLineWidth(1.5f);col3(.18f,.48f,1);
        glBegin(GL_LINE_STRIP);
        for(int j=0;j<(int)sw;j++){float v=y+sh*.5f+sinf(j*.14f+T)*sh*.28f+cosf(j*.06f)*sh*.1f;glVertex2f(x+j,v);}
        glEnd();
        col3(.2f,.55f,1);txt(x+4,y+sh-16,"TRAJECTORY DATA",GLUT_BITMAP_HELVETICA_10);
        char buf[32];sprintf(buf,"T+%05.1fs",T*60);txt(x+4,y+sh-27,buf,GLUT_BITMAP_HELVETICA_10);
    }
    /* banner */
    col3(.07f,.07f,.07f);quad(W-312,H-38,312,38);
    float rc=.5f+.5f*sinf(T*5);col3(rc,.1f,.1f);
    txt(W-308,H-22,"SATELLITE MISSION CONTROL",GLUT_BITMAP_HELVETICA_12);
    col3(1,.8f,0);
    char tk[]=" MOM ORBITER: NOMINAL  | PERIGEE BURN ACTIVE  | ALT:421KM  | ";
    char tk2[256];strcpy(tk2,tk);strcat(tk2,tk);
    txt(W-312+fmodf(T*26,312)-312,H-12,tk2,GLUT_BITMAP_HELVETICA_10);
}

/* ══════════════════════════════════════════
   DIAGRAM BOARD  (x: 448 → W-10)
══════════════════════════════════════════ */
static void drawBoard(void){
    float bx=448,by=H*.12f,bw=W-458.f,bh=H*.76f;

    blend(1);col4(0,0,0,.45f);quad(bx+8,by-8,bw,bh);blend(0);
    glBegin(GL_QUADS);
    col3(.04f,.07f,.18f);glVertex2f(bx,by);glVertex2f(bx+bw,by);
    col3(.02f,.04f,.12f);glVertex2f(bx+bw,by+bh);glVertex2f(bx,by+bh);
    glEnd();
    glLineWidth(3);col3(.28f,.48f,.82f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(bx,by);glVertex2f(bx+bw,by);glVertex2f(bx+bw,by+bh);glVertex2f(bx,by+bh);
    glEnd();glLineWidth(1);

    /* stars */
    for(int i=0;i<NS;i++){
        float sx=bx+starX[i]*bw,sy=by+starY[i]*bh;
        float b=starB[i]*(.55f+.45f*sinf(T*2.2f+i*.7f));
        col3(b,b,b);disk(sx,sy,1.1f,4);
    }

    /* title */
    col3(1,1,1);txt(bx+bw*.20f,by+bh-24,"THE  \"POORI THEORY\"",GLUT_BITMAP_HELVETICA_18);
    col3(.68f,.83f,1);
    txt(bx+bw*.07f,by+bh-40,"SIMPLE HOUSEHOLD WISDOM MEETS COMPLEX SPACE MECHANICS",GLUT_BITMAP_HELVETICA_10);
    col3(.18f,.28f,.55f);
    glBegin(GL_LINES);glVertex2f(bx+10,by+bh-47);glVertex2f(bx+bw-10,by+bh-47);glEnd();

    /* centre */
    float diagCX=bx+bw*.50f,diagCY=by+bh*.46f,orx=162.f,ory=96.f;

    for(int ring=4;ring>=1;ring--){
        blend(1);col4(.28f,.48f,.88f,.06f+ring*.03f);ering(diagCX,diagCY,orx*ring*.42f,ory*ring*.42f,64);blend(0);
    }
    /* oil pan arc */
    glLineWidth(2.5f);col3(.55f,.44f,.28f);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=40;i++){float a=PI+PI*i/40;glVertex2f(diagCX+cosf(a)*125,diagCY+sinf(a)*62+22);}
    glEnd();
    glLineWidth(5);col3(.42f,.32f,.18f);
    glBegin(GL_LINES);glVertex2f(diagCX-125,diagCY+22);glVertex2f(diagCX-200,diagCY+34);glEnd();
    glLineWidth(1);
    col3(.75f,.65f,.38f);
    txt(diagCX-128,diagCY-78,"THE \"OIL PAN\"",GLUT_BITMAP_HELVETICA_10);
    txt(diagCX-118,diagCY-90,"OF GRAVITY",GLUT_BITMAP_HELVETICA_10);

    /* Earth */
    blend(1);col4(.08f,.35f,.9f,.18f);efill(diagCX,diagCY,42,42,32);blend(0);
    col3(.08f,.32f,.85f);efill(diagCX,diagCY,27,27,32);
    col3(.14f,.52f,.22f);efill(diagCX-9,diagCY+5,10,10,14);efill(diagCX+11,diagCY-4,8,8,12);efill(diagCX-2,diagCY-11,6.5f,6.5f,10);
    blend(1);col4(1,1,1,.22f+.1f*sinf(T*.7f));efill(diagCX+5,diagCY+8,8,8,14);blend(0);
    col3(.78f,.90f,1);txt(diagCX-14,diagCY+33,"EARTH",GLUT_BITMAP_HELVETICA_10);

    /* orbit path */
    glLineWidth(1.5f);blend(1);col4(.48f,.78f,1,.42f);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<128;i++){float a=TAU*i/128;glVertex2f(diagCX+cosf(a)*orx,diagCY+sinf(a)*ory);}
    glEnd();blend(0);

    /* trail */
    for(int i=0;i<55;i++){
        float ta=orbiterAngle-i*.045f;
        float px=diagCX+cosf(ta)*orx,py=diagCY+sinf(ta)*ory;
        blend(1);col4(1,.82f,.28f,(1-i/55.f)*.55f);disk(px,py,2.8f-i*.04f,6);blend(0);
    }
    /* burn particles */
    blend(1);glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    for(int i=0;i<nBurn;i++){BurnP*b=&burnP[i];col4(1,.45f+b->life*.35f,.08f,b->life*.85f);disk(b->x,b->y,b->sz*b->life,8);}
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);blend(0);

    /* MOM as Poori */
    float ox=diagCX+cosf(orbiterAngle)*orx,oy=diagCY+sinf(orbiterAngle)*ory,pb=pooriBobble*.45f;
    blend(1);col4(1,.78f,.18f,.22f*(.65f+.35f*engineGlow));efill(ox,oy+pb,24,15,32);blend(0);
    glBegin(GL_TRIANGLE_FAN);
    col3(.84f,.54f,.09f);glVertex2f(ox,oy+pb);
    for(int i=0;i<=36;i++){float a=TAU*i/36,rr=1+.13f*sinf(a*5+T*3.2f),hh=.5f+.5f*fabsf(sinf(a*3));col3(.94f-hh*.1f,.64f-hh*.05f,.09f+hh*.05f);glVertex2f(ox+cosf(a)*16*rr,oy+pb+sinf(a)*10*rr);}
    glEnd();
    col3(.50f,.26f,.04f);disk(ox-4,oy+pb+1,3,8);disk(ox+5,oy+pb-2,2.5f,8);disk(ox,oy+pb+3,2,6);
    blend(1);col4(1,.95f,.7f,.28f);efill(ox-4,oy+pb-3,11,5,12);blend(0);
    /* nozzle */
    float na=orbiterAngle+PI,nx=ox+cosf(na)*12,ny=oy+pb+sinf(na)*9;
    col3(.58f,.58f,.68f);
    glBegin(GL_TRIANGLES);glVertex2f(nx,ny);glVertex2f(nx+cosf(na+.45f)*8,ny+sinf(na+.45f)*8);glVertex2f(nx+cosf(na-.45f)*8,ny+sinf(na-.45f)*8);glEnd();
    if(sinf(orbiterAngle)>.25f){
        float fs=5+engineGlow*9;
        blend(1);col4(1,.5f,0,.85f);disk(nx+cosf(na)*fs*.5f,ny+sinf(na)*fs*.5f,fs*.5f,12);
        col4(1,.9f,.4f,.65f);disk(nx+cosf(na)*fs*.28f,ny+sinf(na)*fs*.28f,fs*.25f,8);blend(0);
    }
    col3(1,.94f,.48f);txt(ox+18,oy+pb+18,"MOM ORBITER",GLUT_BITMAP_HELVETICA_10);
    txt(ox+18,oy+pb+6,"(as a Poori)",GLUT_BITMAP_HELVETICA_10);
    glBegin(GL_LINES);glVertex2f(ox+16,oy+pb+10);glVertex2f(ox+4,oy+pb+2);glEnd();

    /* steps */
    float lx2=bx+14,ly2=by+bh-62,sdy=33;
    const char*steps[5]={"1. INITIAL ORBIT","2. \"FRYING THE EDGES\"","   (PERIGEE BURN)","3. RAISING THE ORBIT","4. TRANS-MARS INJECTION"};
    float sc2[5][3]={{.38f,.90f,.38f},{1,.85f,.18f},{1,.85f,.18f},{.38f,.68f,1},{1,.48f,.18f}};
    for(int i=0;i<5;i++){
        float sy=ly2-i*sdy;
        col3(sc2[i][0],sc2[i][1],sc2[i][2]);
        if(i==4){col3(.82f,.32f,.12f);disk(lx2+6,sy-12,8,16);col3(.68f,.22f,.08f);disk(lx2+2,sy-10,4,8);col3(.82f,.32f,.12f);txt(lx2+18,sy-15,"5. ARRIVAL AT MARS",GLUT_BITMAP_HELVETICA_10);}
        else{disk(lx2+6,sy-12,5,8);txt(lx2+18,sy-15,steps[i],GLUT_BITMAP_HELVETICA_10);}
    }
    /* right labels */
    float rx2=bx+bw-188;
    col3(1,.82f,.28f);
    txt(rx2,by+bh*.76f,"PERIGEE BURN:",GLUT_BITMAP_HELVETICA_10);
    txt(rx2,by+bh*.76f-13,"\"FRYING THE EDGES\"",GLUT_BITMAP_HELVETICA_10);
    txt(rx2,by+bh*.76f-26,"FOR SPEED",GLUT_BITMAP_HELVETICA_10);
    blend(1);col4(1,.82f,.28f,.5f);
    glBegin(GL_LINES);glVertex2f(rx2-6,by+bh*.76f-6);glVertex2f(diagCX+orx*.55f,diagCY+ory*.62f);glEnd();blend(0);
    col3(.48f,.78f,1);txt(rx2,by+bh*.40f,"ORBITAL PATH",GLUT_BITMAP_HELVETICA_10);
    col3(.5f,.8f,1);txt(rx2,by+bh*.57f,"ENGINE BURNS",GLUT_BITMAP_HELVETICA_10);
}

/* ══════════════════════════════════════════
   COOKING STATION + LADY SCIENTIST (together, left-centre)
   Stove centre x≈232, lady stands at x≈295 right behind pan handle
══════════════════════════════════════════ */
static void drawCookingAndLady(void){
    /* ── TABLE ── */
    float tableY=342.f;
    col3(.50f,.46f,.40f);quad(18,tableY,430,14);
    col4(0,0,0,.28f);blend(1);quad(18,tableY,430,6);blend(0);

    /* ── STOVE ── */
    float scx=232.f,stoveY=tableY+14;
    col3(.28f,.30f,.33f);quad(scx-78,stoveY,156,88);
    col3(.22f,.24f,.26f);quad(scx-73,stoveY+5,146,80);
    glLineWidth(3);col3(.18f,.18f,.20f);ering(scx,stoveY+44,46,11,32);glLineWidth(1);
    /* gas pipe & knob */
    col3(.55f,.42f,.28f);quad(scx-8,stoveY-2,16,14);
    col3(.85f,.35f,.12f);quad(scx-14,stoveY-14,28,12);

    /* FLAME */
    for(int fi=0;fi<14;fi++){
        float fa=TAU*fi/14,fx=scx+cosf(fa)*38,fy=stoveY+44+sinf(fa)*10;
        float fh=11+7*sinf(flameDance*16+fi*.75f);
        glBegin(GL_TRIANGLES);
        col3(.08f,.25f,1);glVertex2f(fx-4,fy);glVertex2f(fx+4,fy);
        col3(.75f,.90f,1);glVertex2f(fx,fy+fh);
        glEnd();
    }

    /* ── PAN ── */
    float panCX=scx-8+sinf(T*.3f)*1.5f,panCY=stoveY+58;

    blend(1);col4(0,0,0,.35f);efill(panCX+3,panCY+7,72,24,32);blend(0);
    blend(1);col4(.72f,.58f,.08f,.82f+oilShimmer*.1f);efill(panCX,panCY+7,65,21,32);
    col4(.94f,.82f,.38f,.22f);efill(panCX-12,panCY+4,28,9,16);blend(0);

    glLineWidth(4.5f);col3(.22f,.22f,.24f);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=26;i++){float a=PI+PI*i/26;glVertex2f(panCX+cosf(a)*70,panCY+sinf(a)*28);}
    glEnd();
    glLineWidth(3);col3(.32f,.32f,.34f);ering(panCX,panCY,70,20,32);glLineWidth(1);
    /* handle pointing RIGHT toward board */
    col3(.38f,.28f,.16f);glLineWidth(7);
    glBegin(GL_LINES);glVertex2f(panCX+70,panCY+2);glVertex2f(panCX+145,panCY-14);glEnd();
    glLineWidth(1);

    /* ── POORI ── */
    float px=panCX-10+sinf(T*1.1f)*2.5f,py=panCY+pooriBobble*.75f;
    blend(1);col4(.38f,.18f,0,.28f);efill(px+2,py+5,38,14,32);blend(0);
    /* puffed dome */
    glBegin(GL_TRIANGLE_FAN);
    col3(.92f,.72f,.18f);glVertex2f(px,py-12);
    for(int i=0;i<=26;i++){float a=PI+PI*i/26;col3(.80f,.55f,.10f);glVertex2f(px+cosf(a)*32,py+sinf(a)*11-4);}
    glEnd();
    /* body */
    glBegin(GL_TRIANGLE_FAN);
    col3(.90f,.68f,.14f);glVertex2f(px,py-7);
    for(int i=0;i<=36;i++){float a=TAU*i/36,rr=1+.14f*sinf(a*4+T*4),hh=.5f+.5f*fabsf(sinf(a*3));col3(.88f-hh*.08f,.58f-hh*.06f,.10f);glVertex2f(px+cosf(a)*33*rr,py+sinf(a)*12*rr);}
    glEnd();
    col3(.42f,.20f,.04f);disk(px-9,py-6,4.5f,8);disk(px+13,py-4,3.8f,8);disk(px+4,py-10,3,6);disk(px-3,py+3,3.2f,6);
    blend(1);col4(1,.95f,.75f,.30f);efill(px-5,py-10,11,5,12);blend(0);

    /* oil & smoke particles */
    blend(1);
    for(int i=0;i<nOil;i++){OilP*p=&oilP[i];col4(p->r,p->g,p->b,p->life*.65f);disk(p->x,p->y,p->sz*p->life,6);}
    for(int i=0;i<nSmoke;i++){SmokeP*s=&smokeP[i];col4(.88f,.88f,.85f,s->life*.22f);disk(s->x,s->y,s->sz*(1.4f-s->life*.4f),10);}
    blend(0);

    /* heat haze */
    blend(1);
    for(int h=0;h<6;h++){float hx=panCX+(h-3)*18+sinf(T*2.8f+h)*5,hy=panCY+35+h*17;col4(.90f,.72f,.28f,.055f*(1-h/6.f));efill(hx,hy,11+h*2.5f,5.5f,12);}
    blend(0);

    /* ══ LADY SCIENTIST ══
       Feet at tableY+14, centred at x=298 (right-behind pan handle area) */
    float lx=298.f,lfoot=tableY+14.f,ls=1.20f;

    /* shadow */
    blend(1);col4(0,0,0,.22f);efill(lx,lfoot-4,22*ls,7*ls,12);blend(0);

    /* LEGS (dark navy saree lower) */
    col3(.08f,.06f,.28f);
    quad(lx-11*ls,lfoot,  9*ls,56*ls);
    quad(lx+2*ls, lfoot,  9*ls,56*ls);
    /* gold hem */
    col3(.80f,.64f,.10f);quad(lx-13*ls,lfoot+52*ls,26*ls,3*ls);

    /* TORSO - navy blue */
    col3(.10f,.13f,.52f);quad(lx-16*ls,lfoot+56*ls,32*ls,62*ls);

    /* saree pallu (maroon drape) */
    col3(.55f,.12f,.08f);
    glBegin(GL_QUADS);
    glVertex2f(lx+6*ls, lfoot+92*ls);glVertex2f(lx+16*ls,lfoot+72*ls);
    glVertex2f(lx+18*ls,lfoot+80*ls);glVertex2f(lx+8*ls, lfoot+104*ls);
    glEnd();
    /* gold left border */
    col3(.80f,.64f,.10f);quad(lx-18*ls,lfoot+56*ls,3*ls,62*ls);

    /* ID badge */
    col3(.68f,.72f,.90f);quad(lx-5*ls,lfoot+70*ls,10*ls,13*ls);
    col3(.08f,.08f,.35f);glBegin(GL_LINES);glVertex2f(lx,lfoot+118*ls);glVertex2f(lx,lfoot+83*ls);glEnd();

    /* RIGHT ARM — holding spatula into pan (angled left-down) */
    /* The pan is to her LEFT so right arm goes left and down */
    float ra=-PI*.60f+armSwing*PI/180.f;
    float rex=lx-26*ls, rey=lfoot+96*ls;  /* elbow */
    col3(.70f,.50f,.30f);
    {
        float px2=-sinf(ra)*4*ls,py2=cosf(ra)*4*ls;
        glBegin(GL_QUADS);
        glVertex2f(lx-14*ls+px2,lfoot+100*ls+py2); glVertex2f(lx-14*ls-px2,lfoot+100*ls-py2);
        glVertex2f(rex-px2,rey-py2); glVertex2f(rex+px2,rey+py2);
        glEnd();
    }
    float fa2=ra-.55f;
    float rwx=rex+cosf(fa2)*28*ls,rwy=rey+sinf(fa2)*28*ls;
    {
        float px2=-sinf(fa2)*3.5f*ls,py2=cosf(fa2)*3.5f*ls;
        glBegin(GL_QUADS);
        glVertex2f(rex+px2,rey+py2);glVertex2f(rex-px2,rey-py2);
        glVertex2f(rwx-px2,rwy-py2);glVertex2f(rwx+px2,rwy+py2);
        glEnd();
    }
    /* spatula */
    float sl=52.f,sex=rwx+cosf(fa2)*sl,sey=rwy+sinf(fa2)*sl;
    col3(.68f,.68f,.70f);glLineWidth(3);
    glBegin(GL_LINES);glVertex2f(rwx,rwy);glVertex2f(sex,sey);glEnd();glLineWidth(1);
    float sha2=fa2+PI*.5f;
    col3(.62f,.62f,.64f);
    glBegin(GL_QUADS);
    glVertex2f(sex+cosf(sha2)*12,sey+sinf(sha2)*12);
    glVertex2f(sex-cosf(sha2)*12,sey-sinf(sha2)*12);
    glVertex2f(sex-cosf(sha2)*12+cosf(fa2)*8,sey-sinf(sha2)*12+sinf(fa2)*8);
    glVertex2f(sex+cosf(sha2)*12+cosf(fa2)*8,sey+sinf(sha2)*12+sinf(fa2)*8);
    glEnd();

    /* LEFT ARM — pointing right toward board */
    float la=PI*.10f;
    float lex=lx+24*ls,ley=lfoot+100*ls;
    float lwx=lex+cosf(la-.3f)*30*ls,lwy=ley+sinf(la-.3f)*30*ls;
    col3(.70f,.50f,.30f);
    {
        float px2=-sinf(la)*3.5f*ls,py2=cosf(la)*3.5f*ls;
        glBegin(GL_QUADS);
        glVertex2f(lx+14*ls+px2,lfoot+102*ls+py2);glVertex2f(lx+14*ls-px2,lfoot+102*ls-py2);
        glVertex2f(lex-px2,ley-py2);glVertex2f(lex+px2,ley+py2);
        glEnd();
    }
    float la2f=la-.3f;
    {
        float px2=-sinf(la2f)*3.5f*ls,py2=cosf(la2f)*3.5f*ls;
        glBegin(GL_QUADS);
        glVertex2f(lex+px2,ley+py2);glVertex2f(lex-px2,ley-py2);
        glVertex2f(lwx-px2,lwy-py2);glVertex2f(lwx+px2,lwy+py2);
        glEnd();
    }
    col3(.68f,.48f,.28f);disk(lwx+cosf(la2f)*5*ls,lwy+sinf(la2f)*5*ls,4.5f*ls,8);

    /* NECK */
    col3(.70f,.50f,.30f);quad(lx-5*ls,lfoot+118*ls,10*ls,8*ls);

    /* HEAD */
    float hbob=sinf(T*2.2f)*2.f*ls;
    col3(.70f,.50f,.30f);disk(lx,lfoot+132*ls+hbob,16*ls,24);

    /* hair bun */
    col3(.07f,.04f,.01f);
    efill(lx,lfoot+140*ls+hbob,16*ls,10*ls,20);
    disk(lx+10*ls,lfoot+142*ls+hbob,7.5f*ls,14);

    /* bindi */
    col3(.88f,.08f,.08f);disk(lx,lfoot+133*ls+hbob,2.2f*ls,8);

    /* eyes */
    col3(.04f,.04f,.08f);
    disk(lx-5.5f*ls,lfoot+130*ls+hbob,2.5f*ls,8);
    disk(lx+5.5f*ls,lfoot+130*ls+hbob,2.5f*ls,8);
    col3(1,1,1);
    disk(lx-4.5f*ls,lfoot+130.5f*ls+hbob,.9f*ls,6);
    disk(lx+6.5f*ls,lfoot+130.5f*ls+hbob,.9f*ls,6);

    /* smile */
    col3(.58f,.22f,.12f);
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=8;i++){float a=PI+PI*i/8;glVertex2f(lx+cosf(a)*4.5f*ls,lfoot+123*ls+hbob+sinf(a)*2.8f*ls);}
    glEnd();

    /* earrings */
    col3(.82f,.65f,.05f);
    disk(lx-14*ls,lfoot+128*ls+hbob,2.8f*ls,8);
    disk(lx+14*ls,lfoot+128*ls+hbob,2.8f*ls,8);

    /* ── ROLLING PIN & DOUGH (left of stove) ── */
    float dpX=55,dpY=tableY+30;
    col3(.93f,.90f,.84f);disk(dpX,dpY,20,20);
    col3(.62f,.42f,.22f);quad(dpX-38,dpY+14,76,12);quad(dpX-46,dpY+11,12,18);quad(dpX+34,dpY+11,12,18);

    /* ── CHAI CUP ── */
    float cx=85,cy=tableY+16;
    col3(.55f,.42f,.28f);
    glBegin(GL_QUADS);glVertex2f(cx,cy);glVertex2f(cx+20,cy);glVertex2f(cx+16,cy+28);glVertex2f(cx+4,cy+28);glEnd();
    col3(.52f,.30f,.09f);quad(cx+2,cy+2,16,7);
    blend(1);
    for(int s=0;s<3;s++){col4(.88f,.88f,.85f,.28f-s*.07f);disk(cx+10+sinf(T*2+s*1.5f)*4,cy+30+s*13,4+s*2,8);}
    blend(0);

    /* ── MISSION DOCS ── */
    col3(.90f,.88f,.83f);quad(368,tableY+14,88,108);
    col3(.95f,.93f,.88f);quad(372,tableY+18,88,108);
    col3(.10f,.10f,.38f);
    txt(374,tableY+106,"MOM MISSION",GLUT_BITMAP_HELVETICA_10);
    txt(374,tableY+94,"FLIGHT PLAN",GLUT_BITMAP_HELVETICA_10);
    col3(.5f,.5f,.5f);
    for(int i=0;i<5;i++){glBegin(GL_LINES);glVertex2f(376,tableY+82-i*12);glVertex2f(450,tableY+82-i*12);glEnd();}
}

/* ══════════════════════════════════════════
   BACKGROUND SCIENTISTS
══════════════════════════════════════════ */
static void drawSci(float x,float foot,float sc2,float sr,float sg,float sb,float skr,float skg,float skb,int badge,int cross){
    blend(1);col4(0,0,0,.22f);efill(x,foot-4,20*sc2,7*sc2,12);blend(0);
    col3(sr*.4f+.08f,sg*.4f+.08f,sb*.4f+.12f);
    quad(x-9*sc2,foot,8*sc2,52*sc2);quad(x+1*sc2,foot,8*sc2,52*sc2);
    col3(sr,sg,sb);quad(x-14*sc2,foot+52*sc2,28*sc2,56*sc2);
    if(badge){col3(.65f,.70f,.88f);quad(x-5*sc2,foot+65*sc2,10*sc2,12*sc2);col3(.08f,.08f,.35f);glBegin(GL_LINES);glVertex2f(x,foot+108*sc2);glVertex2f(x,foot+77*sc2);glEnd();}
    col3(skr,skg,skb);
    if(cross){quad(x+12*sc2,foot+80*sc2,5*sc2,30*sc2);quad(x-17*sc2,foot+80*sc2,5*sc2,30*sc2);}
    else{quad(x+12*sc2,foot+82*sc2,5*sc2,28*sc2);quad(x-17*sc2,foot+82*sc2,5*sc2,28*sc2);}
    quad(x-4*sc2,foot+108*sc2,8*sc2,7*sc2);
    disk(x,foot+123*sc2,14*sc2,20);
    col3(.08f,.05f,.02f);efill(x,foot+130*sc2,14*sc2,9*sc2,18);
    col3(.04f,.04f,.08f);disk(x-5*sc2,foot+121*sc2,2*sc2,8);disk(x+5*sc2,foot+121*sc2,2*sc2,8);
}
static void drawAllSci(void){
    float f=342.f;
    /* left */
    drawSci(52, f,.80f,.62f,.12f,.18f,.70f,.50f,.30f,1,0);
    drawSci(100,f,.83f,.18f,.20f,.48f,.72f,.52f,.32f,1,0);
    /* right */
    drawSci(W-340,f,.84f,.72f,.70f,.66f,.70f,.50f,.30f,1,1);
    drawSci(W-278,f,.87f,.28f,.28f,.52f,.74f,.54f,.33f,1,0);
    drawSci(W-218,f,.80f,.50f,.48f,.44f,.68f,.48f,.30f,1,1);
}

/* ══════════════════════════════════════════
   HUD
══════════════════════════════════════════ */
static void drawHUD(void){
    blend(1);
    col4(0,0,0,.68f);quad(0,0,W,30);
    col3(1,.83f,.18f);txt(10,18,"THE POORI THEORY  |  ISRO MOM Mangalyaan Mission Control",GLUT_BITMAP_HELVETICA_12);
    char tb[80];sprintf(tb,"T+%.1fs  |  PERIGEE:421km  |  APOGEE:73800km",T*60);
    col3(.45f,.88f,.45f);txt(W-478,18,tb,GLUT_BITMAP_HELVETICA_12);
    col4(0,.02f,.10f,.72f);quad(0,H-30,W,30);
    col3(1,1,1);txt(10,H-17,"OpenGL C Animation — The Poori Theory | ISRO MOM Mangalyaan",GLUT_BITMAP_HELVETICA_12);
    if((int)(T*2)%2==0){col3(1,.1f,.1f);disk(W-28,H-16,6,10);col3(1,.9f,.9f);txt(W-52,H-19,"REC",GLUT_BITMAP_HELVETICA_10);}
    blend(0);
}

/* ══════════════════════════════════════════
   GLUT
══════════════════════════════════════════ */
static void display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    drawBG();
    drawMonitors();
    drawAllSci();
    drawCookingAndLady();
    drawBoard();
    drawHUD();
    glutSwapBuffers();
}
static void reshape(int w,int h){
    W=w;H=h?h:1;
    glViewport(0,0,W,H);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();gluOrtho2D(0,W,0,H);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
}
static void keyboard(unsigned char k,int x,int y){(void)x;(void)y;if(k==27||k=='q'||k=='Q')exit(0);}

int main(int argc,char**argv){
    srand(12345);
    initScene();
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(W,H);
    glutCreateWindow("The Poori Theory - ISRO MOM Animation");
    glClearColor(.04f,.06f,.14f,1);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16,update,0);
    printf("=========================================\n");
    printf("  THE POORI THEORY  -  OpenGL C  (v2)\n");
    printf("  Lady scientist is AT the stove now!\n");
    printf("  Press Q / ESC to quit\n");
    printf("=========================================\n");
    glutMainLoop();
    return 0;
}