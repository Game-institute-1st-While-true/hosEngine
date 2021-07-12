// 시스템 헤더
#include "DebugConsole.h"

// 수학 헤더
#include <directxmath.h>
#include "DXTK\SimpleMath.h"

// 자기 헤더
#include "Camera.h"

using namespace DirectX::SimpleMath;

hos::cg::Camera::Camera(MatrixBuffer& matrixBuffer, Matrix& viewTM, Matrix& projTM, Matrix& uiViewTM, Matrix& uiProjTM)
	: MatrixData(matrixBuffer), ViewTMRef(viewTM), ProjTMRef(projTM), UIViewTMRef(uiViewTM), UIProjTMRef(uiProjTM)
{
	DEBUG_CONSOLE(L"카메라 Function 클래스 생성 완료.");
}

hos::cg::Camera::~Camera()
{
	DEBUG_CONSOLE(L"카메라 Function 클래스 제거 완료.");
}

void hos::cg::Camera::Set3DCamera_DefaultView(Vector3 vPos, Vector3 vRot, float fov, float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// View행렬 계산
	SetDefaultView(vPos, vRot);

	// Proj행렬 계산
	Set3DCamera(fov, screenWidth, screenHeight, screenNear, screenDepth);

}

void hos::cg::Camera::Set3DCamera_FollowView(Vector3 eyePos, Vector3 lookAt, Vector3 upVector,
											float fov, float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// View행렬 계산
	SetFollowView(eyePos, lookAt, upVector);
	
	// Proj행렬 계산
	Set3DCamera(fov, screenWidth, screenHeight, screenNear, screenDepth);

}

void hos::cg::Camera::Set2DCamera_DefaultView(Vector3 vPos, Vector3 vRot,
											float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// View행렬 계산
	SetDefaultView(vPos, vRot);

	// Proj행렬 계산
	Set2DCamera(screenWidth, screenHeight, screenNear, screenDepth);

}

void hos::cg::Camera::Set2DCamera_FollowView(Vector3 eyePos, Vector3 lookAt, Vector3 upVector,
											float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// View행렬 계산
	SetFollowView(eyePos, lookAt, upVector);

	// Proj행렬 계산
	Set2DCamera(screenWidth, screenHeight, screenNear, screenDepth);
}

void hos::cg::Camera::Set3DUICamera(float screenWidth, float screenHeight)
{
	// View행렬 계산(Indentity)
	this->UIViewTMRef = Matrix::Identity;

	// Proj행렬 계산
	Matrix mOffCenter = DirectX::XMMatrixOrthographicLH(screenWidth, screenHeight, 0.000001f, 1.0f);
	this->UIProjTMRef = mOffCenter;
}

void hos::cg::Camera::Set3DCamera(float fovDegree, float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// 원근 투영에 필요한 값을 계산합니다.
	float FieldOfView = DirectX::XMConvertToRadians(fovDegree);
	float ScreenAspect = (float)screenWidth / (float)screenHeight;

	// 3D렌더링을 위한 원근 투영행렬을 만듭니다.
	Matrix mProj = DirectX::XMMatrixPerspectiveFovLH(FieldOfView, ScreenAspect, screenNear, screenDepth);

	// 계산결과를 버퍼로 전송합니다.
	//MatrixData.SetProjTM(mProj);
	this->ProjTMRef = mProj;
}

void hos::cg::Camera::Set2DCamera(float screenWidth, float screenHeight, float screenNear, float screenDepth)
{
	// 2D렌더링을 위한 직교 투영행렬을 만듭니다.
	Matrix mOrtho = DirectX::XMMatrixOrthographicLH(screenWidth, screenHeight, screenNear, screenDepth);

	// 계산결과를 버퍼로 전송합니다.
	//MatrixData.SetProjTM(mOrtho);
	this->ProjTMRef = mOrtho;
}

void hos::cg::Camera::SetDefaultView(Vector3 vPos, Vector3 vRot)
{
	// 카메라의 회전 행렬 계산
	Matrix mRot = Matrix::CreateFromYawPitchRoll(vRot.y, vRot.x, vRot.z);

	// 카메라의 위치 행렬 계산
	Matrix mPos = Matrix::CreateTranslation(vPos);

	// 카메라의 Local 행렬 계산
	Matrix mLocal = mRot * mPos;
	mLocal = mLocal.Invert();

	// 계산 결과를 역행렬로 바꿔 버퍼로 전송
	/// 자유시점의 View행렬은 카메라 Local행렬의 역행렬.
	//MatrixData.SetViewTM(mLocal);
	this->ViewTMRef = mLocal;
}

void hos::cg::Camera::SetFollowView(Vector3 eyePos, Vector3 lookAt, Vector3 upVector)
{
	// LookAt Matrix를 왼손좌표계로 생성.
	Matrix mView = DirectX::XMMatrixLookAtLH(eyePos, lookAt, upVector);

	// 계산 결과를 버퍼로 전송
	MatrixData.SetViewTM(mView);
	this->ViewTMRef = mView;
}
