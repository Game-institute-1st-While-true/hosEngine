/**************** start of file "Default.fx" ***********************/

#include "CBuffer.fx"

//VS 출력 구조체.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float4 color : COLOR0;
};


// Vertex Shader
VSOutput VS_Main(
				  float4 pos : POSITION,
                  float4 color : COLOR0
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
    o.color = float4(color.xyz, 1.0f);

    return o;
}

// Pixel Shader
float4 PS_Main(
    float4 pos  : SV_POSITION,
    float4 color : COLOR0
) : SV_TARGET
{
    return color;
}

/**************** end of file "Default.fx" ***********************/
