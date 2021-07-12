/**************** start of file "SkyBox.fx" ***********************/

#include "CBuffer.fx"

//VS ��� ����ü.
struct VSOutput
{
    float4 pos  : SV_POSITION;
    float3 uvw  : TEXCOORD0;
};

// Vertex Shader
VSOutput VS_Main(
				  float4 pos : POSITION
				)
{
    // For Projection
    pos.w = 1;

    // Proj�� ������ ���Դٰ� �������� pos(-1 ~ +1)�� world�������� �����ش�(ex. 800 * 600 �������� ������)
    // �� ���� �ٷ� Cube�ڽ��� ��ǥ�Դϴ�.
    float4 uvw = pos;
    uvw = mul(uvw, mIvsProj);
    uvw = mul(uvw, mIvsView);

    //���� ���.
    VSOutput o = (VSOutput) 0;
    o.pos = pos; 
    o.uvw = uvw.xyz;

    return o;
}

// ���÷�
SamplerState smpLinear;
TextureCube texSkyBox : register(t0);     //�������� ���� ����. (�⺻���� t0)

// Pixel Shader
float4 PS_Main(
    float4 pos  : SV_POSITION,
    float3 uvw : TEXCOORD0
) : SV_TARGET
{
    float4 tex = 1;

    if (bFog)
    {
        tex.xyz = FogColor.xyz;
    }

    else
    {
        // �ؽ��� ���ø�
        tex = texSkyBox.Sample(smpLinear, uvw);

    }

    return float4(tex.xyz, 1);
}

/**************** end of file "SkyBox.fx" ***********************/
