#include<iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include "objloader.h"
#include <time.h>

#include <stdlib.h>
#include "camera.h"
#include <stb_image.h>
#include "particle.h"
#include "vec3f.h"

#include "particle_system.h"
#include "constants.h"
#include "cloud.h"

const float PI = 3.1415926535f;

using namespace std;

#undef main

bool mousein = false;
int cube;
int cube1;
int cube2;

objloader obj;  //create an instance of the objloader
particle_system p(3000);

void init()
{
    glClearColor(0.5, 0.5, 0.5, 0.3);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1920 / 1080, 1.0, 500.0); //GLdouble field of view (angle) in y,
        //GLdouble aspect ratio,
        //GLdouble zNear, the distance from the viewer to the near clipping plane,
        //GLdouble zFar, the distance from the viewer to the far clipping plane
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glPointSize(03);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    cube = obj.load("Scene.obj");      //load it
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float col[] = { 0.5,0.5,0.5,0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, col);
    initskybox();
    
}
void init1()
{
    glLoadIdentity();
    
    gluPerspective(55, 1920 / 1080, 1.0, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glTranslatef(0.0,-20.0,0.0);
    cube1 = obj.load("meteor.obj"); 
    cube2 = obj.load("meteor1.obj");
    //load it
    glEnable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    float pos2[] = { 0.0,10.0,2.0,1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, pos2);
    float col[] = { 1.0,1.0,1.0,1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, col);
    
    float col1[] = {0.5,0.5,0.5,0.3};
    glLightfv(GL_LIGHT1, GL_AMBIENT, col1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, col1);
    float col2[] = { 0.0,0.0,0.0,0.1 };
    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT1, GL_AMBIENT, col2);
    float pos3[] = {10.0,0.0,0.0,0.0};
    glLightfv(GL_LIGHT3,GL_POSITION,pos3);
    float col3[] = {0.4,0.4,0.4,0.3};
    glLightfv(GL_LIGHT3,GL_AMBIENT,col3);
    glEnable(GL_LIGHT3);

}


float x=0;

void display1()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    glLoadIdentity();
    Control(0.2, 0.2, mousein);
    drawSkybox(500);
    UpdateCamera();
    glPushMatrix();
    glRotatef(45,0.0,1.0,0.0);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR); //Enable this for foggy light but must disable blend at particle_system then
    //glBlendFunc(GL_ONE, GL_SRC_COLOR);
    glCallList(cube);
    
    glPopMatrix();
    float pos[] = { 10.0,1.0,-2.0,1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT0);
    float pos2[] = { -1.0,1.0,2.0,1.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, pos2);
    float col[] = { 1.0,1.0,1.0,1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, col);
    glEnable(GL_LIGHT1);
    glDisable(GL_LIGHT0);
    glPushMatrix();
    p.advance(DELTA);
    p.draw();
    glPopMatrix();
    glTranslatef(-3.0, 20.0, -40.0);
    glDisable(GL_BLEND);
    glCallList(cube2);
    glEnable(GL_BLEND);
    x += 0.05;
    glPushMatrix();
    glTranslated(1.5-x,0.0,0.0);
    glRotated(50, 0.0, 1.0, 0.0);
    glScalef(2.0,2.0,2.0);
    glCallList(cube1);
    glPopMatrix();
    glPushMatrix();
    glTranslated(1.5 +x, 0.0, 0.0);
    glRotated(50, 0.0, 1.0, 0.0);
    glScalef(2.0, 2.0, 2.0);
    glCallList(cube1);
    glPopMatrix();
    glPushMatrix();
    glTranslated(1.5, 0.0, 0.0+x);
    glRotated(50, 0.0, 1.0, 0.0);
    glScalef(2.0, 2.0, 2.0);
    glCallList(cube1);
    glPopMatrix();
}

int main(int argc, char** argv)
{
    
    
    srand(time(0));
    p.set_gravity(1);
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,2); //Multisampling Anti Aliasing
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,16);

    SDL_Surface* screen = SDL_SetVideoMode(840, 600, 32, SDL_HWSURFACE | SDL_OPENGL); //Creating window

    bool running = true;
    Uint32 start;
    SDL_Event event;
    init();
    init1();
    while (running)
    {
        start = SDL_GetTicks();
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mousein = true;
                SDL_ShowCursor(SDL_DISABLE);
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_p) {
                    mousein = false;
                    SDL_ShowCursor(SDL_ENABLE);
                    break;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    break;

                }
            }
           
        }
       display1();
        SDL_GL_SwapBuffers();
        if (1000 / 30 > (SDL_GetTicks() - start))
            SDL_Delay(1000 / 30 - (SDL_GetTicks() - start));
    }
    SDL_Quit();
    killskybox();
    return 0;
}

