#include "particle_system.h"
#include <math.h>
#include <stb_image.h>
#include <GL/freeglut.h>

particle_system::particle_system(int n)
{
	if (n > MAX_PARTICLES)
		n = MAX_PARTICLES;

	for (int i = 0; i < n; i++)
	{
		particle temp;
		particles.push_back(temp);
	}
}




//Function to advance state of particle system by time t in ms
void particle_system::advance(float time)
{
	vector<particle>::iterator it;
	for (it = particles.begin(); it != particles.end(); it++)
	{
		vec3d force = vec3d((gravity_point - it->get_pos()).unit(), FORCE_MAG);
		it->advance(time, force);
	}
}

//Function to set gravity point
void particle_system::set_gravity(vec3d gravity) {
	gravity_point = (0.0,-1.0,0.0);
}

//Function to add particles
bool particle_system::add_particles(int num)
{
	int i;
	for (i = 0; i < num && particles.size() < MAX_PARTICLES; i++)
	{
		particle p;
		particles.push_back(p);
	}
	return (i >= num);
}

//Function to delete particles(least is 0)
bool particle_system::delete_particles(int num)
{
	int i;
	for (i = 0; i < num && particles.size() > 0; i++)
	{
		particles.pop_back();
	}
	

	return (i >= num);
}

//Function to draw a particle
void particle_system::draw()
{
	//unsigned int text;
	//text = loadTexture();
	//glBindTexture(GL_TEXTURE_2D, text);
	vector<particle>::iterator it;
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_ONE, GL_SRC_COLOR);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	
	for (it = particles.begin(); it != particles.end(); it++) {
		
		vec3d pos = it->get_pos();
		float k = (gravity_point - pos).mag() / (1.5 * LENGTH);
		float x = pos.x;
		float y = pos.y;
		float z = pos.z;
		
		glColor4f(1, k, 0, 0.5);
		/*glBegin(GL_QUADS);
		
		glTexCoord2f(0, 0);
		glVertex3f(x, y, z);
		glTexCoord2f(1, 0);
		glVertex3f(x+0.1, y, z);
		glTexCoord2f(1, 1);
		glVertex3f(x+0.1, y+0.1, z);
		glTexCoord2f(0, 1);
		glVertex3f(x, y+0.1, z);
		glEnd();*/
		glBegin(GL_LINES);
		glVertex3f(x, y, z);
		glVertex3f(x, y, z+0.3);
		glEnd();
		/*glBegin(GL_POINTS);
		glVertex3f(x, y, z);
		
		glEnd();*/
		
	}
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDeleteTextures(1,&text);
}

/*unsigned int particle_system::loadTexture()
{
	//nothing new in here
	unsigned int num;
	int w, h, comp;

	//SDL_Surface* img = SDL_LoadBMP(filename);
	unsigned char* img = stbi_load("snow.png", &w, &h, &comp, STBI_rgb_alpha);
	if (img == nullptr)
		throw(std::string("Failed to load texture"));
	glGenTextures(1, &num);
	glBindTexture(GL_TEXTURE_2D, num);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); //pixel at the end of image will be repeated to the next so no tearing
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); //same as above

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(img);
	//SDL_FreeSurface(img);

	return num;
}*/

particle_system::~particle_system(void)
{
}
