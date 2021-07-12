/**************** start of file "ToSwapChain_MortionBlur.fx" ***********************/

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

Texture2D texDiffuse : register(t0);
Texture2D texVelocityMap : register(t4);
Texture2D texBloomMap : register(t5);
SamplerState smpLinear;

// Pixel Shader
float4 PS_Main(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	//�ؽ��� ���ø�.
   float4 tex = texDiffuse.Sample(smpLinear, uv);

   float2 velocityTex = texVelocityMap.Sample(smpLinear, uv);
   
   float4 bloomTex = texBloomMap.Sample(smpLinear, uv);

   for (int i = 0; i < mortionBlurSampleCount; ++i)
   {
	   uv += velocityTex;
	   float4 addColor = texDiffuse.Sample(smpLinear, uv);
	   tex += addColor;
   }

   tex /= (mortionBlurSampleCount + 1);

   tex.xyz += bloomTex.xyz * bloomPower;

   return float4(tex.xyz, 1);
}

/**************** end of file "ToSwapChain_MortionBlur.fx" ***********************/
