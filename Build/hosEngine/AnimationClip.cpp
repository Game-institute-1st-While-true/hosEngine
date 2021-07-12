// 자기 헤더
#include "Object.h"
#include "AnimationClip.h"
using namespace hos;

const float Infinity = 3.402823466e+38F;


void hos::AnimationClip::SetBoneAnimation(BoneAnimation& boneAni, I32 index)
{
	// 재차 체크
	if (bSaved[index] == false)
	{
		BoneAnimationList[index] = boneAni;
		bSaved[index] = true;
	}
}

bool hos::AnimationClip::CheckBoneAnimation(I32 index)
{
	// 체크
	if (bSaved[index] == false)
	{
		return false;
	}

	return true;
}

hos::AnimationClip::AnimationClip(hos::string_view name)
	: bSaved{ 0, }
{
	this->ClipName = name;
}

hos::AnimationClip::~AnimationClip()
{
	BoneAnimationList.clear();
}

hos::F32 hos::AnimationClip::GetClipStartTime() const
{
	hos::F32 t = Infinity;
	hos::F32 f = 0.0f;

	// 가장 작은 StartTime을 찾는다.
	for (U32 i = 0; i < BoneAnimationList.size(); i++)
	{
		f = BoneAnimationList[i]._GetStartTime();
		t = Min(t, f);
	}

	return t;
}

hos::F32 hos::AnimationClip::GetClipEndTime() const
{
	hos::F32 t = 0.0f;
	hos::F32 f = Infinity;

	// 가장 큰 EndTime을 찾는다.
	for (U32 i = 0; i < BoneAnimationList.size(); i++)
	{
		f = BoneAnimationList[i]._GetEndTime();
		t = Max(t, f);
	}

	return t;
}

hos::U32 hos::AnimationClip::Interpolate(F32 time, hos::Matrix dest[], Vector3 vPos[], Vector3 vScale[], Quaternion vQuat[]) const
{
	U32 aniCount = (U32)BoneAnimationList.size();

	for (hos::U32 i = 0; i < aniCount; i++)
	{
		BoneAnimationList[i]._Interpolate(i, time, dest, vPos, vScale, vQuat);
	}

	return aniCount;
}

const std::vector<U8> hos::AnimationClip::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(ClipName);
	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.Vector("BoneAnimationList", [&] {
			for (int i = 0; i < BoneAnimationList.size(); i++)
			{
				builder.Vector([&] {
					for (int j = 0; j < BoneAnimationList[i].KeyframeList.size(); j++)
					{
						builder.TypedVector([&] {
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetTimePos());
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetPos().x);
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetPos().y);
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetPos().z);
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetScale().x);
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetScale().y);
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetScale().z);
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetRotQuat().x);
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetRotQuat().y);
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetRotQuat().z);
							builder.Float(BoneAnimationList[i].KeyframeList[j].GetRotQuat().w);
							});
					}
					});
			}
			});
		});

	builder.Finish();

	return builder.GetBuffer();
}

bool hos::AnimationClip::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring name = m["Name"].AsString().str();
	ClipName = ut::AnsiToUTF16(name);

	BoneAnimationList.clear();
	auto boneAniList = m["BoneAnimationList"].AsVector();
	for (int i = 0; i < boneAniList.size(); i++)
	{
		auto keyFrameList = boneAniList[i].AsVector();
		BoneAnimation boneAni;
		for (int j = 0; j < keyFrameList.size(); j++)
		{
			auto keyframe = keyFrameList[j].AsTypedVector();
			Keyframe key;
			key.SetTimePos(keyframe[0].AsFloat());
			Vector3 pos, scale;
			Vector4 rotQuat;
			pos.x = keyframe[1].AsFloat();
			pos.y = keyframe[2].AsFloat();
			pos.z = keyframe[3].AsFloat();
			scale.x = keyframe[4].AsFloat();
			scale.y = keyframe[5].AsFloat();
			scale.z = keyframe[6].AsFloat();
			rotQuat.x = keyframe[7].AsFloat();
			rotQuat.y = keyframe[8].AsFloat();
			rotQuat.z = keyframe[9].AsFloat();
			rotQuat.w = keyframe[10].AsFloat();
			key.SetTransform(pos, scale, rotQuat);

			boneAni.AddKeyframe(key);
		}
		BoneAnimationList.push_back(boneAni);
	}

	return true;
}