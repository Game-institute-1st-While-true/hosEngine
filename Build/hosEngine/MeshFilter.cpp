// 자기 헤더
#include "MeshFilter.h"

#include "GameObject.h"

using namespace hos;

hos::com::MeshFilter::MeshFilter()
	:Component(L"MeshFilter"), MeshData(nullptr)
{

}

hos::com::MeshFilter::~MeshFilter()
{

}

hos::com::MeshFilter::MeshFilter(const MeshFilter& dest):
	Component(dest),
	MeshData(dest.MeshData)
{
}

void hos::com::MeshFilter::SetMeshData(Mesh* data)
{
	if (data)
	{
		this->MeshData = data;
	}

	else
	{
		throw std::invalid_argument("유효한 MeshData 주소가 아닙니다.");
	}
}

void hos::com::MeshFilter::RemoveMeshData()
{
	this->MeshData = nullptr;
}

hos::Mesh* hos::com::MeshFilter::GetMeshData() const
{
	if (this->MeshData)
	{
		return MeshData;
	}

	return nullptr;
}

hos::Vector3* hos::com::MeshFilter::GetBBoxAddress()
{
	return this->MeshData->GetBBoxAddress();
}

com::MeshFilter* hos::com::MeshFilter::Clone() const
{
	return new MeshFilter(*this);
}

void hos::com::MeshFilter::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->meshfilter)
	{
		throw std::invalid_argument("this Object already has MeshFilter");
	}

	m_GameObject = gameObject;
	gameObject->meshfilter = this;
}

void hos::com::MeshFilter::UnBind(int index)
{
	if (m_GameObject)
	{
		m_GameObject->meshfilter = nullptr;
		m_GameObject = nullptr;
	}
}

const std::vector<U8> hos::com::MeshFilter::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());
	mbstring meshname = "NULL";
	if (MeshData)
	{
		meshname = ut::UTF16ToAnsi(MeshData->GetMeshName());
	}
	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.String("MeshName",meshname);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::MeshFilter::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring sn = m["Name"].AsString().str();
	string name = ut::AnsiToUTF16(sn);
	if (name != GetName())
	{
		Debug->LogConsole(L"DataManager", L"MeshFilter try Deserialize to" + name);
		return false;
	}
		
	mbstring tmp = m["MeshName"].AsString().str();
	string meshname = ut::AnsiToUTF16(tmp);
	MeshData = g_DataManager->GetMesh(meshname);
	if (nullptr == MeshData)
	{
		MeshData = g_DataManager->LoadMesh(g_Path + L"\\Assets\\Mesh\\" + meshname + Mesh::FILE_EXTENSION);
	}

	return true;
}