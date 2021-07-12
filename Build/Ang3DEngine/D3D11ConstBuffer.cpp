// �ʼ� ���
#include "GoodFunc.h"

// �������
//#include "D3D11DeviceManager.h"
#include "DebugConsole.h"

// �ڱ� ���
#include "D3D11ConstBuffer.h"

using namespace hos::cg;

D3D11ConstBuffer::D3D11ConstBuffer(ID3D11Device* device, ID3D11DeviceContext* context, LPVOID data, UINT size, const WCHAR* bufferName)
	: Data(nullptr), BufferSize(0), DeviceRef(*device), ContextRef(*context)
{
	HRESULT hr = S_OK;
	_tcscpy_s(this->BufferName, bufferName);

	// ������ ����
	this->Data = data;
	BufferSize = size;

	//��� ���� ������, ũ�� �׽�Ʈ: 16����Ʈ ���ĵ� ũ��� ����.
	if (size % 16)
	{
		// TODO: ������� ���� ����! ������ ����ü�� ũ�Ⱑ 16����Ʈ�� ���ĵ��� �ʾҽ��ϴ�!! �޽��� �߰�
		DEBUG_CONSOLET(L"�ش� ������� ������ ����ü�� 16Byte�� ���ĵ��� �ʾҽ��ϴ�.", this->BufferName);
	}

	//��� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;				//���� �������� ����.
	bd.ByteWidth = (UINT)size;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//CPU ���� ����.

	//���긮�ҽ� ����.
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &Data;						//(�ܺ�) ��� ������ ����.
	//sd.SysMemPitch = 0;
	//sd.SysMemSlicePitch = 0;
	
	//��� ���� ����.
	hr = this->DeviceRef.CreateBuffer(&bd, NULL, &ConstBuffer);

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"DynamicConstantBuffer ���� ����", L"Error_ConstBuffer", MB_OK);
#endif
	}

	DEBUG_CONSOLET(L"D3D11_ConstBuffer ������ �����߽��ϴ�.", this->BufferName);
}

D3D11ConstBuffer::~D3D11ConstBuffer()
{
	this->Data = nullptr;
	DEBUG_CONSOLET(L"D3D11_ConstBuffer ���ſ� �����߽��ϴ�.", this->BufferName);
}

HRESULT D3D11ConstBuffer::Update()
{
	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mr;
	ZeroMemory(&mr, sizeof(mr));

	//������� ����
	//hr = GETD3D11CONTEXT->Map(this->ConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	hr = ContextRef.Map(this->ConstBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"UpdateDynamicConstantBuffer : Map ����", L"Error_ConstBuffer", MB_OK);
#endif
		return S_FALSE;
	}

	//��� ���� ����.
	memcpy(mr.pData, Data, BufferSize);

	//������� �ݱ�.
	ContextRef.Unmap(this->ConstBuffer.Get(), 0);

	return S_OK;
}

