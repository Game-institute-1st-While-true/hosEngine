// <Concept>
// Source Name		: LightBuffer.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.03.10

// <Explanation>
// ���� ���� �� ������ ������� ����
/// Render Loop�� ������ LightBuffer�� �ʱ�ȭ�˴ϴ�.
/// ���� ������ �� �������� �̷�������մϴ�.

// Working Log
// 2021.03.10.Hosik	: ����

#pragma once

#include "DXTK\SimpleMath.h"


namespace hos::cg
{
	class alignas(16) LightBuffer	// ������ۿ� �����ʹ� 16����Ʈ�� ���ĵǾ�� �Ѵ�.
	{

	private:
		// ���籤��
		DirectX::SimpleMath::Matrix DirLightViewProj[5];
		DirectX::SimpleMath::Vector4 DirLightDir[5];
		DirectX::SimpleMath::Vector4 DirLightDiffColor[5];
		DirectX::SimpleMath::Vector4 DirLightShaowColor[5];

		DirectX::SimpleMath::Vector4 PointLightPos[20];
		DirectX::SimpleMath::Vector4 PointLightColor[20];
		DirectX::SimpleMath::Vector4 PointLightShadowColor[20];
		DirectX::SimpleMath::Vector4 PointLightAttenuation[20];

		DirectX::SimpleMath::Vector4 AmbLightColor;

		unsigned int DirLightNum;
		unsigned int PointLightNum;
		float ShadowBias;


	public:
		LightBuffer();
		~LightBuffer();

	public:
		// Direction Light
		void AddDirectionLight(DirectX::SimpleMath::Vector4& dir, DirectX::SimpleMath::Vector4& color);

		// Point Light
		void AddPointLight(DirectX::SimpleMath::Vector4& pos, DirectX::SimpleMath::Vector4& color, DirectX::SimpleMath::Vector4& attenuation);

		// Ambient Light
		void AddAmbientLight(DirectX::SimpleMath::Vector4& color);
		

		void UpdateLightBuffForShadow(DirectX::SimpleMath::Vector3 cameraPos, float fovDegree, unsigned int screenWidth, unsigned int screenHeight);
		void ClearDirLight();
		
	private:
		DirectX::SimpleMath::Vector4 GetShadowColor(DirectX::SimpleMath::Vector4& color);
	};
}

