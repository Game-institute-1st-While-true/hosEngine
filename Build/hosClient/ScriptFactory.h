#ifndef _SCRIPT_FACTORY_H
#define _SCRIPT_FACTORY_H

// [2021/02/18 ������]
// ��ũ��Ʈ�� �����ϱ����� ���丮 Ŭ����
// ������ũ��Ʈ�� ������ �ش� ��ũ��Ʈ�� ������, ���� �����ϱ����� �̰��� �߰��ؾ��Ѵ�

#include "hosEngine/COM_Client.h"

namespace hos
{
	class ScriptFactory
	{
	public:
		ScriptFactory();
		~ScriptFactory();

	public:
		com::Script* Create(string_view name);
		void BindObject(com::GameObject* object);
		void BindObject(com::Scene* Scene);
	};
}

#endif // !_SCRIPT_FACTORY_H