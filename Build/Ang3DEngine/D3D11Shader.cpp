// �ʼ� ���
#include "tchar.h"
#include "GoodFunc.h"

// ���̺귯��
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// ���� ���
#include "DebugConsole.h"
//#include "D3D11DeviceManager.h"


#include "D3D11ShaderDesc.h"

// �ڱ� ���
#include "D3D11Shader.h"


hos::cg::D3D11Shader::D3D11Shader(WCHAR* fileName, int shaderIndex, const WCHAR* shaderName, ID3D11Device& deviceRef, ID3D11DeviceContext& contextRef, std::vector<ID3D11SamplerState*>& samplerList)
	: DeviceRef(deviceRef), ContextRef(contextRef), SamplerCount(0), SamplerIndexList(nullptr), samplerListRef(samplerList)
{
	HRESULT hr = S_OK;

	// ���� ��� ����
	_tcscpy_s(this->FileName, fileName);
	_tcscpy_s(this->ShaderName, shaderName);

	// ���̴�, VBLayout, ������� ����.
	hr = InitializeShader();
	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"���̴� ������ �����߽��ϴ�.", this->ShaderName);
		return;
	}

	// ���� ��ο��� ������ '.'�� ���ڿ� �� ������ ������ ������ ���� �̸��� ��´�(����� ��¿�)
	// �̰����� ���� ��� ��� ���� �̸��� ����ϴ�.
	std::wstring name = std::wstring(fileName);
	size_t pos = name.find_last_of('/');
	_tcscpy_s(FileName, &name[pos + 1]);

	// ���� �ε����� �´� Layout�� �����մϴ�.
	hr = CreateVBLayout(shaderIndex);
	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"VBLayout ������ �����߽��ϴ�.", this->ShaderName);
		return;
	}

	// ���÷��� ����ϴ�.
	hr = CreateSamplerIndex(shaderIndex);
	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"Sampler ������ �����߽��ϴ�.", this->ShaderName);
		return;
	}

	DEBUG_CONSOLET(L"Shader ������ �����߽��ϴ�.", this->ShaderName);
	return;
}

hos::cg::D3D11Shader::D3D11Shader(WCHAR* vsFileName, WCHAR* psFileName, int shaderIndex, ID3D11Device& deviceRef, ID3D11DeviceContext& contextRef, std::vector<ID3D11SamplerState*>& samplerList)
	: DeviceRef(deviceRef), ContextRef(contextRef), SamplerCount(0), SamplerIndexList(nullptr), samplerListRef(samplerList)
{
	HRESULT hr = S_OK;

	// ���̴�, VBLayout, ������� ����.
	hr = this->LoadCompliedShader(vsFileName, psFileName, shaderIndex);
	
	// ���� ��ο��� ������ '.'�� ���ڿ� �� ������ ������ ������ ���� �̸��� ��´�(����� ��¿�)
	// �̰����� ���� ��� ��� ���� �̸��� ����ϴ�.
	std::wstring name = std::wstring(vsFileName);
	size_t pos = name.find_last_of('/');
	_tcscpy_s(this->FileName, &name[pos + 1]);

	// ���÷��� ����ϴ�.
	hr = CreateSamplerIndex(shaderIndex);

	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"Shader ������ �����߽��ϴ�.", this->FileName);
		return;
	}
}

hos::cg::D3D11Shader::~D3D11Shader()
{
	SAFE_DELARRY(this->SamplerIndexList);

	DEBUG_CONSOLET(L"Shader�� �����߽��ϴ�.", this->ShaderName);
}

HRESULT hos::cg::D3D11Shader::LoadCompliedShader(WCHAR* vsFileName, WCHAR* psFileName, int shaderIndex)
{
	HRESULT hr = S_OK;

	HANDLE hVSFile = CreateFile(vsFileName, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	byte* vsData = nullptr;
	int vsFileSize = 0;

	if (hVSFile == INVALID_HANDLE_VALUE)
	{
		return S_FALSE;
		//Error opening file, handle here (GetLastError can return why this failed).
	}
	else
	{
		vsFileSize = GetFileSize(hVSFile, NULL);

		vsData = (byte*)malloc(vsFileSize);
		//You can also check if data is NULL, hence malloc failed

		DWORD numRead = 0;

		if (ReadFile(hVSFile, vsData, vsFileSize, &numRead, NULL))
		{
			hr = DeviceRef.CreateVertexShader(vsData, vsFileSize, NULL, VertexShader.GetAddressOf());
		}
		else
		{
			CloseHandle(hVSFile);
			free(vsData);

			return S_FALSE;
		}
	}

	HANDLE hPSFile = CreateFile(psFileName, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (psFileName == INVALID_HANDLE_VALUE)
	{
		return S_FALSE;
		//Error opening file, handle here (GetLastError can return why this failed).
	}
	else
	{
		int fileSize = GetFileSize(hPSFile, NULL);

		byte* data = (byte*)malloc(fileSize);
		//You can also check if data is NULL, hence malloc failed

		DWORD numRead = 0;

		if (ReadFile(hPSFile, data, fileSize, &numRead, NULL))
		{
			hr = DeviceRef.CreatePixelShader(data, fileSize, NULL, PixelShader.GetAddressOf());
		}

		else
		{
			CloseHandle(psFileName);
			free(data);

			return S_FALSE;
			//Read failed, handle error as you wish
		}

		CloseHandle(hPSFile);
		free(data);
	}

	// �̹� ���̾ƿ��� �����Ǿ� �ִٸ� ���� �������� �ʴ´�.
	if (InputLayout)
	{
		return S_FALSE;
	}

	// ���� �ε����� ����Ͽ� ���� �Է±����� "D3D11ShaderDesc.h"���� �޾ƿ´�.
	/// �ʹ� ��å���� �� �迭�� ��glg�µ�.. ��� �ٿ��� �ұ�? ��
	D3D11_INPUT_ELEMENT_DESC* layout = VBlayoutList[shaderIndex];
	UINT numElements = 0;

	// �޾ƿ� ���� �Է±����� ������ ���Ѵ�.
	/// TODO:: �ø�ƽ�� 10�� �̻� ���ٸ�..? �ø�ƽ�� �� �� ���� �� �� �ִ���?
	for (int i = 0; i < 32; i++)
	{
		if (layout[i].SemanticName != NULL)
			numElements++;

		else
			break;
	}

	// �ø�ƽ ���� ��� ó����.
	if (numElements > 32)
	{
		numElements = 32;
	}

	// ���� �Է±��� ��ü ���� Create the input layout
	hr = DeviceRef.CreateInputLayout(layout,
		numElements,
		vsData,
		vsFileSize,
		InputLayout.GetAddressOf()
	);
	return hr;
}


HRESULT hos::cg::D3D11Shader::InitializeShader()
{
	HRESULT hr = S_OK;

	// VS �ε�
	hr = VS_Load();
	if (FAILED(hr))
	{
		return hr;
	}

	// PS �ε�
	hr = PS_Load();
	if (FAILED(hr))
	{
		return hr;
	}

	/// VBLayout�� ������۴� ���� �ٱ� �ܰ迡�� �������ݴϴ�.

	return hr;
}

HRESULT hos::cg::D3D11Shader::VS_Load()
{
	HRESULT hr = S_OK;

	// ���̴� ������
	hr = CompilerLoad("VS_Main", "vs_5_0", VSCode.GetAddressOf());

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"[����] ShaderLoad :: Vertex Shader ������ ����", L"Shader_Error", MB_OK | MB_ICONERROR);
#endif
		return hr;
	}

	// ���� ���̴� ��ü ���� Create a VS Object 
	hr = DeviceRef.CreateVertexShader(VSCode.Get()->GetBufferPointer(),
		VSCode.Get()->GetBufferSize(),
		nullptr,
		VertexShader.GetAddressOf());

	return hr;
}

HRESULT hos::cg::D3D11Shader::PS_Load()
{
	HRESULT hr = S_OK;

	// �ȼ� ���̴� ������ Compile a PixelShader
	ID3DBlob* pPSCode = nullptr;
	hr = CompilerLoad("PS_Main", "ps_5_0", &pPSCode);

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"[����] ShaderLoad :: Pixel Shader ������ ����", L"Shader_Error", MB_OK | MB_ICONERROR);
#endif
		return hr;
	}

	// �ȼ� ���̴� ��ü ���� Create a PS Object 
	hr = DeviceRef.CreatePixelShader(pPSCode->GetBufferPointer(),
		pPSCode->GetBufferSize(),
		nullptr,
		PixelShader.GetAddressOf());

	SAFE_RELEASE(pPSCode);				//�ӽ� ��ü ����.	
	if (FAILED(hr))
	{
		return hr;
	}

	return hr;
}

HRESULT hos::cg::D3D11Shader::CompilerLoad(const CHAR* entry, const CHAR* target, ID3DBlob** ppCode)
{
	HRESULT hr = S_OK;

	ID3DBlob* pError = nullptr;

	//������ �ɼ�1.
	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;		//���켱 ��� ó��. ���� DX9 ���������� �������� ���. �ӵ��� �䱸�ȴٸ�, "��켱" ���� ó���� ��.
	//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;	//��켱 ��� ó��. �ӵ��� ����� ������, ����� ��ġ �� GPU �� �����ؾ� �մϴ�.
	//UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//����� ���� �ɼ� �߰�.
	Flags |= D3DCOMPILE_SKIP_OPTIMIZATION;				//����� ���� �ɼ� �߰�.
#else
	Flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;				//����� ���� �ɼ� �߰�.
#endif

	//���̴� �ҽ� ������.
	hr = D3DCompileFromFile(FileName,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, //�ܺ� ���� Include ���. 
		entry,
		target,
		Flags,				//������ �ɼ�1
		0,					//������ �ɼ�2,  Effect ���� �����Ͻ� �����. �̿ܿ��� ���õ�.
		ppCode,				//[���] �����ϵ� ���̴� �ڵ�.
		&pError				//[���] ������ ���� �ڵ�.
	);

	if (FAILED(hr))
	{
		// TODO: �����Լ��� ��ȯ
		TCHAR message[256] = L"���̴� ������ ����";
		CHAR ent[256]; CHAR tar[256];
		strcpy_s(ent, entry);
		strcpy_s(tar, target);
		ynErrorW(TRUE, message, hr, pError, FileName, ent, tar);
	}

	SAFE_RELEASE(pError);
	return hr;
}

HRESULT hos::cg::D3D11Shader::CreateVBLayout(int shaderIndex)
{
	HRESULT hr = S_OK;

	// �̹� ���̾ƿ��� �����Ǿ� �ִٸ� ���� �������� �ʴ´�.
	if (InputLayout)
	{
#ifdef _DEBUG
		MessageBox(NULL, L"�̹� VBLayout�� ������ Shader�Դϴ�.", L"Error", MB_OK);
#endif
		return S_FALSE;
	}

	// ���� �ε����� ����Ͽ� ���� �Է±����� "D3D11ShaderDesc.h"���� �޾ƿ´�.
	/// �ʹ� ��å���� �� �迭�� �����µ�.. ��� �ٿ��� �ұ�? ��
	D3D11_INPUT_ELEMENT_DESC* layout = VBlayoutList[shaderIndex];
	UINT numElements = 0;

	// �޾ƿ� ���� �Է±����� ������ ���Ѵ�.
	/// TODO:: �ø�ƽ�� 10�� �̻� ���ٸ�..? �ø�ƽ�� �� �� ���� �� �� �ִ���?
	for (int i = 0; i < 32; i++)
	{
		if (layout[i].SemanticName != NULL)
			numElements++;

		else
			break;
	}

	// �ø�ƽ ���� ��� ó����.
	if (numElements > 32)
	{
		numElements = 32;
	}

	// ���� �Է±��� ��ü ���� Create the input layout
	hr = DeviceRef.CreateInputLayout(layout,
		numElements,
		VSCode->GetBufferPointer(),
		VSCode->GetBufferSize(),
		InputLayout.GetAddressOf()
	);
	if (FAILED(hr))	return hr;

	return hr;
}

HRESULT hos::cg::D3D11Shader::CreateSamplerIndex(int shaderIndex)
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));

	// ���÷� ����Ʈ���� �̸� ������ �������� �����´�.
	TEXMODE_LIST* index = samplerListOnShader[shaderIndex];

	while (index[this->SamplerCount] != TEXMODE_LIST::SS_NONE)
	{
		this->SamplerCount++;
	}

	// ���÷� ����Ʈ ����
	this->SamplerIndexList = new int[this->SamplerCount];

	for (unsigned int i = 0; i < this->SamplerCount; i++)
	{
		this->SamplerIndexList[i] = (int)index[i];
	}

	return hr;
}

void hos::cg::D3D11Shader::Draw_Index(unsigned int indexCount)
{
	// ���� ���̾ƿ� ����
	ContextRef.IASetInputLayout(InputLayout.Get());

	// ���̴� ����
	ContextRef.VSSetShader(VertexShader.Get(), NULL, 0);
	ContextRef.PSSetShader(PixelShader.Get(), NULL, 0);

	// PS�� ���÷��� �������ش�. ������ �������� ����.
	//if (SampleState.Get())
	//{
	//	ContextRef.PSSetSamplers(0, 1, SampleState.GetAddressOf());
	//}

	for (unsigned int i = 0; i < this->SamplerCount; i++)
	{
		int index = this->SamplerIndexList[i];
		ContextRef.PSSetSamplers(i, 1, &(samplerListRef[index]));
	}

	// �׸���
	ContextRef.DrawIndexed(indexCount, 0, 0);
}

void hos::cg::D3D11Shader::Draw_Vtx(unsigned int vtxCount)
{
	// ���� ���̾ƿ� ����
	ContextRef.IASetInputLayout(InputLayout.Get());

	// ���̴� ����
	ContextRef.VSSetShader(VertexShader.Get(), NULL, 0);
	ContextRef.PSSetShader(PixelShader.Get(), NULL, 0);

	for (unsigned int i = 0; i < this->SamplerCount; i++)
	{
		ContextRef.PSSetSamplers(i, 1, &(samplerListRef[this->SamplerIndexList[i]]));
	}

	// �׸���
	ContextRef.Draw(vtxCount, 0);
}

int hos::cg::D3D11Shader::ynErrorW(BOOL bMBox, TCHAR* msg, HRESULT hr, ID3DBlob* pBlob, TCHAR* filename, char* EntryPoint, char* ShaderModel)
{

	/*//�����Ű����� ó��.
	TCHAR msgva[2048] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);
	*/
	//�Ķ����, �����ڵ�� ��ȯ.

	TCHAR func[80] = L"";
	::mbstowcs_s(NULL, func, 80, EntryPoint, strlen(EntryPoint));
	TCHAR sm[20] = L"";
	::mbstowcs_s(NULL, sm, 20, ShaderModel, strlen(ShaderModel));


	//���̴� ���� �޼��� �б�.
	TCHAR errw[4096] = L"";
	::mbstowcs_s(NULL, errw, 4096, (char*)pBlob->GetBufferPointer(), pBlob->GetBufferSize());


	//HRESULT ���� ���� �޼��� ��� 
	//�ý������� ���� ��� ���̴� �����޼����� ����Ȯ�ϹǷ� ����.
	TCHAR herr[1024] = L"�Ʒ��� ������ Ȯ���Ͻʽÿ�.";
	/*FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		herr, 1024, NULL);
	*/

	//���� �޼��� ���̱�.
	TCHAR errmsg[2048];
	_stprintf_s(errmsg, L"%s \nFile=%s  Entry=%s  Target=%s  \n�����ڵ�(0x%08X) : %s \n\n%s",
		msg, filename, func, sm,
		hr, herr, errw);

	//(����� ��) VS ���â���� ���..
	//OutputDebugString(L"\n");
	//OutputDebugString(errmsg);


	//�α����Ϸ� ���.
	//...


	//�޼��� â ���..
	if (bMBox)
	{
#ifdef _DEBUG
		MessageBox(NULL, errmsg, L"Shader::Error", MB_OK | MB_ICONERROR);
#endif
	}

	return TRUE;
}