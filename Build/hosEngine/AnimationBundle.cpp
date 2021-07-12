// 자기 헤더
#include "Object.h"
#include "AnimationBundle.h"


#define DEAFLUT_BONE_COUNT 150

using namespace hos;


hos::AnimationBundle::AnimationBundle(hos::string modelName)
	: ModelName(L"")
{
	this->ModelName = modelName;

	// 벡터 Capacity 미리 할당
	BoneNameList.reserve(DEAFLUT_BONE_COUNT);
	BoneHierarchy.reserve(DEAFLUT_BONE_COUNT);
	BoneTransformList.reserve(DEAFLUT_BONE_COUNT);
	BoneOffsetList.resize(DEAFLUT_BONE_COUNT);

	//BoneOffsetList.reserve(50);	// TODO: 으음..?
	AnimationName.reserve(10);
	AnimationClipList.reserve(10);
}

hos::AnimationBundle::~AnimationBundle()
{
	// 애니메이션 제거
/// 애니메이션 Name을 제거하기 전, 해당 스트링을 사용해서 Animations부터 지워준다.
/// unordered_map은 shrink_to_fit이 작동하지 않으므로 일단 구버전 방식인 swap을 사용한다.
/// TODO: swap을 대체할 수 있는지 확인 필요.
	for (int i = 0; i < AnimationClipList.size(); i++)
	{
		SafeDelete(AnimationClipList[AnimationName[i]]);
	}
	std::unordered_map<hos::string, AnimationClip*>().swap(AnimationClipList);

	AnimationName.clear();
	AnimationName.shrink_to_fit();

	BoneNameList.clear();
	BoneNameList.shrink_to_fit();

	BoneHierarchy.clear();
	BoneHierarchy.shrink_to_fit();

	BoneOffsetList.clear();
	BoneOffsetList.shrink_to_fit();

	BoneTransformList.clear();
	BoneTransformList.shrink_to_fit();
}

hos::AnimationClip* hos::AnimationBundle::GetClip(hos::string_view clipName) const
{
	if (AnimationClipList.count(clipName.data()))
	{
		return AnimationClipList.at(clipName.data());
	}

	return nullptr;
};

hos::I32 hos::AnimationBundle::GetBoneIndexByName(hos::string boneName)
{
	I32 currIndex = -1;

	for (I32 i = 0; i < BoneNameList.size(); i++)
	{
		if (BoneNameList[i] == boneName)
		{
			currIndex = i;
			break;
		}
	}

	return currIndex;
}

void hos::AnimationBundle::GetBoneOffSetTM(hos::Matrix dest[])
{
	int _count = (int)BoneOffsetList.size();

	for (int i = 0; i < _count; i++)
	{
		dest[i] = BoneOffsetList[i];
	}
}

void hos::AnimationBundle::AddClip(hos::string clipName, AnimationClip* clip)
{
	this->AnimationName.push_back(clipName);
	this->AnimationClipList.insert({ clipName, clip });
}

const std::vector<U8> hos::AnimationBundle::Serialize()
{
	mbstring modelname = ut::UTF16ToAnsi(ModelName);
	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.Vector("BoneTransformList", [&] {
			for (int i = 0; i < BoneTransformList.size(); i++)
			{
				builder.TypedVector([&] {
					builder.Float(BoneTransformList[i].vPos.x);
					builder.Float(BoneTransformList[i].vPos.y);
					builder.Float(BoneTransformList[i].vPos.z);
					builder.Float(BoneTransformList[i].vRot.x);
					builder.Float(BoneTransformList[i].vRot.y);
					builder.Float(BoneTransformList[i].vRot.z);
					builder.Float(BoneTransformList[i].vScale.x);
					builder.Float(BoneTransformList[i].vScale.y);
					builder.Float(BoneTransformList[i].vScale.z);
					builder.Float(BoneTransformList[i].mTM.m[0][0]);
					builder.Float(BoneTransformList[i].mTM.m[0][1]);
					builder.Float(BoneTransformList[i].mTM.m[0][2]);
					builder.Float(BoneTransformList[i].mTM.m[0][3]);
					builder.Float(BoneTransformList[i].mTM.m[1][0]);
					builder.Float(BoneTransformList[i].mTM.m[1][1]);
					builder.Float(BoneTransformList[i].mTM.m[1][2]);
					builder.Float(BoneTransformList[i].mTM.m[1][3]);
					builder.Float(BoneTransformList[i].mTM.m[2][0]);
					builder.Float(BoneTransformList[i].mTM.m[2][1]);
					builder.Float(BoneTransformList[i].mTM.m[2][2]);
					builder.Float(BoneTransformList[i].mTM.m[2][3]);
					builder.Float(BoneTransformList[i].mTM.m[3][0]);
					builder.Float(BoneTransformList[i].mTM.m[3][1]);
					builder.Float(BoneTransformList[i].mTM.m[3][2]);
					builder.Float(BoneTransformList[i].mTM.m[3][3]);
					});
			}
			});
		builder.String("ModelName", modelname);
		builder.Vector("BoneNameList", [&] {
			for (int i = 0; i < BoneNameList.size(); i++)
			{
				mbstring bonename = ut::UTF16ToAnsi(BoneNameList[i]);
				builder.String(bonename);
			}
			});
		builder.Vector("BoneHierarchy", [&] {
			for (int i = 0; i < BoneHierarchy.size(); i++)
			{
				builder.Int(BoneHierarchy[i]);
			}
			});
		builder.Vector("BoneOffsetList", [&] {
			for (int i = 0; i < BoneOffsetList.size(); i++)
			{
				builder.TypedVector([&] {
					builder.Float(BoneOffsetList[i].m[0][0]);
					builder.Float(BoneOffsetList[i].m[0][1]);
					builder.Float(BoneOffsetList[i].m[0][2]);
					builder.Float(BoneOffsetList[i].m[0][3]);
					builder.Float(BoneOffsetList[i].m[1][0]);
					builder.Float(BoneOffsetList[i].m[1][1]);
					builder.Float(BoneOffsetList[i].m[1][2]);
					builder.Float(BoneOffsetList[i].m[1][3]);
					builder.Float(BoneOffsetList[i].m[2][0]);
					builder.Float(BoneOffsetList[i].m[2][1]);
					builder.Float(BoneOffsetList[i].m[2][2]);
					builder.Float(BoneOffsetList[i].m[2][3]);
					builder.Float(BoneOffsetList[i].m[3][0]);
					builder.Float(BoneOffsetList[i].m[3][1]);
					builder.Float(BoneOffsetList[i].m[3][2]);
					builder.Float(BoneOffsetList[i].m[3][3]);
					});
			}
			});
		builder.Vector("AnimationName", [&] {
			for (int i = 0; i < AnimationName.size(); i++)
			{
				mbstring aniname = ut::UTF16ToAnsi(AnimationName[i]);
				builder.String(aniname);
			}
			});
		builder.Vector("AnimationClip", [&] {
			for (auto [name, clip] : AnimationClipList)
			{
				builder.Blob(clip->Serialize());
			}
			});
		});
	builder.Finish();
	return builder.GetBuffer();
}

bool hos::AnimationBundle::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();

	BoneTransformList.clear();
	auto boneTransformList = m["BoneTransformList"].AsVector();
	for (int i = 0; i < boneTransformList.size(); i++)
	{
		BoneTransform transform;
		auto boneTransform = boneTransformList[i].AsTypedVector();
		transform.vPos.x = boneTransform[0].AsFloat();
		transform.vPos.y = boneTransform[1].AsFloat();
		transform.vPos.z = boneTransform[2].AsFloat();
		transform.vRot.x = boneTransform[3].AsFloat();
		transform.vRot.y = boneTransform[4].AsFloat();
		transform.vRot.z = boneTransform[5].AsFloat();
		transform.vScale.x = boneTransform[6].AsFloat();
		transform.vScale.y = boneTransform[7].AsFloat();
		transform.vScale.z = boneTransform[8].AsFloat();
		transform.mTM.m[0][0] = boneTransform[9].AsFloat();
		transform.mTM.m[0][1] = boneTransform[10].AsFloat();
		transform.mTM.m[0][2] = boneTransform[11].AsFloat();
		transform.mTM.m[0][3] = boneTransform[12].AsFloat();
		transform.mTM.m[1][0] = boneTransform[13].AsFloat();
		transform.mTM.m[1][1] = boneTransform[14].AsFloat();
		transform.mTM.m[1][2] = boneTransform[15].AsFloat();
		transform.mTM.m[1][3] = boneTransform[16].AsFloat();
		transform.mTM.m[2][0] = boneTransform[17].AsFloat();
		transform.mTM.m[2][1] = boneTransform[18].AsFloat();
		transform.mTM.m[2][2] = boneTransform[19].AsFloat();
		transform.mTM.m[2][3] = boneTransform[20].AsFloat();
		transform.mTM.m[3][0] = boneTransform[21].AsFloat();
		transform.mTM.m[3][1] = boneTransform[22].AsFloat();
		transform.mTM.m[3][2] = boneTransform[23].AsFloat();
		transform.mTM.m[3][3] = boneTransform[24].AsFloat();

		BoneTransformList.emplace_back(transform);
	}
	mbstring name = m["ModelName"].AsString().str();
	ModelName = ut::AnsiToUTF16(name);

	BoneNameList.clear();
	auto boneNameList = m["BoneNameList"].AsVector();
	for (int i = 0; i < boneNameList.size(); i++)
	{
		mbstring boneName = boneNameList[i].AsString().str();
		string data = ut::AnsiToUTF16(boneName);
		BoneNameList.push_back(data);
	}
	
	BoneHierarchy.clear();
	auto boneHierarchy = m["BoneHierarchy"].AsVector();
	for (int i = 0; i < boneHierarchy.size(); i++)
	{
		int num = boneHierarchy[i].AsInt32();
		BoneHierarchy.push_back(num);
	}

	BoneOffsetList.clear();
	auto boneOffsetList = m["BoneOffsetList"].AsVector();
	for (int i = 0; i < boneOffsetList.size(); i++)
	{
		auto boneOffset = boneOffsetList[i].AsTypedVector();
		Matrix m;
		m.m[0][0] = boneOffset[0].AsFloat();
		m.m[0][1] = boneOffset[1].AsFloat();
		m.m[0][2] = boneOffset[2].AsFloat();
		m.m[0][3] = boneOffset[3].AsFloat();
		m.m[1][0] = boneOffset[4].AsFloat();
		m.m[1][1] = boneOffset[5].AsFloat();
		m.m[1][2] = boneOffset[6].AsFloat();
		m.m[1][3] = boneOffset[7].AsFloat();
		m.m[2][0] = boneOffset[8].AsFloat();
		m.m[2][1] = boneOffset[9].AsFloat();
		m.m[2][2] = boneOffset[10].AsFloat();
		m.m[2][3] = boneOffset[11].AsFloat();
		m.m[3][0] = boneOffset[12].AsFloat();
		m.m[3][1] = boneOffset[13].AsFloat();
		m.m[3][2] = boneOffset[14].AsFloat();
		m.m[3][3] = boneOffset[15].AsFloat();

		BoneOffsetList.push_back(m);
	}

	AnimationName.clear();
	auto animationName = m["AnimationName"].AsVector();
	for (int i = 0; i < animationName.size(); i++)
	{
		mbstring name = animationName[i].AsString().str();
		string data = ut::AnsiToUTF16(name);
		AnimationName.push_back(data);
	}

	AnimationClipList.clear();
	auto animationClip = m["AnimationClip"].AsVector();
	for (int i = 0; i < animationClip.size(); i++)
	{
		mbstring data(reinterpret_cast<const char*>(animationClip[i].AsBlob().data()), animationClip[i].AsBlob().size());
		AnimationClip* clip = new AnimationClip();
		clip->Deserialize(data);
		std::pair<std::unordered_map<string, AnimationClip*>::iterator, bool> b;
		b = AnimationClipList.insert({ clip->GetName(), clip });
		if (b.second == false)
		{
			SafeDelete(clip);
		}
	}

	return true;
}