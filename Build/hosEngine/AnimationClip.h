#ifndef _ANIMATIONCLIP_H
#define _ANIMATIONCLIP_H

// [2021/03/15 ��ȣ��]
// Ư�� ���� �ִϸ��̼� �� ����.

// LOG
// [2021/03/15 ��ȣ��] Graphics Mk2���� �̵�.
// [2021/04/05] ������ : Serialize/ Deserialize �߰� �ӽ� �����Լ� ����

#pragma once

#include "BoneAnimation.h"


namespace hos
{
	class AnimationClip
	{
		friend class FbxLoader;

	private:
		hos::string ClipName;
		std::vector<BoneAnimation> BoneAnimationList;
		bool bSaved[150];

		hos::Vector3 BBox[9];	// 0 : Center

	public:
		AnimationClip(string_view name = L"Empty");
		~AnimationClip();

	public:
		// ����
		void SetBoneAnimationSize(U32 size) { BoneAnimationList.resize(size); };
		void SetBoneAnimation(BoneAnimation& boneAni, I32 index);

		bool CheckBoneAnimation(I32 index);

		// ����
		hos::F32 GetClipStartTime() const;
		hos::F32 GetClipEndTime() const;

		bool GetStateBoneIndex(U32 index) { return bSaved[index]; };
		string GetName()const { return ClipName; }

		hos::Vector3* GetBBoxAddress() { return BBox; };

		// ����
		U32 Interpolate(F32 time, Matrix boneOffsetList[], Vector3 vPos[] = nullptr, Vector3 vScale[] = nullptr, Quaternion vQuat[] = nullptr) const;

		const std::vector<U8> Serialize();
		bool Deserialize(mbstring_view data);
	};

}

#endif //!_ANIMATIONCLIP_H