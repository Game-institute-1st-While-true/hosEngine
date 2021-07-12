// <Concept>
// Source Name		: VertexDataType.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.03.16

// <Explanation>
// �ܺ� ������Ʈ���� ������ �ʿ��� ���ؽ� ���� ������ �����ϱ� ���� ���� Ŭ����.

// Working Log
// 2021.02.09.Hosik	: ����

#pragma once

#include <DXTK\SimpleMath.h>

namespace hos::cg
{

	class VertexDataType
	{
	public:
		struct FULL_VTX
		{
			DirectX::SimpleMath::Vector3 vPos;
			DirectX::SimpleMath::Vector3 vNrm;
			DirectX::SimpleMath::Vector2 vUV;
			DirectX::SimpleMath::Vector3 vBiNrm;
			DirectX::SimpleMath::Vector3 vTangent;
			float weight[4];
			unsigned int index[4];
		};

		struct UI_VTX
		{
			DirectX::SimpleMath::Vector3 vPos;
			DirectX::SimpleMath::Vector2 vUV;
		};

		struct FACE_INDEX
		{
			unsigned int Index[3];
		};

	};

}
