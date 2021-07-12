/**************** start of file "Default.fx" ***********************/

#include "CBuffer.fx"

//VS 출력 구조체.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float2 uv   : TEXCOORD0;
};


// Vertex Shader
VSOutput VS_Main(
				  float4 pos : POSITION,
                  float2 uv  : TEXCOORD0
				)
{
    // For Projection
    pos.w = 1;

    // 월드 변환
    pos = mul(pos, mTM);

    // 뷰변환
    pos = mul(pos, mView);

    // 원근투영변환
    pos = mul(pos, mProj);

    //정보 출력.
    VSOutput o = (VSOutput) 0;
    o.pos = pos;
    o.uv = uv;

    return o;
}


// 샘플러
SamplerState smpLinear;
Texture2D texDiffuse : register(t0);    // Diffuse Map

// Pixel Shader
float4 PS_Main(
                float4 pos  : SV_POSITION,
                float2 uv   : TEXCOORD0
                ) : SV_TARGET
{
    // 텍스쳐 샘플링
    float4 diffTex = 1;
    diffTex = texDiffuse.Sample(smpLinear, uv);

    float4 col = MtrlDiffColor;

    col.a = diffTex.a;

    return col;
}

/**************** end of file "Default.fx" ***********************/
