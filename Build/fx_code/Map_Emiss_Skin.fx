/**************** start of file "EmissMap_Skin.fx" ***********************/

#include "CBuffer.fx"
#include "CommonSkinning.fx"

//VS ��� ����ü.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float2 uv   : TEXCOORD0;
};

// Vertex Shader
VSOutput VS_Main(
    float4 pos : POSITION,
    float4 nrm : NORMAL,
    float2 uv : TEXCOORD0,
    float4 binormal : BINORMAL,
    float4 tangent : TANGENT,
    float4 weight : BLENDWEIGHT,
    uint4  index : BLENDINDICES
				)
{
    // For Projection
    pos.w = 1;

    // ���� ��ȯ
    pos = mul(pos, mTM);

    // ��Ű�� ��ȯ
    pos = PosSkinning(pos, weight, index);

    // �亯ȯ
    pos = mul(pos, mView);

    // ����������ȯ
    pos = mul(pos, mProj);

    //���� ���.
    VSOutput o = (VSOutput)0;
    o.pos = pos;
    o.uv = uv;

    return o;
}


// ���÷�
SamplerState smpLinear;

// Texture
Texture2D texEmiss   : register(t3);    // Emission Map

// Pixel Shader
float4 PS_Main(
    float4 pos    : SV_POSITION,
    float2 uv : TEXCOORD0
) : SV_TARGET
{
    float4 color = texEmiss.Sample(smpLinear, uv);

    color = color * emissIntensity;

    return float4(color.xyz, 1);
}

/**************** end of file "EmissMap_Skin.fx" ***********************/