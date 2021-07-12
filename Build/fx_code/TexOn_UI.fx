/**************** start of file "TEXOFF_UI.fx" ***********************/

#include "CBuffer.fx"

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
SamplerState smpLinear;

// Pixel Shader
float4 PS_Main(
	float4 pos  : SV_POSITION,
	float2 uv : TEXCOORD0
) : SV_TARGET
{
	//텍스쳐 셈플링.
   float4 tex = texDiffuse.Sample(smpLinear, uv);

   // Off보다 작으면 제거
   clip(uv.x < OffTill.x || uv.y < OffTill.z ? -1 : 1);

   // Till보다 크면 제거
   clip(uv.x > OffTill.y || uv.y > OffTill.w ? -1 : 1);

   //tex.w = tex.a * MtrlDiffColor.w;
   tex = tex * MtrlDiffColor;

   return tex;
}

/**************** end of file "Default.fx" ***********************/
