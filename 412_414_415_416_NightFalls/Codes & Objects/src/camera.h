#pragma once
#include <SDL.h>
#include <SDL_opengl.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <iostream>

#ifndef CAMERA_H
#define CAMERA_H

void drawCube(float size);

void lockCamera();
void moveCamera(float, float);
void moveCameraUp(float, float);
void Control(float, float, bool);
void UpdateCamera();
//for skybox
void drawSkybox(float size);
void initskybox();
void killskybox();
unsigned int loadTexture(const char*);



#endif