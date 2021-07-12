/**************** start of file "CommonLight.fx" ***********************/

// ���� ��Ű�� �Լ�
float4 PosSkinning(float4 pos, float4 weight, uint4 index)
{
    float4 skinVtx = pos;

    // ����, 4���� ����ŭ ���� ��ȯ.
    float4 v0 = mul(pos, mBoneTM[index.x]);
    float4 v1 = mul(pos, mBoneTM[index.y]);
    float4 v2 = mul(pos, mBoneTM[index.z]);
    float4 v3 = mul(pos, mBoneTM[index.w]);

    // ���� ȥ��. (4���� ���븦 ������)..
    skinVtx = (v0 * weight.x) + (v1 * weight.y) + (v2 * weight.z) + (v3 * weight.w);

    return skinVtx;
}

// ��� ����
float4 NrmSkinning(float4 nrm, float4 weight, uint4 index)
{
    float4 skinNrm = nrm;

    // ���, 4���� ����ŭ ���� ��ȯ.
    float4 n0 = mul(nrm, mBoneTM[index.x]);
    float4 n1 = mul(nrm, mBoneTM[index.y]);
    float4 n2 = mul(nrm, mBoneTM[index.z]);
    float4 n3 = mul(nrm, mBoneTM[index.w]);

    // ��� ȥ��
    skinNrm = (n0 * weight.x) + (n1 * weight.y) + (n2 * weight.z) + (n3 * weight.w);

    return skinNrm;
}
