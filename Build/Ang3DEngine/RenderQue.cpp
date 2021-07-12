// ���� ���
#include "DebugConsole.h"

// �ڱ� ���
#include "RenderQue.h"

// ���ӽ����̽�
using namespace std;
using namespace DirectX::SimpleMath;

hos::cg::RenderQue::RenderQue()
	: NowObjectCount(0), MaxObjectCount(DEFAULT_RENDERQUE_COUNT), NowAlphaCount(0), MaxAlphaCount(DEFAULT_RENDERQUE_COUNT),
	NowEffectCount(0), MaxEffectCount(100)
{
	RenderObjectList.resize(MaxObjectCount);
	AlphaObjectList.resize(MaxAlphaCount);
	EffectList.resize(this->MaxEffectCount);

	DebugObjectList.reserve(100);
	UIObjectList.reserve(100);

	DEBUG_CONSOLE(L"RenderQue ���� �Ϸ�.");
}

hos::cg::RenderQue::~RenderQue()
{
	// ������� ����
	RenderObjectList.clear();
	RenderObjectList.shrink_to_fit();

	AlphaObjectList.clear();
	AlphaObjectList.shrink_to_fit();

	EffectList.clear();
	EffectList.shrink_to_fit();

	DebugObjectList.clear();
	DebugObjectList.shrink_to_fit();

	UIObjectList.clear();
	UIObjectList.shrink_to_fit();

	DEBUG_CONSOLE(L"RenderQue ���� �Ϸ�.");
}

/// TODO: �Ķ���� ���� �� �Ͼ�� ���۷������� ������ �� �ְ�?
void hos::cg::RenderQue::AddRenderObejct(RenderObject& objectInfo)
{
	// ���� RenderQue�� �ִ� ũ�⸦ �Ѿ��ٸ� �Ҵ� ������ 2��� �÷��ִ� �Լ��Դϴ�.
	CheckMaxSize();

	// �ʿ��� �����͸� �������ݴϴ�.
	this->RenderObjectList[this->NowObjectCount] = objectInfo;

	// ����ť�� ������ �÷��ݴϴ�.
	this->NowObjectCount++;
}

void hos::cg::RenderQue::AddAlphaObject(RenderObject& objectInfo)
{
	// ���� RenderQue�� �ִ� ũ�⸦ �Ѿ��ٸ� �Ҵ� ������ 2��� �÷��ִ� �Լ��Դϴ�.
	CheckMaxSize_Alpha();

	// �ʿ��� �����͸� �������ݴϴ�.
	objectInfo.ViewDistance = 0.0f;
	this->AlphaObjectList[this->NowAlphaCount] = objectInfo;

	// ����ť�� ������ �÷��ݴϴ�.
	this->NowAlphaCount++;
}

void hos::cg::RenderQue::AddEffectObject(RenderObject& objectInfo)
{
	// ���� RenderQue�� �ִ� ũ�⸦ �Ѿ��ٸ� �Ҵ� ������ 2��� �÷��ִ� �Լ��Դϴ�.
	CheckMaxSize_Effect();

	// �ʿ��� �����͸� �������ݴϴ�.
	objectInfo.ViewDistance = 0.0f;
	this->EffectList[this->NowEffectCount] = objectInfo;

	// ����ť�� ������ �÷��ݴϴ�.
	this->NowEffectCount++;
}

void hos::cg::RenderQue::AddDebugObject(RenderObject& objectInfo)
{
	this->DebugObjectList.push_back(objectInfo);
}

void hos::cg::RenderQue::AddUIObject(RenderUIObejct& objectInfo)
{
	this->UIObjectList.push_back(objectInfo);
}

void hos::cg::RenderQue::ClearObjectRenderQue()
{
	this->NowObjectCount = 0;
	this->NowAlphaCount = 0;
	this->NowEffectCount = 0;
	this->DebugObjectList.clear();
}

void hos::cg::RenderQue::ClearUIRenderQue()
{
	this->UIObjectList.clear();
}

void hos::cg::RenderQue::CheckMaxSize()
{
	// RenderQue�� �ִ� �Ҵ� ������ �����Ѵٸ�?
	if (this->NowObjectCount == this->MaxObjectCount)
	{
		// Vector Ŭ������ ũ�⸦ 2��� �÷��ش�.
		this->MaxObjectCount *= 2;
		RenderObjectList.resize(this->MaxObjectCount);
	}
}

void hos::cg::RenderQue::CheckMaxSize_Alpha()
{
	// AlphaQue�� �ִ� �Ҵ� ������ �����Ѵٸ�?
	if (this->NowAlphaCount == this->MaxAlphaCount)
	{
		// Vector Ŭ������ ũ�⸦ 2��� �÷��ش�.
		this->MaxAlphaCount *= 2;
		AlphaObjectList.resize(this->MaxAlphaCount);
	}
}

void hos::cg::RenderQue::CheckMaxSize_Effect()
{
	// EffectQue�� �ִ� �Ҵ� ������ �����Ѵٸ�?
	if (this->NowEffectCount == this->MaxEffectCount)
	{
		// Vector Ŭ������ ũ�⸦ 2��� �÷��ش�.
		this->MaxEffectCount *= 2;
		EffectList.resize(this->MaxEffectCount);
	}
}

void hos::cg::RenderQue::SortUIQue_LayerOrder()
{
	std::sort(this->UIObjectList.begin(), this->UIObjectList.end());;
}

void hos::cg::RenderQue::CalculateViewDistanceInAlphaQue(Vector3 eyepos)
{
	Vector3 _objectPos;

	for (unsigned int i = 0; i < this->NowAlphaCount; i++)
	{
		if (AlphaObjectList[i].ViewDistance == 0.0f)
		{
			_objectPos = AlphaObjectList[i].vPos;
			AlphaObjectList[i].ViewDistance = DirectX::SimpleMath::Vector3::Distance(eyepos, _objectPos);
		}
	}

	std::sort(this->AlphaObjectList.begin(), this->AlphaObjectList.end());
}

void hos::cg::RenderQue::ViewFrustum(float screenDepth, DirectX::SimpleMath::Matrix projMatrix, DirectX::SimpleMath::Matrix viewMatrix, bool bUI)
{
	// ����ü���� �ּ� Z �Ÿ��� ����մϴ�.
	float zMinimum = -projMatrix._43 / projMatrix._33;
	float r = screenDepth / (screenDepth - zMinimum);

	// ������Ʈ �� ���� �ٽ� ���� ��Ŀ� �����մϴ�.
	projMatrix._33 = r;
	projMatrix._43 = -r * zMinimum;

	// �� ��Ʈ������ ������Ʈ �� �������� ��Ʈ�������� ����ü ��Ʈ������ ����ϴ�.
	Matrix matrix = viewMatrix * projMatrix;

	// ����ü�� ����� ����� ����մϴ�.
	float x = (float)(matrix._14 + matrix._13);
	float y = (float)(matrix._24 + matrix._23);
	float z = (float)(matrix._34 + matrix._33);
	float w = (float)(matrix._44 + matrix._43);
	mPlane[0] = Vector4(x, y, z, w);
	mPlane[0].Normalize();

	// ����ü�� �� ����� ����մϴ�.
	x = (float)(matrix._14 - matrix._13);
	y = (float)(matrix._24 - matrix._23);
	z = (float)(matrix._34 - matrix._33);
	w = (float)(matrix._44 - matrix._43);
	mPlane[1] = Vector4(x, y, z, w);
	mPlane[1].Normalize();

	// ����ü�� ���� ����� ����մϴ�.
	x = (float)(matrix._14 + matrix._11);
	y = (float)(matrix._24 + matrix._21);
	z = (float)(matrix._34 + matrix._31);
	w = (float)(matrix._44 + matrix._41);
	mPlane[2] = Vector4(x, y, z, w);
	mPlane[2].Normalize();

	// ����ü�� ������ ����� ����մϴ�.
	x = (float)(matrix._14 - matrix._11);
	y = (float)(matrix._24 - matrix._21);
	z = (float)(matrix._34 - matrix._31);
	w = (float)(matrix._44 - matrix._41);
	mPlane[3] = Vector4(x, y, z, w);
	mPlane[3].Normalize();

	// ����ü�� �� ����� ����մϴ�.
	x = (float)(matrix._14 - matrix._12);
	y = (float)(matrix._24 - matrix._22);
	z = (float)(matrix._34 - matrix._32);
	w = (float)(matrix._44 - matrix._42);
	mPlane[4] = Vector4(x, y, z, w);
	mPlane[4].Normalize();

	// ����ü�� �Ʒ� ����� ����մϴ�.
	x = (float)(matrix._14 + matrix._12);
	y = (float)(matrix._24 + matrix._22);
	z = (float)(matrix._34 + matrix._32);
	w = (float)(matrix._44 + matrix._42);
	mPlane[5] = Vector4(x, y, z, w);
	mPlane[5].Normalize();

	if (bUI == false)
	{
		// RenderObjectList
		for (unsigned int i = 0; i < this->NowObjectCount; i++)
		{
			if (this->RenderObjectList[i].ViewCullSkip == false)
			{
				bool bView = false;

				for (int j = 0; j < 9; j++)
				{
					bView = CheckFrustum_Point(this->RenderObjectList[i].vBBox[j]);

					if (bView == true)
					{
						this->RenderObjectList[i].InView = true;
						break;
					}
				}

				if (bView == true)
				{
					break;
				}

				this->RenderObjectList[i].InView = false;
			}
		}

		// AlphaObjectList
		for (unsigned int i = 0; i < this->NowAlphaCount; i++)
		{
			if (this->AlphaObjectList[i].ViewCullSkip == false)
			{
				bool bView = false;

				for (int j = 0; j < 9; j++)
				{
					bView = CheckFrustum_Point(this->AlphaObjectList[i].vBBox[j]);

					if (bView == true)
					{
						this->AlphaObjectList[i].InView = true;
						break;
					}
				}

				if (bView == true)
				{
					break;
				}

				this->AlphaObjectList[i].InView = false;
			}
		}

		// EffectList
		for (unsigned int i = 0; i < this->NowEffectCount; i++)
		{
			if (this->EffectList[i].ViewCullSkip == false)
			{
				bool bView = false;

				for (int j = 0; j < 9; j++)
				{
					bView = CheckFrustum_Point(this->EffectList[i].vBBox[j]);

					if (bView == true)
					{
						this->EffectList[i].InView = true;
						break;
					}
				}

				if (bView == true)
				{
					break;
				}

				this->EffectList[i].InView = false;
			}
		}
	}
}

bool hos::cg::RenderQue::CheckFrustum_Point(DirectX::SimpleMath::Vector3 vPos)
{
	for (int i = 0; i < 6; i++)
	{
		float _x = mPlane[i].DotCoordinate(vPos);

		if (_x < 0.0f)
		{
			return false;
		}
	}

	return true;
}

