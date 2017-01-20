#version 410 core
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

uniform sampler2D u_Tex;
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
in vec2 f_texcoord;

out vec4 outColor;


const int W = 4;
const int H = 4;
float radius = 1.0/ 1024.0;
const float windowRadius = 0.5;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void main()
{
    if(abs(f_texcoord.x - 0.5) > windowRadius || abs(f_texcoord.y - 0.5) > windowRadius)
    {
        discard;
    }


    float sumE = 0;

    for(int x = -W; x <= W; x++)
    {
        for(int y = -H; y <= H; y++)
        {
            vec2 samplep = vec2(f_texcoord.s + x*radius, f_texcoord.t + y*radius);
            float sampleValue = texture(u_Tex, samplep).r;
            sumE += sampleValue;
        }
    }

    gl_FragDepth = sumE;
    outColor = vec4(0, 0.5, 0.5, 1);

}

