// <Concept>
// Source Name		: D3D11Shader.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.01.10

// <Explanation>
// D3D11���� ����ϴ� ���̴� Ŭ����

// Working Log
// 2021.01.10.Hosik	: ����
// 2021.02.04.Hosik : Device ���۷��� ����, ynErrorW ��Ȱ
// 2021.05.11.Hosik : �������� Sampler�� ����� �� �ֵ��� �����ϸ鼭 Sampler�� Index�� ������ �ֵ��� ����. 

#pragma once

#include <d3d11_4.h>
#include <wrl.h>

#include <vector>

namespace hos::cg
{
	class D3D11Shader
	{
		// ���� ��� �ν��Ͻ�
	private:
		// Context Ref
		ID3D11Device& DeviceRef;
		ID3D11DeviceContext& ContextRef;
		std::vector<ID3D11SamplerState*>& samplerListRef;

		// ���̴� ���
		WCHAR FileName[256];
		WCHAR ShaderName[256];

		// ���̴� ��ü
		Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;
		Microsoft::WRL::ComPtr<ID3DBlob> VSCode;

		// �׸����
		//Microsoft::WRL::ComPtr<ID3D11SamplerState> SampleState;
		unsigned int SamplerCount;
		int* SamplerIndexList;

	public:
		// ������ �Ҹ���
		D3D11Shader() = delete;
		D3D11Shader(WCHAR* fileName, int shaderIndex, const WCHAR* shaderName, ID3D11Device& deviceRef, ID3D11DeviceContext& contextRef, std::vector<ID3D11SamplerState*>& samplerList);
		D3D11Shader(WCHAR* vsFileName, WCHAR* psFileName, int shaderIndex, ID3D11Device& deviceRef, ID3D11DeviceContext& contextRef, std::vector<ID3D11SamplerState*>& samplerList);
		~D3D11Shader();

	public:
		// �׸���
		void Draw_Index(unsigned int indexCount);
		void Draw_Vtx(unsigned int vtxCount);

	private:
		// ���̴� ������
		HRESULT LoadCompliedShader(WCHAR* vsFileName, WCHAR* psFileName, int shaderIndex);

		HRESULT InitializeShader();
		HRESULT VS_Load();
		HRESULT PS_Load();
		HRESULT CompilerLoad(const CHAR* entry, const CHAR* target, ID3DBlob** ppCode);

		// ����� ���̾ƿ� ������ ���� �Ѱ����� �����ؼ� �����մϴ�.
		HRESULT CreateVBLayout(int shaderIndex);
		HRESULT CreateSamplerIndex(int shaderIndex);

		int ynErrorW(BOOL bMBox, TCHAR* msg, HRESULT hr, ID3DBlob* pBlob, TCHAR* filename, char* EntryPoint, char* ShaderModel);
	};
}