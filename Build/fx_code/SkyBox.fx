/**************** start of file "SkyBox.fx" ***********************/

#include "CBuffer.fx"

//VS 출력 구조체.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float3 uvw  : TEXCOORD0;
};

// Vertex Shader
VSOutput VS_Main(
				  float4 pos : POSITION
				)
{
    // For Projection
    pos.w = 1;

    // Proj이 끝나서 들어왔다고 여겨지는 pos(-1 ~ +1)를 world공간으로 돌려준다(ex. 800 * 600 공간으로 돌려줌)
    // 이 곳이 바로 Cube박스의 좌표입니다.
    float4 uvw = pos;
    uvw = mul(uvw, mIvsProj);
    uvw = mul(uvw, mIvsView);

    //정보 출력.
    VSOutput o = (VSOutput) 0;
    o.pos = pos; 
    o.uvw = uvw.xyz;

    return o;
}

// 샘플러
SamplerState smpLinear;
TextureCube texSkyBox : register(t0);     //레지스터 직접 지정. (기본값은 t0)

// Pixel Shader
float4 PS_Main(
    float4 pos  : SV_POSITION,
    float3 uvw : TEXCOORD0
) : SV_TARGET
{
    float4 tex = 1;

    if (bFog)
    {
        tex.xyz = FogColor.xyz;
    }

    else
    {
        // 텍스쳐 샘플링
        tex = texSkyBox.Sample(smpLinear, uvw);

    }

    return float4(tex.xyz, 1);
}

/**************** end of file "SkyBox.fx" ***********************/
