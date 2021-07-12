// �ʼ� ���
#include "GoodFunc.h"
#include "DebugConsole.h"

#include "DXTK/DDSTextureLoader.h"
#include "DXTK/WICTextureLoader.h"

#include "DXTex/DirectXTex.h"

// ���� ���
#include "D3D11ConstBuffer.h"
#include "VertexDataType.h"

// �ڱ� ���
#include "D3D11VariableManager.h"

// ���ӽ����̽�
using namespace DirectX::SimpleMath;


hos::cg::D3D11VariableManager::D3D11VariableManager(ID3D11Device& device, ID3D11DeviceContext& context)
	:DeviceRef(device), ContextRef(context), VtxBufferUI(nullptr), VtxBuffer3DUI{ 0, }, SpriteFontBatch(nullptr), DefaultFont(nullptr), SkyBoxBuffer(nullptr)
	/// �� ����Ʈ�� ũ��� D3D11Data.h(��Ī)�� D3D11ShaderDesc.h�� �����Ѵ�.
{
	// �������, ���̴� ����Ʈ ���� �Ҵ� �� �ʱ�ȭ
	ConstBufferList.resize((size_t)ConstBufferType::CB_MAX);
	ShaderList.resize((size_t)SHADER_LIST::SHADER_MAX);

	// MeshBuffer, Texture ����Ʈ ���� �Ҵ�
	MeshBufferSetList.reserve(100);
	TextureList.reserve(100);

	// UI, SkyBox �ڿ� ����
	CreateUIBuffer();
	CreateSkyBoxBuffer();

	// Font �ڿ� ����
	this->SpriteFontBatch = new DirectX::SpriteBatch(&ContextRef);

	// �⺻ ��Ʈ ����
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
				DEBUG_CONSOLE(L"Default SpriteFont�� Arial.spriteFont ������ �������� �ʽ��ϴ�.");
				assert(0);
			}
		}
	}

	this->SpriteFontList.reserve(5);

	// SkyBox �ڿ� ����
	this->SkyBoxList.reserve(5);
}

hos::cg::D3D11VariableManager::~D3D11VariableManager()
{
	// SKyBox �ڿ� ����
	for (auto [name, skybox] : SkyBoxList)
	{
		SAFE_RELEASE(skybox);
	}
	SkyBoxList.clear();

	// FontList ����
	for (auto [name, font] : SpriteFontList)
	{
		SAFE_DELETE(font);
	}
	SpriteFontList.clear();

	// �⺻ Font �ڿ� ����
	SAFE_DELETE(this->DefaultFont);
	SAFE_DELETE(this->SpriteFontBatch);

	// UI �� SkyBox �ڿ� ����
	SAFE_RELEASE(VtxBufferUI);
	SAFE_RELEASE(SkyBoxBuffer);

	for (int i = 0; i < 9; i++)
	{
		SAFE_RELEASE(VtxBuffer3DUI[i]);
	}

	// ������� ����Ʈ ����
	SAFE_DELETE_VECTOR(ConstBufferList);

	// ���̴� ����Ʈ ����
	SAFE_DELETE_VECTOR(ShaderList);

	// ���÷� ����
	for (size_t i = 0; i < this->SampleStateList.size(); i++)
	{
		SAFE_RELEASE(this->SampleStateList[i]);
	}

	// Mesh Buffer ����Ʈ ����
	for (auto elem : MeshBufferSetList)
	{
		SAFE_RELEASE(MeshBufferSetList[elem.first].VertexBuffer);
		SAFE_RELEASE(MeshBufferSetList[elem.first].IndexBuffer);
	}
	MeshBufferSetList.clear();
	std::unordered_map<std::wstring, MeshBufferSet>().swap(MeshBufferSetList);

	// �ؽ��� ����Ʈ ����
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

	// ���÷� ����Ʈ���� �̸� ������ �������� �����´�.
	sd = SamplerOptionList[samplerIndex];

	hr = DeviceRef.CreateSamplerState(&sd, &dest);

	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"[����] Create Sampler", L"Error", MB_OK | MB_ICONERROR);
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
		// ������� ��ü�� map/unmap
		ConstBufferList[(size_t)index]->Update();

		// ������۸� VS/PS�� ����
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
		DEBUG_CONSOLET(L"�̹� �ش� Key�� Mesh�� ID3D11Buffer���� �����Ǿ� �ֽ��ϴ�.", meshName);
		hr = S_FALSE;
	}

	ID3D11Buffer* VB = nullptr;
	ID3D11Buffer* IB = nullptr;
	MeshBufferSet BufferSet;

	// ���� ���
	UINT sizeVB = vtxCount * sizeof(VertexDataType::FULL_VTX);
	UINT sizeIB = indexCount * sizeof(unsigned int);

	// ���� ���� �����
	if (FAILED(hr = CreateVB(vtxData, sizeVB, &VB)))
	{
		return false;
	}

	// �ε��� ���� �����
	if (FAILED(hr = CreateIB(indexData, sizeIB, &IB)))
	{
		return false;
	}

	// ����Ʈ�� ����ü ����
	BufferSet.VertexBuffer = VB;
	BufferSet.IndexBuffer = IB;
	BufferSet.IndexCount = indexCount;
	MeshBufferSetList.insert({ meshName, BufferSet });

	DEBUG_CONSOLET(L"D3D11 MeshBufferSet ������ �����߽��ϴ�.", meshName);

	return true;
}

bool hos::cg::D3D11VariableManager::ReleaseMeshBuffer(std::wstring meshName)
{
	if (MeshBufferSetList.count(meshName))
	{
		MeshBufferSetList[meshName].VertexBuffer->Release();
		MeshBufferSetList[meshName].IndexBuffer->Release();
		MeshBufferSetList.erase(meshName);

		DEBUG_CONSOLET(L"D3D11MeshBuffer�� Release�� �����߽��ϴ�.", meshName);

		return true;
	}

	else
	{
		DEBUG_CONSOLET(L"�ش� D3D11MeshBuffer Resource�� �������� �ʾ� Release�Ҽ� �����ϴ�.", meshName);

		return false;
	}

	return true;
}

HRESULT hos::cg::D3D11VariableManager::CreateVB(void* pBuff, UINT size, ID3D11Buffer** dest)
{
	ID3D11Buffer* pVB = nullptr;

	//���� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//���� �����
	bd.ByteWidth = size;							//���� ũ��
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;		//���� �뵵 : "���� ����" ��� ���� 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;								//���ۿ� �� ������ ���� : "������"..

	//���� ���� ����.
	HRESULT hr = DeviceRef.CreateBuffer(&bd, &rd, &pVB);
	if (FAILED(hr))
	{
		//���� ���� �� ����ó��..
#ifdef _DEBUG
		MessageBox(NULL, L"Failed, CreateVB on Model", L"Error_Model", MB_OK);
#endif
		DEBUG_CONSOLE(L"�ӽ� ����: ���� � Node�� D3D11 VB ������ ����!");
		return hr;
	}

	// �ܺη� ����
	*dest = pVB;

	return hr;
}

HRESULT hos::cg::D3D11VariableManager::CreateIB(void* pBuff, UINT size, ID3D11Buffer** dest)
{
	ID3D11Buffer* pIB = nullptr;

	// �ε��� ���� ���� ����.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;					//���� �����
	bd.ByteWidth = size;							//���� ũ��
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;			//���� �뵵 : "���� ����" ��� ���� 
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.pSysMem = pBuff;								//���ۿ� �� ������ ���� : "���� ����"..

	//���� ���� ����.
	HRESULT hr = DeviceRef.CreateBuffer(&bd, &rd, &pIB);
	if (FAILED(hr))
	{
		//���� ���� �� ����ó��..
		//...
#ifdef _DEBUG
		MessageBox(NULL, L"Failed, CreateIB on Model", L"Error_Model", MB_OK);
#endif
		DEBUG_CONSOLE(L"�ӽ� ����: ���� � Node�� D3D11 IB ������ ����!");
		return hr;
	}

	// �ܺη� ����
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
		DEBUG_CONSOLET(L"�̹� �ش� Key�� ID3D11ShaderResourceView(Texture)�� �����Ǿ� �ֽ��ϴ�.", textureName);
		hr = S_FALSE;
		return false;
	}

	//std::wstring _finalRoot = fileRoot + textureName;

	hr = LoadTexture(fileRoot.c_str(), &pTex, bAlpha);

	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"D3D11 Texture �ε忡 �����߽��ϴ�.", fileRoot);
		return false;
	}

	// �ؽ��� ����Ʈ�� �ؽ��ļ� ����
	TextureList.insert({ textureName, pTex });

	DEBUG_CONSOLET(L"D3D11ShaderResourceView ������ �����߽��ϴ�.", textureName);

	return true;
}

bool hos::cg::D3D11VariableManager::ReleaseD3D11Texture(std::wstring textureName)
{
	if (TextureList.count(textureName))
	{
		TextureList[textureName]->Release();
		TextureList.erase(textureName);

		DEBUG_CONSOLET(L"D3D11ShaderResourceView�� Release�� �����߽��ϴ�.", textureName);

		return true;
	}

	else
	{
		DEBUG_CONSOLET(L"�ش� D3D11Texture Resource�� �������� �ʾ� Release�Ҽ� �����ϴ�.", textureName);

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
		DEBUG_CONSOLET(_fontName.data(), L"�ش� ��Ʈ�� Resource�� �������� �ʾ� Arial ��Ʈ�� ����մϴ�.");
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
		DEBUG_CONSOLET(L"�̹� �ش� Key�� ID3D11ShaderResourceView(SkyBox)�� �����Ǿ� �ֽ��ϴ�.", _keyName);
		hr = S_FALSE;
		return false;
	}

	//std::wstring _finalRoot = fileRoot + textureName;

	hr = LoadCubeTexture(_filePath.c_str(), &pTex);

	if (FAILED(hr))
	{
		DEBUG_CONSOLET(L"D3D11 Cube Textrue(SkyBox) �ε忡 �����߽��ϴ�.", _filePath);
		return false;
	}

	// SkyBox ����Ʈ�� �ؽ��ļ� ����
	SkyBoxList.insert({ _keyName, pTex });

	DEBUG_CONSOLET(L"D3D11ShaderResourceView(SkyBox) ������ �����߽��ϴ�.", _keyName);

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
		// 0�� => LeftBottom
		{
		Vector3(0.0f, 1.0f, 0), Vector2(0, 0),
		Vector3(1.0f, 1.0f, 0), Vector2(1, 0),
		Vector3(0.0f, 0.0f, 0), Vector2(0, 1),
		Vector3(1.0f, 0.0f, 0), Vector2(1, 1)
		},

		// 1�� => Bottom
		{
		Vector3(-0.5f, 1.0f, 0), Vector2(0, 0),
		Vector3(0.5f, 1.0f, 0), Vector2(1, 0),
		Vector3(-0.5f, 0.0f, 0), Vector2(0, 1),
		Vector3(0.5f,  0.0f, 0), Vector2(1, 1)
		},

		// 2�� => RightBottom
		{
		Vector3(-1.0f, 1.0f, 0), Vector2(0, 0),
		Vector3(0.0f, 1.0f, 0), Vector2(1, 0),
		Vector3(-1.0f, 0.0f, 0), Vector2(0, 1),
		Vector3(0.0f,  0.0f, 0), Vector2(1, 1)
		},

		// 3�� => Left
		{
		Vector3(0.0f, 0.5f, 0), Vector2(0, 0),
		Vector3(1.0f, 0.5f, 0), Vector2(1, 0),
		Vector3(0.0f, -0.5f, 0), Vector2(0, 1),
		Vector3(1.0f, -0.5f, 0), Vector2(1, 1)
		},

		// 4��=> Center
		{
		Vector3(-0.5f, 0.5f, 0), Vector2(0, 0),
		Vector3(0.5f, 0.5f, 0), Vector2(1, 0),
		Vector3(-0.5f, -0.5f, 0), Vector2(0, 1),
		Vector3(0.5f, -0.5f, 0), Vector2(1, 1)
		},

		// 5��=> Right
		{
		Vector3(-1.0f, 0.5f, 0), Vector2(0, 0),
		Vector3(0.0f, 0.5f, 0), Vector2(1, 0),
		Vector3(-1.0f, -0.5f, 0), Vector2(0, 1),
		Vector3(0.0f, -0.5f, 0), Vector2(1, 1)
		},

		// 6�� => LeftTop
		{
		Vector3(0.0f, 0.0f, 0), Vector2(0, 0),
		Vector3(1.0f, 0.0f, 0), Vector2(1, 0),
		Vector3(0.0f, -1.0f, 0), Vector2(0, 1),
		Vector3(1.0f, -1.0f, 0), Vector2(1, 1)
		},

		// 7�� => Top
		{
		Vector3(-0.5f, 0.0f, 0), Vector2(0, 0),
		Vector3(0.5f, 0.0f, 0), Vector2(1, 0),
		Vector3(-0.5f, -1.0f, 0), Vector2(0, 1),
		Vector3(0.5f, -1.0f, 0), Vector2(1, 1)
		},

		// 8�� => RightTop
		{
		Vector3(-1.0f, 0.0f, 0), Vector2(0, 0),
		Vector3(0.0f, 0.0f, 0), Vector2(1, 0),
		Vector3(-1.0f, -1.0f, 0), Vector2(0, 1),
		Vector3(0.0f, -1.0f, 0), Vector2(1, 1)
		},
	};

	unsigned int vtxSize = 4 * sizeof(hos::cg::VertexDataType::UI_VTX);

	// ���� ���� ����
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

	// 2D ���� ���� ����
	bd.ByteWidth = sizeof(vtxData2D);
	rd.pSysMem = vtxData2D;

	hr = DeviceRef.CreateBuffer(&bd, &rd, &VtxBufferUI);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		MessageBox(NULL, L"2D UI �������۸� ���� �� �����ϴ�.", L"Error_UI", MB_OK);
#endif
		return hr;
	}

	return S_OK;
}

HRESULT hos::cg::D3D11VariableManager::CreateSkyBoxBuffer()
{
	HRESULT hr = S_OK;

	// �簢�� �� �޽�
	Vector3 Quad[4] = {
		{-1.0f,  1.0f,  1.0f},
		{ 1.0f,  1.0f,  1.0f},
		{-1.0f, -1.0f,  1.0f},
		{ 1.0f, -1.0f,  1.0f}
	};

	// ���� ���� ����
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
		DEBUG_CONSOLE(L"SkyBox�� �������۸� ���� �� �����ϴ�.");
		return hr;
	}

	//DEBUG_CONSOLE(L"SkyBox�� �������� ������ �����߽��ϴ�.");
	return hr;
}

HRESULT hos::cg::D3D11VariableManager::LoadTexture(const WCHAR* fileRoot, ID3D11ShaderResourceView** dest, bool* bAlpha)
{
	HRESULT hr = S_OK;
	bool bCheckAlpha = true;

	ID3D11ShaderResourceView* pRV = nullptr;

	DirectX::TexMetadata meta;
	DirectX::ScratchImage* image = new DirectX::ScratchImage();

	// TGA �õ�
	hr = DirectX::LoadFromTGAFile(fileRoot, &meta, *image);

	// TGA ���� => DDS �õ�
	if (FAILED(hr))
	{
		// DDS �õ�
		hr = DirectX::LoadFromDDSFile(fileRoot, DirectX::DDS_FLAGS_NONE, &meta, *image);

		// DDS ���� => WIC �õ�
		if (FAILED(hr))
		{
			hr = DirectX::LoadFromWICFile(fileRoot, DirectX::WIC_FLAGS_NONE, &meta, *image);

			// WIC ���� => AlphaTexture�� ���ɼ��� ����.
			if (SUCCEEDED(hr))
			{
				bCheckAlpha = false;
			}

			// WIC ���� => return
			else
			{
				// ScratchImage ����
				SAFE_DELETE(image);
				DEBUG_CONSOLET(fileRoot, L"Texture Load�� �����߽��ϴ�.");
			}
		}
	}

	// SRV ���� �� ����
	if (SUCCEEDED(hr))
	{
		// AlphaTexture�� ���ɼ��� ���� ��� AlphaTextrue ����(bool Value)�� �����Ϳ� ��ȯ�մϴ�.
		if (bCheckAlpha && bAlpha != nullptr)
		{
			size_t pixelSize = image->GetPixelsSize();
			uint8_t* pixelAdress = image->GetPixels();
			uint8_t alphaValue;

			// ù �ȼ��� ���İ��� Ȯ��
			alphaValue = pixelAdress[3];
			if (alphaValue != 255)
			{
				*bAlpha = true;
			}
		}

		hr = DirectX::CreateShaderResourceView(&(this->DeviceRef), image->GetImages(), image->GetImageCount(), meta, &pRV);
		*dest = pRV;
	}

	// ScratchImage ����
	SAFE_DELETE(image);

	return hr;
}

HRESULT hos::cg::D3D11VariableManager::LoadCubeTexture(const WCHAR* fileRoot, ID3D11ShaderResourceView** dest)
{
	HRESULT hr = S_OK;
	ID3D11ShaderResourceView* pTexRV = nullptr;

	DirectX::TexMetadata meta;
	DirectX::ScratchImage* image = new DirectX::ScratchImage();

	// DDS �õ�
	hr = DirectX::LoadFromDDSFile(fileRoot, DirectX::DDS_FLAGS_NONE, &meta, *image);

	//hr = DirectX::CreateDDSTextureFromFileEx(&(this->DeviceRef), &(this->ContextRef), fileRoot, 0,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
	//	0,
	//	D3D11_RESOURCE_MISC_TEXTURECUBE,		// ť��� ���ҽ� ���� �ɼ�
	//	false, nullptr, &pTexRV);

	hr = DirectX::CreateShaderResourceViewEx(&(this->DeviceRef),
		image->GetImages(),
		image->GetImageCount(),
		meta,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE,		// ť��� ���ҽ� ���� �ɼ�
		false,
		&pTexRV);

	if (FAILED(hr))
	{
		return hr;
	}

	// �ܺη� ����
	*dest = pTexRV;
	SAFE_DELETE(image);

	return hr;
}
