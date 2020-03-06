#pragma once

#include "vec3d.h"
#include "constants.h"
#include "cloud.h"



class particle
{	
	float mass;
	vec3d velocity;
	vec3d position;
	vec3d position1;
	vec3d postion2;

public:
	particle(float = rand() % (MAX_MASS-MIN_MASS) + MIN_MASS,
			vec3d = vec3d(0.0,-0.8,0.0),
			vec3d = vec3d((1-2*rand_float())*LENGTH, (1-2*rand_float())*LENGTH, (1-2*rand_float())*LENGTH));

	//Function to advance state of particle by time t in ms and force in given direction
	void advance(float, vec3d = vec3d(0.0,1.0,0.0));

	//Function to get position
	vec3d get_pos();

	//Function to draw a particle
	void draw(int);

	~particle(void);
};

