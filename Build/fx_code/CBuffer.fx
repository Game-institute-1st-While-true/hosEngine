/**************** start of file "CBuffer.fx" ***********************/

// �⺻ �������
cbuffer cbDEFAULT : register(b0)
{
    matrix mTM;         // ���� ���. 
    matrix mView;       // �� ��ȯ ���. 
    matrix mProj;       // ���� ��ȯ ���. 
    matrix mWV;         // ����-�� ��ȯ ���. 
    matrix mWVP;        // ����-��-���� ��ȯ ���.
    matrix mIvsView;
    matrix mIvsProj;
    float4 vEye;        // ī�޶� ���� ��ǥ
};

// ���� �������
cbuffer cbMATERIAL : register(b1)
{
    float4 MtrlDiffColor;
    float4 MtrlAmbColor;
    float4 MtrlSpecColor;
    float4 OffTill;
    float  SpecularPower;
}

// �� �������
cbuffer cbBONE : register(b2)
{
    matrix mBoneTM[150];
}

 // �� �������
cbuffer cbLIGHT : register(b3)
{
    matrix LightViewProj[5];

    float4 lightDir[5];
    float4 lightDiffColor[5];
    float4 lightShadowColor[5];

    float4 pointLightPos[20];
    float4 pointLightColor[20];
    float4 pointLightShadowColor[20];
    float4 pointLightAttenuation[20];   // x = const // y = Linear // z = Quad //

    float4 lightAbmColor;

    int lightNum;
    int pointLightNum;
    float shadowBias;
}

// PostProcessing Buffer
cbuffer cbSCREEN : register(b4)
{
    matrix nowViewProjIvsTM;

    matrix oldViewProjTM;

    float screenWidth;
    float screenHeight;

    float shadowMapWidth;
    float shadowMapHeight;

    float emissMapWidth;
    float emissMapHeight;

    float emissIntensity;

    float bluePixelInterval;

    float4 vRadialBlurValue;

    float3 FogColor;
    float FogDensity;

    float dTime;
    int mortionBlurSampleCount;

    float bloomPower;
    bool bFog;
}

static float texel = 1.f / shadowMapWidth;

static float2 texelList[9] =
{
    float2(-texel, -texel), float2(0.f, -texel),float2(+texel, -texel),
    float2(-texel, 0.f), float2(0.f, 0.f), float2(+texel, 0.f),
    float2(-texel, +texel), float2(0.f, +texel), float2(+texel, +texel)
};