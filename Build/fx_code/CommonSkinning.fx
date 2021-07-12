/**************** start of file "CommonLight.fx" ***********************/

// 정점 스키닝 함수
float4 PosSkinning(float4 pos, float4 weight, uint4 index)
{
    float4 skinVtx = pos;

    // 정점, 4개의 뼈만큼 각각 변환.
    float4 v0 = mul(pos, mBoneTM[index.x]);
    float4 v1 = mul(pos, mBoneTM[index.y]);
    float4 v2 = mul(pos, mBoneTM[index.z]);
    float4 v3 = mul(pos, mBoneTM[index.w]);

    // 정점 혼합. (4개의 뼈대를 참조시)..
    skinVtx = (v0 * weight.x) + (v1 * weight.y) + (v2 * weight.z) + (v3 * weight.w);

    return skinVtx;
}

// 노멀 블렌딩
float4 NrmSkinning(float4 nrm, float4 weight, uint4 index)
{
    float4 skinNrm = nrm;

    // 노멀, 4개의 뼈만큼 각각 변환.
    float4 n0 = mul(nrm, mBoneTM[index.x]);
    float4 n1 = mul(nrm, mBoneTM[index.y]);
    float4 n2 = mul(nrm, mBoneTM[index.z]);
    float4 n3 = mul(nrm, mBoneTM[index.w]);

    // 노멀 혼합
    skinNrm = (n0 * weight.x) + (n1 * weight.y) + (n2 * weight.z) + (n3 * weight.w);

    return skinNrm;
}
