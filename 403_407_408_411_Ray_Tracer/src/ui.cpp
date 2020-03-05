#define OS_LINUX_CPP
#define HANDMADE_MATH_IMPLEMENTATION
#include <stdio.h>
#include <cstdlib>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "common.h"
#include <cassert>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "HandmadeMath.h"
#include "ui_primitives.h"
#include "ui_objects.h"
#include "ray_data.h"
#include "prng.h"

typedef GLuint guint;

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

static uint ScreenWidth = 1366;
static uint ScreenHeight = 768;

#define HMM_MAT4_PTR(x) ( &( x ).Elements[0][0] )
#define HMM_MAT4P_PTR(x) (&( ( x )->Elements[0][0]  ))
#define ENABLE_GL_DEBUG_PRINT 1
#define MS_TO_SEC(x) ( (x) * 1.0e-3f )

static uint *Quad_elem_indices;
static uint quad_elem_buffer_index;
static uint *Line_elem_indices;

static float CubeVertices[] = {
  // front
  0.5f, 0.5f, 0.5f,
  -0.5f, 0.5f, 0.5f,
  -0.5f, -0.5f, 0.5f,
  0.5f, -0.5f, 0.5f,

  //back
  0.5f, 0.5f, -0.5f,
  -0.5f, 0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,

  // right
  0.5f, 0.5f, 0.5f,
  0.5f, -0.5f, 0.5f,
  0.5f, -0.5f, -0.5f,
  0.5f, 0.5f, -0.5f,

  //left
  -0.5f, 0.5f, 0.5f,
  -0.5f, -0.5f, 0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f, 0.5f, -0.5f,

  // top
  0.5f, 0.5f, 0.5f,
  -0.5f, 0.5f, 0.5f,
  -0.5f, 0.5f, -0.5f,
  0.5f, 0.5f, -0.5f,

  // bottom
  0.5f, -0.5f, 0.5f,
  -0.5f, -0.5f, 0.5f,
  -0.5f, -0.5f, -0.5f,
  0.5f, -0.5f, -0.5f,
};

static float CubeNormals[] = {
  // front
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
  // back
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
  // right
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    // left
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    // top
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    // bottom
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
};

static float CubeTexCoords[] = {
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
};

static v3 *SphereVertices;
static v3 *SphereNormals;
static v2 *SphereTextureCoords;
static v3 *SphereColorBuff;
static uint *SphereIndices;
static uint sphere_element_buffer;

static Rectangle CubeRects[6];

void generate_sphere_vertices( ){
  SphereVertices = array_allocate( v3, 10000 ); 
  SphereNormals = array_allocate( v3, 10000 ); 
  SphereTextureCoords = array_allocate( v2, 10000 ); 
  SphereIndices = array_allocate( uint, 10000 ); 
  SphereColorBuff = array_allocate( v3, 10000 );
  uint pitch_count = 50;
  uint yaw_count = 50;

  f32 pitch_step = 2 * HMM_PI32 / pitch_count;
  f32 yaw_step = HMM_PI32 / yaw_count;

  f32 pitch = 0.0f;
  f32 yaw = HMM_PI32/2;

  const f32 r = 1.0f;
  for ( uint i = 0; i <= yaw_count ; i++ ){
    pitch = 0.0f;

    f32 z = r * HMM_SinF( yaw );
    f32 xy = r * HMM_CosF( yaw );

    for ( uint j = 0; j <= pitch_count ; j++ ){
      f32 x = xy * r * HMM_CosF( pitch );
      f32 y = xy * r * HMM_SinF( pitch );

      array_push( SphereVertices, v3{x,y,z} );
      array_push( SphereNormals, v3{ x,y,z } );
      
      array_push( SphereTextureCoords,
                  v2{ (f32)i/pitch_count, (f32)j/yaw_count } );
      pitch += pitch_step;
    }
    yaw -= yaw_step;
  }

  for( uint i = 0; i < yaw_count; ++i){
    uint k1 = i * (pitch_count + 1);    
    uint k2 = k1 + pitch_count + 1;    
    
    for( uint j = 0; j < pitch_count; ++j, ++k1, ++k2){
      if(i != 0) {
        array_push(SphereIndices,k1);
        array_push(SphereIndices,k2);
        array_push(SphereIndices,k1 + 1);
      }
      
      if(i != (yaw_count-1)){
        array_push( SphereIndices,k1 + 1);
        array_push( SphereIndices,k2);
        array_push( SphereIndices,k2 + 1);
      }
    }
  }
  return;
}

void generate_cube_rects( Rectangle *rects, f32 len ){
  Rectangle front, back, left, right, top, bot;
  f32 half_len = len / 2.0f;
  front.p0 = { -half_len, -half_len, half_len };
  back.p0 =  { -half_len, -half_len, -half_len };
  right.p0 = {  half_len, -half_len, -half_len };
  left.p0 =  { -half_len, -half_len, -half_len };
  top.p0 =   { -half_len, half_len, -half_len };
  bot.p0 =   { -half_len, -half_len, -half_len };

  front.s1 = { 1.0f,0.0f,0.0f }; front.s2 = { 0.0f, 1.0f, 0.0f };
  back.s1 =  { 1.0f,0.0f,0.0f }; back.s2 =  { 0.0f, 1.0f, 0.0f };

  right.s1 = { 0.0f,0.0f,1.0f }; right.s2 = { 0.0f, 1.0f, 0.0f };
  left.s1 = { 0.0f,0.0f,1.0f }; left.s2 = { 0.0f, 1.0f, 0.0f };

  top.s1 = { 1.0f,0.0f,0.0f }; top.s2 = { 0.0f, 0.0f, 1.0f };
  bot.s1 = { 1.0f,0.0f,0.0f }; bot.s2 = { 0.0f, 0.0f, 1.0f };

  front.n = { 0.0f, 0.0f, 1.0f }; 
  back.n = { 0.0f, 0.0f, -1.0f }; 

  right.n = { 1.0f, 0.0f, 0.0f }; 
  left.n = { -1.0f, 0.0f, 0.0f }; 

  top.n = { 0.0f, 1.0f, 0.0f }; 
  bot.n = { 0.0f, -1.0f, 0.0f }; 

  front.l1 = front.l2 = len;
  back.l1 = back.l2 = len;
  right.l1 = right.l2 = len;
  left.l1 = left.l2 = len;
  top.l1 = top.l2 = len;
  bot.l1 = bot.l2 = len;

  rects[ 0 ] = front;
  rects[ 1 ] = back;
  rects[ 2 ] = right;
  rects[ 3 ] = left;
  rects[ 4 ] = top;
  rects[ 5 ] = bot;

}

struct Camera {
  union {
    struct {
      v3 S, U, F;
    };
    v3 basis[3];
  };
  v3 P; 
  enum CameraState {
    ANIMATING = 1,
    STATIC
  };

  bool should_rotate;
  bool should_move;
  CameraState state;
  f32 duration;
  f32 elapsed;
  int dim;
  f32 dist_to_move;
  f32 dist_moved;
  f32 speed;

  f32 pitch; 
  f32 yaw;
  f32 max_pitch;
  f32 max_yaw;

  Plane plane;

  Camera ():max_pitch(80.0f),max_yaw(80.0f){}
  Camera ( const Camera & );
  Camera ( const v3& Eye, const v3& Center, const v3& Up ):
    should_rotate(false),should_move( true ),
    pitch(0.0f),yaw(0.0f),
    max_pitch(80.0f), max_yaw(80.0f) 
  {
    state = STATIC;
    F = HMM_NormalizeVec3(HMM_SubtractVec3(Center, Eye));
    S = HMM_NormalizeVec3(HMM_Cross(F, Up));
    U = HMM_Cross(S, F);
    P = Eye;
  }

  inline void rotate( f32 p, f32 y ){
    if ( !should_rotate ) return;
    pitch += p;
    yaw += y;
    yaw = CLAMP( yaw, -max_yaw, max_yaw );
    f32 cpitch = cos( HMM_RADIANS( pitch ) );
    f32 spitch = sin( HMM_RADIANS( pitch ) );

    f32 cyaw = cos( HMM_RADIANS( yaw ) );
    f32 syaw = sin( HMM_RADIANS( yaw ) );
    
    F.X = -cpitch * cyaw;
    F.Y = -syaw;
    F.Z = -spitch * cyaw ;
    v3 Up = { 0.0f, 1.0f, 0.0f };
    S = HMM_NormalizeVec3(HMM_Cross(F, Up));
    U = HMM_Cross(S, F);

  }

  m4 transform( ) const {
    m4 Result;
    Result.Elements[0][0] = S.X;
    Result.Elements[0][1] = U.X;
    Result.Elements[0][2] = -F.X;
    Result.Elements[0][3] = 0.0f;

    Result.Elements[1][0] = S.Y;
    Result.Elements[1][1] = U.Y;
    Result.Elements[1][2] = -F.Y;
    Result.Elements[1][3] = 0.0f;

    Result.Elements[2][0] = S.Z;
    Result.Elements[2][1] = U.Z;
    Result.Elements[2][2] = -F.Z;
    Result.Elements[2][3] = 0.0f;

    Result.Elements[3][0] = -HMM_DotVec3(S, P);
    Result.Elements[3][1] = -HMM_DotVec3(U, P);
    Result.Elements[3][2] = HMM_DotVec3(F, P);
    Result.Elements[3][3] = 1.0f;

    return (Result);
  }
  
  void move_towards( float t ){
    if ( should_move ) P = P + t * F;
  }
  void move_right( float t ){
    if ( should_move ) P = P + t * S;
  }
  void move_up( float t ){
    if ( should_move ) P = P + t * U;
  }

  void update( float dt ){
    switch ( state ){
      case ANIMATING:
      {
#if 0
        float dist = dt * (float)dist_to_move/duration;
        P += dist * direction;
        elapsed += dt;
        if ( elapsed >= duration ) state = STATIC;
#else
        float dist = dt * speed;
        P += dist * basis[dim];
#endif
        break;
      }
      default:
        break;
    }
  }


  inline void start_animate( int dir,f32 dist, f32 time ){
    if ( !should_move ) { state = STATIC;  return; }
    state = ANIMATING;
    elapsed = 0;
    dim = dir;
    dist_to_move = 1.0f;
    dist_moved = 0.0f;
    speed = dist/MS_TO_SEC( time );
  }


  inline void toggle_move( ){ should_move = !should_move; }

  inline void toggle_rotate(){ should_rotate = !should_rotate; }

  void print( ){
    fprintf( stdout, "Camera Info::\nFront: " );
    print_v3( F );
    fprintf( stdout, "\nRight: " );
    print_v3( S );
    fprintf( stdout, "\nUp: " );
    print_v3( U );
    fprintf( stdout, "\nPoint: " );
    print_v3( P );
    fprintf( stdout, "\n" );
  }

  bool hit_plane(
      const Ray &ray,
      v3 &point )
  {
    float d = HMM_DotVec3( ray.direction, F );
    if ( abs( d ) < TOLERANCE )
      return false;
    v3 temp = P - ray.start;
    float t = HMM_DotVec3( temp, F )/d ;
    point = ray.point_at( t );
    return true;
  }
};


enum Keys {
    KB_KEY_A = 0,
    KB_KEY_B,
    KB_KEY_C,
    KB_KEY_D,
    KB_KEY_E,
    KB_KEY_F,
    KB_KEY_G,
    KB_KEY_H,
    KB_KEY_I,
    KB_KEY_J,
    KB_KEY_K,
    KB_KEY_L,
    KB_KEY_M,
    KB_KEY_N,
    KB_KEY_O,
    KB_KEY_P,
    KB_KEY_Q,
    KB_KEY_R,
    KB_KEY_S,
    KB_KEY_T,
    KB_KEY_U,
    KB_KEY_V,
    KB_KEY_W,
    KB_KEY_X,
    KB_KEY_Y,
    KB_KEY_Z,
    KB_KEY_ESCAPE
};

enum EventType {
    MOUSE_RBUTTON_CLICK = 1,
    MOUSE_LBUTTON_CLICK,
    MOUSE_MOVE,
    KB_PRESS_A,
    KB_PRESS_B,
    KB_PRESS_C,
    KB_PRESS_D,
    KB_PRESS_E,
    KB_PRESS_F,
    KB_PRESS_G,
    KB_PRESS_H,
    KB_PRESS_I,
    KB_PRESS_J,
    KB_PRESS_K,
    KB_PRESS_L,
    KB_PRESS_M,
    KB_PRESS_N,
    KB_PRESS_O,
    KB_PRESS_P,
    KB_PRESS_Q,
    KB_PRESS_R,
    KB_PRESS_S,
    KB_PRESS_T,
    KB_PRESS_U,
    KB_PRESS_V,
    KB_PRESS_W,
    KB_PRESS_X,
    KB_PRESS_Y,
    KB_PRESS_Z,
    KB_REPEAT_A,
    KB_REPEAT_B,
    KB_REPEAT_C,
    KB_REPEAT_D,
    KB_REPEAT_E,
    KB_REPEAT_F,
    KB_REPEAT_G,
    KB_REPEAT_H,
    KB_REPEAT_I,
    KB_REPEAT_J,
    KB_REPEAT_K,
    KB_REPEAT_L,
    KB_REPEAT_M,
    KB_REPEAT_N,
    KB_REPEAT_O,
    KB_REPEAT_P,
    KB_REPEAT_Q,
    KB_REPEAT_R,
    KB_REPEAT_S,
    KB_REPEAT_T,
    KB_REPEAT_U,
    KB_REPEAT_V,
    KB_REPEAT_W,
    KB_REPEAT_X,
    KB_REPEAT_Y,
    KB_REPEAT_Z,
    KB_RELEASE_A,
    KB_RELEASE_B,
    KB_RELEASE_C,
    KB_RELEASE_D,
    KB_RELEASE_E,
    KB_RELEASE_F,
    KB_RELEASE_G,
    KB_RELEASE_H,
    KB_RELEASE_I,
    KB_RELEASE_J,
    KB_RELEASE_K,
    KB_RELEASE_L,
    KB_RELEASE_M,
    KB_RELEASE_N,
    KB_RELEASE_O,
    KB_RELEASE_P,
    KB_RELEASE_Q,
    KB_RELEASE_R,
    KB_RELEASE_S,
    KB_RELEASE_T,
    KB_RELEASE_U,
    KB_RELEASE_V,
    KB_RELEASE_W,
    KB_RELEASE_X,
    KB_RELEASE_Y,
    KB_RELEASE_Z,
    KB_PRESS_ESCAPE,
    KB_RELEASE_ESCAPE,
    KB_REPEAT_ESCAPE
};
struct Event {
  EventType type;
  int mods;
  union {
    struct {
      int button;
    };
    struct {
      int scancode;
    };
    struct {
      f32 xp,yp;
    };
  };
};

inline Event create_mouse_event( EventType t, int m ){
  Event e;
  e.type = t;
  e.mods = m;
  return e;
}
inline Event create_mouse_event( EventType t,f32 xp, f32 yp ){
  Event e;
  e.type = t;
  e.xp = xp;
  e.yp = yp;
  return e;
}
inline Event create_keyboard_event( EventType t, int s, int m ){
  Event e;
  e.type = t;
  e.scancode = s;
  e.mods = m;
  return e;
}

#define MAX_EVENTS 100

static Event Event_Queue[ MAX_EVENTS ];
static uint Event_Count = 0;

// Probably want to mutex it
void event_push_back( Event e ){
  Event_Queue[ Event_Count++ ] = e;
}


static void APIENTRY
glDebugOutput( GLenum source, GLenum type, GLuint id,
                    GLenum severity, GLsizei msgLength,
                    const GLchar* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    print_error("---------------"); 
    print_error("Debug message (%d): %s ",id,message);

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
          print_error( "Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
          print_error( "Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
          print_error( "Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
          print_error( "Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:
          print_error( "Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:
          print_error( "Source: Other"); break;
    }
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
          print_error( "Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
          print_error( "Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
          print_error( "Type: Undefined Behaviour"); break; 
        case GL_DEBUG_TYPE_PORTABILITY:
          print_error( "Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:
          print_error( "Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:
          print_error( "Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
          print_error( "Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:
          print_error( "Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:
          print_error( "Type: Other"); break;
    } 
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
          print_error( "Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:
          print_error( "Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW:
          print_error( "Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
          print_error( "Severity: notification"); break;
    } 
    fprintf(stderr,"\n");
}


void key_callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
  EventType t ;
#define KEY_PRESS_CASE(x) \
  case GLFW_KEY_##x: t = KB_PRESS_##x; break;

#define KEY_REPEAT_CASE(x) \
  case GLFW_KEY_##x: t = KB_REPEAT_##x; break;
#define KEY_RELEASE_CASE(x) \
  case GLFW_KEY_##x: t = KB_RELEASE_##x; break;
  if ( action == GLFW_PRESS ){
    switch ( key ){
      KEY_PRESS_CASE(A)
      KEY_PRESS_CASE(B)
      KEY_PRESS_CASE(C)
      KEY_PRESS_CASE(D)
      KEY_PRESS_CASE(E)
      KEY_PRESS_CASE(F)
      KEY_PRESS_CASE(G)
      KEY_PRESS_CASE(H)
      KEY_PRESS_CASE(I)
      KEY_PRESS_CASE(J)
      KEY_PRESS_CASE(K)
      KEY_PRESS_CASE(L)
      KEY_PRESS_CASE(M)
      KEY_PRESS_CASE(N)
      KEY_PRESS_CASE(O)
      KEY_PRESS_CASE(P)
      KEY_PRESS_CASE(Q)
      KEY_PRESS_CASE(R)
      KEY_PRESS_CASE(S)
      KEY_PRESS_CASE(T)
      KEY_PRESS_CASE(U)
      KEY_PRESS_CASE(V)
      KEY_PRESS_CASE(W)
      KEY_PRESS_CASE(X)
      KEY_PRESS_CASE(Y)
      KEY_PRESS_CASE(Z)
      KEY_PRESS_CASE(ESCAPE)

      default:
        break;
    }
  } else if ( action == GLFW_REPEAT ){
    switch ( key ){
      KEY_REPEAT_CASE(A)
      KEY_REPEAT_CASE(B)
      KEY_REPEAT_CASE(C)
      KEY_REPEAT_CASE(D)
      KEY_REPEAT_CASE(E)
      KEY_REPEAT_CASE(F)
      KEY_REPEAT_CASE(G)
      KEY_REPEAT_CASE(H)
      KEY_REPEAT_CASE(I)
      KEY_REPEAT_CASE(J)
      KEY_REPEAT_CASE(K)
      KEY_REPEAT_CASE(L)
      KEY_REPEAT_CASE(M)
      KEY_REPEAT_CASE(N)
      KEY_REPEAT_CASE(O)
      KEY_REPEAT_CASE(P)
      KEY_REPEAT_CASE(Q)
      KEY_REPEAT_CASE(R)
      KEY_REPEAT_CASE(S)
      KEY_REPEAT_CASE(T)
      KEY_REPEAT_CASE(U)
      KEY_REPEAT_CASE(V)
      KEY_REPEAT_CASE(W)
      KEY_REPEAT_CASE(X)
      KEY_REPEAT_CASE(Y)
      KEY_REPEAT_CASE(Z)
      KEY_REPEAT_CASE(ESCAPE)

      default:
        break;
    }
  } else if ( action == GLFW_RELEASE ){
    switch ( key ){
      KEY_RELEASE_CASE(A)
      KEY_RELEASE_CASE(B)
      KEY_RELEASE_CASE(C)
      KEY_RELEASE_CASE(D)
      KEY_RELEASE_CASE(E)
      KEY_RELEASE_CASE(F)
      KEY_RELEASE_CASE(G)
      KEY_RELEASE_CASE(H)
      KEY_RELEASE_CASE(I)
      KEY_RELEASE_CASE(J)
      KEY_RELEASE_CASE(K)
      KEY_RELEASE_CASE(L)
      KEY_RELEASE_CASE(M)
      KEY_RELEASE_CASE(N)
      KEY_RELEASE_CASE(O)
      KEY_RELEASE_CASE(P)
      KEY_RELEASE_CASE(Q)
      KEY_RELEASE_CASE(R)
      KEY_RELEASE_CASE(S)
      KEY_RELEASE_CASE(T)
      KEY_RELEASE_CASE(U)
      KEY_RELEASE_CASE(V)
      KEY_RELEASE_CASE(W)
      KEY_RELEASE_CASE(X)
      KEY_RELEASE_CASE(Y)
      KEY_RELEASE_CASE(Z)
      KEY_RELEASE_CASE(ESCAPE)

      default:
        break;
    }
  }
  event_push_back( create_keyboard_event(t, scancode, mods ) );
}

void process_keyboard_input( GLFWwindow *window, uint8 *key_map ){
  uint8 press = 0;
#define GET_KEY_STATE(key) \
    press = glfwGetKey( window, GLFW_KEY_##key );\
    key_map[KB_KEY_##key] = ( press==GLFW_RELEASE )?0:1;
  
  GET_KEY_STATE(A)
  GET_KEY_STATE(B)
  GET_KEY_STATE(C)
  GET_KEY_STATE(D)
  GET_KEY_STATE(E)
  GET_KEY_STATE(F)
  GET_KEY_STATE(G)
  GET_KEY_STATE(H)
  GET_KEY_STATE(I)
  GET_KEY_STATE(J)
  GET_KEY_STATE(K)
  GET_KEY_STATE(L)
  GET_KEY_STATE(M)
  GET_KEY_STATE(N)
  GET_KEY_STATE(O)
  GET_KEY_STATE(P)
  GET_KEY_STATE(Q)
  GET_KEY_STATE(R)
  GET_KEY_STATE(S)
  GET_KEY_STATE(T)
  GET_KEY_STATE(U)
  GET_KEY_STATE(V)
  GET_KEY_STATE(W)
  GET_KEY_STATE(X)
  GET_KEY_STATE(Y)
  GET_KEY_STATE(Z)
  GET_KEY_STATE(ESCAPE)
}

void mouse_callback( GLFWwindow *window, double xpos, double ypos ){
  event_push_back(
      create_mouse_event( MOUSE_MOVE, (f32)xpos,(f32)ypos)
      );
  glfwSetCursorPosCallback( window, NULL );
}

void mouse_button_callback(
    GLFWwindow* window,
    int button,
    int action,
    int mods )
{
  if ( action == GLFW_PRESS ){
    switch ( button ){
      case GLFW_MOUSE_BUTTON_RIGHT:
        event_push_back( create_mouse_event( MOUSE_RBUTTON_CLICK,mods));
        break;
      case GLFW_MOUSE_BUTTON_LEFT:
        event_push_back( create_mouse_event( MOUSE_LBUTTON_CLICK, mods));
        break;
      default:
        break;
    }
  }
#if 0
  if ( action == GLFW_PRESS ){
    switch (button){
      case GLFW_MOUSE_BUTTON_RIGHT:
      {
        int viewport[4];
        glGetIntegerv( GL_VIEWPORT, viewport);
        fprintf( stderr, "View port info: %d, %d, %d, %d\n",
            viewport[0], viewport[1], viewport[2], viewport[3] );
        double cp[2];
        if ( glfwGetWindowAttrib(window, GLFW_HOVERED ) ){
          glfwGetCursorPos( window, &cp[0], &cp[1] );
          fprintf( stdout, "%f, %f\n",
                   (f32)cp[0], (f32) cp[1] );
        }
        v3 point = v3{ ( float )cp[0], (float)cp[1], 0.0f };
        v3 wp = HMM_UnProject( point, mvp, SCREEN_WIDTH, SCREEN_HEIGHT );
        fprintf( stdout, "The point in world coords is: " );
        print_v3( wp );
        fprintf( stdout, "\n" );
        break;
      }
      default:
        break;
    }
  }
#endif
}



void resizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
  ScreenHeight = height;
  ScreenWidth = width;
}

#define CLEANUP(x)  __func__##x

int compile_shader( guint *ret, const char *source, GLenum type ){
  uint shader = glCreateShader( type );
  char *buff;
  if ( read_text_file_to_buffer( source, &buff ) == -1 ){
    goto CLEANUP(1);
  }

  glShaderSource( shader, 1, &buff, NULL );
  glCompileShader( shader );

  int success;
  glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
  
  if ( success == GL_FALSE ){
      char infoLog[512];
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      print_error("Failed to compile Shader!\n \
                   Location:\n %s\
                   \n%s\n", source, infoLog );
      goto CLEANUP(2);
  }
  
  *ret = shader; 
  return 0;
CLEANUP(2):
  assert( shader != 0 );
  glDeleteShader( shader );
  free( buff );
CLEANUP(1):
  return -1;
}


int compile_program( guint *program, const char *vsource ,const char *fsource ){
  guint vshader, fshader, prog;
  if ( compile_shader( &vshader, vsource, GL_VERTEX_SHADER ) == -1 ){
    goto CLEANUP(1);
  }
  if ( compile_shader( &fshader, fsource, GL_FRAGMENT_SHADER) == -1 ){
    goto CLEANUP(1);
  }

  prog = glCreateProgram();
  glAttachShader( prog, vshader );
  glAttachShader( prog, fshader );
  glLinkProgram( prog );
  int success;
  glGetProgramiv( prog, GL_LINK_STATUS, &success );
  if ( success == GL_FALSE ){
      char infoLog[512];
      glGetProgramInfoLog( prog , 512, NULL, infoLog );
      print_error( "Failed to link Shader!\n\
                    %s", infoLog );
      goto CLEANUP( 2 );
  }

  *program = prog;
  glDeleteShader( vshader );
  glDeleteShader( fshader );
  return 0;

CLEANUP(2):
  glDeleteShader( vshader );
  glDeleteShader( fshader );

CLEANUP(1):
  *program = 0;
  return -1;

}


void flipImage( uint8 *data, int width, int height){
    int top = 0, bottom = height - 1;
    while ( top <= bottom ){
        uint8 *topPixel = data + top * width * 4;
        uint8 *botPixel = data + bottom* width * 4;
        for ( int i = 0; i < width * 4 ; i++ ){
            uint8 tmp = *topPixel;
            *topPixel++ = *botPixel;
            *botPixel++ = tmp;
        }
        top++;
        bottom--;
    }
}




struct LineVertexBufferData{
  v3 p;
  v3 color;
};

struct ColorVertexData{
  v3 p;
  v3 color;
  v3 n;
};

struct QuadVertexData {
  v3 p0,p1,p2,p3;
  v3 c0,c1,c2,c3;
  v3 n0,n1,n2,n3;
};

struct RenderContext {
  ColorVertexData *color_vertex_data_buff = NULL;
  const uint max_color_vertex_data = 5000;
  uint num_color_vertex = 0;
  GLenum *color_vertex_modes = NULL;
} Rc;

void add_color_vertex( const v3 &p, const v3 &color, GLenum mode ){
  ColorVertexData v = { p, color };
  Rc.color_vertex_data_buff[ Rc.num_color_vertex ] = v;
  Rc.color_vertex_modes[ Rc.num_color_vertex ] = mode;
  Rc.num_color_vertex++;
}

void draw_color_line( v3 start ,v3 end, v3 color ){
  add_color_vertex( start, color, GL_LINES );
  add_color_vertex( end, color, GL_LINES );
}

void draw_color_vertex( const m4 &mvp ){
}

struct GridProgramInfo{
  uint id;
  uint mvp_loc;
  uint corner_loc;
  uint width_loc;

  uint8 pos_id;
  uint8 direction_id;
  uint8 color_id;
};

struct PointLightLocation{
  uint pos_loc;
  uint color_loc;
};

struct SimpleColorShaderProgram {
  uint id;
  uint mvp_loc;
  uint model_loc;
  uint view_loc;
  uint view_pos_loc;
  uint texture0_loc;

  // Frag. shader
  PointLightLocation point_light_loc[4];
  uint num_lights_loc;
  uint amb_loc;

  uint8 pos_id;
  uint8 color_id;
  uint8 normal_id;
  uint8 tex_coords_id;

};


struct SimpleNLColorShaderProgram {
  uint id;
  uint mvp_loc;

  uint8 pos_id;
  uint8 color_id;
  uint8 normal_id;
};



static GridProgramInfo grid_program_info; 
static SimpleColorShaderProgram simple_color_shader_info;
static SimpleNLColorShaderProgram nl_color_shader_info;


int create_grid_program( ){
  if ( compile_program(&grid_program_info.id,
        "./shaders/grid_shader.vert",
        "./shaders/grid_shader.frag" ) == -1 ){
    fprintf(stderr,"Unable to compile Program!\n");
    return -1;
  }
  const int &id = grid_program_info.id;
  
  glUseProgram( id );
  grid_program_info.mvp_loc = glGetUniformLocation( id,"mvp" );
  grid_program_info.corner_loc = glGetUniformLocation( id,"corner_pos" );
  grid_program_info.width_loc = glGetUniformLocation( id,"width" );

  grid_program_info.pos_id = 0;
  grid_program_info.direction_id = 1;
  grid_program_info.color_id = 2;
  return 0;
}

int create_simple_color_shader_program( ){
  if ( compile_program(&simple_color_shader_info.id,
        "./shaders/simple-color-shader.vert",
        "./shaders/simple-color-shader.frag" ) == -1 ){
    fprintf(stderr,"Unable to compile Program!\n");
    return -1;
  }
  const int &id = simple_color_shader_info.id;
  
  glUseProgram( id );
  simple_color_shader_info.mvp_loc =
          glGetUniformLocation( id,"mvp" );

  char buff[256];
  for ( int i = 0; i < 4; i++ ){
    snprintf( buff, 256, "point_lights[%d].pos",i );
    simple_color_shader_info.point_light_loc[i].pos_loc = 
      glGetUniformLocation( id, buff );
    snprintf( buff, 256, "point_lights[%d].color",i );
    simple_color_shader_info.point_light_loc[i].color_loc= 
      glGetUniformLocation( id, buff );
  }
  
  simple_color_shader_info.model_loc = glGetUniformLocation( id, "model");
  simple_color_shader_info.view_loc = glGetUniformLocation( id, "view" );
  simple_color_shader_info.num_lights_loc = glGetUniformLocation( id,
      "num_lights" );
  simple_color_shader_info.amb_loc = glGetUniformLocation( id, "amb" );
  simple_color_shader_info.view_pos_loc=glGetUniformLocation( id, "view_pos" );
  simple_color_shader_info.texture0_loc = glGetUniformLocation( id,"texture0");
  simple_color_shader_info.pos_id = 0;
  simple_color_shader_info.color_id = 1;
  simple_color_shader_info.normal_id = 2;
  simple_color_shader_info.tex_coords_id = 3;

  glUseProgram( 0 );
  return 0;
}

int create_nl_color_shader_program( ){
  if ( compile_program(&nl_color_shader_info.id,
        "./shaders/simple-color-shader-nl.vert",
        "./shaders/simple-color-shader-nl.frag" ) == -1 ){
    fprintf(stderr,"Unable to compile Program!\n");
    return -1;
  }
  const int &id = nl_color_shader_info.id;
  
  glUseProgram( id );
  nl_color_shader_info.mvp_loc =
          glGetUniformLocation( id,"mvp" );

  nl_color_shader_info.pos_id = 0;
  nl_color_shader_info.color_id = 1;
  nl_color_shader_info.normal_id = 2;
  glUseProgram( 0 );
  return 0;
}



#if 0
void AABB_generate_vertex( const AABB &box, v3 *mem ){
  f32 xlen = box.u[0] - box.l[0];
  f32 ylen = box.u[1] - box.l[1];
  f32 zlen = box.u[2] - box.l[2];
  
  //back 
  v3 a0 = box.l;
  v3 a1 = box.l + v3{ xlen, 0, 0 };
  v3 a2 = box.l + v3{ xlen, ylen, 0 };
  v3 a3 = box.l + v3{ 0, ylen, 0 };

  //front 
  v3 a0 = box.l + v3{ 0,0,zlen};
  v3 a1 = box.l + v3{ xlen, 0, zlen };
  v3 a2 = box.l + v3{ xlen, ylen, zlen };
  v3 a3 = box.l + v3{ 0, ylen, zlen };

  //left
  v3 a0 = box.l + v3{ 0,0,0};
  v3 a1 = box.l + v3{ 0,0,zlen };
  v3 a2 = box.l + v3{ 0,ylen,zlen };
  v3 a3 = box.l + v3{ 0,ylen,0 };


  //right
  v3 a0 = box.l + v3{ xlen,0,0};
  v3 a1 = box.l + v3{ xlen,0,zlen };
  v3 a2 = box.l + v3{ xlen,ylen,zlen };
  v3 a3 = box.l + v3{ xlen,ylen,0 };

  // bottom 
  v3 a0 = box.l + v3{ 0,0,0};
  v3 a1 = box.l + v3{ xlen,0,0};
  v3 a2 = box.l + v3{ xlen,0,zlen};
  v3 a3 = box.l + v3{ 0,0,zlen };
  //top 
  v3 a0 = box.l + v3{ 0,ylen,0};
  v3 a1 = box.l + v3{ xlen,ylen,0};
  v3 a2 = box.l + v3{ xlen,ylen,zlen};
  v3 a3 = box.l + v3{ 0,ylen,zlen };

}
#endif



struct ColorQuad {
  v3 p0,p1,p2,p3;
  v3 color;
  v3 n;
};


struct Image {
  int h,w,channels;
  uint8 *data;
};

struct Light {
  Object object;
  v3 color;
};

struct Texture {
  enum TextureType {
    COLOR = 0,
    CHECKER,
    MARBLE,
  };
  TextureType type;
  union {  
    v3 face_colors[6];
    v3 color;
  };
  Image image;
};

struct Material {
  enum MaterialType{
    METALLIC = 0,
    PURE_DIFFUSE,
    GLASS,
  };
  
  MaterialType type;
  Texture texture;
  f32 diffuse, specular, shine;

};

Material create_material_diffuse( Texture t ){
  Material m;
  m.type =  Material::PURE_DIFFUSE;
  m.texture = t;
  m.diffuse = 1.0f;
  m.specular = 0.0f;
  m.shine = 0.0f;
  return m;
}


Material create_material_metallic( Texture t ){
  Material m;
  m.type =  Material::METALLIC;
  m.texture = t;
  m.diffuse = 0.3f;
  m.specular = 8.0f;
  m.shine = 64.0f;
  return m;
}

Material create_material_glass( ){
  Material m;
  Texture t;
  t.type = Texture::COLOR;
  t.color = v3{ 0.0f,0.0f,1.0f };
  m.type = Material::GLASS;
  m.texture = t;
  return m;
}

struct World {
  enum State {
    STATE_INVALID = 0,
    STATE_FREE_VIEW = 1,
    STATE_DETACHED,
    STATE_ON_HOLD,
    STATE_SELECTED,
    STATE_VIEW_CAMERA,
  };

  State state;
  bool show_imgui;

  m4 ui_perspective;
  m4 view_perspective;
  f32 ui_vfov, ui_near_plane, ui_far_plane;
  f32 view_vfov, view_near_plane, view_far_plane;

  m4 perspective;

  Camera ui_camera;
  Camera view_camera;

  Camera *camera;

  Grid grid;
  Cube cube;

  Cube *cubes;
  Sphere *spheres;
  Rectangle *rects;

  Material *cube_materials;
  Material *sphere_materials;
  Material *rect_materials;
 
  v3 *light_pos;
  v3 *light_colors;
  v3 amb_light;
  v3 *light_cube_color;
  v3 *light_sphere_color;
  v3 *light_rect_color;

  Cube *light_cubes;
  Sphere *light_spheres;
  Rectangle *light_rects;

  uint *light_cubes_vao;
  uint *light_cubes_vbo;
  
  uint *light_spheres_vao;
  uint *light_spheres_vbo;

  uint *light_rect_vao;

  
  Line *lines;
  ColorQuad *temp_color_quads;
  ColorQuad *perm_color_quads;
  

  bool is_selected;
  Object selected_object;
  
  GetAABBFunc selected_aabb;
  TranslateFunc selected_move;
  RotateFunc selected_rotate;
  ScaleFunc selected_scale;

  uint selected_index;
  void *selected_data;

  bool is_holding;
  Object hold_object;
  int hold_object_id;

  AABB *boxes;


  // Dear imgui selection stuff
  int cube_face_dropdown;
  v3 cube_face_color;
  v3 cube_side_length;
  f32 sel_cube_length;
  int cube_material_dropdown;
  int cube_texture_dropdown;

  v3 sphere_face_color;
  f32 sel_sphere_radius;
  int sphere_material_dropdown;
  int sphere_texture_dropdown;

  v3 rect_face_color;
  int rect_flip_normal;
  f32 sel_rect_l1;
  f32 sel_rect_l2;
  int rect_material_dropdown;
  int rect_texture_dropdown;

  v3 light_cube_face_color;
  v3 light_sphere_face_color;
  v3 light_rect_face_color;

  int object_select_dropdown;

  uint grid_vao, grid_vbo, grid_ebo;
  uint cube_vao, cube_vbo, cube_ebo;

  uint *cubes_vao;
  uint *cubes_vbo;
  
  uint *spheres_vao;
  uint *spheres_vbo;
  v3 *sphere_colors;
  v3 *rect_colors;

  m4 *spheres_transform;

  m4 *model_matrices[_OBJECT_MAX];

  uint color_vao, color_vbo, color_ebo;  
  uint rect_vao, rect_vbo, rect_ebo;

  uint white_texture, checker_texture, marble_texture;

  v3 *color_vertex_data;
  uint *color_vertex_indices;
  uint *index_stack; // Used as stack
  GLenum *color_vertex_modes; // Used as stack
};
void world_config_simple_color_shader_buffer(
    uint vao, uint vbo, uint ebo,
    size_t vertex_count 
)
{
  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  size_t offset= sizeof(v3)*vertex_count;
  glBufferData( GL_ARRAY_BUFFER,
                3 * offset + sizeof(v2) * vertex_count,
                NULL,
                GL_STREAM_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo);


  glEnableVertexAttribArray(simple_color_shader_info.pos_id );
  glVertexAttribPointer( simple_color_shader_info.pos_id,
                         3,
                         GL_FLOAT, GL_FALSE,
                         sizeof( v3 ), (void *)0 );

  glEnableVertexAttribArray( simple_color_shader_info.color_id );
  glVertexAttribPointer( simple_color_shader_info.color_id,
                         3,
                         GL_FLOAT, GL_FALSE,
                         sizeof(v3),
                         (void *)(offset) );

  glEnableVertexAttribArray( simple_color_shader_info.normal_id );
  glVertexAttribPointer( simple_color_shader_info.normal_id,
                         3,
                         GL_FLOAT, GL_FALSE,
                         sizeof(v3),
                         (void *)( 2 * offset)  );

  glEnableVertexAttribArray( simple_color_shader_info.tex_coords_id );
  glVertexAttribPointer( simple_color_shader_info.tex_coords_id,
                         2,
                         GL_FLOAT, GL_FALSE,
                         sizeof(v2),
                         (void *)( 3 * offset)  );

  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void world_config_nl_color_shader_buffer(
    uint vao, uint vbo, uint ebo,
    size_t size
)
{
  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );

  size_t offset=size;
  glBufferData( GL_ARRAY_BUFFER,
                3 * offset,
                NULL,
                GL_STREAM_DRAW );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo);


  glEnableVertexAttribArray(nl_color_shader_info.pos_id );
  glVertexAttribPointer( nl_color_shader_info.pos_id,
                         3,
                         GL_FLOAT, GL_FALSE,
                         sizeof( v3 ), (void *)0 );

  glEnableVertexAttribArray( nl_color_shader_info.color_id );
  glVertexAttribPointer( nl_color_shader_info.color_id,
                         3,
                         GL_FLOAT, GL_FALSE,
                         sizeof(v3),
                         (void *)(offset) );

  glEnableVertexAttribArray( nl_color_shader_info.normal_id );
  glVertexAttribPointer( nl_color_shader_info.normal_id,
                         3,
                         GL_FLOAT, GL_FALSE,
                         sizeof(v3),
                         (void *)( 2 * offset)  );
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

#define GLASS_COLOR v3{0.0f,0.0f,1.0f};

void world_add_cube_vertex_data( const World &w, uint index ){
  v3 colors[24];
  Material &m = w.cube_materials[index];
  if ( m.type == Material::GLASS ){
    for ( int i = 0; i < 24; i++ ){
      colors[ i ] = GLASS_COLOR;
    }
  } else {
    Texture &tex = m.texture;
    for ( int i = 0; i < 24; i++ ){
      int index = i / 4;
      colors[ i ] = tex.face_colors[ index ];
    }
  }
  
  uint vao = w.cubes_vao[index];
  uint vbo = w.cubes_vbo[index];
  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(CubeVertices), CubeVertices );
  glBufferSubData( GL_ARRAY_BUFFER, 
                   sizeof(CubeVertices),
                   sizeof(colors),
                   colors);
  glBufferSubData( GL_ARRAY_BUFFER, 
                   sizeof(CubeVertices) + sizeof(colors),
                   sizeof(CubeNormals),
                   (void *)CubeNormals);
  glBufferSubData( GL_ARRAY_BUFFER,
                   3*sizeof(CubeVertices),
                   sizeof(CubeTexCoords),
                   (void *)CubeTexCoords );
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
void world_generate_cube_data( uint &vao, uint &vbo, Cube &cube ){
  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );
  
  world_config_simple_color_shader_buffer( 
      vao, vbo, quad_elem_buffer_index, 24 );
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); 
}

void world_add_light_cube_vertex_data(
    const World &w,
    uint index )
{
  v3 colors[24];
  for ( int i = 0; i < 24; i++ ){
    colors[ i ] = w.light_cube_color[index];
  }

  glBindVertexArray( w.light_cubes_vao[index] );
  glBindBuffer( GL_ARRAY_BUFFER, w.light_cubes_vbo[index] );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(CubeVertices), CubeVertices );
  glBufferSubData( GL_ARRAY_BUFFER, 
                   sizeof(CubeVertices),
                   sizeof(colors),
                   colors);
  glBufferSubData( GL_ARRAY_BUFFER, 
                   sizeof(CubeVertices) + sizeof(colors),
                   sizeof(CubeNormals),
                   (void *)CubeNormals);
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void world_generate_light_cube( uint &vao, uint &vbo, Cube &cube ){
  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );
  
  world_config_nl_color_shader_buffer( vao, vbo, 
      quad_elem_buffer_index,
      sizeof(CubeVertices) );
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); 
}

void world_add_sphere_vertex_data( const World &w, uint index ) {
  Material &material = w.sphere_materials[index];
  v3 color = w.sphere_materials[index].texture.color;

  if ( material.type == Material::GLASS ){
    color = GLASS_COLOR;
  }

  for ( uint i = 0; i < array_length( SphereVertices ); i++ ){
    array_push( SphereColorBuff, color );
  }

  size_t data_size = array_length(SphereVertices)*sizeof(*SphereVertices);

  uint vao = w.spheres_vao[index];
  uint vbo = w.spheres_vbo[index];

  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferSubData( GL_ARRAY_BUFFER, 
                   data_size,
                   data_size,
                   SphereColorBuff);
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  array_clear( SphereColorBuff );
}

void world_add_light_sphere_vertex_data(
    const World &w, uint index
    )
{
  v3 &color = w.light_sphere_color[index];
  for ( uint i = 0; i < array_length( SphereVertices ); i++ ){
    array_push( SphereColorBuff, color );
  }

  glBindVertexArray( w.light_spheres_vao[index] );
  glBindBuffer( GL_ARRAY_BUFFER, w.light_spheres_vbo[index] );
  size_t data_size = sizeof(v3) * array_length( SphereVertices );
  glBufferSubData( GL_ARRAY_BUFFER, 
                   data_size,
                   data_size,
                   SphereColorBuff);
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  array_clear( SphereColorBuff );
}

void world_add_cube( World &w, Cube *cube, Material m ){
  uint vao, vbo;
  array_push( w.cubes, *cube );
#if 0
  m4 model = HMM_Scale( v3{ cube->length, cube->length, cube->length } )
     HMM_Translate(cube->pos) ; 
#else
  m4 model = HMM_Translate(cube->pos)*HMM_Scale( v3{ cube->length, cube->length, cube->length } ) ; 
#endif
  array_push( w.model_matrices[OBJECT_CUBE_INSTANCE],model );
  array_push( w.cube_materials, m );

  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );
  world_config_simple_color_shader_buffer( vao, vbo, 
      quad_elem_buffer_index,
      24 );

  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); 

  array_push( w.cubes_vao, vao );
  array_push( w.cubes_vbo, vbo );

  world_add_cube_vertex_data(w, array_length(w.cubes)-1 );

}

void world_add_light_cube( World &w, Cube *cube, v3 color ){
  uint vao, vbo;
  array_push( w.light_cubes, *cube );
  m4 model = HMM_Translate(cube->pos) *
             HMM_Scale( v3{ cube->length, cube->length, cube->length } ); 
  array_push( w.model_matrices[OBJECT_LIGHT_CUBE_INSTANCE],model );
  array_push( w.light_cube_color, color );

  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );
  
  world_config_nl_color_shader_buffer( vao, vbo, 
      quad_elem_buffer_index,
      sizeof(CubeVertices) );
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); 

  array_push( w.light_cubes_vao, vao );
  array_push( w.light_cubes_vbo, vbo );
  world_add_light_cube_vertex_data(w,array_length(w.light_cubes)-1 );

}

void world_add_sphere( World &w, const Sphere &s, Material m){
  uint vao, vbo;
  array_push( w.spheres, s );
  m4 transform = HMM_Translate( s.c ) * HMM_Scale( v3{ s.r, s.r,s.r });
  array_push( w.model_matrices[OBJECT_SPHERE], transform );
  //array_push( w.sphere_colors, color );
  array_push( w.sphere_materials, m );

  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );

  world_config_simple_color_shader_buffer( 
      vao, vbo, sphere_element_buffer,
      array_length(SphereVertices)); 
  
  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  size_t data_size = sizeof(v3) * array_length( SphereVertices );
  glBufferSubData( GL_ARRAY_BUFFER, 0,data_size , SphereVertices );
  glBufferSubData( GL_ARRAY_BUFFER, 
                   2 * data_size,
                   data_size,
                   (void *) SphereNormals );
  glBufferSubData( GL_ARRAY_BUFFER,
                   3*data_size,
                   sizeof(v2) * array_length( SphereTextureCoords ),
                   (void *)SphereTextureCoords );

  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); 

  array_push( w.spheres_vao, vao );
  array_push( w.spheres_vbo, vbo );

  world_add_sphere_vertex_data(w,array_length(w.spheres)-1);
}

void world_add_light_sphere( World &w, const Sphere &s, v3 color ){
  uint vao, vbo;
  array_push( w.light_spheres, s );
  m4 transform = HMM_Translate( s.c ) * HMM_Scale( v3{ s.r, s.r,s.r });
  array_push( w.model_matrices[OBJECT_LIGHT_SPHERE], transform );
  array_push( w.light_sphere_color, color );

  glGenVertexArrays( 1, &vao );
  glGenBuffers( 1, &vbo );
  size_t offset= array_length(SphereVertices)*sizeof(*SphereVertices);
  world_config_nl_color_shader_buffer( vao, vbo, sphere_element_buffer,
      offset );

  
  glBindVertexArray( vao );
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  size_t data_size = sizeof(v3) * array_length( SphereVertices );
  glBufferSubData( GL_ARRAY_BUFFER, 0,data_size , SphereVertices );
  glBufferSubData( GL_ARRAY_BUFFER, 
                   2 * data_size,
                   data_size,
                   (void *) SphereNormals );
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); 

  array_push( w.light_spheres_vao, vao );
  array_push( w.light_spheres_vbo, vbo );

  world_add_light_sphere_vertex_data( w, array_length(w.light_spheres)-1 );
}


void world_add_rect( World &w, const Rectangle &r,Material m ){
  array_push( w.rects, r );
  m4 transform = HMM_Mat4d( 1.0f );
  array_push( w.rect_materials, m );
  array_push( w.model_matrices[OBJECT_RECT], transform );
  //array_push( w.rect_colors, color );

}

void world_add_light_rect( World &w, const Rectangle &r, v3 color ){
  array_push( w.light_rects, r );
  m4 transform = HMM_Mat4d( 1.0f );
  array_push( w.model_matrices[OBJECT_RECT], transform );
  array_push( w.light_rect_color, color );
}



void world_generate_grid_data( World &w, Grid &g ){

  const v3 &color = g.color;
  glGenVertexArrays(1,&w.grid_vao);
  glGenBuffers(1,&w.grid_vbo);
  f32 len = 100.0f;
  glBindVertexArray( w.grid_vao );
  glBindBuffer( GL_ARRAY_BUFFER, w.grid_vbo );
  
  v3 vertex_data[12];

  vertex_data[0] = g.rect.corner + len * g.dir1;
  vertex_data[1] = g.dir2;
  vertex_data[2] = color;

  vertex_data[3] = g.rect.corner;
  vertex_data[4] = g.dir2;
  vertex_data[5] = color;

  vertex_data[6] = g.rect.corner;
  vertex_data[7] = g.dir1;
  vertex_data[8] = color;

  vertex_data[9] = g.rect.corner + len * g.dir2;
  vertex_data[10] = g.dir1;
  vertex_data[11] = color;

  glBufferData( GL_ARRAY_BUFFER,
                sizeof( vertex_data ),
                vertex_data, GL_STATIC_DRAW );

  GLsizei stride = 3 * sizeof(v3);
   
  glEnableVertexAttribArray( grid_program_info.pos_id );
  glEnableVertexAttribArray( grid_program_info.direction_id);
  glEnableVertexAttribArray( grid_program_info.color_id );

  glVertexAttribPointer( grid_program_info.pos_id,
                         3, GL_FLOAT, GL_FALSE,
                         stride,
                         (void *)( 0 ) );

  glVertexAttribPointer( grid_program_info.direction_id,
                         3, GL_FLOAT, GL_FALSE,
                         stride, 
                         (void *)( sizeof(v3) ) );

  glVertexAttribPointer( grid_program_info.color_id,
                         3, GL_FLOAT, GL_FALSE,
                         stride,
                         (void *)( 2 * sizeof(v3) ) );

  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}



void world_draw_AABB(
    const World &w,
    const AABB &box,
    v3 color,
    uint *elem_index )
{
  // Sorry for this shit, but its the best i can do for now
  // TODO: find something better
  f32 xlen = box.u[0] - box.l[0];
  f32 ylen = box.u[1] - box.l[1];
  f32 zlen = box.u[2] - box.l[2];

  array_push( w.color_vertex_data, box.l);
  array_push( w.color_vertex_data, color );
  array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );

  array_push( w.color_vertex_data, box.l + v3{ xlen, 0, 0 });
  array_push( w.color_vertex_data, color );
  array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );


  array_push( w.color_vertex_data, box.l + v3{ xlen, ylen, 0 });
  array_push( w.color_vertex_data, color );
  array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );


  array_push( w.color_vertex_data, box.l + v3{ 0, ylen, 0 });
  array_push( w.color_vertex_data, color );
  array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );

  // front
  array_push( w.color_vertex_data, box.l + v3{ 0,0,zlen});
  array_push( w.color_vertex_data, color );
  array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );


  array_push( w.color_vertex_data, box.l + v3{ xlen, 0, zlen });
  array_push( w.color_vertex_data, color );
  array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );


  array_push( w.color_vertex_data, box.l + v3{ xlen, ylen, zlen });
  array_push( w.color_vertex_data, color );
  array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );


  array_push( w.color_vertex_data, box.l + v3{ 0, ylen, zlen });
  array_push( w.color_vertex_data, color );
  array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );
  
  uint value = *elem_index; 
  
  array_push( w.color_vertex_indices, value );
  array_push( w.color_vertex_indices, value + 1 );

  array_push( w.color_vertex_indices, value + 1 );
  array_push( w.color_vertex_indices, value + 2 );

  array_push( w.color_vertex_indices, value + 2 );
  array_push( w.color_vertex_indices, value + 3 );

  array_push( w.color_vertex_indices, value + 3 );
  array_push( w.color_vertex_indices, value );

  array_push( w.color_vertex_indices, value );
  array_push( w.color_vertex_indices, value + 4 );

  array_push( w.color_vertex_indices, value + 4 );
  array_push( w.color_vertex_indices, value + 5 );

  array_push( w.color_vertex_indices, value + 5 );
  array_push( w.color_vertex_indices, value + 6 );

  array_push( w.color_vertex_indices, value + 6 );
  array_push( w.color_vertex_indices, value + 7 );

  array_push( w.color_vertex_indices, value + 7 );
  array_push( w.color_vertex_indices, value + 4 );

  array_push( w.color_vertex_indices, value + 3 );
  array_push( w.color_vertex_indices, value + 7 );

  array_push( w.color_vertex_indices, value + 2 );
  array_push( w.color_vertex_indices, value + 6 );

  array_push( w.color_vertex_indices, value + 1 );
  array_push( w.color_vertex_indices, value + 5 );

  *elem_index = value + 8;
  return;
}

void world_draw_grid(uint vao,const Grid &g, const m4 &mvp ){
  glUseProgram( grid_program_info.id );
 // Generate rectangle vertices with line vertices
  glUniformMatrix4fv( grid_program_info.mvp_loc,
                      1,GL_FALSE,
                      HMM_MAT4_PTR(mvp) );
  glUniform3fv( grid_program_info.corner_loc, 1,  g.rect.corner.Elements );
  glUniform1f( grid_program_info.width_loc, g.w );

  glBindVertexArray( vao );
  glDrawArraysInstanced( GL_LINES, 0, 12, g.nlines );

  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  glUseProgram( 0 );
}


void world_draw_cube(uint vao, const Cube &cube, const m4 &mvp ){
  glUseProgram( simple_color_shader_info.id );
  glUniformMatrix4fv( simple_color_shader_info.mvp_loc,
                      1,GL_FALSE,
                      HMM_MAT4_PTR(mvp) );
  glBindVertexArray( vao );
  glDrawElements( GL_TRIANGLES,
                  36,
                  GL_UNSIGNED_INT,
                  0  );
  glBindVertexArray( 0 );
  glUseProgram( 0 );
}

void world_draw_sphere( uint vao, const Sphere &s, const m4 &mvp ){
  glUseProgram( simple_color_shader_info.id );
  glUniformMatrix4fv( simple_color_shader_info.mvp_loc,
                      1,GL_FALSE,
                      HMM_MAT4_PTR(mvp) );
  glBindVertexArray( vao );
  glDrawElements( GL_TRIANGLES,
                  array_length(SphereIndices),
                  GL_UNSIGNED_INT,
                  0  );
  glBindVertexArray( 0 );
  glUseProgram( 0 );

}


void draw_world( const World &w ){
  m4 v = w.camera->transform();  
  m4 vp = w.perspective*v;
  world_draw_grid( w.grid_vao,w.grid,vp);

  // draw the non-lighting stuff
  glUseProgram( nl_color_shader_info.id );

  m4 *cube_models = w.model_matrices[ OBJECT_LIGHT_CUBE_INSTANCE ];

  for ( uint i = 0; i < array_length( w.light_cubes ); i++ ){
    m4 mvp = vp * cube_models[i];

    glBindVertexArray( w.light_cubes_vao[i] );
    glUniformMatrix4fv( nl_color_shader_info.mvp_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR(mvp) );
    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0  );
    array_push( w.light_pos, w.light_cubes[i].pos );
    array_push( w.light_colors, w.light_cube_color[i] );
  }

  m4 *sphere_models = w.model_matrices[ OBJECT_LIGHT_SPHERE ];
  for ( uint i = 0; i < array_length( w.light_spheres ); i++ ){
    m4 mvp = vp * sphere_models[i];
    glUniformMatrix4fv( nl_color_shader_info.mvp_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR(mvp) );
    glBindVertexArray( w.light_spheres_vao[i] );
    glDrawElements( GL_TRIANGLES,
                    array_length(SphereIndices),
                    GL_UNSIGNED_INT, 0  );
    glBindVertexArray( 0 );
    array_push( w.light_pos, w.light_spheres[i].c );
    array_push( w.light_colors, w.light_sphere_color[i] );
  }
  uint value = 0;
  for ( uint i = 0; i < array_length( w.light_rects ); i++ ){
    Rectangle &r = w.light_rects[i];
    v3 &color = w.light_rect_color[i];

    array_push( w.light_pos,
        r.p0 + ( r.l1 / 2 )*r.s1 + (r.l2/2)*r.s2 );
    array_push( w.light_colors, w.light_rect_color[i] );

    array_push( w.color_vertex_data, r.p0 );
    array_push( w.color_vertex_data, color );
    array_push( w.color_vertex_data, r.n );

    array_push( w.color_vertex_data, r.p1 );
    array_push( w.color_vertex_data, color );
    array_push( w.color_vertex_data, r.n );

    array_push( w.color_vertex_data, r.p2 );
    array_push( w.color_vertex_data, color );
    array_push( w.color_vertex_data, r.n );


    array_push( w.color_vertex_data, r.p3 );
    array_push( w.color_vertex_data, color );
    array_push( w.color_vertex_data, r.n );

    // push the indices
    array_push( w.color_vertex_indices, value );
    array_push( w.color_vertex_indices, value + 1 );
    array_push( w.color_vertex_indices, value + 2 );
    array_push( w.color_vertex_indices, value + 2 );
    array_push( w.color_vertex_indices, value + 3 );
    array_push( w.color_vertex_indices, value );
    value += 4;
  }

  glBindVertexArray( w.color_vao );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, w.color_ebo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                sizeof(uint) * array_length( w.color_vertex_indices ),
                w.color_vertex_indices,
                GL_STATIC_DRAW );

  glBindBuffer( GL_ARRAY_BUFFER, w.color_vbo );
  glBufferData( GL_ARRAY_BUFFER, 
                sizeof(v3) * array_length( w.color_vertex_data ),
                w.color_vertex_data,
                GL_STATIC_DRAW
              );

  glUniformMatrix4fv( simple_color_shader_info.mvp_loc,
                      1,GL_FALSE,
                      HMM_MAT4_PTR(vp) );
  glDrawElements(GL_TRIANGLES,array_length(w.color_vertex_data),
                 GL_UNSIGNED_INT, 0 );

  array_clear( w.color_vertex_data );
  array_clear( w.color_vertex_indices );


  glUseProgram( 0 );

  // Draw all the lighting stuff
  glUseProgram( simple_color_shader_info.id );
  
  glUniform1i( simple_color_shader_info.texture0_loc, 0 );

  glUniform3fv(simple_color_shader_info.view_pos_loc,
      1, w.camera->P.Elements );

  uint loop_count =  array_length( w.light_pos ) < 4 ? 
                     array_length( w.light_pos ) : 4;
  for ( uint i = 0; i < loop_count ; i++ ){
    PointLightLocation &p = simple_color_shader_info.point_light_loc[i];
    glUniform3fv( p.pos_loc,1, w.light_pos[i].Elements );
    glUniform3fv( p.color_loc,1, w.light_colors[i].Elements );
  }
  glUniform1i( simple_color_shader_info.num_lights_loc, loop_count );
  glUniform3fv( simple_color_shader_info.amb_loc,1, w.amb_light.Elements );
  glUniformMatrix4fv( simple_color_shader_info.view_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR(v) );
  cube_models = w.model_matrices[ OBJECT_CUBE_INSTANCE ];

  for ( uint i = 0; i < array_length( w.cubes ); i++ ){
    m4 mvp = vp * cube_models[i];
    Material &material = w.cube_materials[i];
    Texture &texture = material.texture;
    glActiveTexture(GL_TEXTURE0);

    if ( material.type == Material::GLASS ){
      glBindTexture( GL_TEXTURE_2D, w.white_texture );
    } else {
      switch ( texture.type ){
        case Texture::COLOR:
          glBindTexture( GL_TEXTURE_2D, w.white_texture );
          break;
        case Texture::CHECKER:
          glBindTexture( GL_TEXTURE_2D, w.checker_texture );
          break;
        case Texture::MARBLE:
          glBindTexture( GL_TEXTURE_2D, w.marble_texture );
          break;
      }
    }

    glUniformMatrix4fv( simple_color_shader_info.mvp_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR(mvp) );
    glUniformMatrix4fv( simple_color_shader_info.model_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR( cube_models[i] ) );
    glBindVertexArray( w.cubes_vao[i] );
    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0  );
    glBindVertexArray( 0 );
  }

  sphere_models = w.model_matrices[ OBJECT_SPHERE ];
  for ( uint i = 0; i < array_length( w.spheres ); i++ ){
    m4 mvp = vp * sphere_models[i];

    Material &material = w.sphere_materials[i];
    Texture &texture = material.texture;
    glActiveTexture(GL_TEXTURE0);

    if ( material.type == Material::GLASS ){
      glBindTexture( GL_TEXTURE_2D, w.white_texture );
    } else {
      switch ( texture.type ){
        case Texture::COLOR:
          glBindTexture( GL_TEXTURE_2D, w.white_texture );
          break;
        case Texture::CHECKER:
          glBindTexture( GL_TEXTURE_2D, w.checker_texture );
          break;
        case Texture::MARBLE:
          glBindTexture( GL_TEXTURE_2D, w.marble_texture );
          break;
      }
    }
    glUniformMatrix4fv( simple_color_shader_info.mvp_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR(mvp) );
    glUniformMatrix4fv( simple_color_shader_info.model_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR( sphere_models[i] ) );
    glBindVertexArray( w.spheres_vao[i] );
    glDrawElements( GL_TRIANGLES,
                    array_length(SphereIndices),
                    GL_UNSIGNED_INT, 0  );
    glBindVertexArray( 0 );
  }


  glUniformMatrix4fv( simple_color_shader_info.mvp_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR(vp) );
  glUniformMatrix4fv( simple_color_shader_info.view_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR( v ) );
  glUniformMatrix4fv( simple_color_shader_info.model_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR( HMM_Mat4d(1.0f) ) );
  value = 0;
  for ( uint i = 0; i < array_length( w.rects ); i++ ){
    Rectangle &r = w.rects[i];
    Material &material = w.rect_materials[i];
    Texture &texture = material.texture;
    v3 color = texture.color;

    v2 tc[4];
    if ( material.type == Material::GLASS ){
      glBindTexture( GL_TEXTURE_2D, w.white_texture );
      color  = GLASS_COLOR;
    } else {
      switch ( texture.type ){
        case Texture::COLOR:
          glBindTexture( GL_TEXTURE_2D, w.white_texture );
          tc[0] = v2{ 0.0f, 0.0f };
          tc[1] = v2{ 1.0f, 0.0f };
          tc[2] = v2{ 1.0f, 1.0f };
          tc[3] = v2{ 0.0f, 1.0f };
          break;
        case Texture::CHECKER:
          {
            glBindTexture( GL_TEXTURE_2D, w.checker_texture );
            f32 unit = 0.5f;
            f32 len1 = r.l1/unit, len2 = r.l2/unit;
            tc[0] = v2{ 0.0f, 0.0f };
            tc[1] = v2{ len1, 0.0f };
            tc[2] = v2{ len1, len2 };
            tc[3] = v2{ 0, len2 };
          }
          break;
        case Texture::MARBLE:
          {
            glBindTexture( GL_TEXTURE_2D, w.marble_texture );
            tc[0] = v2{ 0.0f, 0.0f };
            tc[1] = v2{ 1.0f, 0.0f };
            tc[2] = v2{ 1.0f, 1.0f };
            tc[3] = v2{ 0.0f, 1.0f };
          }
          break;
      }
    }
    f32 vertexes[100];
    memcpy( vertexes, r.p0.Elements, sizeof(r.p0) );
    memcpy( vertexes + 3, color.Elements, sizeof(color) );
    memcpy( vertexes + 6, r.n.Elements, sizeof(r.n) );
    memcpy( vertexes + 9, tc[0].Elements, sizeof( tc[0] ) );
  
    memcpy( vertexes + 11, r.p1.Elements, sizeof(r.p0) );
    memcpy( vertexes + 14, color.Elements, sizeof(color) );
    memcpy( vertexes + 17, r.n.Elements, sizeof(r.n) );
    memcpy( vertexes + 20, tc[1].Elements, sizeof( tc[0] ) );

    memcpy( vertexes + 22, r.p2.Elements, sizeof(r.p0) );
    memcpy( vertexes + 25, color.Elements, sizeof(color) );
    memcpy( vertexes + 28, r.n.Elements, sizeof(r.n) );
    memcpy( vertexes + 31, tc[2].Elements, sizeof( tc[0] ) );

    memcpy( vertexes + 33, r.p3.Elements, sizeof(r.p0) );
    memcpy( vertexes + 36, color.Elements, sizeof(color) );
    memcpy( vertexes + 39, r.n.Elements, sizeof(r.n) );
    memcpy( vertexes + 42, tc[3].Elements, sizeof( tc[0] ) );
#if 0
    array_push( w.color_vertex_data, r.p0 );
    array_push( w.color_vertex_data, color );
    array_push( w.color_vertex_data, r.n );

    array_push( w.color_vertex_data, r.p1 );
    array_push( w.color_vertex_data, color );
    array_push( w.color_vertex_data, r.n );

    array_push( w.color_vertex_data, r.p2 );
    array_push( w.color_vertex_data, color );
    array_push( w.color_vertex_data, r.n );


    array_push( w.color_vertex_data, r.p3 );
    array_push( w.color_vertex_data, color );
    array_push( w.color_vertex_data, r.n );
#endif
    // push the indices

    glBindVertexArray( w.rect_vao );
    glBindBuffer( GL_ARRAY_BUFFER, w.rect_vbo );
    glBufferData( GL_ARRAY_BUFFER, 
                  sizeof( f32 ) * 44,
                  vertexes,
                  GL_STATIC_DRAW
                );

    glUniformMatrix4fv( simple_color_shader_info.mvp_loc,
                        1,GL_FALSE,
                        HMM_MAT4_PTR(vp) );
    glDrawElements(GL_TRIANGLES,6,
                   GL_UNSIGNED_INT, 0 );

  }


  glUseProgram( 0 );

  // Create line vertex data for rendering
  
  value = 0;
  for ( int i = 0; i < array_length( w.temp_color_quads ); i++ ){
    const ColorQuad &quad = w.temp_color_quads[i];

    // Push the first triangle
    array_push( w.color_vertex_data, quad.p0 );
    array_push( w.color_vertex_data, quad.color );
    array_push( w.color_vertex_data, quad.n );

    array_push( w.color_vertex_data, quad.p1 );
    array_push( w.color_vertex_data, quad.color );
    array_push( w.color_vertex_data, quad.n );

    array_push( w.color_vertex_data, quad.p2 );
    array_push( w.color_vertex_data, quad.color );
    array_push( w.color_vertex_data, quad.n );


    array_push( w.color_vertex_data, quad.p3 );
    array_push( w.color_vertex_data, quad.color );
    array_push( w.color_vertex_data, quad.n );

    // push the indices
    array_push( w.color_vertex_indices, value );
    array_push( w.color_vertex_indices, value + 1 );
    array_push( w.color_vertex_indices, value + 2 );
    array_push( w.color_vertex_indices, value + 2 );
    array_push( w.color_vertex_indices, value + 3 );
    array_push( w.color_vertex_indices, value );

    value += 4;
  }

  for ( int i = 0; i < array_length( w.perm_color_quads ); i++ ){
    const ColorQuad &quad = w.perm_color_quads[i];

    // Push the first triangle
    array_push( w.color_vertex_data, quad.p0 );
    array_push( w.color_vertex_data, quad.color );
    array_push( w.color_vertex_data, quad.n );

    array_push( w.color_vertex_data, quad.p1 );
    array_push( w.color_vertex_data, quad.color );
    array_push( w.color_vertex_data, quad.n );

    array_push( w.color_vertex_data, quad.p2 );
    array_push( w.color_vertex_data, quad.color );
    array_push( w.color_vertex_data, quad.n );


    array_push( w.color_vertex_data, quad.p3 );
    array_push( w.color_vertex_data, quad.color );
    array_push( w.color_vertex_data, quad.n );

    array_push( w.color_vertex_indices, value );
    array_push( w.color_vertex_indices, value + 1 );
    array_push( w.color_vertex_indices, value + 2 );
    array_push( w.color_vertex_indices, value + 2 );
    array_push( w.color_vertex_indices, value + 3 );
    array_push( w.color_vertex_indices, value );

    value += 4;

  }

  array_push( w.color_vertex_modes, (GLenum)GL_TRIANGLES );
  array_push( w.index_stack, array_length( w.color_vertex_indices ) );


  for ( int i = 0; i < array_length( w.lines ); i++ ){
    // Push the ending point

    array_push( w.color_vertex_data, w.lines[i].start );
    array_push( w.color_vertex_data, w.lines[i].color );
    array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );
    array_push( w.color_vertex_indices, value++ );
    
    // Push the ending point
    array_push( w.color_vertex_data,w.lines[i].end );
    array_push( w.color_vertex_data, w.lines[i].color );

    // Line normal, not necessary and only for debuggin purposes
    // and another shader prob cost too much
    array_push( w.color_vertex_data, v3{0.0f,0.0f,0.0f} );
    array_push( w.color_vertex_indices, value++ );

  }

  for ( uint i = 0; i < array_length( w.boxes ); i++ ){
    world_draw_AABB( w, w.boxes[i], v3{1.0f,1.0f,1.0f }, &value );
  }
  
  array_push( w.color_vertex_modes, (GLenum)GL_LINES );
  array_push( w.index_stack, array_length(w.color_vertex_indices) );

  glUseProgram( nl_color_shader_info.id );
  glBindVertexArray( w.color_vao );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, w.color_ebo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                sizeof(uint) * array_length( w.color_vertex_indices ),
                w.color_vertex_indices,
                GL_STATIC_DRAW );

  glBindBuffer( GL_ARRAY_BUFFER, w.color_vbo );
  glBufferData( GL_ARRAY_BUFFER, 
                sizeof(v3) * array_length( w.color_vertex_data ),
                w.color_vertex_data,
                GL_STATIC_DRAW
              );

  glUniformMatrix4fv( nl_color_shader_info.mvp_loc,
                      1,GL_FALSE,
                      HMM_MAT4_PTR(vp) );
#if 1
  uint prev = 0;
  for ( int i = 0; i < array_length( w.color_vertex_modes);i++ ){
    GLenum mode = w.color_vertex_modes[i];
    uint len = w.index_stack[i];
    uint count = len - prev;
    
    glDrawElements( mode, count,
                    GL_UNSIGNED_INT, (void *)( prev * sizeof(uint) ));

    prev = len;
  }
#else
  for ( int i = 1; i < array_length( w.color_vertex_modes ); i++ ){
    if ( mode != w.color_vertex_modes[i] ){
      len = i - start; 
      glDrawElements( mode, count, GL_UNSIGNED_INT, index );
      start = i;
      mode = w.color_vertex_modes[i];
    }
  }

  if ( start < array_length( w.color_vertex_modes ) ){
    glDrawArrays( mode, start, array_length( w.color_vertex_modes ) - start );
  }
#endif
  glUseProgram( 0 );
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}


bool hit_world(
    const World &w,
    const Ray &r,
    f32 tmin, f32 tmax,
    HitRecord &record )
{
  f32 max = tmax;
  HitRecord temp_record;
  bool hit_anything = false;
  if ( hit_grid( w.grid, r, tmin, max, temp_record ) ){
    hit_anything = true;
      max = temp_record.t;
      record = temp_record;
      record.obj.object = ( void *)&w.grid;
      record.obj.type = OBJECT_GRID;
  }

  for ( int i = 0; i < array_length( w.cubes ); i++ ){
    if ( hit_cube( w.cubes[i], r, tmin, max, temp_record ) ){
      hit_anything = true;
        max = temp_record.t;
        record = temp_record;
        record.obj.index = i;
        record.obj.type = OBJECT_CUBE_INSTANCE;
    }
  }

  for ( int i = 0; i < array_length( w.spheres ); i++ ){
    if ( hit_sphere( w.spheres[i], r, tmin, max, temp_record ) ){
      hit_anything = true;
        max = temp_record.t;
        record = temp_record;
        record.obj.index = i;
        record.obj.type = OBJECT_SPHERE;
    }
  }
  for ( int i = 0 ; i < array_length( w.rects ); i++ ){
    if ( hit_rect( w.rects[i], r, tmin, max, temp_record ) ){
      hit_anything = true;
        max = temp_record.t;
        record = temp_record;
        record.obj.index = i;
        record.obj.type = OBJECT_RECT;
    }
  }
  for ( int i = 0; i < array_length( w.light_cubes ); i++ ){
    if ( hit_cube( w.light_cubes[i], r, tmin, max, temp_record ) ){
      hit_anything = true;
        max = temp_record.t;
        record = temp_record;
        record.obj.index = i;
        record.obj.type = OBJECT_LIGHT_CUBE_INSTANCE;
    }
  }

  for ( int i = 0; i < array_length( w.light_spheres ); i++ ){
    if ( hit_sphere( w.light_spheres[i], r, tmin, max, temp_record ) ){
      hit_anything = true;
        max = temp_record.t;
        record = temp_record;
        record.obj.index = i;
        record.obj.type = OBJECT_LIGHT_SPHERE;
    }
  }

  for ( int i = 0 ; i < array_length( w.light_rects ); i++ ){
    if ( hit_rect( w.light_rects[i], r, tmin, max, temp_record ) ){
      hit_anything = true;
        max = temp_record.t;
        record = temp_record;
        record.obj.index = i;
        record.obj.type = OBJECT_LIGHT_RECT;
    }
  }
  return hit_anything;
}


void dump_texture_data(
    DumpObjectData &data,
    v3 *tex_store,
    uint index )
{
  switch ( data.texture_type ){
    case DumpObjectData::TEXTURE_PLAIN_COLOR:
      data.texture_data.color = tex_store[index];
      break;
    case DumpObjectData::TEXTURE_CHECKER:
      data.texture_data.checker_color[0] = v3{0.0f,0.0f,0.0f};
      data.texture_data.checker_color[1] = v3{1.0f,1.0f,1.0f};
      break;
    case DumpObjectData::TEXTURE_MARBLE:
      data.texture_data.marble_color = tex_store[index];
      break;
    default:
      break;
  }
}

void print_dump_data( DumpObjectData *data ){
}
void convert_material_to_dump(
    DumpObjectData &data,
    const Material &material )
{
  switch ( material.type ){
    case Material::GLASS:
      printf("Dumping Glass material with value %d\n",
          DumpObjectData::MATERIAL_GLASS );
      data.material_type = DumpObjectData::MATERIAL_GLASS;
      data.material_data.ri = 1.33f;
      break;
    case Material::PURE_DIFFUSE:
      data.material_type = DumpObjectData::MATERIAL_PURE_DIFFUSE;
      break;
    case Material::METALLIC:
      data.material_type = DumpObjectData::MATERIAL_METALLIC;
      data.material_data.fuzz = 0.01f;
      break;
  }
}

void convert_texture_to_dump(
    DumpObjectData &data,
    const Texture &texture )
{
  switch ( texture.type ){
    case Texture::COLOR:
      printf("Dumping color texture with value %d\n",
          DumpObjectData::TEXTURE_PLAIN_COLOR );
      data.texture_type= DumpObjectData::TEXTURE_PLAIN_COLOR;
      data.texture_data.color = texture.color;
      break;
    case Texture::MARBLE:
      printf("Dumping marble texture with value %d\n",
          DumpObjectData::TEXTURE_MARBLE );
      data.texture_type = DumpObjectData::TEXTURE_MARBLE;
      data.texture_data.marble_color = texture.color;
      break;
    case Texture::CHECKER:
      printf("Dumping checker texture with value %d\n",
          DumpObjectData::TEXTURE_CHECKER );
      data.texture_type = DumpObjectData::TEXTURE_CHECKER;
      data.texture_data.checker_color[0] = texture.color;
      data.texture_data.checker_color[1] = v3{0.0f,0.0f,0.0f};
      data.texture_data.freq = 8.0f;
      break;
  }
}

void world_dump_rect_data( const World &w, DumpObjectData *store ){
  Rectangle *rects = w.rects;
  for ( uint i = 0; i < array_length( rects ); i++ ){
    Material &material = w.rect_materials[i];
    Texture &texture = material.texture;
    DumpObjectData data;
    data.type = DumpObjectData::RECTANGLE;
    data.object_data.p0 = rects[i].p0;
    data.object_data.s1 = rects[i].s1;
    data.object_data.s2 = rects[i].s2;
    data.object_data.n = rects[i].n;
    data.object_data.l1= rects[i].l1;
    data.object_data.l2= rects[i].l2;

    convert_material_to_dump( data, material );
    convert_texture_to_dump( data, texture );

    array_push( store, data );
  }
}

void world_dump_sphere_data( const World &w, DumpObjectData *store ){
  Sphere *spheres= w.spheres;
  for ( uint i = 0; i < array_length( spheres ); i++ ){
    Material &material = w.sphere_materials[i];
    Texture &texture = material.texture;
    DumpObjectData data;
    data.type = DumpObjectData::SPHERE;
    data.object_data.center = spheres[i].c;
    data.object_data.radius = spheres[i].r;
    convert_material_to_dump( data, material );
    convert_texture_to_dump( data, texture );
    array_push( store, data );
  }
}

void apply_cube_transform_to_rect( Rectangle &r, Cube &cube ){
  q4 quat = HMM_NormalizeQuaternion( cube.orientation );
  r.l1 = cube.length;
  r.l2 = cube.length;

  r.p0 = rotate_vector_by_quaternion( r.p0,quat );
  r.s1 = rotate_vector_by_quaternion( r.s1, quat );
  r.s2 = rotate_vector_by_quaternion( r.s2, quat );


  r.n = rotate_vector_by_quaternion( r.n, quat );
  r.p0 += cube.pos;

}


void world_dump_cube_data( const World &w, DumpObjectData *store ){
  for ( uint i = 0; i < array_length( w.cubes ); i++ ){
    Rectangle rects[6];
    generate_cube_rects( rects, w.cubes[i].length );
    Material &material = w.cube_materials[i];
    Texture &texture = material.texture;

    for ( uint faces = 0; faces < 6; faces++ ){
      DumpObjectData data;
      data.type = DumpObjectData::RECTANGLE;
  // Apply cube transformation to the rectangle
      apply_cube_transform_to_rect(rects[faces], w.cubes[i] );

      data.object_data.p0 = rects[faces].p0;
      data.object_data.s1 = rects[faces].s1;
      data.object_data.s2 = rects[faces].s2;
      data.object_data.n =  rects[faces].n;
      data.object_data.l1=  rects[faces].l1;
      data.object_data.l2=  rects[faces].l2;


      convert_material_to_dump( data, material );
      switch ( texture.type ){
        case Texture::COLOR:
          printf("Dumping color texture with value %d\n",
              DumpObjectData::TEXTURE_PLAIN_COLOR );
          data.texture_type= DumpObjectData::TEXTURE_PLAIN_COLOR;
          data.texture_data.color = texture.face_colors[faces];
          break;
        case Texture::MARBLE:
          printf("Dumping marble texture with value %d\n",
              DumpObjectData::TEXTURE_MARBLE );
          data.texture_type = DumpObjectData::TEXTURE_MARBLE;
          data.texture_data.marble_color = texture.face_colors[faces];
          break;
        case Texture::CHECKER:
          printf("Dumping checker texture with value %d\n",
              DumpObjectData::TEXTURE_CHECKER );
          data.texture_type = DumpObjectData::TEXTURE_CHECKER;
          data.texture_data.checker_color[0] = texture.face_colors[faces];
          data.texture_data.checker_color[1] = v3{0.0f,0.0f,0.0f};
          data.texture_data.freq = 2.0f;
          break;
      }
      array_push( store, data );
    }
  }
}

void world_dump_light_rect_data(
    const World &w,
    DumpObjectData *store )
{
  Rectangle *rects = w.light_rects;
  for ( uint i = 0; i < array_length( rects ); i++ ){
    DumpObjectData data;
    data.type = DumpObjectData::RECTANGLE;
    data.object_data.p0 = rects[i].p0;
    data.object_data.s1 = rects[i].s1;
    data.object_data.s2 = rects[i].s2;
    data.object_data.n = rects[i].n;
    data.object_data.l1= rects[i].l1;
    data.object_data.l2= rects[i].l2;
    data.material_type = DumpObjectData::MATERIAL_DIFFUSE_LIGHT;
    data.material_data.diff_light_color = w.light_rect_color[ i ] * 10.0f;
    array_push( store, data );
  }
}

void world_dump_light_sphere_data(
    const World &w,
    DumpObjectData *store )
{
  Sphere *spheres= w.light_spheres;
  for ( uint i = 0; i < array_length( spheres ); i++ ){
    DumpObjectData data;
    data.type = DumpObjectData::SPHERE;
    data.object_data.center = w.light_spheres[i].c;
    data.object_data.radius = w.light_spheres[i].r;

    data.material_type = DumpObjectData::MATERIAL_DIFFUSE_LIGHT;
    data.material_data.diff_light_color = w.light_sphere_color[ i ] * 10.0f;
    array_push( store, data );
  }
}

void world_dump_light_cube_data(
    const World &w,
    DumpObjectData *store )
{
  Cube *cubes = w.light_cubes;
  for ( uint i = 0; i < array_length( cubes ); i++ ){
    for ( uint faces = 0; faces < 6; faces++ ){
      DumpObjectData data;
      data.type = DumpObjectData::RECTANGLE;
  // Apply cube transformation to the rectangle
      Rectangle r = CubeRects[ faces ];
      apply_cube_transform_to_rect(r, w.light_cubes[i] );
      data.object_data.p0 = r.p0;
      data.object_data.s1 = r.s1;
      data.object_data.s2 = r.s2;
      data.object_data.n =  r.n;
      data.object_data.l1=  r.l1;
      data.object_data.l2=  r.l2;

      data.material_type = DumpObjectData::MATERIAL_DIFFUSE_LIGHT;
      data.material_data.diff_light_color = w.light_cube_color[i] * 10.0f;
      array_push( store, data );
    }
  }
}


void world_dump_scene_to_file(
    const World &w,
    const char *file )
{
  DumpObjectData *data_store = array_allocate( DumpObjectData, 100 );
  DumpCameraData cam;
  cam.look_from = w.view_camera.P;
  cam.look_at = w.view_camera.P + w.view_camera.F;
  cam.z = w.view_near_plane;
  cam.vfov = w.view_vfov;
  cam.aspect_ratio = (f32)ScreenWidth/ScreenHeight;
  cam.aperture = 0.0f;
  cam.focal_dist = 1.0f;

  world_dump_rect_data( w, data_store );
  world_dump_cube_data( w, data_store );
  world_dump_sphere_data( w, data_store );
  world_dump_light_cube_data( w, data_store );
  world_dump_light_sphere_data( w, data_store );
  world_dump_light_rect_data( w, data_store );

  FILE *fp = fopen(file,"wb");
  if( !fp ){
    perror("Unable to open file for output!: ");
    array_free( data_store );
    return;
  }
  fwrite( &cam, sizeof(cam), 1, fp );
  uint32 len = array_length( data_store );
  fwrite( &len, sizeof(len), 1, fp );
  fwrite( data_store,
      sizeof(*data_store),
      array_length( data_store ),
      fp );
  fclose( fp );
  array_free( data_store );
  return;
}
uint8 *load_image( const char *fname, int *width, int *height, int *channels ){
  uint8 *data = (uint8 *)stbi_load( fname, width, height, channels, 4 );
  if ( data == NULL ){
    fprintf( stderr, "Unable to load image %s!", fname );
    return NULL;
  } else {
    fprintf( stdout, "Image loaded successfully. width = %d, height = %d\n", *width, *height );
  }
  return data;
}

Image create_image( const char *fname ){
  Image image;
  image.data = load_image( fname, &image.w, &image.h, &image.channels );
  return image;
}

Texture cube_color_texture( ){
  Texture t;
  t.type = Texture::COLOR;
  t.face_colors[0] = v3{prng_float(), prng_float(), prng_float() };
  t.face_colors[1] = v3{prng_float(), prng_float(), prng_float() };
  t.face_colors[2] = v3{prng_float(), prng_float(), prng_float() };
  t.face_colors[3] = v3{prng_float(), prng_float(), prng_float() };
  t.face_colors[4] = v3{prng_float(), prng_float(), prng_float() };
  t.face_colors[5] = v3{prng_float(), prng_float(), prng_float() };
  return t;
}

Texture sphere_color_texture( ){
  Texture t;
  t.type = Texture::COLOR;
  t.color = v3{prng_float(), prng_float(), prng_float() };
  return t;
}

Texture rectangle_color_texture( ){
  Texture t;
  t.type = Texture::COLOR;
  t.color = v3{prng_float(), prng_float(), prng_float() };
  return t;
}

int main(){
  prng_seed();
  generate_sphere_vertices();
  bool dump_scene_data = false;
  uint dump_count = 0;
  glfwInit();
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  GLFWwindow* window = glfwCreateWindow( ScreenWidth,ScreenHeight,
                       "OpenGl", NULL, NULL );
  if ( !window ){
      print_error("Unable to open window!");
  }

  glfwMakeContextCurrent( window );
  glfwSetFramebufferSizeCallback( window, resizeCallback );
  //glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
  glfwSetCursorPosCallback( window, mouse_callback );
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetKeyCallback( window, key_callback );
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
      print_error("Failed to initialize GLAD");
      return -1;
  }
#if ENABLE_GL_DEBUG_PRINT
  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
  {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE,
                          GL_DONT_CARE,
                          GL_DONT_CARE,
                          0, nullptr,
                          GL_TRUE);
  } else {
    print_error("Unable to set debug context");
  }
#endif
  glEnable( GL_DEPTH_TEST );

  Quad_elem_indices= ( uint * )malloc( 6 * 1000 * sizeof( uint ) );
  uint t[] = { 0, 1, 2, 2, 3, 0 };
  uint *tmp = Quad_elem_indices;
  for ( int i = 0; i < 1000; i++ ){
    for ( int j = 0; j < 6; j++ ){
      *tmp = t[j] + 4 * i;
      tmp++;
    }
  }
  glGenBuffers(1,&quad_elem_buffer_index);
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, quad_elem_buffer_index );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
                sizeof( uint ) * 100,
                Quad_elem_indices,
                GL_STATIC_DRAW
              );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

  glGenBuffers( 1, &sphere_element_buffer );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, sphere_element_buffer );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, 
                sizeof( uint ) * array_length( SphereIndices ),
                SphereIndices,
                GL_STATIC_DRAW );
  if ( create_grid_program() == -1 ){
    return -1;
  }
  if ( create_simple_color_shader_program() == -1 ){
    return -1;
  }
  if ( create_nl_color_shader_program() == -1 ){
    return -1;
  }
  
  // Load images for texture
  //
  uint8 white_texture_data[] = { 255, 255, 255, 255 };
  Image checker_image = create_image( "./assets/checkers.png" );
  Image white_marble_image = create_image( "./assets/White-Marble-1024.png" );
  Image white_image = create_image( "./assets/white.png" );
  // create textures
  Texture checker_texture;
  checker_texture.type = Texture::CHECKER;
  checker_texture.image = checker_image;

  Texture white_marble_texture;
  white_marble_texture.type = Texture::MARBLE;
  white_marble_texture.image = white_marble_image;

  Texture color_texture;
  color_texture.type = Texture::COLOR;
  // create_world
  World *world = (World *)malloc( sizeof(World) );
  World &w = *world;
  w.amb_light = { 0.3f, 0.3f, 0.3f };
  w.grid = create_grid(
            AARect::PLANE_ZX,
            AABB( v3{ -10.0f, 0.0f, -10.0f }, v3{ 10.0f, 0.0f, 10.0f } ),
            0.0f,
            0.1f,
            v3{0.0f,0.0f,1.0f} );
  
  w.ui_vfov = 45;
  w.ui_near_plane = 0.1f;
  w.ui_far_plane = 100.0f;
  w.ui_perspective= HMM_Perspective(45,
                  (float)ScreenWidth/ScreenHeight,
                  0.1f, 10.0f );
  w.view_vfov = 45;
  w.view_near_plane = 0.1f;
  w.view_far_plane = 100.0f;
  w.view_perspective = HMM_Perspective(w.view_vfov,
                  (float)ScreenWidth/ScreenHeight,
                  w.view_near_plane, w.view_far_plane );
  w.perspective = w.ui_perspective;

  uint current_screen_width = ScreenWidth;
  uint current_screen_height = ScreenHeight;
  w.ui_camera = Camera( 
            v3{ 0.0f, 0.5f, 5.0f },
            v3{ 0.0f, 0.5f, -1.0f },
            v3{ 0.0f, 1.0f, 0.0f }
            );

  w.view_camera = Camera( 
            v3{ 0.0f, 0.5f, 5.0f },
            v3{ 0.0f, 0.5f, -1.0f },
            v3{ 0.0f, 1.0f, 0.0f }
            );


  Cube cube = create_cube_one_color( 0.5f, v3{1,0.25,0}, v3 {0,1,0} );

  w.cube.color[Cube::FRONT] = v3{0.82f, 0.36f, 0.45f};
  w.cube.color[Cube::BACK] = v3{0.82f, 0.36f, 0.45f};
  w.cube.color[Cube::LEFT] = v3{0.32f, 0.32f, 0.86f};
  w.cube.color[Cube::RIGHT] = v3{0.32f, 0.32f, 0.86f};
#if 0
  world_generate_cube_data(w.cube_vao, w.cube_vbo, w.cube );
  world_add_cube_vertex_data( w.cube_vao, w.cube_vbo, w.cube );
#endif

  for ( uint i = 0; i < _OBJECT_MAX; i++ ){
    w.model_matrices[ i ] = array_allocate( m4, 10 );
  }

  w.cubes = array_allocate( Cube, 10 );
  w.rects = array_allocate( Rectangle, 10 );
  w.boxes = array_allocate( AABB, 10 );
  w.lines = array_allocate( Line, 10 );
  w.spheres = array_allocate( Sphere, 10 );
  w.spheres_transform = array_allocate( m4, 10 );
  w.sphere_colors = array_allocate( v3, 10 );
  w.rect_colors = array_allocate(v3,10 );

  w.cube_materials = array_allocate( Material, 10 );
  w.sphere_materials = array_allocate( Material, 10 );
  w.rect_materials= array_allocate( Material, 10 );

  w.light_cubes = array_allocate( Cube, 10 );
  w.light_rects = array_allocate( Rectangle, 10 );
  w.light_spheres = array_allocate( Sphere, 10 );
  w.light_pos = array_allocate( v3, 10 );
  w.light_colors = array_allocate( v3, 10 );

  w.light_cube_color = array_allocate( v3, 10 );
  w.light_sphere_color = array_allocate( v3, 10 );
  w.light_rect_color = array_allocate( v3, 10 );

  w.light_cubes_vao = array_allocate( uint, 10 );
  w.light_spheres_vao = array_allocate( uint, 10 );
  w.light_cubes_vbo = array_allocate( uint, 10 );
  w.light_spheres_vbo = array_allocate( uint, 10 );

  w.temp_color_quads = array_allocate( ColorQuad, 10 );
  w.perm_color_quads = array_allocate( ColorQuad, 10 );

  w.cubes_vao = array_allocate( uint, 10 );
  w.cubes_vbo = array_allocate( uint, 10 );
  w.spheres_vao = array_allocate( uint, 10 );
  w.spheres_vbo = array_allocate( uint, 10 );


  w.rect_flip_normal = 0;

  w.color_vertex_data = array_allocate( v3, 1000 );
  w.color_vertex_indices = array_allocate( uint, 3000 );

  w.index_stack = array_allocate( uint, 10 );
  w.color_vertex_modes = array_allocate( GLenum ,10 );
  
  glGenVertexArrays(1, &w.color_vao );
  glGenBuffers( 1, &w.color_vbo );
  glGenBuffers( 1, &w.color_ebo );

  glBindVertexArray( w.color_vao );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, w.color_ebo );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                1000  * sizeof(v3),
                NULL, GL_STATIC_DRAW );

  glBindBuffer( GL_ARRAY_BUFFER, w.color_vbo );
  glBufferData( GL_ARRAY_BUFFER,
                2000 * sizeof(v3),
                NULL, GL_STATIC_DRAW );

  glEnableVertexAttribArray( simple_color_shader_info.pos_id );
  glEnableVertexAttribArray( simple_color_shader_info.color_id );
  glEnableVertexAttribArray( simple_color_shader_info.normal_id );

  glVertexAttribPointer( simple_color_shader_info.pos_id,
                         3, GL_FLOAT, GL_FALSE,
                         3 * sizeof(v3),
                         (void *)( 0 ) );

  glVertexAttribPointer( simple_color_shader_info.color_id,
                         3, GL_FLOAT, GL_FALSE,
                         3 * sizeof(v3),
                         (void *)(sizeof(v3) ) );
  glVertexAttribPointer( simple_color_shader_info.normal_id,
                         3, GL_FLOAT, GL_FALSE,
                         3 * sizeof(v3),
                         (void *)(2 * sizeof(v3) ) );

  glBindVertexArray( 0 );

  glGenVertexArrays( 1, &w.rect_vao );
  glGenBuffers( 1, &w.rect_vbo );
  glBindVertexArray( w.rect_vao );

  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, quad_elem_buffer_index );

  glBindBuffer( GL_ARRAY_BUFFER, w.rect_vbo );
  glBufferData( GL_ARRAY_BUFFER,
                200 * sizeof(v3),
                NULL, GL_STATIC_DRAW );

  glEnableVertexAttribArray( simple_color_shader_info.pos_id );
  glEnableVertexAttribArray( simple_color_shader_info.color_id );
  glEnableVertexAttribArray( simple_color_shader_info.normal_id );
  glEnableVertexAttribArray( simple_color_shader_info.tex_coords_id);

  glVertexAttribPointer( simple_color_shader_info.pos_id,
                         3, GL_FLOAT, GL_FALSE,
                         3 * sizeof(v3) + sizeof(v2),
                         (void *)( 0 ) );

  glVertexAttribPointer( simple_color_shader_info.color_id,
                         3, GL_FLOAT, GL_FALSE,
                         3 * sizeof(v3) + sizeof(v2),
                         (void *)(sizeof(v3) ) );

  glVertexAttribPointer( simple_color_shader_info.normal_id,
                         3, GL_FLOAT, GL_FALSE,
                         3 * sizeof(v3) + sizeof(v2),
                         (void *)(2 * sizeof(v3) ) );

  glVertexAttribPointer( simple_color_shader_info.tex_coords_id,
                         2, GL_FLOAT, GL_FALSE,
                         3 * sizeof(v3) + sizeof(v2),
                         (void *)(3 * sizeof(v3) ) );

  glBindVertexArray( 0 );

  // generate opengl Textures
  glGenTextures( 1, &w.white_texture );
  glGenTextures( 1, &w.checker_texture);
  glGenTextures( 1, &w.marble_texture);

  glBindTexture(GL_TEXTURE_2D, w.white_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0,
      GL_RGBA, 1, 1,
      0, GL_RGBA,
      GL_UNSIGNED_BYTE, white_texture_data );
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, w.checker_texture );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0,
      GL_RGBA, checker_image.w, checker_image.h,
      0, GL_RGBA,
      GL_UNSIGNED_BYTE, checker_image.data );
  glGenerateMipmap(GL_TEXTURE_2D);


  glBindTexture(GL_TEXTURE_2D, w.marble_texture );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0,
      GL_RGBA, white_marble_image.w, white_marble_image.h,
      0, GL_RGBA,
      GL_UNSIGNED_BYTE, white_marble_image.data );
  glGenerateMipmap(GL_TEXTURE_2D);


  Material m = create_material_diffuse( cube_color_texture() );
  world_add_cube( w, &cube, m );
  m = create_material_metallic( sphere_color_texture() );
  world_add_sphere( w,create_sphere( v3{0.0f,0.0f,0.0f}, 1.0f ) , m );
  m = create_material_diffuse( rectangle_color_texture() );
  world_add_rect( w,create_rectangle( v3{-1.0f,2.0f,3.0f } ), m );

  w.model_matrices[OBJECT_CUBE_INSTANCE][0] = cube.base_transform;
  world_generate_grid_data(w, w.grid );
#define WORLD_SET_STATE_FREE_VIEW \
  do {\
    w.camera = &w.ui_camera;\
    w.perspective = w.ui_perspective;\
    w.state = World::STATE_FREE_VIEW;\
    w.show_imgui = false;\
    w.camera->should_rotate = true;\
    w.camera->should_move = true;\
    w.is_selected = false;\
    glfwSetCursorPosCallback( window, mouse_callback );\
  } while ( 0 )

#define WORLD_SET_STATE_DETACHED\
  do {\
    w.camera = &w.ui_camera;\
    w.perspective = w.ui_perspective;\
    w.state = World::STATE_DETACHED;\
    w.show_imgui = true;\
    w.camera->should_rotate = false;\
    w.camera->should_move = true;\
    w.is_selected = false;\
    glfwSetCursorPosCallback( window, NULL );\
  } while ( 0 )

#define WORLD_SET_STATE_SELECTED\
  do {\
    w.camera = &w.ui_camera;\
    w.perspective = w.ui_perspective;\
    w.state = World::STATE_SELECTED;\
    w.show_imgui = true;\
    w.camera->should_rotate = false;\
    w.camera->should_move = false;\
    glfwSetCursorPosCallback( window, NULL );\
  } while ( 0 )

#define WORLD_SET_STATE_ON_HOLD\
  do {\
    w.camera = &w.ui_camera;\
    w.perspective = w.ui_perspective;\
    w.state = World::STATE_ON_HOLD;\
    w.show_imgui = false;\
    w.camera->should_rotate = false;\
    w.camera->should_move = true;\
    w.is_holding = true;\
    glfwSetCursorPosCallback( window, NULL );\
  } while ( 0 )

#define WORLD_SET_STATE_VIEW_CAMERA\
  do {\
    w.camera = &w.view_camera;\
    w.perspective = w.view_perspective;\
    w.state = World::STATE_VIEW_CAMERA;\
    w.show_imgui = true;\
    w.camera->should_rotate = true;\
    w.camera->should_move = true;\
    w.is_holding = false;\
    glfwSetCursorPosCallback( window, mouse_callback );\
  } while ( 0 )
  

  glBindBuffer( GL_ARRAY_BUFFER, 0 );
  int viewport[4];
  glGetIntegerv( GL_VIEWPORT, viewport);
  float dt = 0;
  float current = glfwGetTime();
  m4 vp = HMM_Mat4d(1.0f);
  double cp[2];
  glfwGetCursorPos( window, &cp[0], &cp[1] );
  f32 camera_sensitivity = 0.5f;

  uint8 *key_map = (uint8 *)malloc( sizeof(uint8) * 400 );
  memset( key_map, 0, 400 * sizeof(uint8) );


  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 430 core");
  
  bool show_imgui = false;
  bool show_demo_window = false;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  w.camera = &w.ui_camera;
  WORLD_SET_STATE_FREE_VIEW;

  while ( !glfwWindowShouldClose( window  ) ){
    float now = glfwGetTime();
    dt = now - current;
    current = now;
    
    if ( current_screen_width != ScreenWidth ||
        current_screen_height != ScreenHeight )
    {
      w.ui_perspective= HMM_Perspective(w.ui_vfov,
                  (float)ScreenWidth/ScreenHeight,
                  w.ui_near_plane, w.ui_far_plane );
      w.view_perspective= HMM_Perspective(w.view_vfov,
                  (float)ScreenWidth/ScreenHeight,
                  w.view_near_plane, w.view_far_plane );

      if ( w.state == World::STATE_VIEW_CAMERA ){
        w.perspective = w.view_perspective;
      } else {
        w.perspective = w.ui_perspective;
      }
      current_screen_width = ScreenWidth;
      current_screen_height = ScreenHeight;
    }

    process_keyboard_input( window, key_map );

    //check camera events
    for ( int i = 0; i < Event_Count; i++ ){
      switch ( Event_Queue[i].type ){
        case MOUSE_MOVE:
        {
          f32 dx = Event_Queue[i].xp - cp[0];
          f32 dy = Event_Queue[i].yp - cp[1];
#if 0
          if ( w.state == World::STATE_VIEW_CAMERA ){
          w.camera->rotate( -camera_sensitivity*dx,
                         -camera_sensitivity*dy );
          } else {
          w.camera->rotate( camera_sensitivity*dx,
                         camera_sensitivity*dy );
          }
#endif
          w.camera->rotate( camera_sensitivity*dx,
                         camera_sensitivity*dy );
          cp[0] = Event_Queue[i].xp;
          cp[1] = Event_Queue[i].yp;
          break;
        }
        case MOUSE_RBUTTON_CLICK:
        { 
          if ( w.state == World::STATE_VIEW_CAMERA ) break;
          HitRecord record;
          v3 point = v3{ ( float )cp[0], (float)cp[1], 0.0f };
          v3 wp = HMM_UnProject( point, w.perspective * w.camera->transform(),
                                 ScreenWidth, ScreenHeight);
          Ray ray( w.camera->P, ( wp - w.camera->P ) );

          if ( hit_grid( w.grid, ray, 0.001f, 100.0f, record ) ){
            record.print();
            v3 p0 = grid_get_corner_point( w.grid, record.u, record.v );
            print_v3( p0 );
            v3 p1 = p0 + w.grid.dir1 * w.grid.w;
            v3 p2 = p0 + w.grid.dir1 * w.grid.w + w.grid.dir2 * w.grid.w;
            v3 p3 = p0 + w.grid.dir2 * w.grid.w;
            ColorQuad quad = { p0, p1, p2, p3,
                               v3{ 0.52f,0.15f,0.93f }, // color
                               w.grid.rect.n
                             };
            array_push( w.perm_color_quads, quad );
          }
#if 0
          array_push( w.lines,
                      create_line_from_ray( ray, 10.0f, v3{0,1.0f,0.0f} ) );
#endif
          break;
        }

        case MOUSE_LBUTTON_CLICK:
        {
          switch ( w.state ){
            case World::STATE_VIEW_CAMERA:
            case World::STATE_SELECTED:
              break; 
            case World::STATE_FREE_VIEW: case World::STATE_DETACHED:
            {
              glfwGetCursorPos( window, &cp[0], &cp[1] );
              HitRecord record;

              v3 point = v3{ ( float )cp[0], (float)cp[1], 0.0f };
              v3 wp = HMM_UnProject( point,
                  w.perspective * w.camera->transform(),
                  ScreenWidth, ScreenHeight );
              Ray ray( w.camera->P, ( wp - w.camera->P ) );
              if ( hit_world( w, ray, 0.001f, 100.0f, record ) ){
                w.is_selected = record.obj.type != OBJECT_GRID;

                switch ( record.obj.type ){
                  case OBJECT_LIGHT_CUBE_INSTANCE:
                  case OBJECT_CUBE_INSTANCE: 
                    WORLD_SET_STATE_SELECTED;
                    w.selected_object = record.obj;
                    w.selected_aabb = cube_get_AABB;
                    w.selected_move = cube_move;
                    w.selected_rotate = cube_rotate;
                    w.selected_scale = cube_scale;
                    if ( record.obj.type == OBJECT_CUBE_INSTANCE ){
                      w.cube_face_dropdown = 0;
                      w.selected_data = (void *)(
                          w.cubes + w.selected_object.index );
                      w.sel_cube_length =
                        w.cubes[w.selected_object.index].length;
                      w.cube_material_dropdown = 
                        w.cube_materials[w.selected_object.index].type;
                      w.cube_texture_dropdown = 
                        w.cube_materials[w.selected_object.index].texture.type;
                      w.cube_face_color=
                        w.cube_materials[w.selected_object.index].texture.face_colors[0];
                    } else {
                      w.light_cube_face_color =
                        w.light_cube_color[ w.selected_object.index ];
                      w.selected_data = (void *)(
                        w.light_cubes + w.selected_object.index );
                      w.sel_cube_length =
                      w.light_cubes[w.selected_object.index].length;
                    }
                    break;
                  case OBJECT_LIGHT_SPHERE:
                  case OBJECT_SPHERE:
                    WORLD_SET_STATE_SELECTED;
                    w.selected_object = record.obj;
                    w.selected_aabb = sphere_aabb;
                    w.selected_move = sphere_move;
                    w.selected_rotate = sphere_rotate;
                    w.selected_scale= sphere_scale;
                    if ( record.obj.type == OBJECT_SPHERE ){
                      w.sphere_face_color =
                        w.sphere_colors[ record.obj.index ];
                      w.selected_data = (void *)(
                          w.spheres + w.selected_object.index );
                      w.sel_sphere_radius = 
                          w.spheres[w.selected_object.index].r;
                      w.sphere_material_dropdown = 
                        w.sphere_materials[w.selected_object.index].type;
                      w.sphere_texture_dropdown = 
                        w.sphere_materials[w.selected_object.index].texture.type;
                    } else {
                      w.light_sphere_face_color = w.light_sphere_color[ record.obj.index ];
                      w.selected_data = (void *)(
                          w.light_spheres + w.selected_object.index );
                      w.sel_sphere_radius = 
                          w.light_spheres[w.selected_object.index].r;
                    }
                    break;
                  case OBJECT_LIGHT_RECT:
                  case OBJECT_RECT:
                    WORLD_SET_STATE_SELECTED;
                    w.selected_object = record.obj;
                    w.selected_aabb = rectangle_AABB;
                    w.selected_move = rectangle_move;
                    w.selected_rotate = rectangle_rotate;
                    w.selected_scale= rectangle_scale;
                    if ( record.obj.type == OBJECT_RECT ){
                      w.rect_face_color= w.rect_materials[ w.selected_object.index].texture.color;
                      w.selected_data = (void *)(
                        w.rects+ w.selected_object.index );
                      w.sel_rect_l1= 
                          w.rects[w.selected_object.index].l1;
                      w.sel_rect_l2= 
                          w.rects[w.selected_object.index].l2;
                      w.rect_material_dropdown = 
                        w.rect_materials[w.selected_object.index].type;
                      w.rect_texture_dropdown = 
                        w.rect_materials[w.selected_object.index].texture.type;
                    } else {
                      w.light_rect_face_color = 
                        w.light_rect_color[ record.obj.index ];
                      w.selected_data = (void *)(
                          w.light_rects+ w.selected_object.index );
                      w.sel_rect_l1= 
                          w.light_rects[w.selected_object.index].l1;
                      w.sel_rect_l2= 
                          w.light_rects[w.selected_object.index].l2;
                    }
                    break;

                  case OBJECT_GRID:
                    if ( w.state == World::STATE_FREE_VIEW ) break;
                    WORLD_SET_STATE_ON_HOLD;
                    // Display a see-through silhoulette
                    switch( w.hold_object_id ){
                      case 0: // This is a cube
                      {
                      }
                        break; 
                      case 1: // This is a AARect
                        break;
                    }
                    break;
                  default:
                    break;
                }
              }
              break;
            }

            case World::STATE_ON_HOLD:
            {
              glfwGetCursorPos( window, &cp[0], &cp[1] );
              HitRecord record;

              v3 point = v3{ ( float )cp[0], (float)cp[1], 0.0f };
              v3 wp = HMM_UnProject( point,
                  w.perspective * w.camera->transform(),
                  ScreenWidth, ScreenHeight );
              Ray ray( w.camera->P, ( wp - w.camera->P ) );

              if ( hit_world( w, ray, 0.001f, 100.0f, record ) ){
                if( record.obj.type == OBJECT_GRID ){
                  switch ( w.hold_object_id ){
                    case 0: 
                    {
                      Material m = 
                        create_material_diffuse( cube_color_texture() );
                      Cube cube = create_cube_one_color( 0.2f, record.p,
                        v3 {0,1,0} ) ;
                      world_add_cube( w, &cube, m );
                      WORLD_SET_STATE_SELECTED;
                      w.is_selected = true;
                      w.selected_object.type = OBJECT_CUBE_INSTANCE;
                      w.selected_object.index = array_length(w.cubes)-1;
                      w.selected_aabb = cube_get_AABB;
                      w.selected_move = cube_move;
                      w.selected_rotate = cube_rotate;
                      w.cube_face_dropdown = 0;
                      w.cube_material_dropdown = 
                        w.cube_materials[w.selected_object.index].type;
                      w.cube_texture_dropdown = 
                        w.cube_materials[w.selected_object.index].texture.type;
                      w.cube_face_color=
                        w.cube_materials[w.selected_object.index].texture.face_colors[0];
                      w.selected_data = (void *)(
                          w.cubes + w.selected_object.index );
                      w.sel_cube_length = 0.2f;
                      break;
                    }
                    case 1:
                    {
                      Material mat = create_material_metallic( 
                                    sphere_color_texture() );
                      Sphere s = create_sphere( record.p, 0.5f );
                      world_add_sphere( w,s,mat );
                      WORLD_SET_STATE_SELECTED;
                      w.sel_sphere_radius = 0.5f;
                      w.is_selected = true;
                      w.selected_object.type = OBJECT_SPHERE;
                      w.selected_object.index = array_length(w.spheres)-1;
                      w.selected_aabb = sphere_aabb;
                      w.selected_move = sphere_move;
                      w.selected_rotate = sphere_rotate;
                      w.sphere_material_dropdown = 
                        w.sphere_materials[w.selected_object.index].type;
                      w.sphere_texture_dropdown = 
                 w.sphere_materials[w.selected_object.index].texture.type;
                      w.sphere_face_color=
                   w.sphere_materials[w.selected_object.index].texture.color;

                      w.selected_data = (void *)(
                          w.spheres+ w.selected_object.index );
                      break;
                    }
                    case 2:
                    {
                      Material mat = create_material_diffuse( 
                          rectangle_color_texture() );
                      Rectangle r = create_rectangle( record.p );
                      world_add_rect( w,r,mat );
                      WORLD_SET_STATE_SELECTED;
                      w.sel_rect_l1 = r.l1;
                      w.sel_rect_l2 = r.l2;
                      w.is_selected = true;
                      w.selected_object.type = OBJECT_RECT;
                      w.selected_object.index = array_length(w.rects)-1;
                      w.selected_aabb = rectangle_AABB;
                      w.selected_move = rectangle_move;
                      w.selected_rotate = rectangle_rotate;
                      w.rect_face_color = mat.texture.color;
                      w.rect_material_dropdown = 
                        w.rect_materials[w.selected_object.index].type;
                      w.rect_texture_dropdown = 
                        w.rect_materials[w.selected_object.index].texture.type;
                      w.selected_data = (void *)(
                          w.rects + w.selected_object.index );
                      break;
                    }
                    case 3: 
                    {
                      Cube cube = create_cube_one_color( 0.2f, record.p,
                        v3 {1,1,1} ) ;
                      world_add_light_cube( w, &cube, v3{1,1,1} );
                      WORLD_SET_STATE_SELECTED;
                      w.sel_cube_length = 0.2f;
                      w.is_selected = true;
                      w.selected_object.type = OBJECT_LIGHT_CUBE_INSTANCE;
                      w.selected_object.index = array_length(w.light_cubes)-1;
                      w.selected_aabb = cube_get_AABB;
                      w.selected_move = cube_move;
                      w.selected_rotate = cube_rotate;
                      w.light_cube_face_color = v3{1,1,1};
                      w.selected_data = (void *)(
                          w.light_cubes + w.selected_object.index );
                      break;
                    }
                    case 4:
                    {
                      Sphere s = create_sphere( record.p, 0.5f );
                      world_add_light_sphere( w,s,v3{1.0f,1.0f,1.0f} );
                      WORLD_SET_STATE_SELECTED;
                      w.sel_sphere_radius = 0.5f;
                      w.is_selected = true;
                      w.selected_object.type = OBJECT_LIGHT_SPHERE;
                      w.selected_object.index=array_length(w.light_spheres)-1;
                      w.selected_aabb = sphere_aabb;
                      w.selected_move = sphere_move;
                      w.selected_rotate = sphere_rotate;
                      w.light_sphere_face_color=v3{1.0f,1.0f,1.0f};
                      w.selected_data = (void *)(
                          w.light_spheres+ w.selected_object.index );
                      break;
                    }
                    case 5:
                    {
                      Rectangle r = create_rectangle( record.p );
                      world_add_light_rect( w,r,v3{1,1,1} );
                      WORLD_SET_STATE_SELECTED;
                      w.is_selected = true;
                      w.sel_rect_l1 = r.l1;
                      w.sel_rect_l2 = r.l2;
                      w.selected_object.type = OBJECT_LIGHT_RECT;
                      w.selected_object.index = array_length(w.light_rects)-1;
                      w.selected_aabb = rectangle_AABB;
                      w.selected_move = rectangle_move;
                      w.selected_rotate = rectangle_rotate;
                      w.light_rect_face_color=v3{1.0f,1,1};
                      w.selected_data = (void *)(
                          w.light_rects + w.selected_object.index );
                      break;
                    }
                    default:
                        break;
                  }
                }
              }
              
            }
              break;
            default:
              break;
          }
        }
        break;


#define SELECTED_MOVE_DIST 0.02f
#define SELECTED_MOVE_UP v3{0.0f,1.0f,0.0f}
#define SELECTED_MOVE_RIGHT v3{1.0f,0.0f,0.0f}
#define SELECTED_MOVE_FRONT v3{0.0f,0.0f,-1.0f}
#if 1
        case KB_PRESS_W: case KB_REPEAT_W:
          if ( w.state != World::STATE_SELECTED ){
            w.camera->start_animate( 2, 0.2f ,100);
          } else {
            w.selected_move( w.selected_data,
                SELECTED_MOVE_DIST, SELECTED_MOVE_FRONT,
         w.model_matrices[w.selected_object.type][w.selected_object.index] );
          }
          break;

        case KB_PRESS_S:case KB_REPEAT_S:
          if ( w.state != World::STATE_SELECTED ){
            w.camera->start_animate( 2, -0.2f ,100);
          } else {
            w.selected_move( w.selected_data,
                -SELECTED_MOVE_DIST, SELECTED_MOVE_FRONT,
         w.model_matrices[w.selected_object.type][w.selected_object.index] );
          }
          break;

        case KB_PRESS_A:case KB_REPEAT_A:
          if ( w.state != World::STATE_SELECTED ){
            w.camera->start_animate( 0, -0.2f ,100);
          } else {
            w.selected_move( w.selected_data,
                -SELECTED_MOVE_DIST, SELECTED_MOVE_RIGHT,
         w.model_matrices[w.selected_object.type][w.selected_object.index] );
          }
          break;

        case KB_PRESS_D:case KB_REPEAT_D:
          if ( w.state != World::STATE_SELECTED ){
            w.camera->start_animate( 0, 0.2f ,100);
          } else {
            w.selected_move( w.selected_data,
                SELECTED_MOVE_DIST, SELECTED_MOVE_RIGHT,
         w.model_matrices[w.selected_object.type][w.selected_object.index] );
          }
          break;

        case KB_PRESS_I:case KB_REPEAT_I:
          if ( w.state != World::STATE_SELECTED ){
            w.camera->start_animate( 1, 0.2f ,300);
          } else {
            w.selected_move( w.selected_data,
                SELECTED_MOVE_DIST, SELECTED_MOVE_UP,
         w.model_matrices[w.selected_object.type][w.selected_object.index] );
          }
          break;
        case KB_PRESS_K:case KB_REPEAT_K:
          if ( w.state != World::STATE_SELECTED ){
            w.camera->start_animate( 1, -0.2f ,300);
          } else {
            w.selected_move( w.selected_data,
                -SELECTED_MOVE_DIST, SELECTED_MOVE_UP,
         w.model_matrices[w.selected_object.type][w.selected_object.index] );
          }
          break;
        case KB_PRESS_X: case KB_REPEAT_X:
          show_imgui = !show_imgui;
          break;
        case KB_PRESS_T:
          break;
        case KB_PRESS_R:
          WORLD_SET_STATE_DETACHED;
          w.object_select_dropdown = 0;
          break;
        case KB_PRESS_P:
          w.camera->print();
          break;
        case KB_RELEASE_W:
          if ( !( key_map[KB_KEY_S] || key_map[KB_KEY_A] ||
                  key_map[KB_KEY_D] || key_map[KB_KEY_I] ||
                  key_map[KB_KEY_K] )
             )
          {
            w.camera->state = Camera::STATIC;
          }
          break;
        case KB_RELEASE_S:
          if ( !( key_map[KB_KEY_W] || key_map[KB_KEY_A] ||
                  key_map[KB_KEY_D] || key_map[KB_KEY_I] ||
                  key_map[KB_KEY_K] )
             )
          {
            w.camera->state = Camera::STATIC;
          }
          break;
        case KB_RELEASE_A:
          if ( !( key_map[KB_KEY_S] || key_map[KB_KEY_W] ||
                  key_map[KB_KEY_D] || key_map[KB_KEY_I] ||
                  key_map[KB_KEY_K] )
             )
          {
            w.camera->state = Camera::STATIC;
          }
          break;
        case KB_RELEASE_D:
          if ( !( key_map[KB_KEY_S] || key_map[KB_KEY_A] ||
                  key_map[KB_KEY_W] || key_map[KB_KEY_I] ||
                  key_map[KB_KEY_K] )
             )
          {
            w.camera->state = Camera::STATIC;
          }
          break;
        case KB_RELEASE_I:
          if ( !( key_map[KB_KEY_S] || key_map[KB_KEY_A] ||
                  key_map[KB_KEY_D] || key_map[KB_KEY_W] ||
                  key_map[KB_KEY_K] )
             )
          {
            w.camera->state = Camera::STATIC;
          }
          break;
        case KB_RELEASE_K:
          if ( !( key_map[KB_KEY_S] || key_map[KB_KEY_A] ||
                  key_map[KB_KEY_D] || key_map[KB_KEY_I] ||
                  key_map[KB_KEY_W] )
             )
          {
            w.camera->state = Camera::STATIC;
          }
          break;
        case KB_PRESS_ESCAPE:
          w.camera = &w.ui_camera;
          if ( !(w.state == World::STATE_FREE_VIEW ) ){
            w.camera = &w.ui_camera;
            WORLD_SET_STATE_FREE_VIEW;
          } else {
            WORLD_SET_STATE_DETACHED;
          }
          break;
        case KB_PRESS_C:
          if ( !(w.state == World::STATE_VIEW_CAMERA ) ){
            w.camera = &w.view_camera;
            w.perspective = w.view_perspective;
            WORLD_SET_STATE_VIEW_CAMERA;
          } else {
            w.camera = &w.ui_camera;
            w.perspective = w.ui_perspective;
            WORLD_SET_STATE_FREE_VIEW;
          }
          break;
        case KB_PRESS_Q: case KB_REPEAT_Q:
          if ( w.state == World::STATE_SELECTED ){
            w.selected_rotate( w.selected_data,
                5.0f, v3{0.0f,1.0f,0.0f},
         w.model_matrices[w.selected_object.type][w.selected_object.index] );
          }
          break;
        case KB_PRESS_E: case KB_REPEAT_E:
          if ( w.state == World::STATE_SELECTED ){
            w.selected_rotate( w.selected_data,
                -5.0f, v3{0.0f,1.0f,0.0f},
         w.model_matrices[w.selected_object.type][w.selected_object.index] );
          } 
          break;
#endif
        default:
          break;
      }
    }
    w.camera->update( dt );
    glfwSetCursorPosCallback( window, mouse_callback );

    Event_Count = 0;
    glClearColor(0.0f,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // the deatched state hover
    if ( w.state == World::STATE_DETACHED || w.state == World::STATE_ON_HOLD||
         w.state == World::STATE_VIEW_CAMERA 
        )
    {
      if ( w.state != World::STATE_VIEW_CAMERA )
        glfwGetCursorPos( window, &cp[0], &cp[1] );
      HitRecord record;

      v3 point = v3{ ( float )cp[0], (float)cp[1], 0.0f };
      v3 wp = HMM_UnProject( point, w.perspective * w.camera->transform(),
          ScreenWidth, ScreenHeight );
      Ray ray( w.camera->P, ( wp - w.camera->P ) );

      if ( hit_world( w, ray, 0.001f, 100.0f, record ) ){
        if ( record.obj.type == OBJECT_GRID ){
          Grid *grid = (Grid *)record.obj.object;
          v3 p0 = grid_get_corner_point( *grid, record.u, record.v );
          v3 p1 = p0 + grid->dir1 * grid->w;
          v3 p2 = p0 + grid->dir1 * grid->w + grid->dir2 * grid->w;
          v3 p3 = p0 + grid->dir2 * grid->w;
          ColorQuad quad = { p0, p1, p2, p3,
            v3{ 0.42f,0.65f,0.83f }, // color
            w.grid.rect.n
          };
          array_push( w.temp_color_quads, quad );
        } else if ( record.obj.type == OBJECT_CUBE_INSTANCE ){
          array_push( w.boxes, w.cubes[record.obj.index].bounds );
        } else if ( record.obj.type == OBJECT_SPHERE ){
          array_push( w.boxes, w.spheres[ record.obj.index].box );
        } else if ( record.obj.type == OBJECT_RECT ){
          array_push( w.boxes, w.rects[ record.obj.index].box );
        } else if ( record.obj.type == OBJECT_LIGHT_CUBE_INSTANCE ){
          array_push( w.boxes, w.light_cubes[record.obj.index].bounds );
        } else if ( record.obj.type == OBJECT_LIGHT_SPHERE ){
          array_push( w.boxes, w.light_spheres[ record.obj.index].box );
        } else if ( record.obj.type == OBJECT_LIGHT_RECT ){
          array_push( w.boxes, w.light_rects[ record.obj.index].box );
        }
      }
    }

    if ( w.is_selected ){
      array_push( w.boxes,
          w.selected_aabb( w.selected_data,
            w.model_matrices[w.selected_object.type][w.selected_object.index] ) 
          );
    }
    draw_world(w);
    if ( w.show_imgui ){
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
#define SLIDER_UPPER_LIMIT 10.0f
      // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
      if ( show_demo_window ){ ImGui::ShowDemoWindow(&show_demo_window); }

      // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
      uint i = w.selected_object.index;
      if ( w.is_selected ){

      float f = 0.0f;
      int counter = 0;

      // Create a window called "Hello, world!" and append into it.
      ImGui::Begin("Object Properties");

      // Display some text (you can use a format strings too)
      ImGui::Text("Change some object properties!");

      if ( w.selected_object.type == OBJECT_CUBE_INSTANCE ){
        Cube *cube = &w.cubes[ i ];
        const char* items[] = { 
          "Front", "Back", "Right","Left",
          "Up","Down"
        };

        ImGui::Combo("Cube Face",
            &w.cube_face_dropdown,
            items, IM_ARRAYSIZE(items));

        ImGui::SliderFloat("Cube Length",
                           &w.sel_cube_length,
                           0.05f, SLIDER_UPPER_LIMIT, "ratio = %.3f");

        const char *material_dropdown[] = {
          "Metallic", "Diffuse", "Glass"
        };
        ImGui::Combo("Material",
            &w.cube_material_dropdown,
            material_dropdown, IM_ARRAYSIZE(material_dropdown));

        if ( w.cube_material_dropdown == 2 ){
        } else {

          const char *texture_select[] = {
            "Color","Checker","Marble"
          };
          ImGui::Combo("Texture", &w.cube_texture_dropdown, 
                        texture_select,
                        IM_ARRAYSIZE(texture_select));
          w.cube_face_color = w.cube_materials[i].texture.face_colors[
            w.cube_face_dropdown
          ];
          ImGui::ColorEdit3("clear color", w.cube_face_color.Elements );
        }
      } else if ( w.selected_object.type == OBJECT_SPHERE ){
        ImGui::SliderFloat("Sphere Radius", &w.sel_sphere_radius,
                           0.01f, SLIDER_UPPER_LIMIT, "ratio = %.3f");
        const char *material_dropdown[] = {
          "Metallic", "Diffuse", "Glass"
        };
        ImGui::Combo("Material",
            &w.sphere_material_dropdown,
            material_dropdown, IM_ARRAYSIZE(material_dropdown));

        if ( w.sphere_material_dropdown == 2 ){
        } else {

          const char *texture_select[] = {
            "Color","Checker","Marble"
          };
          ImGui::Combo("Texture", &w.sphere_texture_dropdown, 
                        texture_select,
                        IM_ARRAYSIZE(texture_select));
          w.sphere_face_color = w.sphere_materials[i].texture.color;
          ImGui::ColorEdit3("clear color", w.sphere_face_color.Elements );
        }
        
      } else if ( w.selected_object.type == OBJECT_RECT ){
        // TODO: Two sliders
        if ( ImGui::Button( "Flip Normal" ) ){
          w.rect_flip_normal++;
        }
        ImGui::SliderFloat("Rectangle length 1",
                           &w.sel_rect_l1,
                           0.01f, SLIDER_UPPER_LIMIT, "ratio = %.3f");
        ImGui::SliderFloat("Rectangle length 3",
                           &w.sel_rect_l2,
                           0.01f, SLIDER_UPPER_LIMIT, "ratio = %.3f");
        const char *material_dropdown[] = {
          "Metallic", "Diffuse", "Glass"
        };
        ImGui::Combo("Material",
            &w.rect_material_dropdown,
            material_dropdown, IM_ARRAYSIZE(material_dropdown));

        if ( w.rect_material_dropdown == 2 ){
        } else {

          const char *texture_select[] = {
            "Color","Checker","Marble"
          };
          ImGui::Combo("Texture", &w.rect_texture_dropdown, 
                        texture_select,
                        IM_ARRAYSIZE(texture_select));
          w.rect_face_color = w.rect_materials[i].texture.color;
          ImGui::ColorEdit3("clear color", w.rect_face_color.Elements );
        }

      } else if ( w.selected_object.type != OBJECT_GRID ) {
        v3 *color;
        switch ( w.selected_object.type ){
          case OBJECT_LIGHT_RECT:
            {
              color = &w.light_rect_face_color;
              w.light_rect_face_color= w.light_rect_color[i];
        ImGui::SliderFloat("Rectangle length 1",
                           &w.sel_rect_l1,
                           0.01f, SLIDER_UPPER_LIMIT, "ratio = %.3f");
        ImGui::SliderFloat("Rectangle length 3",
                           &w.sel_rect_l2,
                           0.01f, SLIDER_UPPER_LIMIT, "ratio = %.3f");
            }
            break;
          case OBJECT_LIGHT_SPHERE:
            {
              color = &w.light_sphere_face_color;
              w.light_sphere_face_color = w.light_sphere_color[i];
        ImGui::SliderFloat("Sphere Radius",
                           &w.sel_sphere_radius,
                           0.01f, SLIDER_UPPER_LIMIT, "ratio = %.3f");
            }
            break;
          case OBJECT_LIGHT_CUBE_INSTANCE:
            {
              color = &w.light_cube_face_color;
              w.light_cube_face_color= w.light_cube_color[i];
              ImGui::SliderFloat("Cube Length",
                           &w.sel_cube_length,
                           0.05f, SLIDER_UPPER_LIMIT, "ratio = %.3f");
            }
            break;
          default: break;
        }
        ImGui::ColorEdit3("Change Color",
            color->Elements );
      }
      // Buttons return true when clicked (most widgets return true
      // when edited/activated)

      if (ImGui::Button("Button"))
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
          1000.0f / ImGui::GetIO().Framerate,
          ImGui::GetIO().Framerate);
      ImGui::End();
      switch ( w.selected_object.type ){
        case OBJECT_CUBE_INSTANCE:
        {
          Cube *cube =w.cubes + i;
          if ( w.cube_material_dropdown == 2 ){
            w.cube_materials[i].type = Material::GLASS;
          } else {
            w.cube_materials[i].type =
              (Material::MaterialType)w.cube_material_dropdown;
            switch ( w.cube_texture_dropdown ){
              case 0:
                w.cube_materials[i].texture.type = Texture::COLOR;
                break;
              case 1:
                w.cube_materials[i].texture.type = Texture::CHECKER;
                break;
              case 2:
                w.cube_materials[i].texture.type = Texture::MARBLE;
                break;
              default:
                break;
            }
            w.cube_materials[i].texture.face_colors[ w.cube_face_dropdown ] =
                w.cube_face_color;
          }
          
          f32 l = w.sel_cube_length;
          cube_scale( cube, v3{l,l,l},
                      w.model_matrices[OBJECT_CUBE_INSTANCE][i] );
          world_add_cube_vertex_data(w,i);
        }
          break;
        case OBJECT_SPHERE:
          {
            if ( w.sphere_material_dropdown ==  2 ){
              w.sphere_materials[i].type = Material::GLASS;
            } else {
              w.sphere_materials[i].type =
                (Material::MaterialType)w.sphere_material_dropdown;
              w.sphere_materials[i].texture.type =
                (Texture::TextureType)w.sphere_texture_dropdown;
              w.sphere_materials[i].texture.color = w.sphere_face_color;
            }
            
              world_add_sphere_vertex_data(w,i);
              f32 l = w.sel_sphere_radius;
              sphere_scale(w.spheres+i, v3{l,l,l},
                  w.model_matrices[OBJECT_SPHERE][i] );
            }
          break;
        case OBJECT_RECT:
          {
            if ( w.rect_flip_normal ){
              w.rect_flip_normal--;
              w.rects[i].n *= -1;
            }
            if ( w.rect_material_dropdown ==  2 ){
              w.rect_materials[i].type = Material::GLASS;
            } else {
              w.rect_materials[i].type =
                (Material::MaterialType)w.rect_material_dropdown;
              w.rect_materials[i].texture.type =
                (Texture::TextureType)w.rect_texture_dropdown;
              w.rect_materials[i].texture.color = w.rect_face_color;
            }
              rectangle_scale( w.selected_data,w.sel_rect_l1, w.sel_rect_l2 );
              break;
            }
        case OBJECT_LIGHT_CUBE_INSTANCE:
        {
          Cube *cube =w.light_cubes + i;
          w.light_cube_color[i] = w.light_cube_face_color;
          f32 l = w.sel_cube_length;
          world_add_light_cube_vertex_data(w,i);
          cube_scale( cube, v3{l,l,l},
                      w.model_matrices[OBJECT_LIGHT_CUBE_INSTANCE][i] );
        }
          break;
        case OBJECT_LIGHT_SPHERE:
          {
            w.light_sphere_color[i] = w.light_sphere_face_color;
            world_add_light_sphere_vertex_data(w,i);
            f32 l = w.sel_sphere_radius;
            sphere_scale(w.light_spheres+i, v3{l,l,l},
                w.model_matrices[OBJECT_LIGHT_SPHERE][i] );
          }
          break;
        case OBJECT_LIGHT_RECT:
          w.light_rect_color[i] = w.light_rect_face_color;
          rectangle_scale( w.selected_data,w.sel_rect_l1, w.sel_rect_l2 );
          break;
        default:
          break;
      }

      }

      // 3. Show another simple window.
      if (w.state == World::STATE_DETACHED){  
        ImGui::Begin("Place Objects", &show_another_window);
        const char* world_objects[] = { 
          "Cube", "Sphere", "Plane","Light Cube", "Light Sphere", "Light Plane"
        };
        ImGui::Combo("combo",
                     &w.hold_object_id,
                     world_objects, IM_ARRAYSIZE(world_objects));

        dump_scene_data = ImGui::Button("Dump Scene Data");

        ImGui::End();
      } else if ( w.state == World::STATE_VIEW_CAMERA ){
        ImGui::Begin( "Camera Properties" );
        ImGui::End();
      }

      // Rendering
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // Dear Imgui related events
    }



    glfwSwapBuffers(window);
    glfwPollEvents();
    array_clear( w.color_vertex_data );
    array_clear( w.color_vertex_modes );
    array_clear( w.color_vertex_indices );
    array_clear( w.index_stack );
    array_clear( w.temp_color_quads );
    array_clear( w.boxes );
    array_clear( w.light_pos );
    array_clear( w.light_colors );

    if ( dump_scene_data ){
      dump_scene_data = false;
      char buff[256];
      snprintf( buff, 256, "./bin/dump_file%d.dat", dump_count );
      world_dump_scene_to_file( w,buff );
    }
  }
  glfwTerminate();
}
