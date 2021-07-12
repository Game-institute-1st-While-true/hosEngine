/**************** start of file "BloomCurve.fx" ***********************/

#include "CBuffer.fx"

float GetBloomCurve(float x, float threshold);

//VS 출력 구조체.
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

	// 월드 변환
	pos = mul(pos, mTM);

	// 뷰변환
	pos = mul(pos, mView);

	// 원근투영변환
	pos = mul(pos, mProj);

	//정보 출력.
	VSOutput o = (VSOutput)0;
	o.pos = pos;
	o.uv = uv;

	return o;
}

Texture2D texScreen : register(t0);

SamplerState smpLinear;

// Pixel Shader
float4 PS_Main(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	float4 screenTex = 1.0f;
	screenTex = texScreen.Sample(smpLinear, uv);

	float intensity = max(dot(screenTex.xyz, float3(0.3f, 0.3f, 0.3f)), 0.000001f);

	float bloom_intensity = GetBloomCurve(intensity, 1.0f);
	float4 bloom_color = screenTex * bloom_intensity / intensity;

	return float4(bloom_color.xyz, 1.0f);
}
//
//float4 screenTex = 1.0f;
//screenTex = texScreen.Sample(smpLinear, uv);
//
//float4 color = 0;
//
////float intensity = dot(screenTex.xyz, float3(0.3f, 0.3f, 0.3f));
////float bloom_intensity = GetBloomCurve(intensity, 1.0f);
////float4 bloom_color = screenTex * bloom_intensity / intensity;
//
//if (screenTex.a > 1.0f)
//{
//	color = screenTex;
//}
//
//return float4(color.xyz, 1.0f);

/**************** end of file "BloomCurve.fx" ***********************/


//#define BLOOMCURVE_METHOD_1
//#define BLOOMCURVE_METHOD_2
#define BLOOMCURVE_METHOD_3

float GetBloomCurve(float x, float threshold)
{
	float result = x;
	x *= 2.0f;

#ifdef BLOOMCURVE_METHOD_1
	result = x * 0.05 + max(0, x - threshold) * 0.5; // default threshold = 1.26
#endif

#ifdef BLOOMCURVE_METHOD_2
	result = x * x / 3.2;
#endif

#ifdef BLOOMCURVE_METHOD_3
	result = max(0, x - threshold); // default threshold = 1.0
	result *= result;
#endif 

	return result * 0.5f;
}
