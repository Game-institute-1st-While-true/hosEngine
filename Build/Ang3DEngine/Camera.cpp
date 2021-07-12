// �ý��� ���
#include "DebugConsole.h"

// ���� ���
#include <directxmath.h>
#include "DXTK\SimpleMath.h"

// �ڱ� ���
#include "Camera.h"

using namespace DirectX::SimpleMath;

hos::cg::Camera::Camera(MatrixBuffer& matrixBuffer, Matrix& viewTM, Matrix& projTM, Matrix& uiViewTM, Matrix& uiProjTM)
	: MatrixData(matrixBuffer), ViewTMRef(viewTM), ProjTMRef(projTM), UIViewTMRef(uiViewTM), UIProjTMRef(uiProjTM)
{
	DEBUG_CONSOLE(L"ī�޶� Function Ŭ���� ���� �Ϸ�.");
}

hos::cg::Camera::~Camera()
{
	DEBUG_CONSOLE(L"ī�޶� Function Ŭ���� ���� �Ϸ�.");
}

void hos::cg::Camera::Set3DCamera_DefaultView(Vector3 vPos, Vector3 vRot, float fov, float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// View��� ���
	SetDefaultView(vPos, vRot);

	// Proj��� ���
	Set3DCamera(fov, screenWidth, screenHeight, screenNear, screenDepth);

}

void hos::cg::Camera::Set3DCamera_FollowView(Vector3 eyePos, Vector3 lookAt, Vector3 upVector,
											float fov, float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// View��� ���
	SetFollowView(eyePos, lookAt, upVector);
	
	// Proj��� ���
	Set3DCamera(fov, screenWidth, screenHeight, screenNear, screenDepth);

}

void hos::cg::Camera::Set2DCamera_DefaultView(Vector3 vPos, Vector3 vRot,
											float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// View��� ���
	SetDefaultView(vPos, vRot);

	// Proj��� ���
	Set2DCamera(screenWidth, screenHeight, screenNear, screenDepth);

}

void hos::cg::Camera::Set2DCamera_FollowView(Vector3 eyePos, Vector3 lookAt, Vector3 upVector,
											float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// View��� ���
	SetFollowView(eyePos, lookAt, upVector);

	// Proj��� ���
	Set2DCamera(screenWidth, screenHeight, screenNear, screenDepth);
}

void hos::cg::Camera::Set3DUICamera(float screenWidth, float screenHeight)
{
	// View��� ���(Indentity)
	this->UIViewTMRef = Matrix::Identity;

	// Proj��� ���
	Matrix mOffCenter = DirectX::XMMatrixOrthographicLH(screenWidth, screenHeight, 0.000001f, 1.0f);
	this->UIProjTMRef = mOffCenter;
}

void hos::cg::Camera::Set3DCamera(float fovDegree, float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// ���� ������ �ʿ��� ���� ����մϴ�.
	float FieldOfView = DirectX::XMConvertToRadians(fovDegree);
	float ScreenAspect = (float)screenWidth / (float)screenHeight;

	// 3D�������� ���� ���� ��������� ����ϴ�.
	Matrix mProj = DirectX::XMMatrixPerspectiveFovLH(FieldOfView, ScreenAspect, screenNear, screenDepth);

	// ������� ���۷� �����մϴ�.
	//MatrixData.SetProjTM(mProj);
	this->ProjTMRef = mProj;
}

void hos::cg::Camera::Set2DCamera(float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// 2D�������� ���� ���� ��������� ����ϴ�.
	Matrix mOrtho = DirectX::XMMatrixOrthographicLH(screenWidth, screenHeight, screenNear, screenDepth);

	// ������� ���۷� �����մϴ�.
	//MatrixData.SetProjTM(mOrtho);
	this->ProjTMRef = mOrtho;
}

void hos::cg::Camera::SetDefaultView(Vector3 vPos, Vector3 vRot)
{
	// ī�޶��� ȸ�� ��� ���
	Matrix mRot = Matrix::CreateFromYawPitchRoll(vRot.y, vRot.x, vRot.z);

	// ī�޶��� ��ġ ��� ���
	Matrix mPos = Matrix::CreateTranslation(vPos);

	// ī�޶��� Local ��� ���
	Matrix mLocal = mRot * mPos;
	mLocal = mLocal.Invert();

	// ��� ����� ����ķ� �ٲ� ���۷� ����
	/// ���������� View����� ī�޶� Local����� �����.
	//MatrixData.SetViewTM(mLocal);
	this->ViewTMRef = mLocal;
}

void hos::cg::Camera::SetFollowView(Vector3 eyePos, Vector3 lookAt, Vector3 upVector)
{
	// LookAt Matrix�� �޼���ǥ��� ����.
	Matrix mView = DirectX::XMMatrixLookAtLH(eyePos, lookAt, upVector);

	// ��� ����� ���۷� ����
	MatrixData.SetViewTM(mView);
	this->ViewTMRef = mView;
}
