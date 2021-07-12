/**************** start of file "Default.fx" ***********************/

static const float PI = 3.14159265f;
static const float EPSILON = 1e-6f;

float3 TwoChannelNormalX2(float2 normal);

float Diffuse_Burley(in float NdotL, in float NdotV, in float LdotH, in float roughness);
float3 Fresnel_Shlick(in float3 f0, in float3 f90, in float x);
float3 Specular_BRDF(in float alpha, in float3 specularColor, in float NdotV, in float NdotL, in float LdotH, in float NdotH);
float Specular_D_GGX(in float alpha, in float NdotH);
float G_Shlick_Smith_Hable(float alpha, float LdotH);
float3 Diffuse_IBL(in float3 N);
float3 Specular_IBL(in float3 N, in float3 V, in float lodBias);

// 동차 좌표계
float3 TwoChannelNormalX2(float2 normal)
{
	float2 xy = 2.0f * normal - 1.0f;
	float z = sqrt(1 - dot(xy, xy));
	return float3(xy.x, xy.y, z);
}

// 표면 빛 구하기
float4 LightSurface_P2(
	in float3 V, in float3 N,
	in int numLights, in float4 lightColor[5], in float4 lightDirection[5],
	in float4 albedo, in float roughness, in float metallic, in float ambientOcclusion, in float3 worldPos, in float3 shadow)
{
	// Specular coefficiant - fixed reflectance value for non-metals
	static const float kSpecularCoefficient = 0.04;

	const float NdotV = saturate(dot(N, V));

	// Burley roughness bias
	const float alpha = roughness * roughness;

	// Blend base colors
	const float3 c_diff = lerp(albedo.xyz, float3(0, 0, 0), metallic) * ambientOcclusion;
	const float3 c_spec = lerp(kSpecularCoefficient, albedo.xyz, metallic) * ambientOcclusion;

	// Output color
	float4 acc_color = 0;
	{
		// light vector (to light)
		const float3 L = normalize(-lightDirection[0].xyz);

		// Half vector
		const float3 H = normalize(L + V);

		// products
		float NdotL = saturate(dot(N, L));
		const float LdotH = saturate(dot(L, H));
		const float NdotH = saturate(dot(N, H));

		// Diffuse & specular factors
		float diffuse_factor = Diffuse_Burley(NdotL, NdotV, LdotH, roughness);
		float3 specular = Specular_BRDF(alpha, c_spec, NdotV, NdotL, LdotH, NdotH);

		// Directional light
		float4 LightColor = 0;
		LightColor = lightColor[0];

		acc_color.xyz += NdotL * LightColor.xyz * (((c_diff * diffuse_factor) + specular));
	}

	// Diffuse Light에 Shadow 적용
	float4 shadow_color = float4(shadow.xyz, 1.f);
	acc_color *= shadow_color;

	for (int i = 1; i < numLights; i++)
	{
		// light vector (to light)
		const float3 L = normalize(-lightDirection[i].xyz);

		// Half vector
		const float3 H = normalize(L + V);

		// products
		float NdotL = saturate(dot(N, L));
		const float LdotH = saturate(dot(L, H));
		const float NdotH = saturate(dot(N, H));

		// Diffuse & specular factors
		float diffuse_factor = Diffuse_Burley(NdotL, NdotV, LdotH, roughness);
		float3 specular = Specular_BRDF(alpha, c_spec, NdotV, NdotL, LdotH, NdotH);

		// Directional light
		float4 LightColor = 0;
		LightColor = lightColor[i];

		acc_color.xyz += NdotL * LightColor.xyz * (((c_diff * diffuse_factor) + specular));
	}

	// PointLight
	if (pointLightNum != 0)
	{
		for (int j = 0; j < pointLightNum; j++)
		{
			// light vector (to light)
			const float3 L = normalize(pointLightPos[j].xyz - worldPos);

			// Half vector
			const float3 H = normalize(L + V);

			// products
			float NdotL = saturate(dot(N, L));
			const float LdotH = saturate(dot(L, H));
			const float NdotH = saturate(dot(N, H));

			// Diffuse & specular factors
			float diffuse_factor = Diffuse_Burley(NdotL, NdotV, LdotH, roughness);
			float3 specular = Specular_BRDF(alpha, c_spec, NdotV, NdotL, LdotH, NdotH);

			// Directional light
			float4 LightColor = 0;
			LightColor = lightColor[j];

			float4 light22 = PointLight(j, float4(N.xyz, 0), float4(worldPos.xyz, 0));

			acc_color.xyz += NdotL * light22.xyz * (((c_diff * diffuse_factor) + specular));
		}
	}

	// Ambient Light
	acc_color.xyz += albedo.xyz * lightAbmColor.xyz;

	// Alpha
	acc_color.a = albedo.a;

	// Add diffuse irradiance
	//float3 diffuse_env = Diffuse_IBL(N);
	//acc_color += c_diff * diffuse_env;

	// Add specular radiance 
	//float3 specular_env = Specular_IBL(N, V, roughness);
	//acc_color += c_spec * specular_env;

	return saturate(acc_color);
}


float Diffuse_Burley(in float NdotL, in float NdotV, in float LdotH, in float roughness)
{
	float fd90 = 0.5f + 2.f * roughness * LdotH * LdotH;
	return Fresnel_Shlick(1, fd90, NdotL).x * Fresnel_Shlick(1, fd90, NdotV).x;
}

float3 Fresnel_Shlick(in float3 f0, in float3 f90, in float x)
{
	return f0 + (f90 - f0) * pow(1.f - x, 5.f);
}

float3 Specular_BRDF(in float alpha, in float3 specularColor, in float NdotV, in float NdotL, in float LdotH, in float NdotH)
{
	// Specular D (microfacet normal distribution) component
	float specular_D = Specular_D_GGX(alpha, NdotH);

	// Specular Fresnel
	float3 specular_F = Fresnel_Shlick(specularColor, 1, LdotH);

	// Specular G (visibility) component
	float specular_G = G_Shlick_Smith_Hable(alpha, LdotH);

	return specular_D * specular_F * specular_G;
}

float Specular_D_GGX(in float alpha, in float NdotH)
{
	const float alpha2 = alpha * alpha;
	const float lower = (NdotH * NdotH * (alpha2 - 1)) + 1;
	return alpha2 / max(EPSILON, PI * lower * lower);
}

float G_Shlick_Smith_Hable(float alpha, float LdotH)
{
	return rcp(lerp(LdotH * LdotH, 1, alpha * alpha * 0.25f));
}

//float3 Diffuse_IBL(in float3 N)
//{
//    return IrradianceTexture.Sample(IBLSampler, N);
//}
//
//float3 Specular_IBL(in float3 N, in float3 V, in float lodBias)
//{
//    float mip = lodBias * NumRadianceMipLevels;
//    float3 dir = reflect(-V, N);
//
//    return RadianceTexture.SampleLevel(IBLSampler, dir, mip);
//}