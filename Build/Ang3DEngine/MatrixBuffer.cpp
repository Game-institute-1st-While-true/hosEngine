// �ڱ� ���
#include "MatrixBuffer.h"

// ���ӽ����̽�
using namespace DirectX::SimpleMath;


hos::cg::MatrixBuffer::MatrixBuffer()
	:WorldTM(Matrix::Identity), ViewTM(Matrix::Identity), ProjTM(Matrix::Identity),
	WorldViewTM(Matrix::Identity), WorldViewProjTM(Matrix::Identity)
{

}

hos::cg::MatrixBuffer::~MatrixBuffer()
{
}

void hos::cg::MatrixBuffer::SetWorldTM(const Matrix& mTM)
{
	this->WorldTM = mTM;
	this->WorldViewTM = this->WorldTM * this->ViewTM;
	this->WorldViewProjTM = this->WorldViewTM * this->ProjTM;
}

void hos::cg::MatrixBuffer::SetViewTM(const Matrix& mView)
{
	this->ViewTM = mView;
	this->InverseViewTM = this->ViewTM.Invert();

	// View����� ������ Camera�� ��ġ�� ����.
	Matrix mViewInverse = mView.Invert();
	this->CameraWorldPos = mViewInverse.Translation();
}

void hos::cg::MatrixBuffer::SetProjTM(const Matrix& mProj)
{
	this->ProjTM = mProj;
	this->InverseProjTM = this->ProjTM.Invert();
	this->WorldViewProjTM = this->WorldViewTM * this->ProjTM;
}