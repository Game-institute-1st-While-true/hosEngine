/**************** start of file "Default.fx" ***********************/

#include "CBuffer.fx"

//VS ��� ����ü.
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

    // ���� ��ȯ
    pos = mul(pos, mTM);

    // �亯ȯ
    pos = mul(pos, mView);

    // ����������ȯ
    pos = mul(pos, mProj);

    //���� ���.
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
