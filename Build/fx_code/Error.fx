/**************** start of file "Default.fx" ***********************/

#include "CBuffer.fx"
#include "CommonLight.fx "

//VS ��� ����ü.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float4 color : COLOR0;
};

// Vertex Shader
VSOutput VS_Main(
				  float4 pos : POSITION,
                  float4 nrm : NORMAL
				)
{
    // For Projection
    pos.w = 1;
    nrm.w = 0;

    // ���� ��ȯ
    pos = mul(pos, mTM);
    nrm = mul(nrm, mTM);

    // �亯ȯ
    pos = mul(pos, mView);
    nrm = mul(nrm, mView);

    // DiffuseLight ���
    float4 diffFactor = GetDiffFactor(nrm.xyz, float3(0, -1, 1), mView);
    float4 diffColor = GetLambertLight(diffFactor, float4(1, 0, 1, 1), float4(0.0f, 0.0f, 0.0f, 0.0f), MtrlDiffColor, MtrlAmbColor);
    
    // ����������ȯ
    pos = mul(pos, mProj);

    //���� ���.
    VSOutput o = (VSOutput) 0;
    o.pos = pos; 
    o.color = float4(diffColor.xyz, 1);

    return o;
}

// Pixel Shader
float4 PS_Main(
    float4 pos  : SV_POSITION,
    float4 color : COLOR0
) : SV_TARGET
{
    return color;
}

/**************** end of file "Default.fx" ***********************/
