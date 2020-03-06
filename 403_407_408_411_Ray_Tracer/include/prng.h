#ifndef PRNG_HEADER
#define PRNG_HEADER

#ifdef OS_LINUX_CPP
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
#else
  #include <windows.h>
#endif 
/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

/* This is xoshiro256+ 1.0, our best and fastest generator for floating-point
   numbers. We suggest to use its upper bits for floating-point
   generation, as it is slightly faster than xoshiro256++/xoshiro256**. It
   passes all tests we are aware of except for the lowest three bits,
   which might fail linearity tests (and just those), so if low linear
   complexity is not considered an issue (as it is usually the case) it
   can be used to generate 64-bit outputs, too.

   We suggest to use a sign test to extract a random Boolean value, and
   right shifts to extract subsets of bits.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */


static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}


static uint64_t PRNG_Seed[4];

uint64_t PRNG_Next(void) {
	const uint64_t result = PRNG_Seed[0] + PRNG_Seed[3];

	const uint64_t t = PRNG_Seed[1] << 17;

	PRNG_Seed[2] ^= PRNG_Seed[0];
	PRNG_Seed[3] ^= PRNG_Seed[1];
	PRNG_Seed[1] ^= PRNG_Seed[2];
	PRNG_Seed[0] ^= PRNG_Seed[3];

	PRNG_Seed[2] ^= t;

	PRNG_Seed[3] = rotl(PRNG_Seed[3], 45);

	return result;
}

struct splitmix64_state {
	uint64_t s;
};

uint64_t splitmix64(struct splitmix64_state *state) {
	uint64_t result = state->s;

	state->s = result + 0x9E3779B97f4A7C15;
	result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
	result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
	return result ^ (result >> 31);
}

void prng_seed( ){
  uint8_t buff[8];
#ifdef OS_LINUX_CPP
  int fd = open("/dev/urandom",O_RDONLY);
  assert( fd > 0 );
  if ( read( fd, buff, 8 ) < 0 ){
    fprintf(stderr,"Error Occured!\n");
    return;
  }
  close( fd );
#elif defined(OS_WINDOWS_CPP)
  LARGE_INTEGER large;
  QueryPerformanceCounter( &large );
  memcpy( (void *)buff, (void *)&large, sizeof( buff ) );
#endif 
  struct splitmix64_state state = { *(uint64_t *)&buff };
  uint64_t s0 = splitmix64( &state );
  uint64_t s1 = splitmix64( &state );
  uint64_t s2 = splitmix64( &state );
  uint64_t s3 = splitmix64( &state );

  PRNG_Seed[0] = s0; 
  PRNG_Seed[1] = s1; 
  PRNG_Seed[2] = s2; 
  PRNG_Seed[3] = s3; 
  return;  
}

float prng_float(){
  uint64_t x = PRNG_Next( );
  return (x >> 11) * 0x1.0p-53;
}

uint64_t prng_uint64(){
  return PRNG_Next();
}
#endif
