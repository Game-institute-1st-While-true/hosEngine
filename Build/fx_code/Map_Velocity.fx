/**************** start of file "VelocityMap.fx" ***********************/

#include "CBuffer.fx"

float4 GetWorldPosFromDepth(float2 uv, float depth, matrix viewIvs, matrix projIvs);

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

Texture2D texNowDepth : register(t0);

SamplerState smpLinear;

// Pixel Shader
float4 PS_Main(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	float4 zOverW = 1.0f;
	zOverW = texNowDepth.Sample(smpLinear, uv);

	float4 H = float4(uv.x * 2 - 1, uv.y * -2 + 1, zOverW.r, 1);
	
	float4 D = mul(H, nowViewProjIvsTM);

	float4 worldPos = D / D.w;

	float4 currentPos = H;

	worldPos.w = 1;
	float4 previousPos = mul(worldPos, oldViewProjTM);
	previousPos /= previousPos.w;

	float2 velocity = (currentPos.xy - previousPos.xy) / 2 * dTime;
	velocity.y *= -1;

	//if (velocity.x < 0.001f)
	//{
	//	velocity.x = 0.001f;
	//}

	//if (velocity.y < 0.001f)
	//{
	//	velocity.y = 0.001f;
	//}

	velocity.x = clamp(velocity.x, 0.001f, 0.003f);
	velocity.y = clamp(velocity.y, 0.001f, 0.003f);

	return float4(velocity, 0, 1);
}

/**************** end of file "VelocityMap.fx" ***********************/

float4 GetWorldPosFromDepth(float2 uv, float depth, matrix viewIvs, matrix projIvs)
{
	float x, y, z;
	x = uv.x * 2.0f - 1.0f;
	y = uv.y * -2.0f + 1.0f;
	z = depth;

	float4 ndc = float4(x, y, z, 1.0f);

	float4 wp = mul(ndc, projIvs);
	wp = mul(wp, viewIvs);
	wp = wp / wp.w;

	return wp;
}