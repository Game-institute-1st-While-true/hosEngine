// <Concept>
// Source Name		: Camera.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.02.15

// <Explanation>
// ī�޶� ��� Ŭ����
// �ܺο��� �����͸� ���� �޾Ƽ� ī�޶� Matrix�� ������ ��, ��Ĺ��ۿ� �Է��մϴ�.
// Renderer class���� �ܺο��� Matrix�� �Է� �޾� ���������ο� ���� �Է��� ���� �ֽ��ϴ�.

// <Working Log>
// 2021.02.15.Hosik	: ����
// 2021.03.25.Hosik : ī�޶���, ���� �� �Լ� �߰�. 2.5D�� �ʿ��� �� �ٽ� ����.

// <Memo>
// 2021.03.25.Hosik : UI ī�޶��� ��-����� Value�� ������ �� Ȯ���� �ʿ���.

#pragma once

#include "MatrixBuffer.h"

namespace hos::cg
{

	class Camera
	{
	public:
		enum class CameraType	// ī�޶� Ÿ�� ���
		{
			CAMERA_NONETYPE,
			CAMERA_3D,
			CAMERA_2D,
		};

		enum class CameraMode	// ī�޶� ��� ���
		{
			CAMERA_NONEMODE,
			CAMERA_DEFAULT,
			CAMERA_LOOKAT,
		};

	private:
		// ��� ���ۿ� ���� ����
		MatrixBuffer& MatrixData;
		DirectX::SimpleMath::Matrix& ViewTMRef;
		DirectX::SimpleMath::Matrix& ProjTMRef;
		DirectX::SimpleMath::Matrix& UIViewTMRef;
		DirectX::SimpleMath::Matrix& UIProjTMRef;


	public:
		Camera(MatrixBuffer& matrixBuffer, DirectX::SimpleMath::Matrix& viewTM, DirectX::SimpleMath::Matrix& projTM, DirectX::SimpleMath::Matrix& uiViewTMRef, DirectX::SimpleMath::Matrix& uiProjTMRef);
		~Camera();

	public:
		void Set3DCamera_DefaultView(DirectX::SimpleMath::Vector3 vPos, DirectX::SimpleMath::Vector3 vRot,
									float fov, float screenWidth, float screenHeight, float screenNear, float screenDepth);

		void Set3DCamera_FollowView(DirectX::SimpleMath::Vector3 eyePos, DirectX::SimpleMath::Vector3 lookAt, DirectX::SimpleMath::Vector3 upVector,
									float fov, float screenWidth, float screenHeight, float screenNear, float screenDepth);

		void Set2DCamera_DefaultView(DirectX::SimpleMath::Vector3 vPos, DirectX::SimpleMath::Vector3 vRot,
									float screenWidth, float screenHeight, float screenNear, float screenDepth);

		void Set2DCamera_FollowView(DirectX::SimpleMath::Vector3 eyePos, DirectX::SimpleMath::Vector3 lookAt, DirectX::SimpleMath::Vector3 upVector,
									float screenWidth, float screenHeight, float screenNear, float screenDepth);

		void Set3DUICamera(float screenWidth, float screenHeight);

	private:
		void Set3DCamera(float fov, float screenWidth, float screenHeight, float screenNear, float screenDepth);
		void Set2DCamera(float screenWidth, float screenHeight, float screenNear, float screenDepth);

		void SetDefaultView(DirectX::SimpleMath::Vector3 vPos, DirectX::SimpleMath::Vector3 vRot);
		void SetFollowView(DirectX::SimpleMath::Vector3 eyePos, DirectX::SimpleMath::Vector3 lookAt, DirectX::SimpleMath::Vector3 upVector);
	};
}