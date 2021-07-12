#pragma once
#include <iostream>
#include <vector>	
#include <functional>
namespace gzz
{
	template<typename Ret, typename... Args>
	class RpcTable
	{
		using ARGS_FUNC = std::function<Ret(Args...)>;
		template<class C>
		using CLASS_MEMBER_FUNC = Ret(C::*)(Args...);
		int FuncCount = 0;
	public:
		std::vector<ARGS_FUNC> Table;
		RpcTable() = default;
		virtual ~RpcTable() = default;
		
		// �Լ����̺� Ŭ��� �ƴ� Make �� �Լ����̺��� ������ŭ �ʱ�ȭ�Ѵ�.
		void Clear()
		{
			for (int i = 0; i < FuncCount + 1; i++)
			{
				ARGS_FUNC _func = [=](Args... args) { this->worng_approach(args...); };
				Table.push_back(_func);
			}
		}
	public:
		// �Լ����̺��� ���� + 1 �� ��ŭ �����Ѵ�. ���⼭ ������ �Լ��� ����ȣ����̴�. 
		// �ٸ� �Լ��� ���ε��� ���� ��� �����Լ��� ȣ���Ѵ�.
		void Make(const int functable_count)
		{
			for (int i = 0; i < functable_count + 1; i++)
			{
				ARGS_FUNC _func = [=](Args... args) { this->worng_approach(args...); };
				Table.push_back(_func);
			}
			FuncCount += functable_count;
		};

		template<class C>
		bool Mapping(C* this_, int index, CLASS_MEMBER_FUNC<C> function)
		{
			if (function != nullptr && this_ != nullptr && (index >= 0 || index < FuncCount))
			{
				ARGS_FUNC _func = [=](Args... args) { (this_->*function)(args...); };
				Table[index] = _func;
				return true;
			}
			return false;
		}

		void DelFunc(const int index)
		{
			if (index < 0 || index >= FuncCount)
			{
				ARGS_FUNC _func = [=](Args... args) { this->worng_approach(args...); };
				Table[index] = _func;
			}
		}

		Ret worng_approach(Args... args)
		{
			std::wcout << L"worng approach" << std::endl;
		}

		ARGS_FUNC& operator[](const int index)
		{
			if (index < 0 || index >= FuncCount)
			{
				return Table[FuncCount];
			}
			else
			{
				return Table[index];
			}
		}
	};
}