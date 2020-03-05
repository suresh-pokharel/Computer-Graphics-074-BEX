#ifndef RAY_TEXTURE_H
#define RAY_TEXTURE_H

#include "prng.h"
#include "common.h"
#include "HandmadeMath.h"
#include <float.h>
struct Texture;

struct Perlin {
  Arena *store;
  v3 *random_v3;
  int *permx;
  int *permy;
  int *permz;
  int n;
  float scale;
};


typedef v3 ( *ColorFunc )(
    Texture *texture,
    float u,
    float v,
    const v3 &p );

float perlin_get_noise(Perlin *p, const v3 &point );
float perlin_get_turb(
    Perlin *perlin,
    const v3 &p,
    int depth );

struct Texture {
  enum TextureType {
    NONE,
    PLAIN_COLOR,
    CHECKER,
    PERLIN_NOISE,
    MARBLE
  };
  
  TextureType type;
  ColorFunc get_color;
  union {
    v3 color;
    struct {
      v3 checker_color[2];
      float freq;
    };

    struct {
      Perlin *perlin;
      v3 marble_color;
    };

  };

  Texture (): type(NONE), get_color(NULL){}
};

v3 get_plain_tex_color( Texture *tex, float u, float v, const v3 &p ){
  return tex->color;
}

v3 get_checker_tex_color( Texture *tex, float u, float v, const v3 &p ){
  float f = tex->freq;
  int x = ( sin(p.X*f)  * sin(p.Y*f) * sin( p.Z *f ) ) < 0;
  return tex->checker_color[x];
}

v3 get_perlin_color( Texture *tex, float u, float v, const v3 &p ){
  Perlin *per = tex->perlin;
  float c = perlin_get_turb( per, per->scale * p,7 );
  return c * v3{ 1.0f, 1.0f, 1.0f };
}

v3 get_marble_color( Texture *tex, float u, float v, const v3 &p ){
  Perlin *per = tex->perlin;
  float t= 10*perlin_get_turb( per,p,7 );
  float t1 = 1 + sin( per->scale * p.Z + t );
  return 0.5 * t1 * tex->marble_color;
}


v3 texture_get_color( Texture *tex, float u, float v, const v3 &p ){
  switch ( tex->type ){
    case Texture::PLAIN_COLOR:
      return tex->color;
      break;
    case Texture::NONE:
    default:
      return v3{0,0,0};
      break;
  }
}

Texture create_texture_plain( v3 color ){
  Texture tex;
  tex.type = Texture::PLAIN_COLOR;
  tex.get_color = get_plain_tex_color;
  tex.color = color;
  return tex;
}

Texture create_texture_checker( v3 c0, v3 c1 ,float freq ){
  Texture tex;
  tex.type = Texture::CHECKER;
  tex.get_color = get_checker_tex_color;
  tex.checker_color[0] = c0;
  tex.checker_color[1] = c1;
  tex.freq = freq;
  return tex;
}


float *random_float_array( float *mem, int n ){
  for ( int i = 0; i < n ; i++ ){
    mem[i] = prng_float();
  }
  return mem;
}

v3 *random_v3_array( v3 *mem, int n ){
  for ( int i = 0; i < n ; i++ ){
    mem[i] = HMM_NormalizeVec3( v3{
                  -1 + 2.0f *prng_float(),
                  -1 + 2.0f *prng_float(),
                  -1 + 2.0f *prng_float()
             });
  }
  return mem;
}

template <typename T>
void permute_array( T *list, int n ){
  for ( int i = n-1; i >= 0 ; i-- ){
    // Choose a random index between 0 and -1
    int index = (int)( prng_float() * (i+1) );
    // swap the value at current index with the value at random index
    T temp = list[i];
    list[i] = list[ index ];
    list[index] =temp;
  }
}


Perlin create_perlin( Arena *arena, float scale, int n){
  Perlin p;
  p.store = arena;
  p.scale = scale;
  p.n = n;

  p.random_v3 = random_v3_array(
      (v3 *)arena_alloc( p.store, sizeof(v3)* p.n,8  ),
      p.n 
  );
  p.permx = (int *)arena_alloc( p.store, sizeof(int)*p.n,8 );
  p.permy = (int *)arena_alloc( p.store, sizeof(int)*p.n,8 );
  p.permz = (int *)arena_alloc( p.store, sizeof(int)*p.n,8 );

  for ( int i = 0; i < p.n; i++ ){
    p.permx[i] = i;
    p.permy[i] = i;
    p.permz[i] = i;
  }

  permute_array( p.permx, p.n );
  permute_array( p.permy, p.n );
  permute_array( p.permz, p.n );
  return p;
}

float perlin_gradient( int hash,const v3 &p ){
  switch ( hash & 0xf ){
    case  0: return  p.X + p.Y; // (1,1,0) 
    case  1: return -p.X + p.Y; // (-1,1,0) 
    case  2: return  p.X - p.Y; // (1,-1,0) 
    case  3: return -p.X - p.Y; // (-1,-1,0) 
    case  4: return  p.X + p.Z; // (1,0,1) 
    case  5: return -p.X + p.Z; // (-1,0,1) 
    case  6: return  p.X - p.Z; // (1,0,-1) 
    case  7: return -p.X - p.Z; // (-1,0,-1) 
    case  8: return  p.Y + p.Z; // (0,1,1), 
    case  9: return -p.Y + p.Z; // (0,-1,1), 
    case 10: return  p.Y - p.Z; // (0,1,-1), 
    case 11: return -p.Y - p.Z; // (0,-1,-1) 
    case 12: return  p.Y + p.X; // (1,1,0) 
    case 13: return -p.X + p.Y; // (-1,1,0) 
    case 14: return -p.Y + p.Z; // (0,-1,1) 
    case 15: return -p.Y - p.Z; // (0,-1,-1)
    default: return 0.0f;
  }
}

float perlin_get_noise(Perlin *perlin, const v3 &p){
  v3 t = {
    p.X - Float2Int( p.X ),
    p.Y - Float2Int( p.Y ),
    p.Z - Float2Int( p.Z )
  };
  
  v3 twos = { 2.0f, 2.0f, 2.0f };
  v3 threes= { 3.0f, 3.0f, 3.0f };
  v3 t1= t*t*(threes-twos*t);
	int start[3]= {
   	Float2Int( p.X ) & (255),
   	Float2Int( p.Y ) & (255),
   	Float2Int( p.Z ) & (255)
	};

  float arr[8];
  float *s = arr;
	for ( int i = 0; i < 2; i++ ){
    int p2 = ( start[2] + i  ) & ( 255 );

		for ( int j = 0; j < 2; j++ ){
      int p1 = ( start[1] + j  ) & ( 255 );

      for ( int k = 0; k < 2; k++ ){
        int p0 = ( start[0] + k  ) & ( 255 );
#if 0
         v3 v0 = perlin->random_v3[
          perlin->permx[ p0 ] ^
          perlin->permy[ p1 ] ^
          perlin->permz[ p2 ]
         ];
         *s = ( HMM_DotVec3( v0, v1 ) );
#else
         v3 v1 = v3{ t.X - k, t.Y -j ,t.Z - i };
         int index = perlin->permx[ p0 ] ^
                    perlin->permy[ p1 ] ^ perlin->permz[ p2 ];
         *s = perlin_gradient( index, v1 );
#endif
         s++;
      }
    }
	}

  int n= 8;
  for ( int i = 0; i < 3 ; i++ ){
    for ( int j = 0; j < n; j+=2 ){
      arr[j>>1] = HMM_Lerp( arr[j], t1[i],arr[j+1]); 
    }
    n >>= 1;
  }

#if 0
  return (arr[0] + 1)/2.0f;
#else
  return arr[0]; 
#endif

}

float perlin_get_turb(
    Perlin *perlin,
    const v3 &p,
    int depth )
{
  float freq = 1.0f;
  float weight = 1.0f;
  float sum = 0.0f;
  for ( int i = 0; i < depth; i++ ){
    sum += weight * perlin_get_noise( perlin, freq * p );
    weight *= 0.5f;
    freq *= 2;
  }
  return fabs( sum );
}

Texture create_texture_perlin( Perlin *p ){
  Texture t;
  t.type = Texture::PERLIN_NOISE;
  t.get_color = get_perlin_color;
  t.perlin = p;
  return t;
}
Texture create_texture_marble( Perlin *p,v3 color ){
  Texture t;
  t.type = Texture::MARBLE;
  t.get_color = get_marble_color;
  t.perlin = p;
  t.marble_color = color;
  return t;
}

#endif
