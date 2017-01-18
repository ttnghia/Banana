#version 400 core

//----------------------------------------------------------`--------------------------------
// out variables
layout( location = 0 ) out vec4 fragColor;

in vec3 f_color;

//------------------------------------------------------------------------------------------
void main()
{
    //fragColor = vec4(1, 0, 0, 1);
    fragColor = vec4(f_color.x, f_color.y, f_color.z, 1.0);
}
