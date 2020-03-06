#include "camera.h"

//for skybox

#include <stb_image.h>


float camX = 0.0, camY = 0.0, camZ = 5.0;
float camYaw = 0.0;
float camPitch = 0.0;

// to checkk if the mouse is in the window
#ifndef M_PI
#define M_PI 3.1415926535
#endif

void lockCamera() {
	if (camPitch > 90)
		camPitch = 90;// Camera will be locked once it reaches plus 90 in up direction
	if (camPitch < -90)
		camPitch = -90;
	if (camYaw < 0.0)
		camYaw += 360.0;//this is for horizontal rotation of the camera
	if (camYaw > 360.0)
		camYaw -= 360;
}

void moveCamera(float dist, float dir) {
	float rad = (camYaw + dir)*M_PI/180.0;
	camX -= sin(rad) * dist;
	camZ -= cos(rad) * dist;
}

void moveCameraUp(float dist, float dir) {
	float rad = (camPitch + dir) * M_PI / 180.0;
	camY += sin(rad) * dist;
}

void Control(float movevel, float mousevel, bool mi) {
	if (mi) {
		int MidX = 1366 / 2;
		int MidY = 768 / 2;
		SDL_ShowCursor(SDL_DISABLE);
		int tmpx, tmpy;
		SDL_GetMouseState(&tmpx, &tmpy);
		camYaw += mousevel * (MidX - tmpx);//move tmp cordinate in right x, will be subtracted , for left ,added
		camPitch += mousevel * (MidY - tmpy);
		lockCamera();
		SDL_WarpMouse(MidX,MidY);
		Uint8* state = SDL_GetKeyState(NULL);
		if (state[SDLK_w]) {
			if (camPitch != 90 && camPitch != -90)
				moveCamera(movevel, 0.0);
			moveCameraUp(movevel, 0.0);
		}
		else if (state[SDLK_s]) {
			if (camPitch != 90 && camPitch != -90)
				moveCamera(movevel,180.0);
			moveCameraUp(movevel, 180.0);
		}
		if (state[SDLK_a]) {
			moveCamera(movevel, 90.0);
		}
		else if (state[SDLK_d]) {
			moveCamera(movevel, 270.0);
		}
	}
	glRotatef(-camPitch, 1.0, 0.0, 0.0);
	glRotatef(-camYaw,0.0,1.0,0.0);
}

void UpdateCamera() {
	glTranslatef(-camX,-camY,-camZ);
}

//For Skybox

enum {SKY_LEFT=0,SKY_BACK,SKY_RIGHT,SKY_FRONT,SKY_TOP,SKY_BOTTOM};
//enum is used to assign names to integral constants, the names make a program easy to read and maintain
unsigned int skybox[6];//for loading all of the necessary textures ids in the skybox faces

void initskybox()
{
	skybox[SKY_LEFT] = loadTexture("left.png"); //uses loadtexture function at the bottom to return an id for the texture
	skybox[SKY_BACK] = loadTexture("back.png");
	skybox[SKY_RIGHT] = loadTexture("right.png");
	skybox[SKY_FRONT] = loadTexture("front.png");
	skybox[SKY_TOP] = loadTexture("up.png");
	skybox[SKY_BOTTOM] = loadTexture("down.png"); //do for all faces
}

void killskybox()//Delete the skybox and free memory
{
	glDeleteTextures(6,&skybox[0]);
}

void drawSkybox(float size)
{
    bool b1 = glIsEnabled(GL_TEXTURE_2D);     //new, we left the textures turned on, if it was turned on
    glDisable(GL_LIGHTING); //turn off lighting, when making the skybox
    glDisable(GL_DEPTH_TEST);       //turn off depth texting
    glEnable(GL_TEXTURE_2D);        //and turn on texturing
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_BACK]);  //use the texture we want
    glBegin(GL_QUADS);      //and draw a face
            //back face
    glTexCoord2f(0, 0);      //use the correct texture coordinate
    glVertex3f(size / 2, size / 2, size / 2);       //and a vertex
    glTexCoord2f(1, 0);      //and repeat it...
    glVertex3f(-size / 2, size / 2, size / 2);
    glTexCoord2f(1, 1);
    glVertex3f(-size / 2, -size / 2, size / 2);
    glTexCoord2f(0, 1);
    glVertex3f(size / 2, -size / 2, size / 2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_LEFT]);
    glBegin(GL_QUADS);
    //left face
    glTexCoord2f(0, 0);
    glVertex3f(-size / 2, size / 2, size / 2);
    glTexCoord2f(1, 0);
    glVertex3f(-size / 2, size / 2, -size / 2);
    glTexCoord2f(1, 1);
    glVertex3f(-size / 2, -size / 2, -size / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-size / 2, -size / 2, size / 2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_FRONT]);
    glBegin(GL_QUADS);
    //front face
    glTexCoord2f(1, 0);
    glVertex3f(size / 2, size / 2, -size / 2);
    glTexCoord2f(0, 0);
    glVertex3f(-size / 2, size / 2, -size / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-size / 2, -size / 2, -size / 2);
    glTexCoord2f(1, 1);
    glVertex3f(size / 2, -size / 2, -size / 2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_RIGHT]);
    glBegin(GL_QUADS);
    //right face
    glTexCoord2f(0, 0);
    glVertex3f(size / 2, size / 2, -size / 2);
    glTexCoord2f(1, 0);
    glVertex3f(size / 2, size / 2, size / 2);
    glTexCoord2f(1, 1);
    glVertex3f(size / 2, -size / 2, size / 2);
    glTexCoord2f(0, 1);
    glVertex3f(size / 2, -size / 2, -size / 2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_TOP]);
    glBegin(GL_QUADS);                      //top face
    glTexCoord2f(1, 0);
    glVertex3f(size / 2, size / 2, size / 2);
    glTexCoord2f(0, 0);
    glVertex3f(-size / 2, size / 2, size / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-size / 2, size / 2, -size / 2);
    glTexCoord2f(1, 1);
    glVertex3f(size / 2, size / 2, -size / 2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, skybox[SKY_BOTTOM]);
    glBegin(GL_QUADS);
    //bottom face
    glTexCoord2f(1, 1);
    glVertex3f(size / 2, -size / 2, size / 2);
    glTexCoord2f(0, 1);
    glVertex3f(-size / 2, -size / 2, size / 2);
    glTexCoord2f(0, 0);
    glVertex3f(-size / 2, -size / 2, -size / 2);
    glTexCoord2f(1, 0);
    glVertex3f(size / 2, -size / 2, -size / 2);
    glEnd();
    glEnable(GL_LIGHTING);  //turn everything back, which we turned on, and turn everything off, which we have turned on.
    glEnable(GL_DEPTH_TEST);
    if (!b1)
        glDisable(GL_TEXTURE_2D);
}

unsigned int loadTexture(const char* filename)
{
	//nothing new in here
	unsigned int num;
	int w, h, comp;

	//SDL_Surface* img = SDL_LoadBMP(filename);
	unsigned char* img = stbi_load(filename, &w, &h, &comp, STBI_rgb_alpha);
	if (img == nullptr)
		throw(std::string("Failed to load texture"));
	glGenTextures(1, &num);
	glBindTexture(GL_TEXTURE_2D, num);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); //pixel at the end of image will be repeated to the next so no tearing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); //same as above

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (comp == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	else if (comp == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(img);
	//SDL_FreeSurface(img);
	
	return num;
}