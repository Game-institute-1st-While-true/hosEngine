/**************** start of file "CBuffer.fx" ***********************/

// 기본 상수버퍼
cbuffer cbDEFAULT : register(b0)
{
    matrix mTM;         // 월드 행렬. 
    matrix mView;       // 뷰 변환 행렬. 
    matrix mProj;       // 투영 변환 행렬. 
    matrix mWV;         // 월드-뷰 변환 행렬. 
    matrix mWVP;        // 월드-뷰-투영 변환 행렬.
    matrix mIvsView;
    matrix mIvsProj;
    float4 vEye;        // 카메라 월드 좌표
};

// 재질 상수버퍼
cbuffer cbMATERIAL : register(b1)
{
    float4 MtrlDiffColor;
    float4 MtrlAmbColor;
    float4 MtrlSpecColor;
    float4 OffTill;
    float  SpecularPower;
}

// 뼈 상수버퍼
cbuffer cbBONE : register(b2)
{
    matrix mBoneTM[150];
}

 // 빛 상수버퍼
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