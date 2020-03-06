#version 430 core
layout (location = 0) in vec3 in_pos;
layout (location = 1 ) in vec3 in_color;
layout (location = 2 ) in vec3 in_normal;

out vec3 color;
uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(in_pos.x,in_pos.y,in_pos.z,1.0); 
    color = in_color;
}
