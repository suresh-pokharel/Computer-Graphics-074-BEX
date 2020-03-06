// Vertex shader
// Vertex shader
#version 430 core
layout ( location = 0 ) in vec3 in_pos;
layout ( location = 1 ) in vec3 in_color;
layout ( location = 2 ) in vec3 in_normals;
layout ( location = 3 ) in vec2 in_texcoords;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;
uniform vec3 view_pos;

out vec3 color;
out vec3 Normal;
out vec3 Fpos;
out vec3 vpos;
out vec2 tex_coords;

out mat4 view_matrix;
void main(){
  gl_Position = mvp* vec4( in_pos, 1.0f );
  Fpos = ( model * vec4( in_pos, 1.0f ) ).xyz;
  color = in_color;
  Normal = ( model * vec4( in_normals, 0.0f ) ).xyz;
  vpos = view_pos;
  tex_coords = in_texcoords;
}
