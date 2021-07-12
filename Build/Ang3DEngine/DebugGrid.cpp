// �ý��� ���
#include "GoodFunc.h"

// ���� ���
//#include "D3D11DeviceManager.h"

// �ڱ� ���
#include "DebugGrid.h"

// ���� �����̽�
using namespace DirectX::SimpleMath;


hos::cg::DebugGrid::DebugGrid(ID3D11Device& device, ID3D11DeviceContext& context)
	:DeviceRef(device), ContextRef(context)
{
	this->CreateGrid();

	this->CreateAxis();
}

hos::cg::DebugGrid::~DebugGrid()
{
	SAFE_RELEASE(AxisIndexBuffer);
	SAFE_RELEASE(AxisVertexBuffer);

	SAFE_RELEASE(m_IndexBuffer);
	SAFE_RELEASE(m_VertexBuffer);
}

void hos::cg::DebugGrid::RenderGrid()
{
	// �������� ����, ������ ����.
	UINT stride = sizeof(VTX_GRID);
	UINT offset = 0;

	// �Է¾������ �������� Ȱ��ȭ ����.
	this->ContextRef.IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// �Է� ������� �ε������� Ȱ��ȭ ����.
	this->ContextRef.IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� ����
	this->ContextRef.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

unsigned int hos::cg::DebugGrid::GetGridIndexCount()
{
	return m_IndexCount;
}

void hos::cg::DebugGrid::RenderAxis()
{
	// �������� ����, ������ ����.
	UINT stride = sizeof(VTX_GRID);
	UINT offset = 0;

	// �Է¾������ �������� Ȱ��ȭ ����.
	this->ContextRef.IASetVertexBuffers(0, 1, &AxisVertexBuffer, &stride, &offset);

	// �Է� ������� �ε������� Ȱ��ȭ ����.
	this->ContextRef.IASetIndexBuffer(AxisIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� ����
	this->ContextRef.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

unsigned int hos::cg::DebugGrid::GetAxisIndexCount()
{
	return AxisIndexCount;
}

void hos::cg::DebugGrid::CreateGrid()
{
	// ����, �ε��� ����
	m_VertexCount = (GRID_SIZE + 1) * (GRID_SIZE + 1);
	m_IndexCount = (GRID_SIZE + 1) * 2 * 2;

	// �����迭 ����
	int zCount = 0;
	for (unsigned int i = 0; i < m_VertexCount; i++)
	{
		m_VtxList[i].vPos = Vector3(-(GRID_SIZE/2) + (float)(i % (GRID_SIZE + 1)), 0, -(GRID_SIZE / 2) + (float)zCount);
		m_VtxList[i].vColor = Vector3(0.5f, 0.5f, 0.5f);

		if (i % (GRID_SIZE + 1) == GRID_SIZE)
			zCount++;
	}

	// �ε����迭 ����
	int Count = 0;
	for (int i = 0; i < (GRID_SIZE + 1); i++)
	{
		m_IndexList[Count].a = (GRID_SIZE + 1) * i;
		m_IndexList[Count].b = (GRID_SIZE + 1) * i + GRID_SIZE;
		Count++;
	}

	for (int i = 0; i < (GRID_SIZE + 1); i++)
	{
		m_IndexList[Count].a = i;
		m_IndexList[Count].b = (GRID_SIZE + 1) * GRID_SIZE + i;
		Count++;
	}

	// �������� ����
	CreateVB(m_VtxList, sizeof(VTX_GRID) * (UINT)m_VertexCount, &m_VertexBuffer);

	// �ε������� ����
	CreateIB(m_IndexList, sizeof(INDEX_GRID) * (UINT)m_IndexCount, &m_IndexBuffer);
}

void hos::cg::DebugGrid::CreateAxis()
{
	// ����, �ε��� ����
	AxisVertexCount = 6;
	AxisIndexCount = 6;

	// X(red)
	this->AxisVtxList[0].vPos = Vector3(0, 0, 0);
	this->AxisVtxList[0].vColor = Vector3(1, 0, 0);
	this->AxisVtxList[1].vPos = Vector3(3, 0, 0);
	this->AxisVtxList[1].vColor = Vector3(1, 0, 0);

	// Y(blue)
	this->AxisVtxList[2].vPos = Vector3(0, 0, 0);
	this->AxisVtxList[2].vColor = Vector3(0.3f, 0.3f, 1);
	this->AxisVtxList[3].vPos = Vector3(0, 3, 0);
	this->AxisVtxList[3].vColor = Vector3(0.3f, 0.3f, 1);

	// Z(green)
	this->AxisVtxList[4].vPos = Vector3(0, 0, 0);
	this->AxisVtxList[4].vColor = Vector3(0, 1, 0);
	this->AxisVtxList[5].vPos = Vector3(0, 0, 3);
	this->AxisVtxList[5].vColor = Vector3(0, 1, 0);

	// Index
	this->AxisIndexList[0] = { 0, 1 };
	this->AxisIndexList[1] = { 2, 3 };
	this->AxisIndexList[2] = { 4, 5 };

	// �������� ����
	CreateVB(AxisVtxList, sizeof(VTX_GRID) * 6, &AxisVertexBuffer);

	// �ε������� ����
	CreateIB(AxisIndexList, sizeof(INDEX_GRID) * 3, &AxisIndexBuffer);
}

HRESULT hos::cg::DebugGrid::CreateVB(void* pBuff, UINT size, ID3D11Buffer** ppVB)
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
	HRESULT hr = this->DeviceRef.CreateBuffer(&bd, &rd, &pVB);
	if (FAILED(hr))
	{
		//���� ���� �� ����ó��..
#ifdef _DEBUG
		MessageBox(NULL, L"Failed, CreateVB on Model", L"Error_Model", MB_OK);
#endif
		return hr;
	}

	//�ܺη� ����.
	*ppVB = pVB;

	return hr;
}

HRESULT hos::cg::DebugGrid::CreateIB(void* pBuff, UINT size, ID3D11Buffer** ppIB)
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
	HRESULT hr = this->DeviceRef.CreateBuffer(&bd, &rd, &pIB);
	if (FAILED(hr))
	{
		//���� ���� �� ����ó��..
		//...
#ifdef _DEBUG
		MessageBox(NULL, L"Failed, CreateIB on Model", L"Error_Model", MB_OK);
#endif
		return hr;
	}

	//�ܺη� ����.
	*ppIB = pIB;

	return hr;
}