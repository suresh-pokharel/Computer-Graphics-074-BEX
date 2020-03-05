#include <stdio.h>
#include <inttypes.h>
#define OS_LINUX_CPP
#define HANDMADE_MATH_IMPLEMENTATION
#include <float.h>
#include "common.h"
#include "../include/prng.h"
#include "../include/HandmadeMath.h"
#include "../include/primitives.h"
#include <cstring>

int less_than( void *x ){
  return *(int *)x < 10;
}

void print_m4( m4 &m ){
  for ( int i = 0; i < 4; i++ ){
    for (int j = 0; j < 4; j++ ){
      fprintf( stdout, "%f ", m[i][j]);
    }
    fprintf( stdout, "\n" );
  }
}

int main( void ){
  for ( int i = 0; i < 26; i++ ){
    fprintf( stdout, "GET_KEY_STATE(%c)\n",'A'+i );

  }
  return 0;
}
