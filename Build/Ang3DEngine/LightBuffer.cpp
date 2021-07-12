// ���� ���
#include <directxmath.h>

#include "DebugConsole.h"
#include <string>

// �ڱ� ���
#include "LightBuffer.h"

// ���ӽ����̽�
using namespace DirectX::SimpleMath;

hos::cg::LightBuffer::LightBuffer()
	: DirLightViewProj{ Matrix::Identity, Matrix::Identity , Matrix::Identity },

	DirLightDir{ { 0, 0, 0, 1 }, {0, 0, 0, 1}, { 0, 0, 0, 1 } },
	DirLightDiffColor{ { 0, 0, 0, 1 }, {0, 0, 0, 1}, { 0, 0, 0, 1 } },
	DirLightShaowColor{ { 0, 0, 0, 1 }, {0, 0, 0, 1}, { 0, 0, 0, 1 } },

	PointLightPos{ { 0, 0, 0, 1 }, },
	PointLightColor{ { 0, 0, 0, 1 }, },
	PointLightShadowColor{ { 0, 0, 0, 1 }, },
	PointLightAttenuation{ { 0, 0, 0, 1 }, },

	AmbLightColor(0, 0, 0, 1),

	DirLightNum(0), PointLightNum(0), ShadowBias(0.005f)
{
}

hos::cg::LightBuffer::~LightBuffer()
{
}

void hos::cg::LightBuffer::AddDirectionLight(Vector4& dir, Vector4& color)
{
	// DirLight�� ������ �̹� 3����� ���̻� �����͸� �߰����� �ʴ´�.
	if (this->DirLightNum != 5)
	{
		// �� ���� ����
		this->DirLightDir[this->DirLightNum] = dir;
		this->DirLightDiffColor[this->DirLightNum] = color;

		// (1.0f - RGB �� �������� ���)�� �׸��� ����� �������ش�.
		Vector4 _shadowColor = this->GetShadowColor(color);
		this->DirLightShaowColor[this->DirLightNum] = _shadowColor;

		// ���� ���� ���� ����
		this->DirLightNum++;
	}
}

void hos::cg::LightBuffer::AddPointLight(DirectX::SimpleMath::Vector4& pos, DirectX::SimpleMath::Vector4& color, DirectX::SimpleMath::Vector4& attenuation)
{
	// Light�� ������ �̹� 20����� ���̻� �����͸� �߰����� �ʴ´�.
	if (this->PointLightNum != 20)
	{
		// �� ���� ����
		this->PointLightPos[this->PointLightNum] = pos;
		this->PointLightColor[this->PointLightNum] = color;
		this->PointLightAttenuation[this->PointLightNum] = attenuation;

		// (1.0f - RGB �� �������� ���)�� �׸��� ����� �������ش�.
		Vector4 _shadowColor = this->GetShadowColor(color);
		this->PointLightShadowColor[this->PointLightNum] = _shadowColor;

		// ���� ���� ���� ����
		this->PointLightNum++;
	}
}

void hos::cg::LightBuffer::AddAmbientLight(DirectX::SimpleMath::Vector4& color)
{
	this->AmbLightColor += color;
	this->AmbLightColor.w = 1.0f;
}

void hos::cg::LightBuffer::UpdateLightBuffForShadow(Vector3 cameraPos, float fovDegree, unsigned int screenWidth, unsigned int screenHeight)
{
	if (this->DirLightNum == 0)
	{
		return;
	}

	static float _Radius = 55.0f;

	Vector3 _lightDir;
	_lightDir.x = this->DirLightDir[0].x;
	_lightDir.y = this->DirLightDir[0].y;
	_lightDir.z = this->DirLightDir[0].z;

	Vector3 _lightPos = -2.0f * _Radius * _lightDir + cameraPos;
	Vector3 _targetPos = cameraPos;
	_targetPos.y -= 0.000001f;

	Matrix V = XMMatrixLookAtLH(_lightPos, _targetPos, Vector3::Up);

	Vector3 sphereCenterLS = Vector3::Transform(_targetPos, V);

	float l = sphereCenterLS.x - _Radius;
	float b = sphereCenterLS.y - _Radius;
	float n = sphereCenterLS.z - _Radius;
	float r = sphereCenterLS.x + _Radius;
	float t = sphereCenterLS.y + _Radius;
	float f = sphereCenterLS.z + _Radius;
	Matrix P = DirectX::XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	Matrix T =
	{
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f
	};

	Matrix S = V * P * T;

	this->DirLightViewProj[0] = S;

	//// Direction Lihgt ����
	//for (unsigned int i = 0; i < this->DirLightNum; i++)
	//{
	//	// �׸��� ���� ������ ���� �� ���������� ViewProj ��ȯ ����� �����ش�.
	//	Vector3 pos =  Vector3(this->DirLightDir[i].x, this->DirLightDir[i].y, this->DirLightDir[i].z) * -30.f;	// TODO_ �ش� ���� �󸶰� �����Ѱ�?
	//	Vector3 look = Vector3(this->DirLightDir[i].x, this->DirLightDir[i].y, this->DirLightDir[i].z);

	//	float fovRadian = fovDegree * 3.141592654f / 180;
	//	float screenAspect = (float)screenWidth / (float)screenHeight;

	//	Matrix mLightView = DirectX::XMMatrixLookAtLH(pos, look, Vector3::Up);
	//	Matrix mLightProj = DirectX::XMMatrixPerspectiveFovLH(fovRadian, screenAspect, 1.0f, 200.f);
	//	Matrix mLightViewProj = mLightView * mLightProj;

	//	this->DirLightViewProj[i] = mLightViewProj;
	//}

	//if (GetAsyncKeyState(VK_HOME))
	//	this->ShadowBias += 0.001;

	//if (GetAsyncKeyState(VK_END))
	//	this->ShadowBias -= 0.001;
	//
	//
	//DEBUG_CONSOLE(L"���� " + std::to_wstring(ShadowBias));

}

// �� �����Ӹ��� �ʱ�ȭ���־�� �Ѵ�.
void hos::cg::LightBuffer::ClearDirLight()
{
	// ������ �ʱ�ȭ���ش�.
	this->DirLightNum = 0;
	this->PointLightNum = 0;

	// AmbLight�� ���� Frame���� �ٽ� �������־�� ������ ZeroColor�� �ʱ�ȭ���ش�.
	AmbLightColor = Vector4(0, 0, 0, 1);
}

DirectX::SimpleMath::Vector4 hos::cg::LightBuffer::GetShadowColor(Vector4& color)
{
	// (1.0f - RGB �� �������� ���)�� �׸��� ����� �������ش�.
	Vector4 _lightColor = color;
	float _totalColorValue = (_lightColor.x + _lightColor.y + _lightColor.z) / 3;
	float _shadowValue = 1 - _totalColorValue;

	return Vector4(_shadowValue, _shadowValue, _shadowValue, 1.0f);
}
