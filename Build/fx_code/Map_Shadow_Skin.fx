/**************** start of file "ShadowMap_Skin.fx" ***********************/

#include "CBuffer.fx"
#include "CommonSkinning.fx"


//VS ��� ����ü.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float4 uv  : TEXCOORD0;
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

    // Light ���������� ��ȯ
    pos = mul(pos, mTM);
    pos = PosSkinning(pos, weight, index);
    pos = mul(pos, LightViewProj[0]);

    //���� ���.
    VSOutput o = (VSOutput)0;
    o.pos = pos;
    o.uv = pos; // Z���� �����ϱ� ���� ���� ���� �����Ѵ�.

    return o;
}

// Pixel Shader
void PS_Main(
    float4 pos    : SV_POSITION,
    float4 uv     : TEXCOORD0
)
{
    //uv = uv / uv.w;

    //// �÷� ����
    //float4 col = 0;
    //col.r = uv.z / uv.w;
    //col.w = 1;

    //return col;
}

/**************** end of file "Default.fx" ***********************/
