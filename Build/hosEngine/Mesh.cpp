#include "Object.h"

#include "Mesh.h"

using namespace hos;

hos::Mesh::Mesh()
	: MeshName(L""), FullVtxList(nullptr), IndexList(nullptr),
	VtxCount(0), IndexCount(0), mNodeTM(hos::Matrix::Identity),
	DefaultMaterialName(L""), BoundingBoxSize({0.f, 0.f, 0.f})
{
}

hos::Mesh::~Mesh()
{
	SafeDeleteArray(FullVtxList);
	SafeDeleteArray(IndexList);
}

const std::vector<U8> hos::Mesh::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(MeshName);
	mbstring materialName = ut::UTF16ToAnsi(DefaultMaterialName);

	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name.c_str());
		builder.UInt("VertexCount", VtxCount);
		builder.Vector("Vertex", [&] {
			for (U32 i = 0; i < VtxCount; i++)
			{
				builder.Vector([&] {
					builder.Float(FullVtxList[i].vPos.x);
					builder.Float(FullVtxList[i].vPos.y);
					builder.Float(FullVtxList[i].vPos.z);
					builder.Float(FullVtxList[i].vNrm.x);
					builder.Float(FullVtxList[i].vNrm.y);
					builder.Float(FullVtxList[i].vNrm.z);
					builder.Float(FullVtxList[i].vUV.x);
					builder.Float(FullVtxList[i].vUV.y);
					builder.Float(FullVtxList[i].vBiNrm.x);
					builder.Float(FullVtxList[i].vBiNrm.y);
					builder.Float(FullVtxList[i].vBiNrm.z);
					builder.Float(FullVtxList[i].vTangent.x);
					builder.Float(FullVtxList[i].vTangent.y);
					builder.Float(FullVtxList[i].vTangent.z);
					builder.Float(FullVtxList[i].weight[0]);
					builder.Float(FullVtxList[i].weight[1]);
					builder.Float(FullVtxList[i].weight[2]);
					builder.Float(FullVtxList[i].weight[3]);
					builder.UInt(FullVtxList[i].index[0]);
					builder.UInt(FullVtxList[i].index[1]);
					builder.UInt(FullVtxList[i].index[2]);
					builder.UInt(FullVtxList[i].index[3]);
					});
			}
			});
		builder.UInt("IndexCount", IndexCount);
		builder.Vector("Index", [&] {
			for (U32 i = 0; i < IndexCount; i++)
			{
				builder.UInt(IndexList[i]);
			}
			});
		builder.TypedVector("NodeTM", [&] {
			builder.Float(mNodeTM.m[0][0]);
			builder.Float(mNodeTM.m[0][1]);
			builder.Float(mNodeTM.m[0][2]);
			builder.Float(mNodeTM.m[0][3]);
			builder.Float(mNodeTM.m[1][0]);
			builder.Float(mNodeTM.m[1][1]);
			builder.Float(mNodeTM.m[1][2]);
			builder.Float(mNodeTM.m[1][3]);
			builder.Float(mNodeTM.m[2][0]);
			builder.Float(mNodeTM.m[2][1]);
			builder.Float(mNodeTM.m[2][2]);
			builder.Float(mNodeTM.m[2][3]);
			builder.Float(mNodeTM.m[3][0]);
			builder.Float(mNodeTM.m[3][1]);
			builder.Float(mNodeTM.m[3][2]);
			builder.Float(mNodeTM.m[3][3]);
			});
		builder.TypedVector("GeoTM", [&] {
			builder.Float(mGeoTM.m[0][0]);
			builder.Float(mGeoTM.m[0][1]);
			builder.Float(mGeoTM.m[0][2]);
			builder.Float(mGeoTM.m[0][3]);
			builder.Float(mGeoTM.m[1][0]);
			builder.Float(mGeoTM.m[1][1]);
			builder.Float(mGeoTM.m[1][2]);
			builder.Float(mGeoTM.m[1][3]);
			builder.Float(mGeoTM.m[2][0]);
			builder.Float(mGeoTM.m[2][1]);
			builder.Float(mGeoTM.m[2][2]);
			builder.Float(mGeoTM.m[2][3]);
			builder.Float(mGeoTM.m[3][0]);
			builder.Float(mGeoTM.m[3][1]);
			builder.Float(mGeoTM.m[3][2]);
			builder.Float(mGeoTM.m[3][3]);
			});
		builder.String("MaterialName", materialName);
		});

	builder.Finish();

	return builder.GetBuffer();
}

bool hos::Mesh::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}
	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring name = m["Name"].AsString().str();
	MeshName = ut::AnsiToUTF16(name);

	VtxCount = m["VertexCount"].AsUInt32();
	SafeDelete(FullVtxList);
	FullVtxList = new cg::VertexDataType::FULL_VTX[VtxCount];
	auto vertex = m["Vertex"].AsVector();
	for (U32 i = 0; i < VtxCount; i++)
	{
		auto v = vertex[i].AsVector();
		FullVtxList[i].vPos.x = v[0].AsFloat();
		FullVtxList[i].vPos.y = v[1].AsFloat();
		FullVtxList[i].vPos.z = v[2].AsFloat();
		FullVtxList[i].vNrm.x = v[3].AsFloat();
		FullVtxList[i].vNrm.y = v[4].AsFloat();
		FullVtxList[i].vNrm.z = v[5].AsFloat();
		FullVtxList[i].vUV.x = v[6].AsFloat();
		FullVtxList[i].vUV.y = v[7].AsFloat();
		FullVtxList[i].vBiNrm.x = v[8].AsFloat();
		FullVtxList[i].vBiNrm.y = v[9].AsFloat();
		FullVtxList[i].vBiNrm.z = v[10].AsFloat();
		FullVtxList[i].vTangent.x = v[11].AsFloat();
		FullVtxList[i].vTangent.y = v[12].AsFloat();
		FullVtxList[i].vTangent.z = v[13].AsFloat();
		FullVtxList[i].weight[0] = v[14].AsFloat();
		FullVtxList[i].weight[1] = v[15].AsFloat();
		FullVtxList[i].weight[2] = v[16].AsFloat();
		FullVtxList[i].weight[3] = v[17].AsFloat();
		FullVtxList[i].index[0] = v[18].AsUInt32();
		FullVtxList[i].index[1] = v[19].AsUInt32();
		FullVtxList[i].index[2] = v[20].AsUInt32();
		FullVtxList[i].index[3] = v[21].AsUInt32();
	}

	IndexCount = m["IndexCount"].AsInt32();
	SafeDelete(IndexList);
	IndexList = new U32[IndexCount];
	auto index = m["Index"].AsVector();
	for (U32 i = 0; i < IndexCount; i++)
	{
		IndexList[i] = index[i].AsUInt32();
	}

	auto nodetm = m["NodeTM"].AsTypedVector();
	mNodeTM.m[0][0] = nodetm[0].AsFloat();
	mNodeTM.m[0][1] = nodetm[1].AsFloat();
	mNodeTM.m[0][2] = nodetm[2].AsFloat();
	mNodeTM.m[0][3] = nodetm[3].AsFloat();
	mNodeTM.m[1][0] = nodetm[4].AsFloat();
	mNodeTM.m[1][1] = nodetm[5].AsFloat();
	mNodeTM.m[1][2] = nodetm[6].AsFloat();
	mNodeTM.m[1][3] = nodetm[7].AsFloat();
	mNodeTM.m[2][0] = nodetm[8].AsFloat();
	mNodeTM.m[2][1] = nodetm[9].AsFloat();
	mNodeTM.m[2][2] = nodetm[10].AsFloat();
	mNodeTM.m[2][3] = nodetm[11].AsFloat();
	mNodeTM.m[3][0] = nodetm[12].AsFloat();
	mNodeTM.m[3][1] = nodetm[13].AsFloat();
	mNodeTM.m[3][2] = nodetm[14].AsFloat();
	mNodeTM.m[3][3] = nodetm[15].AsFloat();

	auto geotm = m["GeoTM"].AsTypedVector();
	mGeoTM.m[0][0] = geotm[0].AsFloat();
	mGeoTM.m[0][1] = geotm[1].AsFloat();
	mGeoTM.m[0][2] = geotm[2].AsFloat();
	mGeoTM.m[0][3] = geotm[3].AsFloat();
	mGeoTM.m[1][0] = geotm[4].AsFloat();
	mGeoTM.m[1][1] = geotm[5].AsFloat();
	mGeoTM.m[1][2] = geotm[6].AsFloat();
	mGeoTM.m[1][3] = geotm[7].AsFloat();
	mGeoTM.m[2][0] = geotm[8].AsFloat();
	mGeoTM.m[2][1] = geotm[9].AsFloat();
	mGeoTM.m[2][2] = geotm[10].AsFloat();
	mGeoTM.m[2][3] = geotm[11].AsFloat();
	mGeoTM.m[3][0] = geotm[12].AsFloat();
	mGeoTM.m[3][1] = geotm[13].AsFloat();
	mGeoTM.m[3][2] = geotm[14].AsFloat();
	mGeoTM.m[3][3] = geotm[15].AsFloat();

	mbstring materialname = m["MaterialName"].AsString().str();
	DefaultMaterialName = ut::AnsiToUTF16(materialname);

	return true;
}