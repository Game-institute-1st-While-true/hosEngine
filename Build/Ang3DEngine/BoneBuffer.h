// <Concept>
// Source Name		: BoneBuffer.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.02.24

// <Explanation>
// Ư�� ����� BoneTM�� ���������� ����

// Working Log
// 2021.02.24.Hosik	: ����

#pragma once

#include "DXTK\SimpleMath.h"
#include "TempDefine.h"

namespace hos::cg
{
	class alignas(16) BoneBuffer	// ������ۿ� �����ʹ� 16����Ʈ�� ���ĵǾ�� �Ѵ�.
	{
	private:
		DirectX::SimpleMath::Matrix BoneTM[MAX_BONE];

	public:
		BoneBuffer();
		~BoneBuffer();

	public:
		void SetBoneTM(DirectX::SimpleMath::Matrix* matrixArray, unsigned int size);

	private:

	};
}

