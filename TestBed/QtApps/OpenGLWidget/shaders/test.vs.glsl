#version 400 core

uniform mat4 viewProjectionMatrix;

in vec3 v_coord;
in vec3 v_color;

out vec3 f_color;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void main()
{
    vec4 worldCoord = viewProjectionMatrix * vec4(v_coord, 1.0);
    gl_Position = worldCoord;

    f_color = v_color;

}
