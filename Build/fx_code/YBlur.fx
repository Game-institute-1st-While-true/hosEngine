/**************** start of file "EmissMap_VBlur.fx" ***********************/

#include "CBuffer.fx"
#include "CommonPostProcess.fx"

//VS 출력 구조체.
struct VSOutput
{
	float4 pos  : SV_POSITION;
	float2 uv   : TEXCOORD0;
	float2 vTex1 : TEXCOORD1;
	float2 vTex2 : TEXCOORD2;
	float2 vTex3 : TEXCOORD3;
	float2 vTex4 : TEXCOORD4;
	float2 vTex5 : TEXCOORD5;
	float2 vTex6 : TEXCOORD6;
	float2 vTex7 : TEXCOORD7;
	float2 vTex8 : TEXCOORD8;
	float2 vTex9 : TEXCOORD9;
	float2 vTex10 : TEXCOORD10;
	float2 vTex11 : TEXCOORD11;
	float2 vTex12 : TEXCOORD12;
	float2 vTex13 : TEXCOORD13;
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
	float texelSize = 1.0f / emissMapHeight;

	//정보 출력.
	VSOutput o = (VSOutput)0;
	o.pos = pos;
	o.uv = uv;

	o.vTex1 =  uv + float2(0.0f, texelSize * -(bluePixelInterval * 6.0f));
	o.vTex2 = uv + float2(0.0f, texelSize * -(bluePixelInterval * 5.0f));
	o.vTex3 = uv + float2(0.0f, texelSize * -(bluePixelInterval * 4.0f));
	o.vTex4 = uv + float2(0.0f, texelSize * -(bluePixelInterval * 3.0f));
	o.vTex5 = uv + float2(0.0f, texelSize * -(bluePixelInterval * 2.0f));
	o.vTex6 = uv + float2(0.0f, texelSize * -(bluePixelInterval * 1.0f));
	o.vTex7 = uv;
	o.vTex8 =  uv + float2(0.0f, texelSize * (bluePixelInterval * 1.0f));
	o.vTex9 =  uv + float2(0.0f, texelSize * (bluePixelInterval * 2.0f));
	o.vTex10 = uv + float2(0.0f, texelSize * (bluePixelInterval * 3.0f));
	o.vTex11 = uv + float2(0.0f, texelSize * (bluePixelInterval * 4.0f));
	o.vTex12 = uv + float2(0.0f, texelSize * (bluePixelInterval * 5.0f));
	o.vTex13 = uv + float2(0.0f, texelSize * (bluePixelInterval * 6.0f));

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
	float2 vTex1 : TEXCOORD1,
	float2 vTex2 : TEXCOORD2,
	float2 vTex3 : TEXCOORD3,
	float2 vTex4 : TEXCOORD4,
	float2 vTex5 : TEXCOORD5,
	float2 vTex6 : TEXCOORD6,
	float2 vTex7 : TEXCOORD7,
	float2 vTex8 : TEXCOORD8,
	float2 vTex9 : TEXCOORD9,
	float2 vTex10 : TEXCOORD10,
	float2 vTex11 : TEXCOORD11,
	float2 vTex12 : TEXCOORD12,
	float2 vTex13 : TEXCOORD13
) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	color += texEmiss.Sample(smpLinear, vTex1) * BlurWeights[0];
	color += texEmiss.Sample(smpLinear, vTex2) * BlurWeights[1];
	color += texEmiss.Sample(smpLinear, vTex3) * BlurWeights[2];
	color += texEmiss.Sample(smpLinear, vTex4) * BlurWeights[3];
	color += texEmiss.Sample(smpLinear, vTex5) * BlurWeights[4];
	color += texEmiss.Sample(smpLinear, vTex6) * BlurWeights[5];
	color += texEmiss.Sample(smpLinear, vTex7) * BlurWeights[6];
	color += texEmiss.Sample(smpLinear, vTex8) * BlurWeights[7];
	color += texEmiss.Sample(smpLinear, vTex9) * BlurWeights[8];
	color += texEmiss.Sample(smpLinear, vTex10) * BlurWeights[9];
	color += texEmiss.Sample(smpLinear, vTex11) * BlurWeights[10];
	color += texEmiss.Sample(smpLinear, vTex12) * BlurWeights[11];
	color += texEmiss.Sample(smpLinear, vTex13) * BlurWeights[12];

	color.a = 1.0f;

	return color;
}

/**************** end of file "EmissMap_VBlur.fx" ***********************/
