// ���� ���
#include "Transform.h"
#include "MeshFilter.h"
#include "Material.h"
#include "GameObject.h"
#include "Animation.h"

// �ڱ� ���
#include "MeshRenderer.h"

using namespace hos;

hos::com::MeshRenderer::MeshRenderer()
	:Component(L"MeshRenderer"), RenderMode(hos::cg::RendererDesc::RenderModeList::RENDER_DEFAULT), MeshFilterRef(nullptr), MaterialRef(nullptr), bEffect(false)
{
	// �⺻ Material ����
	MaterialRef = g_DataManager->GetDefaultMaterial();
}

hos::com::MeshRenderer::~MeshRenderer()
{

}

hos::com::MeshRenderer::MeshRenderer(const MeshRenderer& dest) :
	Component(dest),
	RenderMode(dest.RenderMode),
	MaterialRef(dest.MaterialRef),
	MeshFilterRef(nullptr),
	bEffect(dest.bEffect)
{
	if (dest.m_GameObject)
	{
		MeshFilterRef = dest.m_GameObject->meshfilter;
	}
}

hos::U32 hos::com::MeshRenderer::GetDefaultBoneIndex()
{
	return MeshFilterRef->GetDefaultBoneIndex();
}

void hos::com::MeshRenderer::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->meshrenderer || gameObject->skinnedmeshrenderer)
	{
		throw std::invalid_argument("this Object already has Renderer");
	}

	gameObject->meshrenderer = this;
	m_GameObject = gameObject;
}

void hos::com::MeshRenderer::UnBind(int index)
{
	if (m_GameObject)
	{
		m_GameObject->meshrenderer = nullptr;
		m_GameObject = nullptr;
	}
}

void hos::com::MeshRenderer::AddMeshFilter(MeshFilter* meshFilter)
{
	if (meshFilter)
	{
		this->MeshFilterRef = meshFilter;
	}

	else
	{
		throw std::invalid_argument("��ȿ�� MeshFilter �ּҰ� �ƴմϴ�.");
	}
}

void hos::com::MeshRenderer::RemoveMeshFilter()
{
	this->MeshFilterRef = nullptr;
}

void hos::com::MeshRenderer::SetMaterial(hos::string materialName)
{
	this->MaterialRef = g_DataManager->GetMaterial(materialName);

	// PBR üũ
	hos::Material::MATERIAL_TYPE type = this->MaterialRef->GetMaterialType();
	if (type == hos::Material::MATERIAL_TYPE::MATERIAL_STANDARD_PBR)
	{
		this->RenderMode = hos::cg::RendererDesc::RenderModeList::RENDER_PBR;
	}
}

void hos::com::MeshRenderer::RemoveMaterial()
{
	this->MaterialRef = g_DataManager->GetDefaultMaterial();
}

void hos::com::MeshRenderer::Update()
{
	if (g_Renderer)
	{
		// �ʼ� Component Check
		Transform* _transform = m_GameObject->transform;

		if (_transform == nullptr)
		{
			Debug->LogConsole(L"MeshRenderer", m_GameObject->GetName() + L"Transform ������Ʈ�� ������ Object Render�� �Ұ����մϴ�.");
		}

		// MeshFilter�� �ִ� ���
		MeshFilter* _meshFilter = m_GameObject->meshfilter;
		if (_meshFilter)
		{
			// RenderQue�� ������ ����ü ����
			hos::cg::RenderQue::RenderObject _renderObject;

			// Base Data
			if (_meshFilter->GetMeshData() == nullptr)
			{
				Debug->LogConsole(L"MeshRenderer", m_GameObject->GetName() + L"MeshFilter�� MeshData�� �������� �ʾ� Object Render�� �Ұ����մϴ�.");
				return;
			}

			// �ʿ� ��� Get
			Matrix mWorld = _transform->GetTransformMatrix();

			// BoundingBox Update
			hos::Vector3* _refBBox = _meshFilter->GetBBoxAddress();
			for (int i = 0; i < 9; i++)
			{
				_renderObject.vBBox[i] = _refBBox[i] * mWorld;
			}

			// Ư�� ���� Mesh�� BoundingBox�� ���� ũ�⺸�� ũ�ٸ� ViewCulling�� �������� �ʴ´�.
			hos::Vector3 _BBoxSize = _meshFilter->GetBBoxSize();
			if (_BBoxSize.x > 10.f || _BBoxSize.y > 10.f || _BBoxSize.z > 10.f)
			{
				_renderObject.ViewCullSkip = true;
			}

			// RenderObject ����
			_renderObject.MeshName = _meshFilter->GetMeshName();
			_renderObject.vPos = _transform->GetPosition();
			_renderObject.mTM = mWorld;
			_renderObject.RenderMode = this->RenderMode;

			//// Animation Component Check
			//Animation* _animation = m_GameObject->GetRoot()->GetComponent< Animation >();

			//if (_animation)
			//{
			//	// BoneCount üũ(120�� ������ �׸��� �ʴ´�.)
			//	U32 _boneCount = _animation->GetBoneCount();
			//	if (_boneCount == 0 || _boneCount > MAX_BONE)
			//	{
			//		g_Renderer->DebugLog(__FUNCTION__, m_GameObject->GetName(), L"�ش� ������Ʈ�� Animation Bone�� 0���̰ų� MAX_BONE(�⺻150��)�� �ʰ��մϴ�.");
			//	}

			//	// ���� ������ 1 ~ MAX_BONE �� ���� Animation�� �����Ѵ�.
			//	else
			//	{
			//		_renderObject.mTM = _transform->GetTransformMatrix();
			//		_renderObject.bAnimation = true;
			//	}
			//}

			bool bAlpha = false;

			if (MaterialRef)
			{
				this->MaterialRef->CopyMaterialValue(_renderObject.MaterialInfo);

				// Material Check
				// Albedo Map(Main Texture)
				if (MaterialRef->IsThereAlbedo())
				{
					_renderObject.MaterialInfo.bAlbedo = true;
					_renderObject.MaterialInfo.TexNameAlbedo = MaterialRef->GetTexNameAlbedo();

					bAlpha = g_DataManager->IsAlpha(_renderObject.MaterialInfo.TexNameAlbedo);
				}

				// Normal Map
				if (MaterialRef->IsThereNormal())
				{
					_renderObject.MaterialInfo.bNormal = true;
					_renderObject.MaterialInfo.TexNameNormal = MaterialRef->GetTexNameNormal();
				}

				// AmbientOclusion(r), Roghness(g), Metalic(b) Map
				if (MaterialRef->IsThereARM())
				{
					_renderObject.MaterialInfo.bARM = true;
					_renderObject.MaterialInfo.TexNameARM = MaterialRef->GetTexNameARM();
				}

				// Emiss Map
				if (MaterialRef->IsThereEmiss())
				{
					_renderObject.MaterialInfo.bEmiss = true;
					_renderObject.MaterialInfo.TexNameEmiss = MaterialRef->GetTexNameEmiss();
				}
			}

			// ����ť ���
			if (bEffect)
			{
				g_Renderer->AddEffectObject(_renderObject);
			}
			else
			{

				if (bAlpha)
				{
					g_Renderer->AddAlphaObject(_renderObject);
				}

				else
				{
					g_Renderer->AddRenderObejct(_renderObject);
				}
			}
		}
		// MeshFilter�� ���� ��� �ƹ� �͵� ���� �ʴ´�.
		else
		{
			Debug->LogConsole(L"MeshRenderer", m_GameObject->GetName() + L"MeshFilter�� ������ Object Render�� �Ұ����մϴ�.");
			return;
		}
	}
}

const std::vector<U8> hos::com::MeshRenderer::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());
	mbstring materialName;
	if (MaterialRef)
	{
		materialName = ut::UTF16ToAnsi(MaterialRef->GetMaterialName());
	}

	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		builder.UInt("RenderMode", static_cast<U32>(RenderMode));
		builder.String("MaterialName", materialName);
		builder.Bool("bEffect", this->bEffect);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::MeshRenderer::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"DataManager", L"MeshRenderer try Deserialize to" + name);
		return false;
	}
	bool active = m["IsActive"].AsBool();
	SetActive(active);

	U32 mode = m["RenderMode"].AsUInt32();
	RenderMode = static_cast<cg::RendererDesc::RenderModeList>(mode);

	mbstring tmp = m["MaterialName"].AsString().str();
	string materialName = ut::AnsiToUTF16(tmp);

	bool effect = m["bEffect"].AsBool();

	if (m_GameObject)
	{
		MeshFilterRef = m_GameObject->meshfilter;
	}
	MaterialRef = g_DataManager->GetMaterial(materialName);
	if (MaterialRef == g_DataManager->DefaultMaterial)
	{
		MaterialRef = g_DataManager->LoadMaterial(g_Path + L"\\Assets\\Material\\" + materialName + Material::FILE_EXTENSION);
	}

	return true;
}

com::MeshRenderer* hos::com::MeshRenderer::Clone() const
{
	return new MeshRenderer(*this);
}

hos::Material* hos::com::MeshRenderer::GetMaterialRef() const
{
	if (this->MaterialRef)
	{
		return MaterialRef;
	}

	return nullptr;
}
