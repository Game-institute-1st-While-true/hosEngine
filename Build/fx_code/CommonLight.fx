/**************** start of file "CommonLight.fx" ***********************/

typedef struct LightInfo
{
	float4 diffColor;
	//float4 ambColor;
	float4 mtrlColor;
}LIGHTINFO;

float4 GetDiffFactor(float3 viewNrm, float3 lightDir, matrix mView)
{
	// 물체에서 빛의 방향으로 변경
	float4 N = float4(viewNrm, 0);
	float4 L = float4(lightDir, 0) * -1;

	// 빛 반사 벡터의 View 공간화
	L = mul(L, mView);

	// 노멀라이즈
	N = normalize(N);
	L = normalize(L);

	// 조명 Factor 계산
	float4 lightFactor = max(dot(N, L), 0);

	return lightFactor;
}

float4 GetLambertLight(float4 diffFactor, float4 lightDiffColor, float4 lightAmbColor, float4 mtrlDiffColor, float4 mtrlAmbColor)
{
	// 조명 계산 
	float4 diff = diffFactor * lightDiffColor * mtrlDiffColor;
	float4 amb = lightAmbColor * mtrlAmbColor;

	// 포화도 처리 : 광량의 합을 1로 제한합니다.
	return saturate(diff + amb);
}

float3 GetLightColor(float3 viewNrm, float3 lightDir, matrix mView, float4 lightDiffColor, float4 lightAmbColor)
{
    // 물체에서 빛의 방향으로 변경
    float4 N = float4(viewNrm, 0);
    float4 L = float4(lightDir, 0) * -1;

    // 빛 반사 벡터의 View 공간화
    L = mul(L, mView);

    // 노멀라이즈
    N = normalize(N);
    L = normalize(L);

    // 조명 Factor 계산
    float4 diffFactor = max(dot(N, L), 0);

    // 조명 계산 
    float4 diff = diffFactor * lightDiffColor;
    float4 amb = lightAmbColor;

    float4 color = saturate(diff + amb);

    // 포화도 처리 : 광량의 합을 1로 제한합니다.
    return color.xyz;
}

float4 PointLight(unsigned int lightIndex, float4 worldnrm, float4 worldpos)
{
	// 상수버퍼의 점광원 위치(TM)를 viewpos와 비교하기 위해  View계산.
	float4 P = pointLightPos[lightIndex];
	//P = mul(P, mView);

	// 점광원과 픽셀간의 거리를 View 공간에서 구함.
	float D = distance(worldpos, P);

	// 현재 픽셀에서 점광원을 향하는 방향을 View 단위에서 구한 뒤 노멀라이즈
	P = P - worldpos;
	P = normalize(P);

	// Attenuation
	float4 att = pointLightAttenuation[lightIndex];
	float finalAtt = (att.x + att.y * D + att.z * (D * D));

	// 점광원 공식
	float4 Pdiff = max(dot(worldnrm, P), 0) * pointLightColor[lightIndex] / finalAtt;

	return Pdiff;
}