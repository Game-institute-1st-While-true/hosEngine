/**************** start of file "TEXOFF_UI.fx" ***********************/

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
SamplerState smpLinear;

// Pixel Shader
float4 PS_Main(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	//�ؽ��� ���ø�.
   float4 tex = texDiffuse.Sample(smpLinear, uv);

   // Off���� ������ ����
   clip(uv.x < OffTill.x || uv.y < OffTill.z ? -1 : 1);

   // Till���� ũ�� ����
   clip(uv.x > OffTill.y || uv.y > OffTill.w ? -1 : 1);

   //tex.w = tex.a * MtrlDiffColor.w;
   tex = tex * MtrlDiffColor;

   return tex;
}

/**************** end of file "Default.fx" ***********************/
