// 필수 헤더
#include "tchar.h"
#include "GoodFunc.h"

// 라이브러리
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// 참조 헤더
#include "DebugConsole.h"
//#include "D3D11DeviceManager.h"


#include "D3D11ShaderDesc.h"

// 자기 헤더
#include "D3D11Shader.h"


hos::cg::D3D11Shader::D3D11Shader(WCHAR* fileName, int shaderIndex, const WCHAR* shaderName, ID3D11Device& deviceRef, ID3D11DeviceContext& contextRef, std::vector<ID3D11SamplerState*>& samplerList)
	: DeviceRef(deviceRef), ContextRef(contextRef), SamplerCount(0), SamplerIndexList(nullptr), samplerListRef(samplerList)
{
	HRESULT hr = S_OK;

	// 파일 경로 저장
	_tcscpy_s(this->FileName, fileName);
	_tcscpy_s(this->ShaderName, shaderName);

	// 쉐이더, VBLayout, 상수버퍼 생성.
	hr = InitializeShader();
	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"쉐이더 생성에 실패했습니다.", this->ShaderName);
		return;
	}

	// 파일 경로에서 마지막 '.'와 문자열 끝 사이의 구간을 복사해 파일 이름을 얻는다(디버그 출력용)
	// 이곳에서 파일 경로 대신 파일 이름만 남깁니다.
	std::wstring name = std::wstring(fileName);
	size_t pos = name.find_last_of('/');
	_tcscpy_s(FileName, &name[pos + 1]);

	// 받은 인덱스에 맞는 Layout을 생성합니다.
	hr = CreateVBLayout(shaderIndex);
	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"VBLayout 생성에 실패했습니다.", this->ShaderName);
		return;
	}

	// 샘플러를 만듭니다.
	hr = CreateSamplerIndex(shaderIndex);
	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"Sampler 생성에 실패했습니다.", this->ShaderName);
		return;
	}

	DEBUG_CONSOLET(L"Shader 생성에 성공했습니다.", this->ShaderName);
	return;
}

hos::cg::D3D11Shader::D3D11Shader(WCHAR* vsFileName, WCHAR* psFileName, int shaderIndex, ID3D11Device& deviceRef, ID3D11DeviceContext& contextRef, std::vector<ID3D11SamplerState*>& samplerList)
	: DeviceRef(deviceRef), ContextRef(contextRef), SamplerCount(0), SamplerIndexList(nullptr), samplerListRef(samplerList)
{
	HRESULT hr = S_OK;

	// 쉐이더, VBLayout, 상수버퍼 생성.
	hr = this->LoadCompliedShader(vsFileName, psFileName, shaderIndex);
	
	// 파일 경로에서 마지막 '.'와 문자열 끝 사이의 구간을 복사해 파일 이름을 얻는다(디버그 출력용)
	// 이곳에서 파일 경로 대신 파일 이름만 남깁니다.
	std::wstring name = std::wstring(vsFileName);
	size_t pos = name.find_last_of('/');
	_tcscpy_s(this->FileName, &name[pos + 1]);

	// 샘플러를 만듭니다.
	hr = CreateSamplerIndex(shaderIndex);

	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"Shader 생성에 실패했습니다.", this->FileName);
		return;
	}
}

hos::cg::D3D11Shader::~D3D11Shader()
{
	SAFE_DELARRY(this->SamplerIndexList);

	DEBUG_CONSOLET(L"Shader를 해제했습니다.", this->ShaderName);
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

	// 이미 레이아웃이 생성되어 있다면 새로 생성하지 않는다.
	if (InputLayout)
	{
		return S_FALSE;
	}

	// 받은 인덱스를 사용하여 정점 입력구조를 "D3D11ShaderDesc.h"에서 받아온다.
	/// 너무 대책없이 긴 배열이 잡glg는데.. 어떻게 줄여야 할까? ☆
	D3D11_INPUT_ELEMENT_DESC* layout = VBlayoutList[shaderIndex];
	UINT numElements = 0;

	// 받아온 정점 입력구조의 갯수를 구한다.
	/// TODO:: 시멘틱을 10개 이상 쓴다면..? 시멘틱을 몇 개 까지 쓸 수 있더라?
	for (int i = 0; i < 32; i++)
	{
		if (layout[i].SemanticName != NULL)
			numElements++;

		else
			break;
	}

	// 시멘틱 갯수 경고 처리용.
	if (numElements > 32)
	{
		numElements = 32;
	}

	// 정접 입력구조 객체 생성 Create the input layout
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

	// VS 로드
	hr = VS_Load();
	if (FAILED(hr))
	{
		return hr;
	}

	// PS 로드
	hr = PS_Load();
	if (FAILED(hr))
	{
		return hr;
	}

	/// VBLayout과 상수버퍼는 이후 바깥 단계에서 생성해줍니다.

	return hr;
}

HRESULT hos::cg::D3D11Shader::VS_Load()
{
	HRESULT hr = S_OK;

	// 쉐이더 컴파일
	hr = CompilerLoad("VS_Main", "vs_5_0", VSCode.GetAddressOf());

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"[실패] ShaderLoad :: Vertex Shader 컴파일 실패", L"Shader_Error", MB_OK | MB_ICONERROR);
#endif
		return hr;
	}

	// 정점 셰이더 객체 생성 Create a VS Object 
	hr = DeviceRef.CreateVertexShader(VSCode.Get()->GetBufferPointer(),
		VSCode.Get()->GetBufferSize(),
		nullptr,
		VertexShader.GetAddressOf());

	return hr;
}

HRESULT hos::cg::D3D11Shader::PS_Load()
{
	HRESULT hr = S_OK;

	// 픽셀 셰이더 컴파일 Compile a PixelShader
	ID3DBlob* pPSCode = nullptr;
	hr = CompilerLoad("PS_Main", "ps_5_0", &pPSCode);

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"[실패] ShaderLoad :: Pixel Shader 컴파일 실패", L"Shader_Error", MB_OK | MB_ICONERROR);
#endif
		return hr;
	}

	// 픽셀 셰이더 객체 생성 Create a PS Object 
	hr = DeviceRef.CreatePixelShader(pPSCode->GetBufferPointer(),
		pPSCode->GetBufferSize(),
		nullptr,
		PixelShader.GetAddressOf());

	SAFE_RELEASE(pPSCode);				//임시 개체 제거.	
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

	//컴파일 옵션1.
	UINT Flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;		//열우선 행렬 처리. 구형 DX9 이전까지의 전통적인 방식. 속도가 요구된다면, "행우선" 으로 처리할 것.
	//UINT Flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;	//행우선 행렬 처리. 속도의 향상이 있지만, 행렬을 전치 후 GPU 에 공급해야 합니다.
	//UINT Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
	Flags |= D3DCOMPILE_DEBUG;							//디버깅 모드시 옵션 추가.
	Flags |= D3DCOMPILE_SKIP_OPTIMIZATION;				//디버깅 모드시 옵션 추가.
#else
	Flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;				//디버깅 모드시 옵션 추가.
#endif

	//셰이더 소스 컴파일.
	hr = D3DCompileFromFile(FileName,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, //외부 파일 Include 허용. 
		entry,
		target,
		Flags,				//컴파일 옵션1
		0,					//컴파일 옵션2,  Effect 파일 컴파일시 적용됨. 이외에는 무시됨.
		ppCode,				//[출력] 컴파일된 셰이더 코드.
		&pError				//[출력] 컴파일 에러 코드.
	);

	if (FAILED(hr))
	{
		// TODO: 안전함수로 전환
		TCHAR message[256] = L"셰이더 컴파일 실패";
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

	// 이미 레이아웃이 생성되어 있다면 새로 생성하지 않는다.
	if (InputLayout)
	{
#ifdef _DEBUG
		MessageBox(NULL, L"이미 VBLayout이 생성된 Shader입니다.", L"Error", MB_OK);
#endif
		return S_FALSE;
	}

	// 받은 인덱스를 사용하여 정점 입력구조를 "D3D11ShaderDesc.h"에서 받아온다.
	/// 너무 대책없이 긴 배열이 잡히는데.. 어떻게 줄여야 할까? ☆
	D3D11_INPUT_ELEMENT_DESC* layout = VBlayoutList[shaderIndex];
	UINT numElements = 0;

	// 받아온 정점 입력구조의 갯수를 구한다.
	/// TODO:: 시멘틱을 10개 이상 쓴다면..? 시멘틱을 몇 개 까지 쓸 수 있더라?
	for (int i = 0; i < 32; i++)
	{
		if (layout[i].SemanticName != NULL)
			numElements++;

		else
			break;
	}

	// 시멘틱 갯수 경고 처리용.
	if (numElements > 32)
	{
		numElements = 32;
	}

	// 정접 입력구조 객체 생성 Create the input layout
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

	// 샘플러 리스트에서 미리 설정된 프리셋을 가져온다.
	TEXMODE_LIST* index = samplerListOnShader[shaderIndex];

	while (index[this->SamplerCount] != TEXMODE_LIST::SS_NONE)
	{
		this->SamplerCount++;
	}

	// 샘플러 리스트 생성
	this->SamplerIndexList = new int[this->SamplerCount];

	for (unsigned int i = 0; i < this->SamplerCount; i++)
	{
		this->SamplerIndexList[i] = (int)index[i];
	}

	return hr;
}

void hos::cg::D3D11Shader::Draw_Index(unsigned int indexCount)
{
	// 정점 레이아웃 설정
	ContextRef.IASetInputLayout(InputLayout.Get());

	// 셰이더 설정
	ContextRef.VSSetShader(VertexShader.Get(), NULL, 0);
	ContextRef.PSSetShader(PixelShader.Get(), NULL, 0);

	// PS에 샘플러를 설정해준다. 없으면 세팅하지 않음.
	//if (SampleState.Get())
	//{
	//	ContextRef.PSSetSamplers(0, 1, SampleState.GetAddressOf());
	//}

	for (unsigned int i = 0; i < this->SamplerCount; i++)
	{
		int index = this->SamplerIndexList[i];
		ContextRef.PSSetSamplers(i, 1, &(samplerListRef[index]));
	}

	// 그리기
	ContextRef.DrawIndexed(indexCount, 0, 0);
}

void hos::cg::D3D11Shader::Draw_Vtx(unsigned int vtxCount)
{
	// 정점 레이아웃 설정
	ContextRef.IASetInputLayout(InputLayout.Get());

	// 셰이더 설정
	ContextRef.VSSetShader(VertexShader.Get(), NULL, 0);
	ContextRef.PSSetShader(PixelShader.Get(), NULL, 0);

	for (unsigned int i = 0; i < this->SamplerCount; i++)
	{
		ContextRef.PSSetSamplers(i, 1, &(samplerListRef[this->SamplerIndexList[i]]));
	}

	// 그리기
	ContextRef.Draw(vtxCount, 0);
}

int hos::cg::D3D11Shader::ynErrorW(BOOL bMBox, TCHAR* msg, HRESULT hr, ID3DBlob* pBlob, TCHAR* filename, char* EntryPoint, char* ShaderModel)
{

	/*//가변매개변수 처리.
	TCHAR msgva[2048] = L"";
	va_list vl;
	va_start(vl, msg);
	_vstprintf(msgva, msg, vl);
	va_end(vl);
	*/
	//파라미터, 유니코드로 전환.

	TCHAR func[80] = L"";
	::mbstowcs_s(NULL, func, 80, EntryPoint, strlen(EntryPoint));
	TCHAR sm[20] = L"";
	::mbstowcs_s(NULL, sm, 20, ShaderModel, strlen(ShaderModel));


	//셰이더 오류 메세지 읽기.
	TCHAR errw[4096] = L"";
	::mbstowcs_s(NULL, errw, 4096, (char*)pBlob->GetBufferPointer(), pBlob->GetBufferSize());


	//HRESULT 에서 에러 메세지 얻기 
	//시스템으로 부터 얻는 셰이더 오류메세지는 부정확하므로 생략.
	TCHAR herr[1024] = L"아래의 오류를 확인하십시오.";
	/*FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		herr, 1024, NULL);
	*/

	//에러 메세지 붙이기.
	TCHAR errmsg[2048];
	_stprintf_s(errmsg, L"%s \nFile=%s  Entry=%s  Target=%s  \n에러코드(0x%08X) : %s \n\n%s",
		msg, filename, func, sm,
		hr, herr, errw);

	//(디버깅 중) VS 출력창으로 출력..
	//OutputDebugString(L"\n");
	//OutputDebugString(errmsg);


	//로그파일로 출력.
	//...


	//메세지 창 출력..
	if (bMBox)
	{
#ifdef _DEBUG
		MessageBox(NULL, errmsg, L"Shader::Error", MB_OK | MB_ICONERROR);
#endif
	}

	return TRUE;
}