// vertex shader, particle render
#version 410 core

#define COLOR_MODE_UNIFORM_MATERIAL   0
#define COLOR_MODE_RANDOM             1
#define COLOR_MODE_RAMP               2
#define COLOR_MODE_OBJ_INDEX          3
#define COLOR_MODE_VELOCITY_MAGNITUDE 4

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

uniform uint  u_nParticles;
uniform int   u_ColorMode;
uniform float u_VColorMin;
uniform float u_VColorMax;
uniform vec3  u_ColorMinVal;
uniform vec3  u_ColorMaxVal;

uniform vec4  u_ClipPlane;
uniform float u_PointRadius;
uniform int   u_IsPointView;
uniform int   u_UseAniKernel;
uniform int   u_ScreenWidth;
uniform int   u_ScreenHeight;

//------------------------------------------------------------------------------------------
uniform vec3  u_vPositionMin;
uniform vec3  u_vPositionMax;
uniform float u_vAniKernelMin;
uniform float u_vAniKernelMax;

in ivec3 v_Position;
in float v_fColor;
in int   v_iColor;
in ivec3 v_AnisotropyMatrix0;
in ivec3 v_AnisotropyMatrix1;
in ivec3 v_AnisotropyMatrix2;

flat out vec3 f_ViewCenter;
flat out vec3 f_Color;
flat out mat3 f_AniMatrix;

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
    } else if(u_ColorMode == COLOR_MODE_OBJ_INDEX) {
        float t = (float(v_iColor) - u_VColorMin) / (u_VColorMax - u_VColorMin);
        return mix(u_ColorMinVal, u_ColorMaxVal, t);
    } else {
        float range = u_VColorMax - u_VColorMin;
        if(range > 0) {
            return mix(u_ColorMinVal, u_ColorMaxVal, (v_fColor - u_VColorMin) / range);
        } else {
            return u_ColorMinVal;
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
    vec3  diff     = u_vPositionMax - u_vPositionMin;
    vec3  position = v_Position * diff / 65535.0f + u_vPositionMin;
    vec4  eyeCoord = viewMatrix * vec4(position, 1.0);
    vec3  posEye   = vec3(eyeCoord);
    float dist     = length(posEye);

    vec3 aniMat0 = vec3(0);
    vec3 aniMat1 = vec3(0);
    vec3 aniMat2 = vec3(0);
    if(u_UseAniKernel == 1) {
        float diffA = u_vAniKernelMax - u_vAniKernelMin;
        aniMat0 = v_AnisotropyMatrix0 * diffA / 65535.0f + vec3(u_vAniKernelMin);
        aniMat1 = v_AnisotropyMatrix1 * diffA / 65535.0f + vec3(u_vAniKernelMin);
        aniMat2 = v_AnisotropyMatrix2 * diffA / 65535.0f + vec3(u_vAniKernelMin);
    }

    mat4 T = (u_UseAniKernel == 0) ?
             mat4(u_PointRadius, 0, 0, 0,
                  0, u_PointRadius, 0, 0,
                  0, 0, u_PointRadius, 0,
                  position.x, position.y, position.z, 1.0) :
             mat4(aniMat0[0] * u_PointRadius, aniMat0[1] * u_PointRadius, aniMat0[2] * u_PointRadius, 0,
                  aniMat1[0] * u_PointRadius, aniMat1[1] * u_PointRadius, aniMat1[2] * u_PointRadius, 0,
                  aniMat2[0] * u_PointRadius, aniMat2[1] * u_PointRadius, aniMat2[2] * u_PointRadius, 0,
                  position.x, position.y, position.z, 1.0);



    /////////////////////////////////////////////////////////////////
    // output
    f_ViewCenter = posEye;
    f_Color      = generateVertexColor();
    f_AniMatrix  = (u_UseAniKernel == 0) ? mat3(0) : mat3(aniMat0, aniMat1, aniMat2);

#ifdef UNIT_SPHERE_ISOLATED_PARTICLE
    float sx = length(aniMat0);
    float sy = length(aniMat1);
    float sz = length(aniMat2);

    if(abs(sx - sy) < 1e-2 && abs(sy - sz) < 1e-2 && abs(sz - sx) < 1e-2) {
        T = mat4(u_PointRadius, 0, 0, 0,
                 0, u_PointRadius, 0, 0,
                 0, 0, u_PointRadius, 0,
                 position.x, position.y, position.z, 1.0);

        f_AniMatrix = mat3(1);
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
