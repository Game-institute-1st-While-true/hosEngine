/**************** start of file "ToSwapChain_RadialBlur.fx" ***********************/

#include "CBuffer.fx"

float4 psRadialBlur(float4 color, float2 texCoord);


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
   float4 bloomTex = texBloomMap.Sample(smpLinear, uv);
   float2 velocityTex = texVelocityMap.Sample(smpLinear, uv);

   // �⺻ �÷�
   float4 color = tex;

   // ��Ǻ�
   for (int i = 0; i < mortionBlurSampleCount; ++i)
   {
	   uv += velocityTex;
	   float4 addColor = texDiffuse.Sample(smpLinear, uv);
	   color += addColor;
   }
   color /= (mortionBlurSampleCount + 1);

   // ���� ��
   float4 radialColor = psRadialBlur(color, uv);

   // ���ȿ��
   radialColor.xyz += bloomTex.xyz * bloomPower;

   return float4(radialColor.xyz, 1);
}

/**************** end of file "ToSwapChain_RadialBlur.fx" ***********************/

// ���ø��� ��ġ����
#define NUM_RADIALBLUR_SAMPLES 10
static const float samples[NUM_RADIALBLUR_SAMPLES] = { -0.08,-0.05,-0.03,-0.02,-0.01,0.01,0.02,0.03,0.05,0.08 };

// ���� ��
float4 psRadialBlur(float4 color, float2 texCoord)
{
	//float4  vRadialBlurValue.xy=center, .z=sampleDist, .w=sampleStrength

	// ���� ��ġ���� ����� ���� ���۵Ǵ� ���������� ���͸� ���Ѵ�
	float2 dir = vRadialBlurValue.xy - texCoord;

	// �� ������������ �Ÿ�(������ ũ��)
	float dist = length(dir);

	// �������ͷ� �����
	dir = dir / dist;

	float4 sum = color;

	// ���� ��ġ���� �� ������ �������� NUM_SAMPLES���� �ȼ��� ���ø��ؼ� �����Ѵ�
	for (int i = 0; i < NUM_RADIALBLUR_SAMPLES; i++)
	{
		float2 tempuv = texCoord + dir * samples[i] * vRadialBlurValue.z;

		sum += texDiffuse.Sample(smpLinear, tempuv);
		//sum += tex2D( SamplerSrc0, texCoord + dir * ( ( i - NUM_RADIALBLUR_SAMPLES / 2 ) * 0.01 ) * vRadialBlurValue.z );
	}

	// ���� �ȼ� + NUM_RADIALBLUR_SAMPLES���� �����ȼ� = NUM_RADIALBLUR_SAMPLES+1
	// �������� ���ø��� �ȼ������� ������ ��հ��� ���Ѵ�
	sum *= 1.0 / (NUM_RADIALBLUR_SAMPLES + 1);

	// ���������� �Ÿ��� ���� ����ġ�� �ش�(�־������� �� Blur��Ų��)
	float t = saturate(dist * vRadialBlurValue.w);

	// ���� �ȼ��� ������� �ȼ��� �ռ��Ѵ�
	float4 blur = lerp(color, sum, t);
	//    float4 src = tex2D(SamplerSrc1, texCoord);

	return blur;
}