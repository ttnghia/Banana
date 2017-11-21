// vertex shader, particle render
#version 410 core

#define COLOR_MODE_UNIFORM_MATERIAL 0
#define COLOR_MODE_RANDOM           1
#define COLOR_MODE_RAMP             2
#define COLOR_MODE_FROM_DATA        3

#define UNIT_SPHERE_ISOLATED_PARTICLE

uniform vec3 colorRamp[] = vec3[] (vec3(1.0, 0.0, 0.0),
                                   vec3(1.0, 0.5, 0.0),
                                   vec3(1.0, 1.0, 0.0),
                                   vec3(1.0, 0.0, 1.0),
                                   vec3(0.0, 1.0, 0.0),
                                   vec3(0.0, 1.0, 1.0),
                                   vec3(0.0, 0.0, 1.0));

layout(std140) uniform CameraData
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 invViewMatrix;
    mat4 invProjectionMatrix;
    mat4 shadowMatrix;
    vec4 camPosition;
};

uniform uint u_nParticles;
uniform int  u_ColorMode;
uniform int  u_ColorDataSize;
uniform vec3 u_ColorDataMin;
uniform vec3 u_ColorDataMax;

//uniform float u_PointScale;
uniform vec4  u_ClipPlane;
uniform float u_PointRadius;
uniform int   u_IsPointView;
uniform int   u_UseAnisotropyKernel;
uniform int   u_ScreenWidth;
uniform int   u_ScreenHeight;

//------------------------------------------------------------------------------------------
uniform vec3  u_MinPosition;
uniform vec3  u_MaxPosition;
uniform float u_MinAniMatrix;
uniform float u_MaxAniMatrix;

in ivec3 v_Position;
in int   v_Color1;
in ivec3 v_Color3;
in ivec3 v_AnisotropyMatrix0;
in ivec3 v_AnisotropyMatrix1;
in ivec3 v_AnisotropyMatrix2;

out vec3      f_ViewCenter;
out vec3      f_Color;
flat out mat3 f_AnisotropyMatrix;

//------------------------------------------------------------------------------------------
float rand(vec2 co)
{
    float a  = 12.9898f;
    float b  = 78.233f;
    float c  = 43758.5453f;
    float dt = dot(co.xy, vec2(a, b));
    float sn = mod(dt, 3.14);
    return fract(sin(sn) * c);
}

vec3 generateVertexColor()
{
    if(u_ColorMode == COLOR_MODE_RANDOM) {
        return vec3(rand(vec2(gl_VertexID, gl_VertexID)),
                    rand(vec2(gl_VertexID + 1, gl_VertexID)),
                    rand(vec2(gl_VertexID, gl_VertexID + 1)));
    } else if(u_ColorMode == COLOR_MODE_RAMP) {
        float segmentSize = float(u_nParticles) / 6.0f;
        float segment     = floor(float(gl_VertexID) / segmentSize);
        float t           = (float(gl_VertexID) - segmentSize * segment) / segmentSize;
        vec3  startVal    = colorRamp[int(segment)];
        vec3  endVal      = colorRamp[int(segment) + 1];
        return mix(startVal, endVal, t);
    } else {
        if(u_ColorDataSize == 1) {
            return mix(u_ColorDataMin, u_ColorDataMax, float(v_Color1) / 65535.0f);
        } else {
            return vec3(mix(u_ColorDataMin.x, u_ColorDataMax.x, float(v_Color3.x) / 65535.0f),
                        mix(u_ColorDataMin.y, u_ColorDataMax.y, float(v_Color3.y) / 65535.0f),
                        mix(u_ColorDataMin.z, u_ColorDataMax.z, float(v_Color3.z) / 65535.0f));
        }
    }
}

//------------------------------------------------------------------------------------------
const mat4 D = mat4(1., 0., 0., 0.,
                    0., 1., 0., 0.,
                    0., 0., 1., 0.,
                    0., 0., 0., -1.);
void ComputePointSizeAndPosition(mat4 T)
{
    vec2 xbc;
    vec2 ybc;

    mat4  R = transpose(projectionMatrix * viewMatrix * T);
    float A = dot(R[ 3 ], D * R[ 3 ]);
    float B = -2. * dot(R[ 0 ], D * R[ 3 ]);
    float C = dot(R[ 0 ], D * R[ 0 ]);
    xbc[ 0 ] = (-B - sqrt(B * B - 4. * A * C)) / (2.0 * A);
    xbc[ 1 ] = (-B + sqrt(B * B - 4. * A * C)) / (2.0 * A);
    float sx = abs(xbc[ 0 ] - xbc[ 1 ]) * .5 * u_ScreenWidth;

    A        = dot(R[ 3 ], D * R[ 3 ]);
    B        = -2. * dot(R[ 1 ], D * R[ 3 ]);
    C        = dot(R[ 1 ], D * R[ 1 ]);
    ybc[ 0 ] = (-B - sqrt(B * B - 4. * A * C)) / (2.0 * A);
    ybc[ 1 ] = (-B + sqrt(B * B - 4. * A * C)) / (2.0 * A);
    float sy = abs(ybc[ 0 ] - ybc[ 1 ]) * .5 * u_ScreenHeight;

    float pointSize = ceil(max(sx, sy));
    gl_PointSize = pointSize;
}

//------------------------------------------------------------------------------------------
void main()
{
    vec3  diff     = u_MaxPosition - u_MinPosition;
    vec3  position = v_Position * diff / 65535.0f + u_MinPosition;
    vec4  eyeCoord = viewMatrix * vec4(position, 1.0);
    vec3  posEye   = vec3(eyeCoord);
    float dist     = length(posEye);

    mat4 T = (u_UseAnisotropyKernel == 0) ?
             mat4(u_PointRadius, 0, 0, 0,
                  0, u_PointRadius, 0, 0,
                  0, 0, u_PointRadius, 0,
                  position.x, position.y, position.z, 1.0) :
             mat4(v_AnisotropyMatrix0[0] * u_PointRadius, v_AnisotropyMatrix0[1] * u_PointRadius, v_AnisotropyMatrix0[2] * u_PointRadius, 0,
                  v_AnisotropyMatrix1[0] * u_PointRadius, v_AnisotropyMatrix1[1] * u_PointRadius, v_AnisotropyMatrix1[2] * u_PointRadius, 0,
                  v_AnisotropyMatrix2[0] * u_PointRadius, v_AnisotropyMatrix2[1] * u_PointRadius, v_AnisotropyMatrix2[2] * u_PointRadius, 0,
                  position.x, position.y, position.z, 1.0);



    /////////////////////////////////////////////////////////////////
    // output
    f_ViewCenter       = posEye;
    f_Color            = generateVertexColor();
    f_AnisotropyMatrix = (u_UseAnisotropyKernel == 0) ? mat3(0) : mat3(v_AnisotropyMatrix0, v_AnisotropyMatrix1, v_AnisotropyMatrix2);

#ifdef UNIT_SPHERE_ISOLATED_PARTICLE
    float sx = length(v_AnisotropyMatrix0);
    float sy = length(v_AnisotropyMatrix1);
    float sz = length(v_AnisotropyMatrix2);

    if(abs(sx - sy) < 1e-2 && abs(sy - sz) < 1e-2 && abs(sz - sx) < 1e-2) {
        T = mat4(u_PointRadius, 0, 0, 0,
                 0, u_PointRadius, 0, 0,
                 0, 0, u_PointRadius, 0,
                 position.x, position.y, position.z, 1.0);

        f_AnisotropyMatrix = mat3(1);
    }
#endif

    if(u_IsPointView == 1) {
        gl_PointSize = 2.0;
    } else {
        ComputePointSizeAndPosition(T);
    }

//    if(u_PointScale < 0)
//        eyeCoord *= 2;

    gl_Position        = projectionMatrix * eyeCoord;
    gl_ClipDistance[0] = dot(vec4(position, 1.0), u_ClipPlane);
}
