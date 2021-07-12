// <Concept>
// Source Name		: D3D11ParticleSystem.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.06.25

// <Explanation>
// D3D11�� Particle System Ŭ����
// �ӽ÷� Particle�� ������ ���Ÿ� �����Ѵ�.

// Working Log
// 2021.06.25.Hosik	: ����

#pragma once

#include <d3d11_4.h>
#include "DXTK\SimpleMath.h"

namespace hos::cg
{

	class D3D11ParticleSystem
	{
		// Ŭ���� ��� �ν��Ͻ�
	private:
		struct ParticleType
		{
			DirectX::SimpleMath::Vector3 vPos = { 0.f, 0.f, 0.f };
			DirectX::SimpleMath::Vector3 Color = { 1.f, 1.f, 1.f };;
			float velocity = 0.0f;
			bool active = false;
		};

		struct VertexType
		{
			DirectX::SimpleMath::Vector3 position;
			DirectX::SimpleMath::Vector2 uv;
			DirectX::SimpleMath::Vector4 color;
		};

		// Device
		ID3D11Device& DeviceRef;
		ID3D11DeviceContext& ContextRef;

		// Texture
		ID3D11ShaderResourceView* ParticalTexture;

		int m_vertexCount;
		int m_indexCount;
		VertexType* m_vertices;
		ID3D11Buffer* m_vertexBuffer;
		ID3D11Buffer* m_indexBuffer;

		// Particle Value
		DirectX::SimpleMath::Vector3 Deviation;

		float m_particleVelocity;
		float m_particleVelocityVariation;

		float m_particleSize;
		float m_accumulatedTime;
		float m_particlesPerSecond;

		unsigned int m_maxParticles;
		unsigned int m_currentParticleCount;

		// Particle List
		ParticleType* m_particleList;


		// ������ �Ҹ���
	public:
		D3D11ParticleSystem() = delete;
		D3D11ParticleSystem(ID3D11Device* device, ID3D11DeviceContext* context);
		~D3D11ParticleSystem();

		// �ܺ� ��� �Լ�
	public:
		void Frame(float dTime);
		void Render();

		// ���� �Լ�
	private:
		bool SetParticle();
		HRESULT CreateTexture();
		bool CreateVertexBuffer();

		void EmitParticles(float frameTime);
		void UpdateParticles(float frameTime);
		void KillParticles();

		bool UpdateBuffer();

	};

}