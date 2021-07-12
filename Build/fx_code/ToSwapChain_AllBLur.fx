/**************** start of file "ToSwapChain_RadialBlur.fx" ***********************/

#include "CBuffer.fx"

float4 psRadialBlur(float4 color, float2 texCoord);


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
	//텍스쳐 셈플링.
   float4 tex = texDiffuse.Sample(smpLinear, uv);
   float4 bloomTex = texBloomMap.Sample(smpLinear, uv);
   float2 velocityTex = texVelocityMap.Sample(smpLinear, uv);

   // 기본 컬러
   float4 color = tex;

   // 모션블러
   for (int i = 0; i < mortionBlurSampleCount; ++i)
   {
	   uv += velocityTex;
	   float4 addColor = texDiffuse.Sample(smpLinear, uv);
	   color += addColor;
   }
   color /= (mortionBlurSampleCount + 1);

   // 라디알 블러
   float4 radialColor = psRadialBlur(color, uv);

   // 블룸효과
   radialColor.xyz += bloomTex.xyz * bloomPower;

   return float4(radialColor.xyz, 1);
}

/**************** end of file "ToSwapChain_RadialBlur.fx" ***********************/

// 샘플링될 위치값들
#define NUM_RADIALBLUR_SAMPLES 10
static const float samples[NUM_RADIALBLUR_SAMPLES] = { -0.08,-0.05,-0.03,-0.02,-0.01,0.01,0.02,0.03,0.05,0.08 };

// 라디알 블러
float4 psRadialBlur(float4 color, float2 texCoord)
{
	//float4  vRadialBlurValue.xy=center, .z=sampleDist, .w=sampleStrength

	// 현재 위치에서 레디얼 블러가 시작되는 지점까지의 벡터를 구한다
	float2 dir = vRadialBlurValue.xy - texCoord;

	// 블러 시작점까지의 거리(벡터의 크기)
	float dist = length(dir);

	// 단위벡터로 만든다
	dir = dir / dist;

	float4 sum = color;

	// 현재 위치에서 블러 시작점 방향으로 NUM_SAMPLES개의 픽셀을 샘플링해서 누적한다
	for (int i = 0; i < NUM_RADIALBLUR_SAMPLES; i++)
	{
		float2 tempuv = texCoord + dir * samples[i] * vRadialBlurValue.z;

		sum += texDiffuse.Sample(smpLinear, tempuv);
		//sum += tex2D( SamplerSrc0, texCoord + dir * ( ( i - NUM_RADIALBLUR_SAMPLES / 2 ) * 0.01 ) * vRadialBlurValue.z );
	}

	// 최초 픽셀 + NUM_RADIALBLUR_SAMPLES개의 누적픽셀 = NUM_RADIALBLUR_SAMPLES+1
	// 누적값을 샘플링된 픽셀개수로 나누어 평균값을 구한다
	sum *= 1.0 / (NUM_RADIALBLUR_SAMPLES + 1);

	// 시작점과의 거리에 따라서 가중치를 준다(멀어질수록 더 Blur시킨다)
	float t = saturate(dist * vRadialBlurValue.w);

	// 현재 픽셀과 누적평균 픽셀을 합성한다
	float4 blur = lerp(color, sum, t);
	//    float4 src = tex2D(SamplerSrc1, texCoord);

	return blur;
}