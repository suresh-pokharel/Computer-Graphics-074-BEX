#include <Windows.h>
#include <gl/glut.h>

#include "particle.h"

//This file is for one single particle, this is the basis for Particle_system, don't delete Jbis

particle::particle(float _mass, vec3d _velocity, vec3d _position) : mass(_mass), velocity(_velocity), position(_position){

}

//function to advance state by time t in ms
void particle::advance(float t, vec3d force)
{
	//calculating acceleration
	vec3d acc = force / mass;

	//calculating velocity
	velocity = velocity + acc*(t/1000.0);
	
	if(velocity.mag() >= MAX_VELOCITY)
		velocity = vec3d(velocity.unit(), MAX_VELOCITY);

	//changing position
	position = position+velocity*(t/1000.0);

	/*if(position.x <= -LENGTH)
		position.x = LENGTH;
	else if(position.x >= LENGTH)
		position.x = LENGTH;*/

	if(position.y <= -LENGTH)
		position.y = LENGTH;
	else if(position.y >= LENGTH)
		position.y = -LENGTH;

	if(position.z <= -LENGTH)
		position.z = LENGTH;
	else if(position.z >= LENGTH)
		position.z = -LENGTH;
}


particle::~particle(void){
}

//Function to get position
vec3d particle :: get_pos()
{
	return position;
}

