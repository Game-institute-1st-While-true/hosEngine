/**************** start of file "CombineScreen.fx" ***********************/

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
	VSOutput o = (VSOutput)0;
	o.pos = pos;
	o.uv = uv;

	return o;
}

Texture2D texScreen1 : register(t0);
Texture2D texScreen2 : register(t1);

SamplerState smpLinear;

// Pixel Shader
float4 PS_Main(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	float4 add1 = 1;
	add1 = texScreen1.Sample(smpLinear, uv);

	float4 add2 = 1;
	add2 = texScreen2.Sample(smpLinear, uv);

	float4 color = 1;
	color = add1 + add2;

	return float4(color.xyz, 1);
}

/**************** end of file "CombineScreen.fx" ***********************/