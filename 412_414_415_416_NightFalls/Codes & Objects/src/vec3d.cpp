#include <cmath>
#include "vec3d.h"

const double PI = 3.1415926535897932384626433832795;

//--------Constructors
//Contructor for vector class given (x, y, z)(null vector by default)
vec3d :: vec3d(float _x, float _y, float _z)
{
	x=_x;
	y=_y;
	z=_z;
}

//Constructor to initialize a vector with a vector and magnitude (the new vector will share the direction of given vector)
vec3d :: vec3d(vec3d vec, float mag)
{
	vec.normalize();
	if(mag == 0)
		throw "invalid vector";
	x=mag*vec.x;
	y=mag*vec.y;
	z=mag*vec.z;
}


//---------Basic arithmetic operations
//addition
vec3d vec3d :: operator+(const vec3d& vec) const
{
	return vec3d(x+vec.x, y+vec.y, z+vec.z);
}

//Subtraction
vec3d vec3d :: operator-(const vec3d& vec) const
{
	return vec3d(x-vec.x, y-vec.y, z-vec.z);
}

//--------Scalar operations
//Multiplication
vec3d vec3d :: operator*(float val) const
{
	return vec3d(x * val, y * val, z*val);
}

//Scalar division
vec3d vec3d :: operator/(float val) const
{
	return vec3d(x / val, y / val, z/val);
}


//--------Vector Product
//Dot product
float vec3d :: operator*(const vec3d& vec) const
{
	return x * vec.x + y * vec.y + z * vec.z;
}

//Cross product
vec3d vec3d :: cross(const vec3d& rhs) const
{
	vec3d temp(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
	return temp;
}

//--------Other operations
//to return magnitude of vector
float vec3d :: mag() const
{
	return sqrt(x*x + y*y + z*z);
}

//to return unit vector in the direction of vector
vec3d vec3d :: unit() const
{
	float m = mag();
	if(m == 0)
		throw "invalid vector";
	return vec3d(x/m, y/m, z/m);
}

//to normalize the vector
void vec3d :: normalize()
{
	float m = mag();
	if(m == 0)
		throw "invalid vector";
	x = x/m;
	y = y/m;
	z = z/m;
}

//to return the inverted vector
vec3d vec3d :: inverse() const
{
	return (*this)*-1;
}

//to return projection vector
vec3d vec3d :: proj(const vec3d& vec) const
{
	float m = vec.mag();
	if(m == 0)
		throw "invalid vector";
	return vec.unit()*(operator*(vec)/m);
}

//to return the angle between vectors in radians
double vec3d :: angle(const vec3d& vec) const
{
	float this_mag = mag();
	float vec_mag = vec.mag();
	if(this_mag == 0 || vec_mag == 0)
		throw "invalid vector";
	return acos(operator*(vec)/(this_mag*vec_mag));
}

//to rotate a vector by angle radians
void vec3d :: rotate(const vec3d& vec, double angle)
{
	//Setup rotation matrix
	double rotationMatrix[4][4];

	double L = (vec.x*vec.x + vec.y * vec.y + vec.z * vec.z);
    double u2 = vec.x * vec.x;
    double v2 = vec.y * vec.y;
    double w2 = vec.z * vec.z; 
 
    rotationMatrix[0][0] = (u2 + (v2 + w2) * cos(angle)) / L;
    rotationMatrix[0][1] = (vec.x * vec.y * (1 - cos(angle)) - vec.z * sqrt(L) * sin(angle)) / L;
    rotationMatrix[0][2] = (vec.x * vec.z * (1 - cos(angle)) + vec.y * sqrt(L) * sin(angle)) / L;
    rotationMatrix[0][3] = 0.0; 
 
    rotationMatrix[1][0] = (vec.x * vec.y * (1 - cos(angle)) + vec.z * sqrt(L) * sin(angle)) / L;
    rotationMatrix[1][1] = (v2 + (u2 + w2) * cos(angle)) / L;
    rotationMatrix[1][2] = (vec.y * vec.z * (1 - cos(angle)) - vec.x * sqrt(L) * sin(angle)) / L;
    rotationMatrix[1][3] = 0.0; 
 
    rotationMatrix[2][0] = (vec.x * vec.z * (1 - cos(angle)) - vec.y * sqrt(L) * sin(angle)) / L;
    rotationMatrix[2][1] = (vec.y * vec.z * (1 - cos(angle)) + vec.x * sqrt(L) * sin(angle)) / L;
    rotationMatrix[2][2] = (w2 + (u2 + v2) * cos(angle)) / L;
    rotationMatrix[2][3] = 0.0; 
 
    rotationMatrix[3][0] = 0.0;
    rotationMatrix[3][1] = 0.0;
    rotationMatrix[3][2] = 0.0;
    rotationMatrix[3][3] = 1.0;

	double inputMatrix[4][1] = {0.0, 0.0, 0.0};
	double outputMatrix[4][1] = {0.0, 0.0, 0.0, 0.0};

	inputMatrix[0][0] = x;
    inputMatrix[1][0] = y;
    inputMatrix[2][0] = z;
    inputMatrix[3][0] = 1.0;

	for(int i = 0; i < 4; i++ ){
        for(int j = 0; j < 1; j++){
            outputMatrix[i][j] = 0;
            for(int k = 0; k < 4; k++){
                outputMatrix[i][j] += rotationMatrix[i][k] * inputMatrix[k][j];
            }
        }
    }

	x = (float)outputMatrix[0][0];
	y = (float)outputMatrix[1][0];
	z = (float)outputMatrix[2][0];

}

//friend stream insertion and extraction
ostream& operator<<(ostream& out, const vec3d& vec)
{
	out << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
	return out;
}