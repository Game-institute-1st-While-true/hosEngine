/**************** start of file "Default.fx" ***********************/

#include "CBuffer.fx"

//VS 출력 구조체.
struct VSOutput
{
    float4 pos  : SV_POSITION;
};

// Vertex Shader
VSOutput VS_Main(
				  float4 pos : POSITION
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

    return o;
}

// Pixel Shader
float4 PS_Main(
    float4 pos    : SV_POSITION
) : SV_TARGET
{
    return float4(pos.z, 0, 0, 1);
}

/**************** end of file "Default.fx" ***********************/
