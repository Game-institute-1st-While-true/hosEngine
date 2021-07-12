// <Concept>
// Source Name		: HContainer.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.02.09

// <Explanation>
// ���������� �����ϱ� ���� ���ø� Ŭ����

// Working Log
// 2021.02.09.Hosik	: ����

#pragma once
#include <vector>

namespace hos
{
	template <typename Data>
	class HContainer
	{
	public:
		typedef class HContainer<Data> Container;
		Data* m_pParent = nullptr;
		Data* m_pCurData = nullptr;

		std::vector<Data*> m_pChildList;
		int	m_ChildCnt = 0;

	public:
		void Release();					// ���� ����

		bool AddChild(Data* pChild);							//�ڽ����� ����ϱ�.
		void SetParent(Data* pParent) { m_pParent = pParent; }	//�θ� �����ϱ�.
		const Data* GetParent() { return m_pParent; }			//�θ� ������ ���.
		std::vector<Data*>* GetChildList() { return &m_pChildList; }		//�ڽ� ����Ʈ ���.��

	public:
		HContainer();
		HContainer(Data* pCurData);
		~HContainer();
	};

	template<typename Data>
	inline void HContainer<Data>::Release()
	{
		// Container�� �������ݴϴ�.
		/// ��ü�� ���� ������ �� ���� �����ش�.
		m_pParent = nullptr;

		m_pChildList.clear();
		m_pChildList.shrink_to_fit();
	}

	template<typename Data>
	inline bool HContainer<Data>::AddChild(Data* pChild)
	{
		m_pChildList.push_back(pChild);
		m_ChildCnt++;

		return true;
	}

	template<typename Data>
	inline HContainer<Data>::HContainer()
	{
		m_pChildList.reserve(10);
	}

	template<typename Data>
	inline HContainer<Data>::HContainer(Data* pCurData)
	{
	}

	template<typename Data>
	inline HContainer<Data>::~HContainer()
	{
	}
}