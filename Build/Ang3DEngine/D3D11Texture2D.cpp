// ���� ���
#include "GoodFunc.h"
#include "DebugConsole.h"

// �ڱ� ���
#include "D3D11Texture2D.h"


hos::cg::D3D11Texture2D::D3D11Texture2D(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width /*= 512*/, unsigned int height /*= 512*/, unsigned int valueAA /*= 1*/)
	: DeviceRef(*device), ContextRef(*context), Width(width), Height(height), ValueAA(ValueAA)
{

}

hos::cg::D3D11Texture2D::~D3D11Texture2D()
{

}

void hos::cg::D3D11Texture2D::CopyTexture(ID3D11Texture2D* texture)
{
	// Texture Buffer ����
	//this->ContextRef.CopyResource(this->TextureBuffer.Get(), texture);

	// Texture�� ShaderResourceView ����
	this->CreateShaderResourceView(texture);
}

ID3D11ShaderResourceView* hos::cg::D3D11Texture2D::GetSRV()
{
	if (this->ShaderResourceView.Get() == nullptr)
	{
		DEBUG_CONSOLE(L"WARNING!! D3D11ShaderResourceView�� �������� ���� D3D11Texture2D�� Resource�� ��û�߽��ϴ�.");

		return nullptr;
	}

	return this->ShaderResourceView.Get();
}

HRESULT hos::cg::D3D11Texture2D::CreateShaderResourceView(ID3D11Texture2D* texture)
{
	//���̴����ҽ��� ���� ����.
	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;				//�ؽ�ó�� ������������.
	sd.Texture2D.MipLevels = 1;								//2D SRV �߰� ���� : �Ӹ� ����.
	sd.Texture2D.MostDetailedMip = 0;
	//sd.Texture2DMS.UnusedField_NothingToDefine = 0;		//2D SRV+AA �߰� ����

	if (this->ValueAA == 1)
	{
		sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		//2D SRV.
	}

	else
	{
		sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;		//2D SRV + AA
	}

	//���̴����ҽ��� ����.
	HRESULT hr = this->DeviceRef.CreateShaderResourceView(texture, &sd, this->ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"Ư�� D3D11_Texture2D�� CreateShaderResourceView ������ �����߽��ϴ�.");
		return hr;
	}

	return hr;
}
