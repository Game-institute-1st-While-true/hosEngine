// 참조 헤더
#include "DebugConsole.h"

// 자기 헤더
#include "RenderQue.h"

// 네임스페이스
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

	DEBUG_CONSOLE(L"RenderQue 생성 완료.");
}

hos::cg::RenderQue::~RenderQue()
{
	// 구성요소 삭제
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

	DEBUG_CONSOLE(L"RenderQue 제거 완료.");
}

/// TODO: 파라미터 복사 안 일어나고 래퍼런스에서 복사할 수 있게?
void hos::cg::RenderQue::AddRenderObejct(RenderObject& objectInfo)
{
	// 현재 RenderQue의 최대 크기를 넘었다면 할당 공간을 2배로 늘려주는 함수입니다.
	CheckMaxSize();

	// 필요한 데이터를 복사해줍니다.
	this->RenderObjectList[this->NowObjectCount] = objectInfo;

	// 렌더큐의 갯수를 늘려줍니다.
	this->NowObjectCount++;
}

void hos::cg::RenderQue::AddAlphaObject(RenderObject& objectInfo)
{
	// 현재 RenderQue의 최대 크기를 넘었다면 할당 공간을 2배로 늘려주는 함수입니다.
	CheckMaxSize_Alpha();

	// 필요한 데이터를 복사해줍니다.
	objectInfo.ViewDistance = 0.0f;
	this->AlphaObjectList[this->NowAlphaCount] = objectInfo;

	// 렌더큐의 갯수를 늘려줍니다.
	this->NowAlphaCount++;
}

void hos::cg::RenderQue::AddEffectObject(RenderObject& objectInfo)
{
	// 현재 RenderQue의 최대 크기를 넘었다면 할당 공간을 2배로 늘려주는 함수입니다.
	CheckMaxSize_Effect();

	// 필요한 데이터를 복사해줍니다.
	objectInfo.ViewDistance = 0.0f;
	this->EffectList[this->NowEffectCount] = objectInfo;

	// 렌더큐의 갯수를 늘려줍니다.
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
	// RenderQue의 최대 할당 갯수에 도달한다면?
	if (this->NowObjectCount == this->MaxObjectCount)
	{
		// Vector 클래스의 크기를 2배로 늘려준다.
		this->MaxObjectCount *= 2;
		RenderObjectList.resize(this->MaxObjectCount);
	}
}

void hos::cg::RenderQue::CheckMaxSize_Alpha()
{
	// AlphaQue의 최대 할당 갯수에 도달한다면?
	if (this->NowAlphaCount == this->MaxAlphaCount)
	{
		// Vector 클래스의 크기를 2배로 늘려준다.
		this->MaxAlphaCount *= 2;
		AlphaObjectList.resize(this->MaxAlphaCount);
	}
}

void hos::cg::RenderQue::CheckMaxSize_Effect()
{
	// EffectQue의 최대 할당 갯수에 도달한다면?
	if (this->NowEffectCount == this->MaxEffectCount)
	{
		// Vector 클래스의 크기를 2배로 늘려준다.
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
	// 절두체에서 최소 Z 거리를 계산합니다.
	float zMinimum = -projMatrix._43 / projMatrix._33;
	float r = screenDepth / (screenDepth - zMinimum);

	// 업데이트 된 값을 다시 투영 행렬에 설정합니다.
	projMatrix._33 = r;
	projMatrix._43 = -r * zMinimum;

	// 뷰 매트릭스와 업데이트 된 프로젝션 매트릭스에서 절두체 매트릭스를 만듭니다.
	Matrix matrix = viewMatrix * projMatrix;

	// 절두체의 가까운 평면을 계산합니다.
	float x = (float)(matrix._14 + matrix._13);
	float y = (float)(matrix._24 + matrix._23);
	float z = (float)(matrix._34 + matrix._33);
	float w = (float)(matrix._44 + matrix._43);
	mPlane[0] = Vector4(x, y, z, w);
	mPlane[0].Normalize();

	// 절두체의 먼 평면을 계산합니다.
	x = (float)(matrix._14 - matrix._13);
	y = (float)(matrix._24 - matrix._23);
	z = (float)(matrix._34 - matrix._33);
	w = (float)(matrix._44 - matrix._43);
	mPlane[1] = Vector4(x, y, z, w);
	mPlane[1].Normalize();

	// 절두체의 왼쪽 평면을 계산합니다.
	x = (float)(matrix._14 + matrix._11);
	y = (float)(matrix._24 + matrix._21);
	z = (float)(matrix._34 + matrix._31);
	w = (float)(matrix._44 + matrix._41);
	mPlane[2] = Vector4(x, y, z, w);
	mPlane[2].Normalize();

	// 절두체의 오른쪽 평면을 계산합니다.
	x = (float)(matrix._14 - matrix._11);
	y = (float)(matrix._24 - matrix._21);
	z = (float)(matrix._34 - matrix._31);
	w = (float)(matrix._44 - matrix._41);
	mPlane[3] = Vector4(x, y, z, w);
	mPlane[3].Normalize();

	// 절두체의 윗 평면을 계산합니다.
	x = (float)(matrix._14 - matrix._12);
	y = (float)(matrix._24 - matrix._22);
	z = (float)(matrix._34 - matrix._32);
	w = (float)(matrix._44 - matrix._42);
	mPlane[4] = Vector4(x, y, z, w);
	mPlane[4].Normalize();

	// 절두체의 아래 평면을 계산합니다.
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

