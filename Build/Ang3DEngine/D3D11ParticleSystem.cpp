// �ý��� ���
#include "GoodFunc.h"
#include "DebugConsole.h"

// ���� ���
#include "DXTex/DirectXTex.h"


// �ڱ� ���
#include "D3D11ParticleSystem.h"

// ���� �����̽�
using namespace DirectX::SimpleMath;
using namespace hos::cg;

// �Լ� ���


D3D11ParticleSystem::D3D11ParticleSystem(ID3D11Device* device, ID3D11DeviceContext* context)
	:DeviceRef(*device), ContextRef(*context)
{
	// ��ƼŬ ����Ʈ �ʱ�ȭ
	m_particleList = new ParticleType[m_maxParticles];

	// ��ƼŬ ����
	this->SetParticle();

	// �ؽ��� ����
	this->CreateTexture();

	// ���� ����
	this->CreateVertexBuffer();
}

D3D11ParticleSystem::~D3D11ParticleSystem()
{
	SAFE_RELEASE(m_indexBuffer);

	SAFE_RELEASE(m_vertexBuffer);
	SAFE_DELARRY(m_vertices);

	SAFE_DELARRY(m_particleList);

	SAFE_RELEASE(ParticalTexture);
}

void D3D11ParticleSystem::Frame(float dTime)
{
	this->KillParticles();

	this->EmitParticles(dTime);

	this->UpdateParticles(dTime);

	std::wstring fff = L"Now Particle: " + std::to_wstring(this->m_currentParticleCount);

	DEBUG_CONSOLE(fff);

	this->UpdateBuffer();
}

void D3D11ParticleSystem::Render()
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	ContextRef.IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	ContextRef.IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ���� ���ۿ��� ������ �Ǿ�� �ϴ� ������Ƽ�� ������ �����մϴ�.
	ContextRef.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool D3D11ParticleSystem::CreateVertexBuffer()
{
	// ���� �迭�� �ִ� ���� ���� �����մϴ�.
	m_vertexCount = m_maxParticles * 6;

	// �ε��� �迭�� �ִ� �ε��� ���� �����մϴ�.
	m_indexCount = m_vertexCount;

	// ������ �� ���ڿ� ���� ���� �迭�� ����ϴ�.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �ε��� �迭�� �ʱ�ȭ�մϴ�.
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� ���� ������ ������ �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ��ħ�� ���� ���۸� ����ϴ�.
	if (FAILED(DeviceRef.CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// ���� �ε��� ������ ������ �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����ϴ�.
	if (FAILED(DeviceRef.CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// �� �̻� �ʿ����� �����Ƿ� �ε��� �迭�� �����մϴ�.
	delete[] indices;
	indices = 0;

	return true;
}

bool hos::cg::D3D11ParticleSystem::SetParticle()
{
	// ���� �� �� ��ƼŬ�� ��ġ �� �� �ִ� ������ ������ �����մϴ�.
	Deviation.x = 0.5f;
	Deviation.y = 0.1f;
	Deviation.z = 2.0f;

	// ��ƼŬ�� �ӵ��� �ӵ� ��ȭ�� �����մϴ�.
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// ��ƼŬ�� ������ ũ�⸦ �����մϴ�.
	m_particleSize = 0.2f;

	// �ʴ� ���� �� ��ƼŬ ���� �����մϴ�.
	m_particlesPerSecond = 250.0f;

	// ��ƼŬ �ý��ۿ� ���Ǵ� �ִ� ��ƼŬ ���� �����մϴ�.
	m_maxParticles = 5000;

	// ��ƼŬ ����Ʈ�� �����մϴ�.
	m_particleList = new ParticleType[m_maxParticles];
	if (!m_particleList)
	{
		return false;
	}

	// ��ƼŬ ����Ʈ�� �ʱ�ȭ �մϴ�.
	for (unsigned int i = 0; i < m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	// ���� ������� �����Ƿ� ���� ��ƼŬ ���� 0���� �ʱ�ȭ�մϴ�.
	m_currentParticleCount = 0;

	// �ʴ� ��ƼŬ ���� �ӵ��� �ʱ� ���� �ð��� ����ϴ�.
	m_accumulatedTime = 0.0f;
	return true;
}

HRESULT D3D11ParticleSystem::CreateTexture()
{
	HRESULT hr = S_OK;
	bool bCheckAlpha = true;
	WCHAR fileRoot[256] = L"../../Resource/star.dds";

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
			}
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = DirectX::CreateShaderResourceView(&(this->DeviceRef), image->GetImages(), image->GetImageCount(), meta, &ParticalTexture);
	}

	// ScratchImage ����
	SAFE_DELETE(image);

	return hr;
}

void D3D11ParticleSystem::EmitParticles(float frameTime)
{	// ������ �ð��� ������ŵ�ϴ�.
	m_accumulatedTime += frameTime;

	// ���� ��ƼŬ ����� false�� �����մϴ�.
	bool emitParticle = false;

	// �� ��ƼŬ�� ������ �ð����� Ȯ���մϴ�.
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// ���� �� ��ƼŬ�� ������ ������ �� �ϳ��� �����մϴ�.
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// ���� ���� ȭ �� ��ƼŬ �Ӽ��� �����մϴ�.
		float positionX = (((float)rand() - (float)rand()) / RAND_MAX) * Deviation.x;
		float positionY = (((float)rand() - (float)rand()) / RAND_MAX) * Deviation.y;
		float positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * Deviation.z;

		float velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		float red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		float blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		// ���� ������ ���� ��ƼŬ�� �ڿ��� ������ �������ؾ��ϹǷ� ��ƼŬ �迭�� �����ؾ� �մϴ�.
		// Z ���̸� ����Ͽ� �����ϹǷ� ��Ͽ��� ��ƼŬ�� �����ؾ� �ϴ� ��ġ�� ã�ƾ��մϴ�.
		int index = 0;
		bool found = false;
		while (!found)
		{
			if ((m_particleList[index].active == false) || (m_particleList[index].vPos.z < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// ���� �� ��ġ�� �˾����Ƿ� �ε������� �� ��ġ �� �迭�� �����Ͽ� �� ��ƼŬ�� ���� ������ �������մϴ�.
		int i = m_currentParticleCount;
		int j = i - 1;

		while (i != index)
		{
			m_particleList[i] = m_particleList[j];
			i--;
			j--;
		}

		// ���� ��Ȯ�� ���� ������ ��ƼŬ �迭�� �����Ͻʽÿ�.
		m_particleList[index].vPos.x = positionX;
		m_particleList[index].vPos.y = positionY;
		m_particleList[index].vPos.z = positionZ;
		m_particleList[index].Color.x = red;
		m_particleList[index].Color.y = green;
		m_particleList[index].Color.z = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;
	}

}

void D3D11ParticleSystem::UpdateParticles(float frameTime)
{
	// �� �������� ��ġ, �ӵ� �� ������ �ð��� ����Ͽ� �Ʒ������� �̵��Ͽ� ��� ��ƼŬ�� ������Ʈ�մϴ�.
	for (unsigned int i = 0; i < m_currentParticleCount; i++)
	{
		m_particleList[i].vPos.y = m_particleList[i].vPos.y - (m_particleList[i].velocity * frameTime * 0.001f);
	}
}

void D3D11ParticleSystem::KillParticles()
{
	// Ư�� ���� ������ ��� ��� ��ƼŬ�� �����մϴ�.
	for (unsigned int i = 0; i < m_maxParticles; i++)
	{
		if ((m_particleList[i].active == true) && (m_particleList[i].vPos.y < -3.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// ���� ��� ����ִ� ��ƼŬ�� �迭���� �̵����� �ı��� ��ƼŬ�� ����� �迭�� �ùٸ��� ���ĵ� ���·� �����մϴ�.
			for (unsigned int j = i; j < m_maxParticles - 1; j++)
			{
				m_particleList[j] = m_particleList[j + 1];
			}
		}
	}
}

bool D3D11ParticleSystem::UpdateBuffer()
{
	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	VertexType* verticesPtr;
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// ���� ��ƼŬ ��� �迭���� ���� �迭�� ����ϴ�. �� ��ƼŬ�� �� ���� �ﰢ������ ������� �����Դϴ�.
	int index = 0;

	for (unsigned int i = 0; i < m_currentParticleCount; i++)
	{
		// ���� �Ʒ�.
		m_vertices[index].position = Vector3(m_particleList[i].vPos.x - m_particleSize, m_particleList[i].vPos.y - m_particleSize, m_particleList[i].vPos.z);
		m_vertices[index].uv = Vector2(0.0f, 1.0f);
		m_vertices[index].color = Vector4(m_particleList[i].Color.x, m_particleList[i].Color.y, m_particleList[i].Color.z, 1.0f);
		index++;

		// ���� ��.
		m_vertices[index].position = Vector3(m_particleList[i].vPos.x - m_particleSize, m_particleList[i].vPos.y + m_particleSize, m_particleList[i].vPos.z);
		m_vertices[index].uv = Vector2(0.0f, 0.0f);
		m_vertices[index].color = Vector4(m_particleList[i].Color.x, m_particleList[i].Color.y, m_particleList[i].Color.z, 1.0f);
		index++;

		// ������ �Ʒ�.
		m_vertices[index].position = Vector3(m_particleList[i].vPos.x + m_particleSize, m_particleList[i].vPos.y - m_particleSize, m_particleList[i].vPos.z);
		m_vertices[index].uv = Vector2(1.0f, 1.0f);
		m_vertices[index].color = Vector4(m_particleList[i].Color.x, m_particleList[i].Color.y, m_particleList[i].Color.z, 1.0f);
		index++;

		// ������ �Ʒ�.
		m_vertices[index].position = Vector3(m_particleList[i].vPos.x + m_particleSize, m_particleList[i].vPos.y - m_particleSize, m_particleList[i].vPos.z);
		m_vertices[index].uv = Vector2(1.0f, 1.0f);
		m_vertices[index].color = Vector4(m_particleList[i].Color.x, m_particleList[i].Color.y, m_particleList[i].Color.z, 1.0f);
		index++;

		// ���� ��.
		m_vertices[index].position = Vector3(m_particleList[i].vPos.x - m_particleSize, m_particleList[i].vPos.y + m_particleSize, m_particleList[i].vPos.z);
		m_vertices[index].uv = Vector2(0.0f, 0.0f);
		m_vertices[index].color = Vector4(m_particleList[i].Color.x, m_particleList[i].Color.y, m_particleList[i].Color.z, 1.0f);
		index++;

		// ������ ��.
		m_vertices[index].position = Vector3(m_particleList[i].vPos.x + m_particleSize, m_particleList[i].vPos.y + m_particleSize, m_particleList[i].vPos.z);
		m_vertices[index].uv = Vector2(1.0f, 0.0f);
		m_vertices[index].color = Vector4(m_particleList[i].Color.x, m_particleList[i].Color.y, m_particleList[i].Color.z, 1.0f);
		index++;
	}

	// ���� ���۸� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(ContextRef.Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ���� ������ �����͸� ����Ű�� �����͸� ��´�.
	verticesPtr = (VertexType*)mappedResource.pData;

	// �����͸� ���� ���ۿ� �����մϴ�.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// ���� ������ ����� �����մϴ�.
	ContextRef.Unmap(m_vertexBuffer, 0);

	return true;
}