/**************** start of file "Default.fx" ***********************/

#include "CBuffer.fx"

//VS ��� ����ü.
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

    // ���� ��ȯ
    pos = mul(pos, mTM);

    // �亯ȯ
    pos = mul(pos, mView);

    // ����������ȯ
    pos = mul(pos, mProj);

    //���� ���.
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
