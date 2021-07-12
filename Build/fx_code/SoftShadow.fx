/**************** start of file "Default.fx" ***********************/

#include "CBuffer.fx"
#include "CommonLight.fx"
#include "CommonPBR.fx"

//VS 출력 구조체.
struct VSOutput
{
	float4 PositionPS : SV_Position;
	float4 shadowuv   : TEXCOORD2;
};

// Vertex Shader
VSOutput VS_Main(
	float4 pos : POSITION
)
{
	// Vout
	VSOutput vout;

	// For Projection
	pos.w = 1;

	// 월드 변환
	pos = mul(pos, mTM);

	// 쉐도우맵용 uv
	float4 shadow = pos;
	shadow = mul(shadow, LightViewProj[0]);

	// View, Proj
	pos = mul(pos, mView);
	pos = mul(pos, mProj);

	// Vout
	vout.PositionPS = pos;
	vout.shadowuv = shadow;

	return vout;
}

// 샘플러
SamplerComparisonState smpShadow : register(s0);

// Texture
Texture2D texShadow  : register(t6);

// Pixel Shader
float4 PS_Main(
	float4 PositionPS : SV_Position,
	float4 shadowuv : TEXCOORD2
) : SV_TARGET
{
	// ShadowMap
	float4 Depth = shadowuv;

	// 동차 나누기
	//shadowuv = shadowuv / shadowuv.w;
	shadowuv.x = shadowuv.x * 0.5f + (0.5f + (0.5f / shadowMapWidth));
	shadowuv.y = shadowuv.y * -0.5f + (0.5f + (0.5f / shadowMapHeight));
	//shadowuv.x = shadowuv.x * 0.5f + 0.5f;
	//shadowuv.y = shadowuv.y * -0.5f + 0.5f;

	float percentLit = 0.0f;

	[unroll(9)]
	for (int i = 0; i < 9; i++)
	{
		float2 uv = shadowuv.xy + texelList[i];
		percentLit += texShadow.SampleCmpLevelZero(smpShadow, uv.xy, shadowuv.z - shadowBias).x;
	}

	percentLit /= 9;

	float4 shadowColor = 1;
	shadowColor *= percentLit;
	shadowColor.xyz += lightShadowColor[0].xyz;
	//shadowColor.xyz = lightShadowColor[0].xyz * percentLit;

	return float4(shadowColor.xyz, 1.f);
}