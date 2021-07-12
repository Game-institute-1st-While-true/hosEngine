#ifndef _SINGLETON_H
#define _SINGLETON_H
#include <mutex>

// [2020/10/15 ������]
// �̱��� Ŭ������ ����� ��ũ��

//LOG
// [2020/10/23 ������] ���ø�Ŭ���� ���� ��ũ�η� �����Ѵ�. ���ø�Ŭ������ ���� �̱����� ������ �ʹ����� ����.
// [2020/11/16 ������] �̱��� Ŭ������ ����� �� ����

// warnning : Ŭ���� ������ �ֻ�ܿ� ��ũ�θ� ����ϵ��� ����
#define SINGLETON_CLASS(name)							\
private:												\
	name();												\
	~name();											\
public:													\
	name(const name&) = delete;							\
	name& operator=(const name&) = delete;				\
	name(name&&) = delete;								\
	name& operator=(name&&) = delete;					\
														\
	inline static name* name##Ins;						\
public:													\
	static name* GetIns()								\
	{													\
		if (name##Ins == nullptr)						\
		{												\
			name##Ins = new name();						\
		}												\
		return name##Ins;								\
	}													\
	static void Destory()								\
	{													\
		if (name##Ins)									\
		{												\
			delete name##Ins;							\
			name##Ins = nullptr;						\
		}												\
	}													\
private:												\

#define THREAD_SAFE_SINGLETON_CLASS(name)					\
private:													\
	name();													\
	~name();												\
	inline static std::mutex InstanceMutex = std::mutex();	\
public:														\
	name(const name&) = delete;								\
	name& operator=(const name&) = delete;					\
	name(name&&) = delete;									\
	name& operator=(name&&) = delete;						\
															\
	inline static name* name##Ins;							\
public:														\
	static name* GetIns()									\
	{														\
		InstanceMutex.lock();								\
		if (name##Ins == nullptr)							\
		{													\
			name##Ins = new name();							\
		}													\
		InstanceMutex.unlock();								\
		return name##Ins;									\
	}														\
	static void Destory()									\
	{														\
		if (name##Ins)										\
		{													\
			delete name##Ins;								\
			name##Ins = nullptr;							\
		}													\
	}														\
private:													\

/*
namespace hos
{
	template<typename T>
	class Singleton
	{
	private:
		inline static T* Instance = nullptr;
	protected:
		Singleton() = default;
		virtual ~Singleton() = default;

	private:
		//�̱��� Ŭ������ ���� ����
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		//�̱��� Ŭ������ �̵� ����
		Singleton(Singleton&&) = delete;
		Singleton& operator=(Singleton&&) = delete;

	public:
		static T* GetIns()
		{
			if (Instance == nullptr)
			{
				Instance = new T;
			}
			return Instance;
		}

		void Destory()
		{
			if (Instance)
			{
				delete Instance;
				Instance = nullptr;
			}
		}
	};
}
//*/

#endif // !_SINGLETON_H