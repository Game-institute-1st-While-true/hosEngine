#ifndef _SINGLETON_H
#define _SINGLETON_H

// [2020/10/15 ������]
// �̱��� Ŭ������ ����� ��ũ��

//LOG
// [2020/10/23 ������] ���ø�Ŭ���� ���� ��ũ�η� �����Ѵ�. ���ø�Ŭ������ ���� �̱����� ������ �ʹ����� ����.
// [2020/10/23 ������] Ŭ���� ������ �ֻ�ܿ� ��ũ�θ� ����ϵ��� ����

#define SINGLETON_CLASS(name)							\
private:												\
	name();												\
	~name();											\
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

#endif // !_SINGLETON_H