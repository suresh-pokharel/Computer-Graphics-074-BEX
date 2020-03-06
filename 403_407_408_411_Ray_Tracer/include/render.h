typedef unsigned int uint;

#include <assert.h>

static void *rect_storage = NULL;
static size_t rect_storage_capacity = 0;
static size_t rect_number = 0;

struct Image {
  int h,w,channels;
  uchar *data;
};
#if 0
static uint ELEMENT_INDICES[] = {
  0,1,2,
  2,3,0,

  4,5,6,
  6,7,4,

  8,9,10,
  10,11,8,
  
  12, 13, 14,
  14, 15, 12,

  16, 17, 18,
  18, 19, 16,

  20, 21, 22,
  22, 23, 20,

  24, 25, 26,
  24, 26, 24,
};
#endif

static uint *ELEMENT_INDICES;

struct LinesSOA {
  float *x;
  float *y;
  float *r;
  float *g;
  float *b;
  int n;
  size_t total;
};

union ColorVertex {
  struct {
    v3 pos;
    v3 color;
    v3 n;
  };
  float data[9];
};

union TextureVertex {
  struct {
    v3 pos;
    v3 color;
    v3 n;
    v2 uv;
  };
  float data[11];
};

enum MaterialId {
  MATERIAL_NONE = 0,
  MATERIAL_GOLD,
  MATERIAL_BRONZE,
  MATERIAL_WOOD,
  MATERIAL_BRICK,
  MATERIAL_SILVER,
  MATERIAL_SHINY_BLACK,
  MATERIAL_WHITE,
  MATERIAL_DEFAULT,
  _MATERIAL_TOTAL_,
};

struct Material {
  MaterialId id;
  uint flags;
  v3 amb;
  v3 diff;
  v3 spec;
  float shine;
  Image diff_map; 
};





enum  RenderGroupIndex{
  RENDER_GROUP_0 = 0,
  RENDER_GROUP_1 = 1,
  RENDER_GROUP_2 = 2,
  RENDER_GROUP_3 = 3,
  RENDER_GROUP_4 = 4,
  RENDER_GROUP_5 = 5
};

struct MaterialLocation{
  uint flags;

  uint diff;
  uint amb;
  uint spec;
  uint shine;
  uint diff_map;
  uint spec_map;
};

enum LightType : uint{
  LIGHT_NONE,
  LIGHT_POINT = 0x1,
  LIGHT_DIRECTIONAL = 0x2
};

enum MaterialFlags:uint{
   MATERIAL_USE_DIFFUSE_MAP = 0x1,
   MATERIAL_USE_DIFFUSE_COLOR = 0x2,
   MATERIAL_USE_SPECULAR_MAP = 0x4
};

struct Light {
  LightType type;
  uint flags;
  union {
    v3 pos;
    v3 direction;
  };
  v3 amb;
  v3 diff;
  v3 spec;
  v3 attn;
};


struct Render {
  uint model[10];
  uint view[10];
  uint transform[10];

  uint light_position[10];
  uint sampler_location[10][10];
  uint light_type_location[10];
  uint light_flags_location[10];
  MaterialLocation material_location[10];
  uint programs[ 10 ]; // 1 program for each render group
  float aspect_ratio;
  m4 projection;
  float fov;
  float near,far;

  m4 *line_model_mat;
  int num_lines;

  m4 *cursor_pos;
  uint vao[10];
  uint vbo[10][5];
  uint texture[10][10];
  int num_texture;
  int obj_total;
  
  Camera *camera;

  ColorVertex *quad_buffer;
  TextureVertex *tex_vertex_buffer;
  int group_len[10];
  m4 group_model[ 10 ];
  int num_quads;
  
  Material materials[10];
  Light lights[10];
  uint light_location_amb[10];
  uint light_location_diff[10];
  uint light_location_spec[10];
  uint light_location_pos[10];
  uint light_location_type[10];
  uint light_location_attn[10];
};

const size_t group_max_size = 500; // in number of vertexes 

void render_push_tex_quad(
    Render *render,
    RenderGroupIndex index,
    v3 p1, v3 p2, v3 p3, v3 p4, // Vertices in anti-clockwise order
    float r, float g, float b,
    v3 n,
    v2 *uv
)
{
  if ( !render->tex_vertex_buffer ) {
    fprintf( stderr, "Render quad buffer not initialized!\n" );
  }
  
  if ( render->group_len[index] >= group_max_size ){
    fprintf( stderr, "Max vertex limit for render group %d reached!\n", index );
    return;
  }
  
  TextureVertex *vertex = ( render->tex_vertex_buffer+ index * group_max_size ) + render->group_len[index];
  render->group_len[ index ] += 4;

  
  vertex[0].pos = p1;
  vertex[1].pos = p2;
  vertex[2].pos = p3;
  vertex[3].pos = p4;

  vertex[0].color = {r,g,b};
  vertex[1].color = {r,g,b};
  vertex[2].color = {r,g,b};
  vertex[3].color = {r,g,b};

  vertex[0].n = n;
  vertex[1].n = n;
  vertex[2].n = n;
  vertex[3].n = n;



  vertex[0].uv = uv[0]; 
  vertex[1].uv = uv[1];
  vertex[2].uv = uv[2];
  vertex[3].uv = uv[3];
}

void render_update_tex_quads( Render *render ){
  for ( int i = 0; i <  ( sizeof(render->group_len)/sizeof(int) ); i++ ){
    if ( render->group_len[ i ] ){
        glBindVertexArray( render->vao[ i ] );
        glBindBuffer( GL_ARRAY_BUFFER, render->vbo[ i ][0] );
        TextureVertex *v = render->tex_vertex_buffer+ i * group_max_size;
        glBufferData( GL_ARRAY_BUFFER, render->group_len[i] * sizeof(TextureVertex), (void *)v, GL_STATIC_DRAW  );
    }
  }
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void render_push_quad_to_draw(
    Render *render,
    RenderGroupIndex index,
    v3 p1, v3 p2, v3 p3, v3 p4, // Vertices in anti-clockwise order
    float r, float g, float b,
    v3 n,
    m4 model
)
{
  if ( !render->quad_buffer ) {
    fprintf( stderr, "Render quad buffer not initialized!\n" );
  }
  
  if ( render->group_len[index] >= group_max_size ){
    fprintf( stderr, "Max vertex limit for render group %d reached!\n", index );
    return;
  }
  
  ColorVertex *vertex = ( render->quad_buffer + index * group_max_size ) + render->group_len[index];
  render->group_len[ index ] += 4;

  
  vertex[0].pos = p1;
  vertex[1].pos = p2;
  vertex[2].pos = p3;
  vertex[3].pos = p4;

  vertex[0].n = n;
  vertex[1].n = n;
  vertex[2].n = n;
  vertex[3].n = n;

  vertex[0].color = {r,g,b};
  vertex[1].color = {r,g,b};
  vertex[2].color = {r,g,b};
  vertex[3].color = {r,g,b};

}

#if 0
void render_update_quads( Render *render ){
  for ( int i = 0; i <  ( sizeof(render->group_len)/sizeof(int) ); i++ ){
    if ( render->group_len[ i ] ){
        glBindVertexArray( render->vao[ i ] );
        glBindBuffer( GL_ARRAY_BUFFER, render->vbo[ i ] );
        ColorVertex *v = render->quad_buffer + i * group_max_size;
        glBufferData( GL_ARRAY_BUFFER, render->group_len[i] * sizeof(ColorVertex), (void *)v, GL_STATIC_DRAW  );
    }
  }
  glBindVertexArray( 0 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
#endif



void display_matrix( m4 *m ){
  printf(" %f %f %f %f\n", m->Elements[0][0], m->Elements[0][1], m->Elements[0][2], m->Elements[0][3] );
  printf(" %f %f %f %f\n", m->Elements[1][0], m->Elements[1][1], m->Elements[1][2], m->Elements[1][3] );
  printf(" %f %f %f %f\n", m->Elements[2][0], m->Elements[2][1], m->Elements[2][2], m->Elements[2][3] );
  printf(" %f %f %f %f\n", m->Elements[3][0], m->Elements[3][1], m->Elements[3][2], m->Elements[3][3] );
}



void render_draw( Render &render){

  m4 projection = HMM_Perspective(
      render.fov, render.aspect_ratio,
      render.near, render.far 
  );
  m4 pv = projection * ( render.camera )->transform;
  // RENDER_GROUP_0
  m4 total = HMM_MultiplyMat4( pv , render.group_model[RENDER_GROUP_0] ); 

  glUseProgram( render.programs[ RENDER_GROUP_0 ] );
  glUniformMatrix4fv( render.transform[RENDER_GROUP_0], 1 ,GL_FALSE, HMM_MAT4_PTR( total ) );
  glUniformMatrix4fv( render.view[RENDER_GROUP_0], 1, GL_FALSE, HMM_MAT4_PTR( render.camera->transform ) );
  glUniformMatrix4fv( render.model[RENDER_GROUP_0], 1, GL_FALSE, HMM_MAT4_PTR( render.group_model[RENDER_GROUP_0] ) );
  
  Material *m = render.materials + RENDER_GROUP_0;
  glUniform3fv( loc->amb, 1, &(m->amb.Elements[0] ) );
  glUniform3fv( loc->diff, 1, &(m->diff.Elements[0] ) );
  glUniform3fv( loc->spec, 1, &(m->spec.Elements[0] ) );
  glUniform1f( loc->shine,  m->shine );

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, render.texture[ RENDER_GROUP_0 ][0] );
  glActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, render.texture[ RENDER_GROUP_0 ][1] );

  glUniform1i( loc->diff_map, 0 );
  glUniform1i( loc->spec_map, 1 );
  glUniform1ui( loc->flags, m->flags );
 
  // Might have to move this multiplication into the shaders
  Light *l = render.lights;
  v4 lps;
  v3 t = l->pos;
  if ( l->type == LIGHT_POINT ){
    lps = HMM_Vec4( t[0], t[1], t[2], 1.0f );
  } else {
    lps = HMM_Vec4( t[0], t[1], t[2], 0.0f );
  }
  //v4 adjusted = HMM_MultiplyMat4ByVec4( render.camera->transform, lps );
  v4 adjusted = render.camera->transform * lps; 

  glUniform3fv( render.light_location_pos[0],1, &adjusted.Elements[0]);
  glUniform3fv( render.light_location_amb[0],1,&l->amb.Elements[0] );
  glUniform3fv( render.light_location_diff[0],1,&l->diff.Elements[0] );
  glUniform3fv( render.light_location_spec[0],1,&l->spec.Elements[0] );
  glUniform3fv( render.light_location_attn[0],1,&l->attn.Elements[0] );
  glUniform1ui( render.light_location_type[0], render.lights[0].type );

  glBindVertexArray( render.vao[ RENDER_GROUP_0 ] );
  glDrawElementsInstanced( GL_TRIANGLES,
                           ( render.group_len[RENDER_GROUP_0] << 2 ) * 6,
                           GL_UNSIGNED_INT,
                           0,
                           5);


  for ( int i = 0; i <  ( sizeof(render.group_len)/sizeof(int) ); i++ ){
    render.group_len[i] = 0;
  }

}

