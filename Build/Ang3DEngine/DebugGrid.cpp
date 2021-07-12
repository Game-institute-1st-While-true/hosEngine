// 시스템 헤더
#include "GoodFunc.h"

// 참조 헤더
//#include "D3D11DeviceManager.h"

// 자기 헤더
#include "DebugGrid.h"

// 네임 스페이스
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
	// 정점버퍼 단위, 오프셋 설정.
	UINT stride = sizeof(VTX_GRID);
	UINT offset = 0;

	// 입력어셈블러에 정점버퍼 활성화 설정.
	this->ContextRef.IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// 입력 어셈블러에 인덱스버퍼 활성화 설정.
	this->ContextRef.IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형태 설정
	this->ContextRef.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

unsigned int hos::cg::DebugGrid::GetGridIndexCount()
{
	return m_IndexCount;
}

void hos::cg::DebugGrid::RenderAxis()
{
	// 정점버퍼 단위, 오프셋 설정.
	UINT stride = sizeof(VTX_GRID);
	UINT offset = 0;

	// 입력어셈블러에 정점버퍼 활성화 설정.
	this->ContextRef.IASetVertexBuffers(0, 1, &AxisVertexBuffer, &stride, &offset);

	// 입력 어셈블러에 인덱스버퍼 활성화 설정.
	this->ContextRef.IASetIndexBuffer(AxisIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형태 설정
	this->ContextRef.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

unsigned int hos::cg::DebugGrid::GetAxisIndexCount()
{
	return AxisIndexCount;
}

void hos::cg::DebugGrid::CreateGrid()
{
	// 정점, 인덱스 갯수
	m_VertexCount = (GRID_SIZE + 1) * (GRID_SIZE + 1);
	m_IndexCount = (GRID_SIZE + 1) * 2 * 2;

	// 정점배열 생성
	int zCount = 0;
	for (unsigned int i = 0; i < m_VertexCount; i++)
	{
		m_VtxList[i].vPos = Vector3(-(GRID_SIZE/2) + (float)(i % (GRID_SIZE + 1)), 0, -(GRID_SIZE / 2) + (float)zCount);
		m_VtxList[i].vColor = Vector3(0.5f, 0.5f, 0.5f);

		if (i % (GRID_SIZE + 1) == GRID_SIZE)
			zCount++;
	}

	// 인덱스배열 생성
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

	// 정점버퍼 생성
	CreateVB(m_VtxList, sizeof(VTX_GRID) * (UINT)m_VertexCount, &m_VertexBuffer);

	// 인덱스버퍼 생성
	CreateIB(m_IndexList, sizeof(INDEX_GRID) * (UINT)m_IndexCount, &m_IndexBuffer);
}

void hos::cg::DebugGrid::CreateAxis()
{
	// 정점, 인덱스 갯수
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

	// 정점버퍼 생성
	CreateVB(AxisVtxList, sizeof(VTX_GRID) * 6, &AxisVertexBuffer);

	// 인덱스버퍼 생성
	CreateIB(AxisIndexList, sizeof(INDEX_GRID) * 3, &AxisIndexBuffer);
}

HRESULT hos::cg::DebugGrid::CreateVB(void* pBuff, UINT size, ID3D11Buffer** ppVB)
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
	HRESULT hr = this->DeviceRef.CreateBuffer(&bd, &rd, &pVB);
	if (FAILED(hr))
	{
		//에러 검증 및 예외처리..
#ifdef _DEBUG
		MessageBox(NULL, L"Failed, CreateVB on Model", L"Error_Model", MB_OK);
#endif
		return hr;
	}

	//외부로 리턴.
	*ppVB = pVB;

	return hr;
}

HRESULT hos::cg::DebugGrid::CreateIB(void* pBuff, UINT size, ID3D11Buffer** ppIB)
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
	HRESULT hr = this->DeviceRef.CreateBuffer(&bd, &rd, &pIB);
	if (FAILED(hr))
	{
		//에러 검증 및 예외처리..
		//...
#ifdef _DEBUG
		MessageBox(NULL, L"Failed, CreateIB on Model", L"Error_Model", MB_OK);
#endif
		return hr;
	}

	//외부로 리턴.
	*ppIB = pIB;

	return hr;
}