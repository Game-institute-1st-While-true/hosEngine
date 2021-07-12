// 필수 헤더
#include "GoodFunc.h"

// 참조헤더
//#include "D3D11DeviceManager.h"
#include "DebugConsole.h"

// 자기 헤더
#include "D3D11ConstBuffer.h"

using namespace hos::cg;

D3D11ConstBuffer::D3D11ConstBuffer(ID3D11Device* device, ID3D11DeviceContext* context, LPVOID data, UINT size, const WCHAR* bufferName)
	: Data(nullptr), BufferSize(0), DeviceRef(*device), ContextRef(*context)
{
	HRESULT hr = S_OK;
	_tcscpy_s(this->BufferName, bufferName);

	// 데이터 복사
	this->Data = data;
	BufferSize = size;

	//상수 버퍼 생성전, 크기 테스트: 16바이트 정렬된 크기로 생성.
	if (size % 16)
	{
		// TODO: 상수버퍼 생성 실패! 데이터 구조체의 크기가 16바이트로 정렬되지 않았습니다!! 메시지 추가
		DEBUG_CONSOLET(L"해당 상수버퍼 데이터 구조체가 16Byte로 정렬되지 않았습니다.", this->BufferName);
	}

	//상수 버퍼 정보 설정.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;				//동적 정점버퍼 설정.
	bd.ByteWidth = (UINT)size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//CPU 접근 설정.

	//서브리소스 설정.
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &Data;						//(외부) 상수 데이터 설정.
	//sd.SysMemPitch = 0;
	//sd.SysMemSlicePitch = 0;
	
	//상수 버퍼 생성.
	hr = this->DeviceRef.CreateBuffer(&bd, NULL, &ConstBuffer);

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"DynamicConstantBuffer 생성 실패", L"Error_ConstBuffer", MB_OK);
#endif
	}

	DEBUG_CONSOLET(L"D3D11_ConstBuffer 생성에 성공했습니다.", this->BufferName);
}

D3D11ConstBuffer::~D3D11ConstBuffer()
{
	this->Data = nullptr;
	DEBUG_CONSOLET(L"D3D11_ConstBuffer 제거에 성공했습니다.", this->BufferName);
}

HRESULT D3D11ConstBuffer::Update()
{
	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	//상수버퍼 접근
	//hr = GETD3D11CONTEXT->Map(this->ConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	hr = ContextRef.Map(this->ConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"UpdateDynamicConstantBuffer : Map 실패", L"Error_ConstBuffer", MB_OK);
#endif
		return S_FALSE;
	}

	//상수 버퍼 갱신.
	memcpy(mr.pData, Data, BufferSize);

	//상수버퍼 닫기.
	ContextRef.Unmap(this->ConstBuffer.Get(), 0);

	return S_OK;
}

