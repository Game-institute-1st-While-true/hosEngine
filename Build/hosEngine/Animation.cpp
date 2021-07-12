// 참조 헤더
#include "GameObject.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "Transform.h"
#include "MeshFilter.h"

// 애니메이션 Data 참조
#include "AnimationBundle.h"
#include "AnimationClip.h"

// 자기 헤더
#include "Animation.h"

using namespace hos;


hos::com::Animation::Animation()
	:Component(L"Animation"), bAnimation(false), BundleName(L""), ClipName(L""), AniBundle(nullptr), AniClip(nullptr),
	NowTick(0.0f), BoneCount(0), TimeScale(1.0f), IsAnimationEnd(false), bPlay(true), bPlayOnce(false),
	bReversePlay(false),
	StartTick(0.0f), LastTick(0.0f),
	bBlendState(false), bBlendIn(false), BlendInTime(0.0f), BlendInClipName(L""), BlendInClip(nullptr)
{
}

hos::com::Animation::~Animation()
{
}

hos::com::Animation::Animation(const Animation& dest) :
	Component(dest), bAnimation(dest.bAnimation), BundleName(dest.BundleName), ClipName(dest.ClipName),
	NowTick(0.0f), BoneCount(dest.BoneCount), AniBundle(nullptr), AniClip(nullptr), TimeScale(1.0f), IsAnimationEnd(false), bPlay(true), bPlayOnce(false),
	bReversePlay(false),
	StartTick(0.0f), LastTick(0.0f),
	bBlendState(false), bBlendIn(false), BlendInTime(0.0f), BlendInClipName(L""), BlendInClip(nullptr)
{
	// 번들, 클립 저장
	this->AniBundle = g_DataManager->GetAnimationBundle(this->BundleName);
	if (AniBundle)
	{
		this->AniClip = this->AniBundle->GetClip(this->ClipName);
		// 정보 저장
		AniBundle->GetBoneOffSetTM(this->BoneOffTM);

		// BoneCount 저장
		this->BoneCount = (hos::U32)AniBundle->GetBoneListSize();

		// Tick 저장
		this->StartTick = this->AniClip->GetClipStartTime();
		this->LastTick = this->AniClip->GetClipEndTime();

		// Blend In Copy
		if (this->bBlendIn)
		{
			this->BlendInClip = this->AniBundle->GetClip(this->BlendInClipName);
		}
	}
}

void hos::com::Animation::SetAnimation(hos::string bundleName, hos::string clipName, hos::F32 startPerecet, bool bPlayOnce)
{
	AnimationBundle* _bundleData = g_DataManager->GetAnimationBundle(bundleName);

	// Bundle 유효성 체크
	if (_bundleData == nullptr)
	{
		Debug->LogConsole(L"Animation", bundleName + L"존재하지 않는 번들입니다.");
		this->bAnimation = false;
		return;
	}

	// Clip 유효성 체크
	AnimationClip* _clipData = _bundleData->GetClip(clipName);

	if (_clipData == nullptr)
	{
		Debug->LogConsole(L"Animation", clipName + L"(" + bundleName + L") 존재하지 않는 클립입니다.");
		this->bAnimation = false;
		return;
	}

	// 설정
	this->bAnimation = true;
	this->bPlay = true;
	this->BundleName = bundleName;
	this->ClipName = clipName;
	this->NowTick = _clipData->GetClipEndTime() * startPerecet / 100.f;
	this->TimeScale = 1.0f;
	this->bPlayOnce = bPlayOnce;
	this->bReversePlay = false;
	this->bBlendState = false;
	this-> IsAnimationEnd = false;

	// 번들, 클립 저장
	this->AniBundle = _bundleData;
	this->AniClip = _clipData;

	// BoneOffSet 저장
	_bundleData->GetBoneOffSetTM(this->BoneOffTM);

	// BoneCount 저장
	this->BoneCount = (hos::U32)AniBundle->GetBoneListSize();

	// Tick 저장
	this->StartTick = this->AniClip->GetClipStartTime();
	this->LastTick = this->AniClip->GetClipEndTime();
}

void hos::com::Animation::SetReverseAnimation(hos::string bundleName, hos::string clipName, hos::F32 startPercent, hos::F32 endPercent, bool bPlayOnce)
{
	AnimationBundle* _bundleData = g_DataManager->GetAnimationBundle(bundleName);

	// Bundle 유효성 체크
	if (_bundleData == nullptr)
	{
		Debug->LogConsole(L"Animation", bundleName + L"존재하지 않는 번들입니다.");
		this->bAnimation = false;
		return;
	}

	// Clip 유효성 체크
	AnimationClip* _clipData = _bundleData->GetClip(clipName);

	if (_clipData == nullptr)
	{
		Debug->LogConsole(L"Animation", clipName + L"(" + bundleName + L") 존재하지 않는 클립입니다.");
		this->bAnimation = false;
		return;
	}

	// 설정
	this->bAnimation = true;
	this->bPlay = true;
	this->BundleName = bundleName;
	this->ClipName = clipName;
	this->NowTick = _clipData->GetClipEndTime() * startPercent / 100;
	this->TimeScale = 1.0f;
	this->bPlayOnce = bPlayOnce;
	this->bReversePlay = true;

	// 번들, 클립 저장
	this->AniBundle = _bundleData;
	this->AniClip = _clipData;

	// BoneOffSet 저장
	_bundleData->GetBoneOffSetTM(this->BoneOffTM);

	// BoneCount 저장
	this->BoneCount = (hos::U32)AniBundle->GetBoneListSize();

	// Tick 저장
	this->StartTick = _clipData->GetClipEndTime() * startPercent / 100;
	this->LastTick = _clipData->GetClipEndTime() * endPercent / 100;
}


void hos::com::Animation::SetBlendAnimation(hos::string bundleName, hos::string clipName, hos::string_view blendClipName, float blendPoint, float blendTime, bool bPlayOnce)
{
	// 통상 애니메이션 세팅
	this->SetAnimation(bundleName, clipName);

	// Bundle 유효성 체크(다시!)
	if (this->AniBundle == nullptr)
	{
		if (g_Renderer)
		{
			Debug->LogConsole(L"Animation", L"Bundle이 존재하지 않아 BlendClip을 추가할 수 없습니다." + this->m_GameObject->GetName());
			return;
		}
	}

	// Clip 유효성 체크
	AnimationClip* _clipData = this->AniBundle->GetClip(blendClipName);
	if (_clipData == nullptr)
	{
		Debug->LogConsole(L"Animation", clipName + L"(" + bundleName + L") 존재하지 않는 클립입니다." + this->m_GameObject->GetName());
		this->bBlendState = false;
		return;
	}

	// 애니메이션 세팅
	this->bPlayOnce = bPlayOnce;
	this->bPlay = true;
	this->bReversePlay = false;

	// 블렌드 애니메이션 세팅
	this->bBlendState = true;
	this->BlendInClipName = blendClipName;
	this->BlendInClip = _clipData;
	this->BlendInTime = blendTime;
	this->BlendPointTick = blendPoint;
}

void hos::com::Animation::RemoveAnimation()
{
	this->bAnimation = false;
	this->BundleName = L"";
	this->ClipName = L"";
}

hos::com::Animation* hos::com::Animation::Clone() const
{
	return new Animation(*this);
}

void hos::com::Animation::Update()
{
	if (this->bAnimation)
	{
		if (this->bPlay == false)
		{
			return;
		}

		// dTime, Tick 처리
		F32 dTime = (F32)Time->DeltaTime();

		if (this->bReversePlay == false)
		{
			this->NowTick += dTime * this->TimeScale;
		}

		else
		{
			this->NowTick -= dTime * this->TimeScale;
		}

		// Bundle과 Clip이 모두 있다면
		if (this->AniBundle && this->AniClip)
		{
			// Animation의 Tick을 진행한다.
			this->ProcessTickCount();

			// BlendState를 판단한다.
			this->DetectionBlendState();

			if (this->IsAnimationEnd == true)
			{
				return;
			}

			// BlendState에 따라 Animation을 계산한다.
			if (this->bBlendIn)
			{
				this->AnimateBlendIn();
			}

			else
			{
				this->AnimateMainClip();
			}

			// 애니메이션 결과를 GameObject Heirachy의 Bone에 적용한다.
			for (hos::U32 i = 0; i < this->BoneCount; i++)
			{
				ChangeBoneTransform(i);
			}
		}
	}

	// if(this->bAnimation == false)
	else
	{
		Debug->LogConsole(L"Animation", m_GameObject->GetName() + L"애니메이션이 세팅되어 있지 않습니다.");
	}
}

hos::Matrix hos::com::Animation::GetAniTMbyBoneName(hos::string boneName)
{
	U32 boneIndex = this->AniBundle->GetBoneIndexByName(boneName);

	return AnimationTM[boneIndex];
}

void hos::com::Animation::ProcessTickCount()
{
	if (this->bReversePlay == false)
	{
		// Tick 저장
		this->StartTick = this->AniClip->GetClipStartTime();
		this->LastTick = this->AniClip->GetClipEndTime();

		// 현재 틱이 마지막 틱보다 높다면, Animation은 끝난 것이다.
		if (this->NowTick > this->LastTick)
		{

			// 1회만 재생이라면? 마지막 틱으로 고정해준다.
			if (this->bPlayOnce)
			{
				this->NowTick = this->AniClip->GetClipEndTime();
			}

			// 여러번 재생이라면? 다음 프레임을 위해 보정해준다.
			else
			{
				// 마지막 틱보다 작아질 때 까지 마지막 틱을 빼준다(재생 지점은 같다)
				while (this->NowTick > this->LastTick)
				{
					this->NowTick -= this->LastTick;

					if (this->LastTick <= 0)
					{
						Debug->LogConsole(L"Animation", m_GameObject->GetName() + L"Animation Clip의 lastTick이 0보다 작습니다.");
						return;
					}
				}
			}

			this->IsAnimationEnd = true;

			// Blend는 1회만 진행한 뒤,ㅡ 다음 BlendSet이 있을 때 까지 비활성화 한다.
			this->bBlendState = false;

			if (this->bPlayOnce)
			{
				this->bPlay = false;
			}

		}

		// Animation Not Yet End
		else
		{
			IsAnimationEnd = false;
		}

		// 현재 틱이 StartTick보다 작다면 StartTick으로 초기화해준다.
		if (this->NowTick < this->StartTick)
		{
			this->NowTick = this->StartTick;
		}
	}

	// 역재생일 경우
	else
	{
		// 현재 틱이 마지막 틱보다 작다면, Animation은 끝난 것이다.
		if (this->NowTick < this->LastTick)
		{

			// 1회만 재생이라면? 마지막 틱으로 고정해준다.
			if (this->bPlayOnce)
			{
				this->NowTick = this->LastTick;
			}

			// 여러번 재생이라면? 다음 프레임을 위해 보정해준다.
			else
			{
				// 역재생의 길이
				float lengthReverse = this->AniClip->GetClipEndTime() - this->LastTick;

				// 역재생용 프레임 보정(역재생의 LastTick보다 커질 때 까지 보정값을 더해준다.)
				while (this->NowTick < this->LastTick)
				{
					this->NowTick += lengthReverse;

					//if (this->LastTick <= 0)
					//{
					//	g_Renderer->DebugLog(__FUNCTION__, m_GameObject->GetName(), L"Animation Clip의 lastTick이 0보다 작습니다.");
					//	return;
					//}
				}
			}

			this->IsAnimationEnd = true;

			if (this->bPlayOnce)
			{
				this->bPlay = false;
			}

		}

		// Animation Not Yet End
		else
		{
			IsAnimationEnd = false;
		}

		// 현재 틱이 StartTick보다 크다면 StartTick으로 초기화해준다.
		if (this->NowTick > this->StartTick)
		{
			this->NowTick = this->StartTick;
		}
	}
}

void hos::com::Animation::DetectionBlendState()
{
	if (this->bBlendState && this->BlendInTime > 0.0f && this->BlendInClip &&
		this->NowTick <= this->BlendInTime)
	{
		this->bBlendIn = true;
	}

	else
	{
		this->bBlendIn = false;
	}
}

void hos::com::Animation::AnimateMainClip()
{
	// 보정한 Tick을 기반으로 애니메이션을 보간받아 클래스 인스턴스에 저장한다.
	this->BoneCount = AniClip->Interpolate(this->NowTick, AnimationTM);
}

void hos::com::Animation::AnimateBlendIn()
{
	F32 blendTime = this->BlendInTime;

	// MainAnimation
	Vector3 mainPos[MAX_BONE], mainScale[MAX_BONE];
	Quaternion mainQuat[MAX_BONE];
	Matrix mainTM[MAX_BONE];
	this->BoneCount = this->AniClip->Interpolate(this->NowTick, mainTM, mainPos, mainScale, mainQuat);

	// BlendIn Animation
	F32 inStartTick = this->BlendPointTick;
	Vector3 inPos[MAX_BONE], inScale[MAX_BONE];
	Quaternion inQuat[MAX_BONE];
	BlendInClip->Interpolate(inStartTick, nullptr, inPos, inScale, inQuat);

	// Final Animation
	Vector3 finalPos, finalScale;
	Quaternion finalQuat;
	Matrix mScale, mRot, mTrans, mFinal;
	F32 lerpPercent = this->NowTick / blendTime;

	// 뼈들의 Matrix 구하기
	for (U32 i = 0; i < this->BoneCount; i++)
	{
		/////final
		finalPos = Vector3::Lerp(inPos[i], mainPos[i], lerpPercent);
		finalScale = Vector3::Lerp(inScale[i], mainScale[i], lerpPercent);
		finalQuat = Quaternion::Slerp(inQuat[i], mainQuat[i], lerpPercent);

		mScale = Matrix::CreateScale(finalScale);
		mRot = Matrix::CreateFromQuaternion(finalQuat);
		mTrans = Matrix::CreateTranslation(finalPos);
		AnimationTM[i] = mScale * mRot * mTrans;
	}
}

void hos::com::Animation::ChangeBoneTransform(int boneIndex)
{
	// 현재 Object의 최상단부터 BoneList에 맞는 객체들을 찾아준다.
	hos::string boneName = this->AniBundle->GetBoneName(boneIndex);
	Transform* _rootTransform = m_GameObject->transform;
	GameObject* bone = m_GameObject->FindObjectByBoneTag(boneName);

	if (bone)
	{
		// Adjust AnimationTM
		hos::Matrix _mUser = _rootTransform->GetTransformMatrix();
		hos::Matrix _mBone = this->AniBundle->GetBoneTransformList()[boneIndex].mTM;
		hos::Matrix _mFinal = AnimationTM[boneIndex] * _mUser;

		// 뼈 행렬 변경
		Transform* _parents = bone->transform->GetParent();
		hos::Matrix mLocal = _mFinal * _parents->GetTransformMatrix().Invert();

		// 로컬 요소 얻기
		hos::Vector3 vLocalPos, vLocalScale;
		Quaternion vLocalQuat;
		Rotator vLocalRotator;
		mLocal.Decompose(vLocalScale, vLocalQuat, vLocalPos);
		vLocalRotator = vLocalQuat;

		// 로컬 Transform 업데이트
		bone->transform->SetLocalTransform(vLocalPos, vLocalRotator, vLocalScale);
	}
}

const std::vector<U8> hos::com::Animation::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());
	mbstring bundleName = ut::UTF16ToAnsi(BundleName);
	mbstring clipName = ut::UTF16ToAnsi(ClipName);
	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		builder.String("BundleName", bundleName);
		builder.String("ClipName", clipName);
		builder.UInt("BoneCount", BoneCount);
		builder.Vector("BoneOffTM", [&] {
			for (int i = 0; i < MAX_BONE; i++)
			{
				builder.TypedVector([&] {
					builder.Float(BoneOffTM[i].m[0][0]);
					builder.Float(BoneOffTM[i].m[0][1]);
					builder.Float(BoneOffTM[i].m[0][2]);
					builder.Float(BoneOffTM[i].m[0][3]);
					builder.Float(BoneOffTM[i].m[1][0]);
					builder.Float(BoneOffTM[i].m[1][1]);
					builder.Float(BoneOffTM[i].m[1][2]);
					builder.Float(BoneOffTM[i].m[1][3]);
					builder.Float(BoneOffTM[i].m[2][0]);
					builder.Float(BoneOffTM[i].m[2][1]);
					builder.Float(BoneOffTM[i].m[2][2]);
					builder.Float(BoneOffTM[i].m[2][3]);
					builder.Float(BoneOffTM[i].m[3][0]);
					builder.Float(BoneOffTM[i].m[3][1]);
					builder.Float(BoneOffTM[i].m[3][2]);
					builder.Float(BoneOffTM[i].m[3][3]);
					});
			}
			});
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::Animation::Deserialize(mbstring_view data)
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
		Debug->LogConsole(L"Animation", L"Animation try Deserialize to" + name);
		return false;
	}
	bool active = m["IsActive"].AsBool();
	SetActive(active);

	mbstring bundletmp = m["BundleName"].AsString().str();
	BundleName = ut::AnsiToUTF16(bundletmp);
	mbstring cliptmp = m["ClipName"].AsString().str();
	ClipName = ut::AnsiToUTF16(cliptmp);
	AniBundle = g_DataManager->GetAnimationBundle(BundleName);
	if (nullptr == AniBundle)
	{
		AniBundle = g_DataManager->LoadAnimationBundle(g_Path + L"\\Assets\\Anim\\" + BundleName + AnimationBundle::FILE_EXTENSION);
	}
	bAnimation = false;
	if (AniBundle)
	{
		AniClip = AniBundle->GetClip(ClipName);
		if (AniClip)
		{
			bAnimation = true;
		}
	}
	BoneCount = m["BoneCount"].AsUInt32();
	auto boneoff = m["BoneOffTM"].AsVector();
	for (int i = 0; i < MAX_BONE; i++)
	{
		auto matrix = boneoff[i].AsTypedVector();

		BoneOffTM[i].m[0][0] = matrix[0].AsFloat();
		BoneOffTM[i].m[0][1] = matrix[1].AsFloat();
		BoneOffTM[i].m[0][2] = matrix[2].AsFloat();
		BoneOffTM[i].m[0][3] = matrix[3].AsFloat();
		BoneOffTM[i].m[1][0] = matrix[4].AsFloat();
		BoneOffTM[i].m[1][1] = matrix[5].AsFloat();
		BoneOffTM[i].m[1][2] = matrix[6].AsFloat();
		BoneOffTM[i].m[1][3] = matrix[7].AsFloat();
		BoneOffTM[i].m[2][0] = matrix[8].AsFloat();
		BoneOffTM[i].m[2][1] = matrix[9].AsFloat();
		BoneOffTM[i].m[2][2] = matrix[10].AsFloat();
		BoneOffTM[i].m[2][3] = matrix[11].AsFloat();
		BoneOffTM[i].m[3][0] = matrix[12].AsFloat();
		BoneOffTM[i].m[3][1] = matrix[13].AsFloat();
		BoneOffTM[i].m[3][2] = matrix[14].AsFloat();
		BoneOffTM[i].m[3][3] = matrix[15].AsFloat();
	}

	return true;
}