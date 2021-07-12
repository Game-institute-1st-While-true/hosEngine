/**************** start of file "Default.fx" ***********************/

#include "CBuffer.fx"
#include "CommonSkinning.fx"

//VS 출력 구조체.
struct VSOutput
{
    float4 pos  : SV_POSITION;
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
    // SkinPos
    float4 skinPos = float4(pos.xyz, 1);
    skinPos = mul(skinPos, mTM);
    skinPos = PosSkinning(skinPos, weight, index);
    skinPos = mul(skinPos, mView);
    skinPos = mul(skinPos, mProj);
  
    //정보 출력.
    VSOutput o = (VSOutput) 0;
    o.pos = skinPos; 

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
