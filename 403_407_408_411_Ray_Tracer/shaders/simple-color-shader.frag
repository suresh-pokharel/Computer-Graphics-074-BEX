// FRAGMENT SHADER
#version 430 core
in vec3 Normal;
in vec3 Fpos; // fragment position
in vec3 color;
in vec3 vpos;
in vec2 tex_coords;

struct PointLight{
  vec3 pos;
  vec3 color;
};

uniform PointLight point_lights[4];
uniform int num_lights;
uniform vec3 amb;

out vec4 frag_color;
uniform sampler2D texture0;

vec3 calculate_point_light( PointLight light, vec3 fpos, vec3 n, vec3 view_dir)
{
  
  vec3 ftol = normalize(light.pos - fpos);
  float diffuse = max( dot(ftol,n), 0 );
  vec3 rdir = reflect( -ftol, n );
  float spec  = pow( max( dot( view_dir, rdir ), 0 ), 64 );

  vec3 ret = ( 0.6f * diffuse + 0.9f * spec ) * light.color;
  return ret;

}

void main(){
  vec3 Norm = normalize( Normal );
  vec3 result = vec3( 0.0f, 0.0f, 0.0f );

  for ( int i = 0; i < num_lights; i++ ){
    PointLight light = point_lights[i];
    vec3 view_dir = normalize( vpos - Fpos );
    result += calculate_point_light( light, Fpos, Norm, view_dir );
  }
  vec4 c = texture(texture0, tex_coords ) * vec4(color, 1.0);  
  vec4 final = vec4( ( result + amb ),1.0f ) * c;
  frag_color = final;
}

//void main(){
//  vec3 Norm = normalize( Normal );
//  vec3 ftol = nomalize( fpos - light.pos );
//  float diff = max( dot(ftol,Norm),0);
//  vec3 diffuse = light.color * diff;
//  vec3 view_direction = normalize( 
//  vec3 ref = reflect(  );
//
//}
