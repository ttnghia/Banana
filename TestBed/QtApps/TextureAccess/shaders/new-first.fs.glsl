#version 410 core
//------------------------------------------------------------------------------------------

uniform sampler2D u_Tex;
//------------------------------------------------------------------------------------------
in vec2 f_texcoord;

out vec4 outColor1;
out vec4 outColor2;
out vec4 outColor3;


const int W = 4;
float radius = 1.0/ 1024.0;

const float windowRadius = 0.5;

//------------------------------------------------------------------------------------------
void main()
{
    if(abs(f_texcoord.x - 0.5) > windowRadius || abs(f_texcoord.y - 0.5) > windowRadius)
    {
        discard;
    }

    float sumE = 0;

    for(int x = -W; x <= W; x++)
    {
        vec2 samplep = vec2(f_texcoord.s + x*radius, f_texcoord.t);
        float sampleValue = texture(u_Tex, samplep).r;
        sumE += sampleValue;
    }


    outColor1 = vec4(sumE, sumE, sumE, sumE);
    outColor2 = vec4(sumE, sumE, sumE, sumE);
    outColor3 = vec4(sumE, sumE, sumE, sumE);
}

