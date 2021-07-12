// <Concept>
// Source Name		: MatrixBuffer.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.02.03

// <Explanation>
// DirectX�� ������ۿ� ����� ��Ĺ���.
// �ٸ� �׷���API(ex.Vulkan, OpenGL ��)������ ��� ���Ǵ��� ���� �𸥴�.

// Working Log
// 2021.02.03.Hosik	: ����

#pragma once

#include "DXTK\SimpleMath.h"

namespace hos::cg
{
	class alignas(16) MatrixBuffer	// ������ۿ� �����ʹ� 16����Ʈ�� ���ĵǾ�� �Ѵ�.
	{
	private:
		DirectX::SimpleMath::Matrix WorldTM;
		DirectX::SimpleMath::Matrix ViewTM;
		DirectX::SimpleMath::Matrix ProjTM;
		DirectX::SimpleMath::Matrix WorldViewTM;
		DirectX::SimpleMath::Matrix WorldViewProjTM;
		DirectX::SimpleMath::Matrix InverseViewTM;
		DirectX::SimpleMath::Matrix InverseProjTM;
		DirectX::SimpleMath::Vector3 CameraWorldPos;

	public:
		MatrixBuffer();
		~MatrixBuffer();

	public:
		void SetWorldTM(const DirectX::SimpleMath::Matrix& mTM);
		void SetViewTM(const DirectX::SimpleMath::Matrix& mView);
		void SetProjTM(const DirectX::SimpleMath::Matrix& mProj);

	private:

	};
}

