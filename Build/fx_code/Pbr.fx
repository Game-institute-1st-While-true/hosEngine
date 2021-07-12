/**************** start of file "Default.fx" ***********************/

#include "CBuffer.fx"
#include "CommonLight.fx"
#include "CommonPBR.fx"

//VS 출력 구조체.
struct VSOutput
{
	float4 PositionPS : SV_Position;
	float4 shadowuv   : TEXCOORD2;
	float3 WorldPos   : TEXCOORD3;
	float3 ViewPos	  : TEXCOORD4;
	float3 Normal     : NORMAL;
	float3 Binormal   : BINORMAL;
	float3 Tangent    : TANGENT;
	float3 ViewVector : TEXCOORD1;
	float2 TexCoord   : TEXCOORD0;
};

// Vertex Shader
VSOutput VS_Main(
	float4 pos : POSITION,
	float4 nrm : NORMAL,
	float2 uv : TEXCOORD0,
	float4 binormal : BINORMAL,
	float4 tangent : TANGENT
)
{
	// Vout
	VSOutput vout;

	// For Projection
	pos.w = 1;
	nrm.w = 0;

	// 월드 변환
	pos = mul(pos, mTM);
	nrm = normalize(mul(nrm, mTM));
	
	vout.WorldPos = pos.xyz;

	// 쉐도우맵용 uv
	float4 shadow = pos;
	shadow = mul(shadow, LightViewProj[0]);

	// ViewVector
	float3 viewVector = normalize(vEye.xyz - pos.xyz);

	// View, Proj
	pos = mul(pos, mView);

	vout.ViewPos = pos.xyz;

	pos = mul(pos, mProj);


	vout.PositionPS = pos;
	vout.shadowuv = shadow;
	vout.Normal = nrm.xyz;
	vout.Binormal = binormal.xyz;
	vout.Tangent = tangent.xyz;
	vout.ViewVector = viewVector;
	vout.TexCoord = uv;

	return vout;
}

// 샘플러
SamplerState smpLinear : register(s0);
SamplerState smpBorder : register(s1);
SamplerComparisonState smpShadow : register(s2);

// Texture
Texture2D texDiffuse : register(t0);    // Diffuse Map
Texture2D texNormal  : register(t1);    // Normal Map
Texture2D texARM     : register(t2);    // AO / Roughness / Metalic Map
Texture2D texEmiss   : register(t3);    // EmissION mAP

Texture2D texShadow  : register(t6);

// Pixel Shader
float4 PS_Main(
	float4 PositionPS : SV_Position,
	float4 shadowuv   : TEXCOORD2,
	float3 WorldPos   : TEXCOORD3,
	float3 ViewPos	  : TEXCOORD4,
	float3 Normal     : NORMAL,
	float3 Binormal   : BINORMAL,
	float3 Tangent    : TANGENT,
	float3 ViewVector : TEXCOORD1,
	float2 TexCoord   : TEXCOORD0
) : SV_TARGET
{
	// 출력용 컬러
	float4 color = 0;
	float alpha = 1;

	// Diffuse 샘플링
	float4 albedoTex = 1;
	albedoTex = texDiffuse.Sample(smpLinear, TexCoord);
	alpha = albedoTex.w;

	// 감마 코렉션(인코딩->리니어: 어둡게)
	albedoTex = albedoTex * albedoTex;

	// Emiss 샘플링
	float4 emissTex = 1;
	emissTex = texEmiss.Sample(smpLinear, TexCoord);

	// ARM Map 샘플링
	float4 armTex = 1;
	armTex = texARM.Sample(smpLinear, TexCoord);

	// Normal Map 샘플링
	float4 normalTex = 1;
	normalTex = texNormal.Sample(smpLinear, TexCoord);

	// Before lighting, peturb the surface's normal by the one given in normal map.
	float3 localNormal = TwoChannelNormalX2(normalTex.xy);
	float3x3 TBN = float3x3(Tangent.xyz, Binormal.xyz, Normal.xyz);
	float3 N = normalize(mul(localNormal, TBN));

	// ShadowMap
	float4 Depth = shadowuv;

	// 동차 나누기
	//shadowuv = shadowuv / shadowuv.w;
	shadowuv.x = shadowuv.x * 0.5f + (0.5f + (0.5f / shadowMapWidth));
	shadowuv.y = shadowuv.y * -0.5f + (0.5f + (0.5f / shadowMapHeight));
	//shadowuv.x = shadowuv.x * 0.5f + 0.5f;
	//shadowuv.y = shadowuv.y * -0.5f + 0.5f;

	float percentLit = 0.0f;

	[unroll(9)]
	for (int i = 0; i < 9; i++)
	{
		float2 uv = shadowuv.xy + texelList[i];
		percentLit += texShadow.SampleCmpLevelZero(smpShadow, uv.xy, shadowuv.z - shadowBias).x;
	}

	percentLit /= 9;

	float4 shadowColor = 1;
	shadowColor *= percentLit;
	shadowColor.xyz += lightShadowColor[0].xyz;



	// 쉐도우맵 샘플링
	//float4 shadowColor = 1;
	//float shadowMap = texShadow.Sample(smpBorder, shadowuv).x;

	
	//shadowColor = (shadowMap * Depth.w < Depth.z - shadowBias) ? lightShadowColor[0] : 1;
	//shadowColor.w = 1;

	// Shade surface
	//color = LightSurface_P(ViewVector, N, lightNum, lightDiffColor, lightDir, albedoTex, armTex.g, armTex.b, armTex.r, lightColor, pLightDir);
	color = LightSurface_P2(ViewVector, N, lightNum, lightDiffColor, lightDir, albedoTex, armTex.g, armTex.b, armTex.r, WorldPos, shadowColor.xyz);

	// 감마코렉션(리니어->인코딩: 밝게)
	color = pow(color, 0.5f);

	//// 픽셀 안개(거리)
	float d = length(ViewPos);
	float f = 1 / exp(d * FogDensity);
	color = f * color + (1.f - f) * float4(FogColor.xyz, 1.f);

	// Alpha
	color.a = alpha;

	// emission
	color += emissTex;

	return color;
}