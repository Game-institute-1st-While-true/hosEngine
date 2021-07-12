/**************** start of file "CommonLight.fx" ***********************/

typedef struct LightInfo
{
	float4 diffColor;
	//float4 ambColor;
	float4 mtrlColor;
}LIGHTINFO;

float4 GetDiffFactor(float3 viewNrm, float3 lightDir, matrix mView)
{
	// ��ü���� ���� �������� ����
	float4 N = float4(viewNrm, 0);
	float4 L = float4(lightDir, 0) * -1;

	// �� �ݻ� ������ View ����ȭ
	L = mul(L, mView);

	// ��ֶ�����
	N = normalize(N);
	L = normalize(L);

	// ���� Factor ���
	float4 lightFactor = max(dot(N, L), 0);

	return lightFactor;
}

float4 GetLambertLight(float4 diffFactor, float4 lightDiffColor, float4 lightAmbColor, float4 mtrlDiffColor, float4 mtrlAmbColor)
{
	// ���� ��� 
	float4 diff = diffFactor * lightDiffColor * mtrlDiffColor;
	float4 amb = lightAmbColor * mtrlAmbColor;

	// ��ȭ�� ó�� : ������ ���� 1�� �����մϴ�.
	return saturate(diff + amb);
}

float3 GetLightColor(float3 viewNrm, float3 lightDir, matrix mView, float4 lightDiffColor, float4 lightAmbColor)
{
    // ��ü���� ���� �������� ����
    float4 N = float4(viewNrm, 0);
    float4 L = float4(lightDir, 0) * -1;

    // �� �ݻ� ������ View ����ȭ
    L = mul(L, mView);

    // ��ֶ�����
    N = normalize(N);
    L = normalize(L);

    // ���� Factor ���
    float4 diffFactor = max(dot(N, L), 0);

    // ���� ��� 
    float4 diff = diffFactor * lightDiffColor;
    float4 amb = lightAmbColor;

    float4 color = saturate(diff + amb);

    // ��ȭ�� ó�� : ������ ���� 1�� �����մϴ�.
    return color.xyz;
}

float4 PointLight(unsigned int lightIndex, float4 worldnrm, float4 worldpos)
{
	// ��������� ������ ��ġ(TM)�� viewpos�� ���ϱ� ����  View���.
	float4 P = pointLightPos[lightIndex];
	//P = mul(P, mView);

	// �������� �ȼ����� �Ÿ��� View �������� ����.
	float D = distance(worldpos, P);

	// ���� �ȼ����� �������� ���ϴ� ������ View �������� ���� �� ��ֶ�����
	P = P - worldpos;
	P = normalize(P);

	// Attenuation
	float4 att = pointLightAttenuation[lightIndex];
	float finalAtt = (att.x + att.y * D + att.z * (D * D));

	// ������ ����
	float4 Pdiff = max(dot(worldnrm, P), 0) * pointLightColor[lightIndex] / finalAtt;

	return Pdiff;
}