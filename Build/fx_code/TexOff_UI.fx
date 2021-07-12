/**************** start of file "TEXOFF_UI.fx" ***********************/

#include "CBuffer.fx"

//VS ��� ����ü.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float2 uv 	: TEXCOORD0;
};

// Vertex Shader
VSOutput VS_Main(
				  float4 pos : POSITION,
                  float2 uv : TEXCOORD
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
    o.uv = uv;

    return o;
}

// Pixel Shader
float4 PS_Main(
    float4 pos  : SV_POSITION,
	float2 uv   : TEXCOORD0    
) : SV_TARGET
{
    return MtrlDiffColor;
}

/**************** end of file "Default.fx" ***********************/