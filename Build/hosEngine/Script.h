#ifndef _SCRIPT_H
#define _SCRIPT_H

// [2020/12/10 ������]
// ��ũ��Ʈ
// ������ �����ϴ� ��ũ��Ʈ ������Ʈ ���� ���躯�濡 ���� ������� ���� ���ɼ��� �ִ�
// �ӽ� ���� �س��´�

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/03/27] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�

#include "Component.h"

namespace hos::com
{
	class Script : public Component
	{
	private:
		string ScriptName;
		int EditerIndex;

	public:
		Script();
		Script(string_view scriptName);
		~Script();

	protected:
		Script(const Script& dest);

	public:
		string GetScriptName();
		void SetScriptName(string_view scriptName);
		void SetExecutionOrder(int order);
		int GetEditorIndex() const { return EditerIndex; };
		void SetEditorIndex(int index) { EditerIndex = index; };

		Script* Clone() const override;
		void Reset()override;
		void OnEnable()override;
		void OnDisable()override;

		const std::vector<U8> Serialize()final;
		bool Deserialize(mbstring_view data)final;
	};
}

#endif // !_SCRIPT_H