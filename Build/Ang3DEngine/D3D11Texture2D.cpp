// 참조 헤더
#include "GoodFunc.h"
#include "DebugConsole.h"

// 자기 헤더
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
	// Texture Buffer 복사
	//this->ContextRef.CopyResource(this->TextureBuffer.Get(), texture);

	// Texture로 ShaderResourceView 생성
	this->CreateShaderResourceView(texture);
}

ID3D11ShaderResourceView* hos::cg::D3D11Texture2D::GetSRV()
{
	if (this->ShaderResourceView.Get() == nullptr)
	{
		DEBUG_CONSOLE(L"WARNING!! D3D11ShaderResourceView가 생성되지 않은 D3D11Texture2D에 Resource를 요청했습니다.");

		return nullptr;
	}

	return this->ShaderResourceView.Get();
}

HRESULT hos::cg::D3D11Texture2D::CreateShaderResourceView(ID3D11Texture2D* texture)
{
	//셰이더리소스뷰 정보 구성.
	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;				//텍스처와 동일포멧유지.
	sd.Texture2D.MipLevels = 1;								//2D SRV 추가 설정 : 밉멥 설정.
	sd.Texture2D.MostDetailedMip = 0;
	//sd.Texture2DMS.UnusedField_NothingToDefine = 0;		//2D SRV+AA 추가 설정

	if (this->ValueAA == 1)
	{
		sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;		//2D SRV.
	}

	else
	{
		sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;		//2D SRV + AA
	}

	//셰이더리소스뷰 생성.
	HRESULT hr = this->DeviceRef.CreateShaderResourceView(texture, &sd, this->ShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"특정 D3D11_Texture2D의 CreateShaderResourceView 생성에 실패했습니다.");
		return hr;
	}

	return hr;
}
