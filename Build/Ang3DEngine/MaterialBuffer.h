// <Concept>
// Source Name		: MaterialBuffer.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.02.18

// <Explanation>
// DirectX�� ���� ���ۿ� ����� ����.
// �ٸ� �׷���API(ex.Vulkan, OpenGL ��)������ ��� ���Ǵ��� ���� �𸥴�.

// Working Log
// 2021.02.18.Hosik	: ����

#pragma once

#include "DXTK\SimpleMath.h"


namespace hos::cg
{

	class alignas(16) MaterialBuffer
	{
		// ��� �ν��Ͻ�
	private:
		DirectX::SimpleMath::Vector4 DiffuseColor;
		DirectX::SimpleMath::Vector4 AmbientColor;
		DirectX::SimpleMath::Vector4 SpecularColor;
		DirectX::SimpleMath::Vector4 OffTill;
		float SpecularPower;


		// ������ �Ҹ���
	public:
		MaterialBuffer();
		~MaterialBuffer();

		// �ܺ� �Լ�
	public:
		void SetDiffColor(const DirectX::SimpleMath::Vector4& color);
		void SetAmbColor (const DirectX::SimpleMath::Vector4& color);
		void SetSpecColor(const DirectX::SimpleMath::Vector4& color);
		void SetOffTill  (const DirectX::SimpleMath::Vector4& offTill);
		void SetSpecPower(const float& power);
	};

}