// <Concept>
// Source Name		: D3D11VariableMananger.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2020.11.10

// <Explanation>
// D3D11 ȯ�濡�� �������, ���̴� ���� ������ ȯ�� ������ ����ϱ� ���� �Ŵ��� Ŭ����
// D3D11Renderer���� Ȱ����.

// Working Log
// 2021.01.10.Hosik	: ����
// 2021.01.11.Hosik	: ������� �߰�
// 2021.02.04.Hosik : D3D11����̽��ν��Ͻ��� ���۷��� ���� �ʵ��� ����.
// 2021.03.16.Hosik : �׷��ȿ��� �� ���ҽ������͸� �����Ͽ� Ŭ���� ���� �缳��.

#pragma once

#include <d3d11_4.h>
#include <vector>
#include <unordered_map>

#include "DXTK/SpriteBatch.h"
#include "DXTK/SpriteFont.h"

#include "D3D11Data.h"

#include "D3D11Shader.h"
#include "D3D11ShaderDesc.h"

namespace hos::cg
{
	class D3D11ConstBuffer;

	class D3D11VariableManager
	{
	public:
		struct MeshBufferSet
		{
			ID3D11Buffer* VertexBuffer = nullptr;
			ID3D11Buffer* IndexBuffer = nullptr;
			unsigned int IndexCount = 0;
		};

	public:
		D3D11VariableManager() = delete;
		D3D11VariableManager(ID3D11Device& device, ID3D11DeviceContext& context);
		~D3D11VariableManager();

	private:
		// ����̽� Ref
		ID3D11Device& DeviceRef;
		ID3D11DeviceContext& ContextRef;

		// �������, ���̴� ����
		std::vector<D3D11ConstBuffer*> ConstBufferList;	// �⺻��Ŀ� �������
		std::vector<D3D11Shader*> ShaderList;

		// ���÷�
		std::vector<ID3D11SamplerState*> SampleStateList;

		// VB, IB, Texture List
		std::unordered_map<std::wstring, MeshBufferSet> MeshBufferSetList;
		std::unordered_map <std::wstring, ID3D11ShaderResourceView*> TextureList;	// �ؽ��� ����Ʈ

		// UI�� �ڿ�
		ID3D11Buffer* VtxBufferUI;
		ID3D11Buffer* VtxBuffer3DUI[9];

		// Font�� �ڿ�
		DirectX::SpriteBatch* SpriteFontBatch;
		DirectX::SpriteFont* DefaultFont;
		std::unordered_map<std::wstring, DirectX::SpriteFont*> SpriteFontList;

		// SkyMap
		ID3D11Buffer* SkyBoxBuffer;
		std::unordered_map<std::wstring, ID3D11ShaderResourceView*> SkyBoxList;

		// PreRender��
		ID3D11ShaderResourceView* nullSRV[5];


	public:
		//void BindDevice(ID3D11Device* device, ID3D11DeviceContext* context) { DeviceRef = device;	ContextRef = context; };

		void CreateConstBuffer(ID3D11Device* device, ID3D11DeviceContext* context, LPVOID data, UINT size, ConstBufferType type, const WCHAR* bufferName);
		void CreateShader(WCHAR* fileName, int shaderIndex, const WCHAR* shaderName, std::vector<ID3D11SamplerState*>& samplerList);
		void CreateShader(WCHAR* vsFileName, WCHAR* psFileName, int shaderIndex, const WCHAR* shaderName, std::vector<ID3D11SamplerState*>& samplerList);
		HRESULT CreateSamplerIndex(int samplerIndex);

		std::vector<ID3D11SamplerState*>& GetSamplerList() { return this->SampleStateList; };

		void UpdateConstBuffer(unsigned int index);


		void AllClearMeshVariableList();
		void AllClearTextureVariableList();


		D3D11Shader* GetShader(SHADER_LIST shaderIndex);
		void DrawByIndex(SHADER_LIST shaderType, unsigned int indexCount);
		void DrawByVertex(SHADER_LIST shaderType, unsigned int vertexCount);

		// VB, IB
		MeshBufferSet& GetMeshBuffer(std::wstring meshName) { return MeshBufferSetList.at(meshName); };
		ID3D11Buffer* GetUIBuffer() { return VtxBufferUI; };
		ID3D11Buffer* Get3DUIBuffer(unsigned int index) { return VtxBuffer3DUI[index - 1]; };

		bool CreateMeshBuffer(std::wstring meshName, void* vtxData, unsigned int vtxCount, void* indexData, unsigned int indexCount);
		bool ReleaseMeshBuffer(std::wstring meshName);
		HRESULT CreateVB(void* pBuff, UINT size, ID3D11Buffer** dest);
		HRESULT CreateIB(void* pBuff, UINT size, ID3D11Buffer** dest);


		// Texture
		ID3D11ShaderResourceView* GetTexture(std::wstring textureName);

		bool CreateD3D11Texture(std::wstring textureName, std::wstring fileRoot, bool* bAlpha = nullptr);
		bool ReleaseD3D11Texture(std::wstring textureName);

		// Font
		void BeginFont(ID3D11BlendState* state) { this->SpriteFontBatch->Begin(DirectX::SpriteSortMode::SpriteSortMode_Deferred, state); };
		void EndFont() { this->SpriteFontBatch->End(); };

		bool CreateSpriteFont(std::wstring_view fontName, uint8_t const* data, size_t dataSize);

		DirectX::SpriteFont* GetFont(std::wstring_view fontName);
		DirectX::SpriteBatch* GetBatch() { return this->SpriteFontBatch; };


		// SkyBox
		bool LoadSkyBox(std::wstring_view skyBoxName, std::wstring_view filePath);
		ID3D11Buffer* GetSkyBoxBuffer() { return SkyBoxBuffer; };
		ID3D11ShaderResourceView* GetSkyBox(std::wstring_view skyboxName);

	private:
		HRESULT CreateUIBuffer();
		HRESULT CreateSkyBoxBuffer();

		HRESULT LoadTexture(const WCHAR* fileRoot, ID3D11ShaderResourceView** destm, bool* bAlpha = nullptr);
		HRESULT LoadCubeTexture(const WCHAR* fileRoot, ID3D11ShaderResourceView** dest);

	};
}