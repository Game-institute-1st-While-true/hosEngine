// <Concept>
// Source Name		: DebugGrid.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.02.19

// <Explanation>
// ����� ȯ�濡�� �� ���ü� �� ��ǥ�� Ȯ���� ���� ����� �׸���.
// �ϴ� D3D11 ȯ����Դϴ�.


// Working Log
// 2021.02.19.Hosik	: ����

#pragma once

#include <d3d11_4.h>
#include "DXTK\Simplemath.h"

#define GRID_SIZE 20

namespace hos::cg
{

	class DebugGrid
	{
	public:
		struct VTX_GRID
		{
			DirectX::SimpleMath::Vector3 vPos;
			DirectX::SimpleMath::Vector3 vColor;
		};

		struct INDEX_GRID
		{
			UINT a;
			UINT b;
		};

		// ���� ��� �ν��Ͻ�
	private:
		// Renderer
		ID3D11Device& DeviceRef;
		ID3D11DeviceContext& ContextRef;

		// �׸���
		VTX_GRID m_VtxList[(GRID_SIZE + 1) * (GRID_SIZE + 1)];
		INDEX_GRID m_IndexList[(GRID_SIZE + 1) * 2];

		ID3D11Buffer* m_VertexBuffer = nullptr;
		ID3D11Buffer* m_IndexBuffer = nullptr;
		unsigned int m_VertexCount = 0;
		unsigned int m_IndexCount = 0;

		// Axis
		VTX_GRID AxisVtxList[6];
		INDEX_GRID AxisIndexList[3];

		ID3D11Buffer* AxisVertexBuffer = nullptr;
		ID3D11Buffer* AxisIndexBuffer = nullptr;
		unsigned int AxisVertexCount = 0;
		unsigned int AxisIndexCount = 0;

		// ����&�Ҹ���
	public:
		DebugGrid() = delete;
		DebugGrid(ID3D11Device& device, ID3D11DeviceContext& context);
		~DebugGrid();

		// �ܺ� ��� ���� �Լ�
	public:
		void RenderGrid();
		unsigned int GetGridIndexCount();

		void RenderAxis();
		unsigned int GetAxisIndexCount();

		// ���ο� �Լ�
	private:
		void CreateGrid();
		void CreateAxis();

		HRESULT CreateVB(void* pBuff, UINT size, ID3D11Buffer** ppVB);
		HRESULT CreateIB(void* pBuff, UINT size, ID3D11Buffer** ppIB);
	};
}