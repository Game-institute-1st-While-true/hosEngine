#pragma once

#include <string>
#include <list>	
#include <vector>
#include <map>
#include <set>
#include <cstring>

/// <summary>
///	[2020-12-06 ��ȿ��]
///	thread safe�������� Data Chunk
/// </summary>

namespace gzz
{
	static constexpr int MAX_STREAM_SIZE = 1024;
	class Chunk
	{
	private:
		// ���۸� ���� ��ġ
		unsigned int Position;
		// ������ ����
		unsigned int Offset;
		// ����
		char Buf[MAX_STREAM_SIZE];

	public:
		Chunk();
		~Chunk();

	public:
		void Clear()
		{
			Position = 0;
			Offset = 0;
		}

		char* GetBuf()
		{
			return Buf;
		}
		bool IsEmpty()
		{
			return (Offset <= 0);
		}
		int Size()
		{
			return Offset;
		}
		void SetOffset(const int size)
		{
			Offset = size;
		}
		void Next(const int offset)
		{
			Position = offset;
		}

		void Copy(const char* buf, const int size)
		{
			memcpy_s(Buf, MAX_STREAM_SIZE, buf, size);
			Offset = size;
		}

		void Copy(const char* buf, const int src, const int size)
		{
			memcpy_s(Buf + src, MAX_STREAM_SIZE, buf, size);
			Offset = size;
		}

		template <typename T>
		void operator << (T& _target)
		{
			WriteBuffer(_target);
		}
		template <typename T>
		void operator >> (T& _target)
		{
			ReadBuffer(_target);
		}


		Chunk& AddChunk(Chunk& ch);
		inline Chunk& operator += (Chunk& ch)
		{
			return AddChunk(ch);
		}

	protected:
		inline bool CheckWrite(const int _len)
		{
			// ������� + ������� : ��ü���� ��
			if (Offset + _len > MAX_STREAM_SIZE)
			{
				return false;
			}
			return true;
		}

		inline bool CheckRead(const int _len)
		{
			// ���� ��ġ + ���� ���� : ���� ���� ��
			if (Position + _len > Offset)
			{
				return false;
			}
			return true;
		}

		template <typename T>
		inline void WriteBuffer(T& _value)
		{
			int size = sizeof(T);
			if (CheckWrite(size) == false)
			{
				return;
			}
			memcpy_s(Buf + Offset, MAX_STREAM_SIZE - Offset, &_value, size);
			Offset += size;
		}
		template <typename T>
		inline void WriteBuffer(T* _value)
		{
			// �ش� �������� ���� �־��ش�.
			WriteBuffer(*_value);
		}

		template <typename T>
		inline void WriteBuffer(std::list<T>& list);

		template <typename T>
		inline void WriteBuffer(std::vector<T>& vec);

		template <typename T>
		inline void ReadBuffer(T& _target)
		{
			int size = sizeof(T);

			if (CheckRead(size) == false)
			{
				return;
			}
			memcpy_s(&_target, size, Buf + Position, size);
			Position += size;
		}

		template <typename T>
		inline void ReadBuffer(std::list<T>& list);

		template <typename T>
		inline void ReadBuffer(std::vector<T>& vec);
	};

	/// Ư��ȭ
	template<typename T>
	inline void Chunk::WriteBuffer(std::list<T>& list)
	{
		unsigned int size = list.size();

		if (CheckWrite(sizeof(int)) == false)
		{
			return;
		}
		WriteBuffer(size);

		for (auto data : list)
		{
			WriteBuffer(data);
		}
	}

	template<typename T>
	inline void Chunk::WriteBuffer(std::vector<T>& vec)
	{
		unsigned int size = vec.size();

		if (CheckWrite(sizeof(int)) == false)
		{
			return;
		}
		WriteBuffer(size);

		for (auto data : vec)
		{
			WriteBuffer(data);
		}
	}

	template<typename T>
	inline void Chunk::ReadBuffer(std::list<T>& list)
	{
		list.clear();
		unsigned int size = 0;
		if (CheckRead(sizeof(int)) == false)
		{
			return;
		}

		ReadBuffer(size);

		T data;
		for (unsigned int i = 0; i < size; i++)
		{
			ReadBuffer(data);
			list.push_back(data);
		}
	}

	template<typename T>
	inline void Chunk::ReadBuffer(std::vector<T>& vec)
	{
		vec.clear();
		unsigned int size = 0;
		if (CheckRead(sizeof(int)) == false)
		{
			return;
		}

		ReadBuffer(size);
		T data;
		for (unsigned int i = 0; i < size; i++)
		{
			ReadBuffer(data);
			vec.push_back(data);
		}
	}

	// ��Ʈ���� { ��ü������ + buf[0] + buf[1] + ... buf[n] }
	template<>
	inline void Chunk::WriteBuffer(std::wstring& _value)
	{
		unsigned int size = static_cast<unsigned int>(_value.size());

		if (CheckWrite(sizeof(int)) == false)
		{
			return;
		}

		WriteBuffer(size);

		if (CheckWrite(size * sizeof(wchar_t)) == false)
		{
			return;
		}

		for (unsigned int i = 0; i < size; i++)
		{
			WriteBuffer(_value[i]);
		}
	}

	template<>
	inline void Chunk::ReadBuffer(std::wstring& _value)
	{
		unsigned int size = 0;
		if (CheckRead(sizeof(int)) == false)
		{
			return;
		}

		ReadBuffer(size);
		if (CheckRead(size * sizeof(wchar_t)) == false)
		{
			return;
		}

		wchar_t one_wchar;
		for (unsigned int i = 0; i < size; i++)
		{
			ReadBuffer(one_wchar);
			_value += one_wchar;
		}
	}
}