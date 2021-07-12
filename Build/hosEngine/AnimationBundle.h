#ifndef _ANIMATIONBUNDLE_H
#define _ANIMATIONBUNDLE_H

// [2021/03/15 ��ȣ��]
// Ư�� ���� �ִϸ��̼� Ŭ��(����) ����.

// LOG
// [2021/03/15 ��ȣ��] Graphics Mk2���� �̵�.
// [2021/04/05] ������ : Serialize/ Deserialize, FILE_EXTENSION, GetModelName() �߰�

#pragma once

#include "AnimationClip.h"

namespace hos
{

	class AnimationBundle
	{
	public:
		static constexpr string_literal FILE_EXTENSION = L".bundle";
	public:
		struct BoneTransform
		{
			DirectX::SimpleMath::Vector3 vPos;
			DirectX::SimpleMath::Vector3 vRot;
			DirectX::SimpleMath::Vector3 vScale;
			DirectX::SimpleMath::Matrix mTM;
		};

		std::vector<BoneTransform> BoneTransformList;

	private:
		// �� �̸�
		hos::string ModelName;

		// �� ����
		std::vector<hos::string> BoneNameList;		// �� �̸��� ����
		std::vector<I32> BoneHierarchy;			// �� �������� �ε���
		std::vector<DirectX::SimpleMath::Matrix> BoneOffsetList;		// �� ���� �ִϸ��̼� ���


		// �ִϸ��̼� ����
		std::vector<hos::string> AnimationName;
		std::unordered_map<hos::string, AnimationClip*> AnimationClipList;

		// ������ �Ҹ���
	public:
		AnimationBundle() = delete;
		AnimationBundle(hos::string modelName);
		~AnimationBundle();

		// �ܺ� ���� �Լ�
	public:
		// ����
		I32 GetBoneListSize() const { return (I32)BoneHierarchy.size(); };
		std::vector<DirectX::SimpleMath::Matrix>& GetBoneOffSetList() { return BoneOffsetList; };
		std::vector<BoneTransform>& GetBoneTransformList() { return BoneTransformList; };
		std::vector<hos::string>& GetBoneList() { return BoneNameList; };
		AnimationClip* GetClip(hos::string_view clipName) const;
		std::vector<hos::string>& GetAnimationClipName() { return AnimationName; };

		I32 GetBoneIndexByName(hos::string boneName);

		void GetBoneOffSetTM(hos::Matrix dest[]);
		hos::string GetBoneName(hos::U32 index) { return BoneNameList[index]; };

		string GetName() const { return ModelName; }
		// ����
		void AddBoneHierarchy(I32 index) { BoneHierarchy.push_back(index); };
		void SetBoneName(hos::string boneName) { BoneNameList.push_back(boneName); };
		void ResizeBoneOffSetList(I32 count) { BoneOffsetList.resize(count); BoneTransformList.resize(count); };
		void AddClip(hos::string clipName, AnimationClip* clip);

		const std::vector<U8> Serialize();
		bool Deserialize(mbstring_view data);
	};

}

#endif //!_ANIMATIONBUNDLE_H