// 필수 헤더
#include "GoodFunc.h"
#include "DebugConsole.h"

#include "DXTK/DDSTextureLoader.h"
#include "DXTK/WICTextureLoader.h"

#include "DXTex/DirectXTex.h"

// 참조 헤더
#include "D3D11ConstBuffer.h"
#include "VertexDataType.h"

// 자기 헤더
#include "D3D11VariableManager.h"

// 네임스페이스
using namespace DirectX::SimpleMath;


hos::cg::D3D11VariableManager::D3D11VariableManager(ID3D11Device& device, ID3D11DeviceContext& context)
	:DeviceRef(device), ContextRef(context), VtxBufferUI(nullptr), VtxBuffer3DUI{ 0, }, SpriteFontBatch(nullptr), DefaultFont(nullptr), SkyBoxBuffer(nullptr)
	/// 각 리스트의 크기는 D3D11Data.h(가칭)과 D3D11ShaderDesc.h를 참조한다.
{
	// 상수버퍼, 쉐이더 리스트 공간 할당 및 초기화
	ConstBufferList.resize((size_t)ConstBufferType::CB_MAX);
	ShaderList.resize((size_t)SHADER_LIST::SHADER_MAX);

	// MeshBuffer, Texture 리스트 공간 할당
	MeshBufferSetList.reserve(100);
	TextureList.reserve(100);

	// UI, SkyBox 자원 생성
	CreateUIBuffer();
	CreateSkyBoxBuffer();

	// Font 자원 생성
	this->SpriteFontBatch = new DirectX::SpriteBatch(&ContextRef);

	// 기본 폰트 생성
	try
	{
		this->DefaultFont = new DirectX::SpriteFont(&DeviceRef, L"./Assets/Font/Arial.spriteFont");
	}
	catch (...)
	{
		try
		{
			this->DefaultFont = new DirectX::SpriteFont(&DeviceRef, L"../../Resource/Font/Arial.spriteFont");
		}

		catch (...)
		{
			try
			{
				this->DefaultFont = new DirectX::SpriteFont(&DeviceRef, L"../Resource/Font/Arial.spriteFont");
			}

			catch(...)
			{
				DEBUG_CONSOLE(L"Default SpriteFont인 Arial.spriteFont 파일이 존재하지 않습니다.");
				assert(0);
			}
		}
	}

	this->SpriteFontList.reserve(5);

	// SkyBox 자원 생성
	this->SkyBoxList.reserve(5);
}

hos::cg::D3D11VariableManager::~D3D11VariableManager()
{
	// SKyBox 자원 제거
	for (auto [name, skybox] : SkyBoxList)
	{
		SAFE_RELEASE(skybox);
	}
	SkyBoxList.clear();

	// FontList 제거
	for (auto [name, font] : SpriteFontList)
	{
		SAFE_DELETE(font);
	}
	SpriteFontList.clear();

	// 기본 Font 자원 제거
	SAFE_DELETE(this->DefaultFont);
	SAFE_DELETE(this->SpriteFontBatch);

	// UI 및 SkyBox 자원 제거
	SAFE_RELEASE(VtxBufferUI);
	SAFE_RELEASE(SkyBoxBuffer);

	for (int i = 0; i < 9; i++)
	{
		SAFE_RELEASE(VtxBuffer3DUI[i]);
	}

	// 상수버퍼 리스트 제거
	SAFE_DELETE_VECTOR(ConstBufferList);

	// 쉐이더 리스트 제거
	SAFE_DELETE_VECTOR(ShaderList);

	// 샘플러 제거
	for (size_t i = 0; i < this->SampleStateList.size(); i++)
	{
		SAFE_RELEASE(this->SampleStateList[i]);
	}

	// Mesh Buffer 리스트 제거
	for (auto elem : MeshBufferSetList)
	{
		SAFE_RELEASE(MeshBufferSetList[elem.first].VertexBuffer);
		SAFE_RELEASE(MeshBufferSetList[elem.first].IndexBuffer);
	}
	MeshBufferSetList.clear();
	std::unordered_map<std::wstring, MeshBufferSet>().swap(MeshBufferSetList);

	// 텍스쳐 리스트 제거
	for (auto elem : TextureList)
	{
		SAFE_RELEASE(elem.second);
	}
	TextureList.clear();
	std::unordered_map <std::wstring, ID3D11ShaderResourceView*>().swap(TextureList);
}

void hos::cg::D3D11VariableManager::AllClearMeshVariableList()
{
	for (auto elem : MeshBufferSetList)
	{
		std::wstring meshName = elem.first;
		ReleaseMeshBuffer(meshName);
	}

	MeshBufferSetList.clear();
	std::unordered_map<std::wstring, MeshBufferSet>().swap(MeshBufferSetList);
}

void hos::cg::D3D11VariableManager::AllClearTextureVariableList()
{
	for (auto elem : TextureList)
	{
		std::wstring textureName = elem.first;
		ReleaseD3D11Texture(textureName);
	}

	TextureList.clear();
	std::unordered_map <std::wstring, ID3D11ShaderResourceView*>().swap(TextureList);
}

void hos::cg::D3D11VariableManager::CreateConstBuffer(ID3D11Device* device, ID3D11DeviceContext* context, LPVOID data, UINT size, ConstBufferType type, const WCHAR* bufferName)
{
	ConstBufferList[(size_t)type] = new D3D11ConstBuffer(device, context, data, size, bufferName);
}

void hos::cg::D3D11VariableManager::CreateShader(WCHAR* fileName, int shaderIndex, const WCHAR* shaderName, std::vector<ID3D11SamplerState*>& samplerList)
{
	ShaderList[shaderIndex] = new D3D11Shader(fileName, shaderIndex, shaderName, this->DeviceRef, this->ContextRef, samplerList);
}

void hos::cg::D3D11VariableManager::CreateShader(WCHAR* vsFileName, WCHAR* psFileName, int shaderIndex, const WCHAR* shaderName, std::vector<ID3D11SamplerState*>& samplerList)
{
	ShaderList[shaderIndex] = new D3D11Shader(vsFileName, psFileName, shaderIndex, this->DeviceRef, this->ContextRef, samplerList);
}

HRESULT hos::cg::D3D11VariableManager::CreateSamplerIndex(int samplerIndex)
{
	HRESULT hr = S_OK;

	ID3D11SamplerState* dest = nullptr;

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));

	// 샘플러 리스트에서 미리 설정된 프리셋을 가져온다.
	sd = SamplerOptionList[samplerIndex];

	hr = DeviceRef.CreateSamplerState(&sd, &dest);

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"[실패] Create Sampler", L"Error", MB_OK | MB_ICONERROR);
#endif
		return hr;
	}

	this->SampleStateList.push_back(dest);

	return hr;
}

void hos::cg::D3D11VariableManager::UpdateConstBuffer(unsigned int index)
{
	if (ConstBufferList[(size_t)index])
	{
		// 상수버퍼 자체의 map/unmap
		ConstBufferList[(size_t)index]->Update();

		// 상수버퍼를 VS/PS에 세팅
		ContextRef.VSSetConstantBuffers(index, 1, ConstBufferList[(size_t)index]->GetID3D11Buffer());
		ContextRef.PSSetConstantBuffers(index, 1, ConstBufferList[(size_t)index]->GetID3D11Buffer());
	}
}

hos::cg::D3D11Shader* hos::cg::D3D11VariableManager::GetShader(hos::cg::SHADER_LIST shaderIndex)
{
	return ShaderList[(size_t)shaderIndex];
}

void hos::cg::D3D11VariableManager::DrawByIndex(SHADER_LIST shaderType, unsigned int indexCount)
{
	ShaderList[(size_t)shaderType]->Draw_Index(indexCount);
}

void hos::cg::D3D11VariableManager::DrawByVertex(SHADER_LIST shaderType, unsigned int vertexCount)
{
	ShaderList[(size_t)shaderType]->Draw_Vtx(vertexCount);
}

bool hos::cg::D3D11VariableManager::CreateMeshBuffer(std::wstring meshName, void* vtxData, unsigned int vtxCount, void* indexData, unsigned int indexCount)
{
	HRESULT hr = S_OK;

	if (MeshBufferSetList.count(meshName))
	{
		DEBUG_CONSOLET(L"이미 해당 Key로 Mesh의 ID3D11Buffer들이 생성되어 있습니다.", meshName);
		hr = S_FALSE;
	}

	ID3D11Buffer* VB = nullptr;
	ID3D11Buffer* IB = nullptr;
	MeshBufferSet BufferSet;

	// 정보 얻기
	UINT sizeVB = vtxCount * sizeof(VertexDataType::FULL_VTX);
	UINT sizeIB = indexCount * sizeof(unsigned int);

	// 정점 버퍼 만들기
	if (FAILED(hr = CreateVB(vtxData, sizeVB, &VB)))
	{
		return false;
	}

	// 인덱스 버퍼 만들기
	if (FAILED(hr = CreateIB(indexData, sizeIB, &IB)))
	{
		return false;
	}

	// 리스트에 구조체 전달
	BufferSet.VertexBuffer = VB;
	BufferSet.IndexBuffer = IB;
	BufferSet.IndexCount = indexCount;
	MeshBufferSetList.insert({ meshName, BufferSet });

	DEBUG_CONSOLET(L"D3D11 MeshBufferSet 생성에 성공했습니다.", meshName);

	return true;
}

bool hos::cg::D3D11VariableManager::ReleaseMeshBuffer(std::wstring meshName)
{
	if (MeshBufferSetList.count(meshName))
	{
		MeshBufferSetList[meshName].VertexBuffer->Release();
		MeshBufferSetList[meshName].IndexBuffer->Release();
		MeshBufferSetList.erase(meshName);

		DEBUG_CONSOLET(L"D3D11MeshBuffer의 Release에 성공했습니다.", meshName);

		return true;
	}

	else
	{
		DEBUG_CONSOLET(L"해당 D3D11MeshBuffer Resource가 존재하지 않아 Release할수 없습니다.", meshName);

		return false;
	}

	return true;
}

HRESULT hos::cg::D3D11VariableManager::CreateVB(void* pBuff, UINT size, ID3D11Buffer** dest)
{
	ID3D11Buffer* pVB = nullptr;

	//정점 버퍼 정보 구성.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//버퍼 사용방식
	bd.ByteWidth = size;							//버퍼 크기
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//버퍼 용도 : "정점 버퍼" 용로 설정 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;								//버퍼에 들어갈 데이터 설정 : "정점들"..

	//정점 버퍼 생성.
	HRESULT hr = DeviceRef.CreateBuffer(&bd, &rd, &pVB);
	if (FAILED(hr))
	{
		//에러 검증 및 예외처리..
#ifdef _DEBUG
		MessageBox(NULL, L"Failed, CreateVB on Model", L"Error_Model", MB_OK);
#endif
		DEBUG_CONSOLE(L"임시 에러: 무언가 어떤 Node의 D3D11 VB 생성에 실패!");
		return hr;
	}

	// 외부로 전달
	*dest = pVB;

	return hr;
}

HRESULT hos::cg::D3D11VariableManager::CreateIB(void* pBuff, UINT size, ID3D11Buffer** dest)
{
	ID3D11Buffer* pIB = nullptr;

	// 인덱스 버퍼 정보 구성.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//버퍼 사용방식
	bd.ByteWidth = size;							//버퍼 크기
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;			//버퍼 용도 : "색인 버퍼" 용로 설정 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;								//버퍼에 들어갈 데이터 설정 : "색인 정보"..

	//색인 버퍼 생성.
	HRESULT hr = DeviceRef.CreateBuffer(&bd, &rd, &pIB);
	if (FAILED(hr))
	{
		//에러 검증 및 예외처리..
		//...
#ifdef _DEBUG
		MessageBox(NULL, L"Failed, CreateIB on Model", L"Error_Model", MB_OK);
#endif
		DEBUG_CONSOLE(L"임시 에러: 무언가 어떤 Node의 D3D11 IB 생성에 실패!");
		return hr;
	}

	// 외부로 전달
	*dest = pIB;

	return hr;
}

ID3D11ShaderResourceView* hos::cg::D3D11VariableManager::GetTexture(std::wstring textureName)
{
	try
	{
		return TextureList.at(textureName);
	}
	catch (std::out_of_range e)
	{
		return nullptr;
	}
}

bool hos::cg::D3D11VariableManager::CreateD3D11Texture(std::wstring textureName, std::wstring fileRoot, bool* bAlpha)
{
	HRESULT hr = S_OK;
	ID3D11ShaderResourceView* pTex = nullptr;

	if (TextureList.count(textureName))
	{
		DEBUG_CONSOLET(L"이미 해당 Key로 ID3D11ShaderResourceView(Texture)가 생성되어 있습니다.", textureName);
		hr = S_FALSE;
		return false;
	}

	//std::wstring _finalRoot = fileRoot + textureName;

	hr = LoadTexture(fileRoot.c_str(), &pTex, bAlpha);

	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"D3D11 Texture 로드에 실패했습니다.", fileRoot);
		return false;
	}

	// 텍스쳐 리스트에 텍스쳐셋 전달
	TextureList.insert({ textureName, pTex });

	DEBUG_CONSOLET(L"D3D11ShaderResourceView 생성에 성공했습니다.", textureName);

	return true;
}

bool hos::cg::D3D11VariableManager::ReleaseD3D11Texture(std::wstring textureName)
{
	if (TextureList.count(textureName))
	{
		TextureList[textureName]->Release();
		TextureList.erase(textureName);

		DEBUG_CONSOLET(L"D3D11ShaderResourceView의 Release에 성공했습니다.", textureName);

		return true;
	}

	else
	{
		DEBUG_CONSOLET(L"해당 D3D11Texture Resource가 존재하지 않아 Release할수 없습니다.", textureName);

		return false;
	}

	return true;
}

bool hos::cg::D3D11VariableManager::CreateSpriteFont(std::wstring_view fontName, uint8_t const* data, size_t dataSize)
{
	DirectX::SpriteFont* _SpriteFont = new DirectX::SpriteFont(&(this->DeviceRef), data, dataSize);

	if (_SpriteFont)
	{
		_SpriteFont->SetDefaultCharacter('*');
		SpriteFontList.insert({ fontName.data() , _SpriteFont });

		return true;
	}
	SAFE_DELETE(_SpriteFont);
	return false;
}

DirectX::SpriteFont* hos::cg::D3D11VariableManager::GetFont(std::wstring_view fontName)
{
	std::wstring _fontName = fontName.data();
	DirectX::SpriteFont* _font = nullptr;

	if (_fontName == L"Arial")
	{
		_font = DefaultFont;
	}

	else if (this->SpriteFontList.count(_fontName) != 0)
	{
		_font = SpriteFontList.at(_fontName);
	}

	else
	{
		_font = DefaultFont;
		DEBUG_CONSOLET(_fontName.data(), L"해당 폰트가 Resource에 존재하지 않아 Arial 폰트를 사용합니다.");
	}

	return _font;
}

bool hos::cg::D3D11VariableManager::LoadSkyBox(std::wstring_view skyBoxName, std::wstring_view filePath)
{
	HRESULT hr = S_OK;
	ID3D11ShaderResourceView* pTex = nullptr;

	std::wstring _keyName = skyBoxName.data();
	std::wstring _filePath = filePath.data();

	if (SkyBoxList.count(_keyName))
	{
		DEBUG_CONSOLET(L"이미 해당 Key로 ID3D11ShaderResourceView(SkyBox)가 생성되어 있습니다.", _keyName);
		hr = S_FALSE;
		return false;
	}

	//std::wstring _finalRoot = fileRoot + textureName;

	hr = LoadCubeTexture(_filePath.c_str(), &pTex);

	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"D3D11 Cube Textrue(SkyBox) 로드에 실패했습니다.", _filePath);
		return false;
	}

	// SkyBox 리스트에 텍스쳐셋 전달
	SkyBoxList.insert({ _keyName, pTex });

	DEBUG_CONSOLET(L"D3D11ShaderResourceView(SkyBox) 생성에 성공했습니다.", _keyName);

	return true;
}

ID3D11ShaderResourceView* hos::cg::D3D11VariableManager::GetSkyBox(std::wstring_view skyboxName)
{
	try
	{
		return SkyBoxList.at(skyboxName.data());
	}
	catch (std::out_of_range e)
	{
		return nullptr;
	}
}

HRESULT hos::cg::D3D11VariableManager::CreateUIBuffer()
{
	HRESULT hr = S_OK;

	hos::cg::VertexDataType::UI_VTX	 vtxData2D[4] =
	{
		Vector3(0, 0, 0), Vector2(0, 0),
		Vector3(1, 0, 0), Vector2(1, 0),
		Vector3(0, 1, 0), Vector2(0, 1),
		Vector3(1, 1, 0), Vector2(1, 1),
	};

	hos::cg::VertexDataType::UI_VTX	 vtxData3D[9][4] =
	{
		// 0번 => LeftBottom
		{
		Vector3(0.0f, 1.0f, 0), Vector2(0, 0),
		Vector3(1.0f, 1.0f, 0), Vector2(1, 0),
		Vector3(0.0f, 0.0f, 0), Vector2(0, 1),
		Vector3(1.0f, 0.0f, 0), Vector2(1, 1)
		},

		// 1번 => Bottom
		{
		Vector3(-0.5f, 1.0f, 0), Vector2(0, 0),
		Vector3(0.5f, 1.0f, 0), Vector2(1, 0),
		Vector3(-0.5f, 0.0f, 0), Vector2(0, 1),
		Vector3(0.5f,  0.0f, 0), Vector2(1, 1)
		},

		// 2번 => RightBottom
		{
		Vector3(-1.0f, 1.0f, 0), Vector2(0, 0),
		Vector3(0.0f, 1.0f, 0), Vector2(1, 0),
		Vector3(-1.0f, 0.0f, 0), Vector2(0, 1),
		Vector3(0.0f,  0.0f, 0), Vector2(1, 1)
		},

		// 3번 => Left
		{
		Vector3(0.0f, 0.5f, 0), Vector2(0, 0),
		Vector3(1.0f, 0.5f, 0), Vector2(1, 0),
		Vector3(0.0f, -0.5f, 0), Vector2(0, 1),
		Vector3(1.0f, -0.5f, 0), Vector2(1, 1)
		},

		// 4번=> Center
		{
		Vector3(-0.5f, 0.5f, 0), Vector2(0, 0),
		Vector3(0.5f, 0.5f, 0), Vector2(1, 0),
		Vector3(-0.5f, -0.5f, 0), Vector2(0, 1),
		Vector3(0.5f, -0.5f, 0), Vector2(1, 1)
		},

		// 5번=> Right
		{
		Vector3(-1.0f, 0.5f, 0), Vector2(0, 0),
		Vector3(0.0f, 0.5f, 0), Vector2(1, 0),
		Vector3(-1.0f, -0.5f, 0), Vector2(0, 1),
		Vector3(0.0f, -0.5f, 0), Vector2(1, 1)
		},

		// 6번 => LeftTop
		{
		Vector3(0.0f, 0.0f, 0), Vector2(0, 0),
		Vector3(1.0f, 0.0f, 0), Vector2(1, 0),
		Vector3(0.0f, -1.0f, 0), Vector2(0, 1),
		Vector3(1.0f, -1.0f, 0), Vector2(1, 1)
		},

		// 7번 => Top
		{
		Vector3(-0.5f, 0.0f, 0), Vector2(0, 0),
		Vector3(0.5f, 0.0f, 0), Vector2(1, 0),
		Vector3(-0.5f, -1.0f, 0), Vector2(0, 1),
		Vector3(0.5f, -1.0f, 0), Vector2(1, 1)
		},

		// 8번 => RightTop
		{
		Vector3(-1.0f, 0.0f, 0), Vector2(0, 0),
		Vector3(0.0f, 0.0f, 0), Vector2(1, 0),
		Vector3(-1.0f, -1.0f, 0), Vector2(0, 1),
		Vector3(0.0f, -1.0f, 0), Vector2(1, 1)
		},
	};

	unsigned int vtxSize = 4 * sizeof(hos::cg::VertexDataType::UI_VTX);

	// 정점 버퍼 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(vtxData3D);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = vtxData3D[0];

	for (int i = 0; i < 9; i++)
	{
		rd.pSysMem = vtxData3D[i];
		hr = DeviceRef.CreateBuffer(&bd, &rd, &VtxBuffer3DUI[i]);
	}

	// 2D 정점 버퍼 생성
	bd.ByteWidth = sizeof(vtxData2D);
	rd.pSysMem = vtxData2D;

	hr = DeviceRef.CreateBuffer(&bd, &rd, &VtxBufferUI);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"2D UI 정점버퍼를 만들 수 없습니다.", L"Error_UI", MB_OK);
#endif
		return hr;
	}

	return S_OK;
}

HRESULT hos::cg::D3D11VariableManager::CreateSkyBoxBuffer()
{
	HRESULT hr = S_OK;

	// 사각형 판 메쉬
	Vector3 Quad[4] = {
		{-1.0f,  1.0f,  1.0f},
		{ 1.0f,  1.0f,  1.0f},
		{-1.0f, -1.0f,  1.0f},
		{ 1.0f, -1.0f,  1.0f}
	};

	// 정점 버퍼 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Quad);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = Quad;

	hr = DeviceRef.CreateBuffer(&bd, &rd, &(this->SkyBoxBuffer));
	if (FAILED(hr))
	{
		DEBUG_CONSOLE(L"SkyBox의 정점버퍼를 만들 수 없습니다.");
		return hr;
	}

	//DEBUG_CONSOLE(L"SkyBox의 정점버퍼 생성에 성공했습니다.");
	return hr;
}

HRESULT hos::cg::D3D11VariableManager::LoadTexture(const WCHAR* fileRoot, ID3D11ShaderResourceView** dest, bool* bAlpha)
{
	HRESULT hr = S_OK;
	bool bCheckAlpha = true;

	ID3D11ShaderResourceView* pRV = nullptr;

	DirectX::TexMetadata meta;
	DirectX::ScratchImage* image = new DirectX::ScratchImage();

	// TGA 시도
	hr = DirectX::LoadFromTGAFile(fileRoot, &meta, *image);

	// TGA 실패 => DDS 시도
	if (FAILED(hr))
	{
		// DDS 시도
		hr = DirectX::LoadFromDDSFile(fileRoot, DirectX::DDS_FLAGS_NONE, &meta, *image);

		// DDS 실패 => WIC 시도
		if (FAILED(hr))
		{
			hr = DirectX::LoadFromWICFile(fileRoot, DirectX::WIC_FLAGS_NONE, &meta, *image);

			// WIC 성공 => AlphaTexture일 가능성이 없다.
			if (SUCCEEDED(hr))
			{
				bCheckAlpha = false;
			}

			// WIC 실패 => return
			else
			{
				// ScratchImage 삭제
				SAFE_DELETE(image);
				DEBUG_CONSOLET(fileRoot, L"Texture Load에 실패했습니다.");
			}
		}
	}

	// SRV 생성 및 전달
	if (SUCCEEDED(hr))
	{
		// AlphaTexture의 가능성이 있을 경우 AlphaTextrue 여부(bool Value)를 포인터에 반환합니다.
		if (bCheckAlpha && bAlpha != nullptr)
		{
			size_t pixelSize = image->GetPixelsSize();
			uint8_t* pixelAdress = image->GetPixels();
			uint8_t alphaValue;

			// 첫 픽셀의 알파값만 확인
			alphaValue = pixelAdress[3];
			if (alphaValue != 255)
			{
				*bAlpha = true;
			}
		}

		hr = DirectX::CreateShaderResourceView(&(this->DeviceRef), image->GetImages(), image->GetImageCount(), meta, &pRV);
		*dest = pRV;
	}

	// ScratchImage 삭제
	SAFE_DELETE(image);

	return hr;
}

HRESULT hos::cg::D3D11VariableManager::LoadCubeTexture(const WCHAR* fileRoot, ID3D11ShaderResourceView** dest)
{
	HRESULT hr = S_OK;
	ID3D11ShaderResourceView* pTexRV = nullptr;

	DirectX::TexMetadata meta;
	DirectX::ScratchImage* image = new DirectX::ScratchImage();

	// DDS 시도
	hr = DirectX::LoadFromDDSFile(fileRoot, DirectX::DDS_FLAGS_NONE, &meta, *image);

	//hr = DirectX::CreateDDSTextureFromFileEx(&(this->DeviceRef), &(this->ContextRef), fileRoot, 0,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
	//	0,
	//	D3D11_RESOURCE_MISC_TEXTURECUBE,		// 큐브맵 리소스 구성 옵션
	//	false, nullptr, &pTexRV);

	hr = DirectX::CreateShaderResourceViewEx(&(this->DeviceRef),
		image->GetImages(),
		image->GetImageCount(),
		meta,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE,		// 큐브맵 리소스 구성 옵션
		false,
		&pTexRV);

	if (FAILED(hr))
	{
		return hr;
	}

	// 외부로 리턴
	*dest = pTexRV;
	SAFE_DELETE(image);

	return hr;
}
