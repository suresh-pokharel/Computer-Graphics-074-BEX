#include <stdlib.h>
#include <time.h>
#include <iostream>

using namespace std;

#include "cloud.h"

float rand_float()
{
	float value = rand()/float(RAND_MAX);
	return value;
}