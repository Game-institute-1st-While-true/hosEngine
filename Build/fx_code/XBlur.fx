/**************** start of file "EmissMap_HBlur.fx" ***********************/

#include "CBuffer.fx"
#include "CommonPostProcess.fx"

//VS 출력 구조체.
struct VSOutput
{
	float4 pos  : SV_POSITION;
	float2 uv   : TEXCOORD0;
	float2 hTex1 : TEXCOORD1;
	float2 hTex2 : TEXCOORD2;
	float2 hTex3 : TEXCOORD3;
	float2 hTex4 : TEXCOORD4;
	float2 hTex5 : TEXCOORD5;
	float2 hTex6 : TEXCOORD6;
	float2 hTex7 : TEXCOORD7;
	float2 hTex8 : TEXCOORD8;
	float2 hTex9 : TEXCOORD9;
	float2 hTex10 : TEXCOORD10;
	float2 hTex11 : TEXCOORD11;
	float2 hTex12 : TEXCOORD12;
	float2 hTex13 : TEXCOORD13;
};

// Vertex Shader
VSOutput VS_Main(
	float4 pos : POSITION,
	float2 uv : TEXCOORD0
)
{
	// For Projection
	pos.w = 1;

	// WVP 변환
	pos = mul(pos, mWVP);

	// 추가 Tex 계산
	float texelSize = 1.0f / emissMapWidth;

	//정보 출력.
	VSOutput o = (VSOutput)0;
	o.pos = pos;
	o.uv = uv;

	o.hTex1 = uv + float2(texelSize * -(bluePixelInterval * 6.0f), 0.0f);
	o.hTex2 = uv + float2(texelSize * -(bluePixelInterval * 5.0f), 0.0f);
	o.hTex3 = uv + float2(texelSize * -(bluePixelInterval * 4.0f), 0.0f);
	o.hTex4 = uv + float2(texelSize * -(bluePixelInterval * 3.0f), 0.0f);
	o.hTex5 = uv + float2(texelSize * -(bluePixelInterval * 2.0f), 0.0f);
	o.hTex6 = uv + float2(texelSize * -(bluePixelInterval * 1.0f), 0.0f);
	o.hTex7 = uv;
	o.hTex8 = uv + float2(texelSize * (bluePixelInterval * 1.0f), 0.0f);
	o.hTex9 = uv + float2(texelSize * (bluePixelInterval * 2.0f), 0.0f);
	o.hTex10 = uv + float2(texelSize * (bluePixelInterval * 3.0f), 0.0f);
	o.hTex11 = uv + float2(texelSize * (bluePixelInterval * 4.0f), 0.0f);
	o.hTex12 = uv + float2(texelSize * (bluePixelInterval * 5.0f), 0.0f);
	o.hTex13 = uv + float2(texelSize * (bluePixelInterval * 6.0f), 0.0f);

	return o;
}

// 샘플러
SamplerState smpLinear;

// Texture
Texture2D texEmiss   : register(t0);    // Emission Map

// Pixel Shader
float4 PS_Main(
	float4 pos    : SV_POSITION,
	float2 uv : TEXCOORD0,
	float2 hTex1 : TEXCOORD1,
	float2 hTex2 : TEXCOORD2,
	float2 hTex3 : TEXCOORD3,
	float2 hTex4 : TEXCOORD4,
	float2 hTex5 : TEXCOORD5,
	float2 hTex6 : TEXCOORD6,
	float2 hTex7 : TEXCOORD7,
	float2 hTex8 : TEXCOORD8,
	float2 hTex9 : TEXCOORD9,
	float2 hTex10 : TEXCOORD10,
	float2 hTex11 : TEXCOORD11,
	float2 hTex12 : TEXCOORD12,
	float2 hTex13 : TEXCOORD13
) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	color += texEmiss.Sample(smpLinear, hTex1) * BlurWeights[0];
	color += texEmiss.Sample(smpLinear, hTex2) * BlurWeights[1];
	color += texEmiss.Sample(smpLinear, hTex3) * BlurWeights[2];
	color += texEmiss.Sample(smpLinear, hTex4) * BlurWeights[3];
	color += texEmiss.Sample(smpLinear, hTex5) * BlurWeights[4];
	color += texEmiss.Sample(smpLinear, hTex6) * BlurWeights[5];
	color += texEmiss.Sample(smpLinear, hTex7) * BlurWeights[6];
	color += texEmiss.Sample(smpLinear, hTex8) * BlurWeights[7];
	color += texEmiss.Sample(smpLinear, hTex9) * BlurWeights[8];
	color += texEmiss.Sample(smpLinear, hTex10) * BlurWeights[9];
	color += texEmiss.Sample(smpLinear, hTex11) * BlurWeights[10];
	color += texEmiss.Sample(smpLinear, hTex12) * BlurWeights[11];
	color += texEmiss.Sample(smpLinear, hTex13) * BlurWeights[12];

	color.a = 1.0f;

	return color;
}

/**************** end of file "EmissMap_VBlur.fx" ***********************/
