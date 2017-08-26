// vertex shader, particle render
#version 410 core

layout(std140) uniform CameraData
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 invViewMatrix;
    mat4 invProjectionMatrix;
    mat4 shadowMatrix;
    vec4 camPosition;
};

uniform float u_PointRadius;
uniform float u_PointScale;
uniform vec4  u_ClipPlane;

//------------------------------------------------------------------------------------------
in vec3       v_Position;
in vec3       v_Color;

out vec3      f_ViewCenter;
out vec3      f_Color;
//------------------------------------------------------------------------------------------
void main()
{
    vec4  eyeCoord = viewMatrix * vec4(v_Position, 1.0);
    vec3  posEye   = vec3(eyeCoord);
    float dist     = length(posEye);

    f_ViewCenter       = posEye;
    f_Color            = v_Color;

    gl_PointSize = u_PointRadius * (u_PointScale / dist);
    gl_Position  = projectionMatrix * eyeCoord;
    gl_ClipDistance[0] = dot(vec4(v_Position, 1.0), u_ClipPlane);
}
