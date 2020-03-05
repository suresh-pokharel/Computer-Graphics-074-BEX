#include <math.h>

enum CameraState {
  CAMERA_MOVE_FRONT,
  CAMERA_MOVE_BACK,
  CAMERA_MOVE_LEFT,
  CAMERA_MOVE_RIGHT,
  CAMERA_MOVE_UP,
  CAMERA_MOVE_DOWN,
  CAMERA_REST,
};


struct Camera {
  v3 pos;
  v3 front;
  v3 right;
  v3 up;
  m4 transform; 

  CameraState state;
  float dist_to_move;
  float dist_moved;
  uint32 elapsed;
  uint32 duration;
  float distance;
  v3 direction;
};


void create_camera( Camera *cam, float x, float y, float z ){
  cam->pos[0] = x;
  cam->pos[1] = y;
  cam->pos[2] = z;
}

void camera_set_target_dir( Camera *cam, float x, float y, float z ){
  cam->front[0] = -x;
  cam->front[1] = -y;
  cam->front[2] = -z;
  
  v3 aup = {0.0f,1.0f,0.0f};
  cam->front = HMM_NormalizeVec3( cam->front );
  cam->right = HMM_NormalizeVec3( HMM_Cross( aup, cam->front ) );
  cam->up = HMM_NormalizeVec3( HMM_Cross( cam->front, cam->right) );
}

inline void camera_move_front( Camera *cam, float k ){
  cam->pos += k * cam->front;
}

inline void camera_move_right( Camera *cam, float k ){
  cam->pos += k * cam->right;
}

inline void camera_move_up( Camera *cam, float k ){
  cam->pos += k * cam->up;
}


void camera_get_transform( Camera *cam ){
  float *dst = (float *)&(cam->transform);

  dst[0] = cam->right[0];
  dst[1] = cam->up[0];
  dst[2] = cam->front[0];
  dst[3] = 0.0f;

  dst[4] = cam->right[1];
  dst[5] = cam->up[1];
  dst[6] = cam->front[1];
  dst[7] = 0.0f;

  dst[8] = cam->right[2];
  dst[9] = cam->up[2];
  dst[10] = cam->front[2];
  dst[11] = 0.0f;

  dst[12] = -HMM_DotVec3( cam->right, cam->pos );
  dst[13] = -HMM_DotVec3( cam->up, cam->pos );
  dst[14] = -HMM_DotVec3( cam->front, cam->pos );
  dst[15] = 1.0f;
}


float *get_ortho_matrix( float *dst,  
        float n,
        float f,
        float r,
        float l,
        float t,
        float b)
{
  dst[0] = 2/(r-l);
  dst[1] = 0;
  dst[2] = 0;
  dst[3] = 0.0f;

  dst[4] = 0;
  dst[5] = 2/(t-b);
  dst[6] = 0;
  dst[7] = 0;

  dst[8] = 0;
  dst[9] = 0;
  dst[10] = -2/(f-n);
  dst[11] = 0.0f;

  dst[12] = -(r+l)/(r-l);
  dst[13] = -(t+b)/(t-b);
  dst[14] = -(f+n)/(f-n);
  dst[15] = 1.0f;
  return dst;

}

float *get_translate_matrix( float *dst,
    float x,
    float y,
    float z )
{
  dst[0] = 1;
  dst[1] = 0;
  dst[2] = 0;
  dst[3] = 0;

  dst[4] = 0;
  dst[5] = 1.0f;
  dst[6] = 0;
  dst[7] = 0;

  dst[8] = 0;
  dst[9] = 0;
  dst[10] = 1.0f;
  dst[11] = 0;

  dst[12] = x; 
  dst[13] = y;
  dst[14] = z;
  dst[15] = 1.0f;
  return dst;
}

