#version 410 core
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

uniform sampler2D u_Tex1;
uniform sampler2D u_Tex2;
uniform sampler2D u_Tex3;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
in vec2 f_texcoord;

out vec4 outColor;

const float windowRadius = 0.5;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void main()
{
    if(abs(f_texcoord.x - 0.5) > windowRadius || abs(f_texcoord.y - 0.5) > windowRadius)
    {
        discard;
    }

    vec4 sampleValue1 = texture(u_Tex1, f_texcoord);
    vec4 sampleValue2 = texture(u_Tex2, f_texcoord);
    vec4 sampleValue3 = texture(u_Tex3, f_texcoord);

    float sumE = sampleValue1.x + sampleValue1.y + sampleValue1.z + sampleValue1.w +
                    sampleValue2.x + sampleValue2.y + sampleValue2.z + sampleValue2.w
                    + sampleValue3.x + sampleValue3.y + sampleValue3.z + sampleValue3.w;;
    gl_FragDepth = sumE;

    outColor = vec4(0.5, 0.0, 0.5, 1);

}

