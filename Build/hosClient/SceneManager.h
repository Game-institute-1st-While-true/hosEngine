#ifndef _SCENE_MANAGEMENT_H
#define _SCENE_MANAGEMENT_H

// [2020/10/22 ������]
// ���ӳ��� ������ ������ �Ŵ��� Ŭ����

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/04/12] ������ : ���� ���ο��� Ŭ���̾�Ʈ�� �̵�
// [2021/04/16] ��ȿ�� : Change Scene ���� 

#include "hosEngine\COM_Client.h"
#include "ScriptFactory.h"

namespace hos
{
	class SceneManager
	{
	public:
		enum { START_SCENE = 0 };

	public:
		SceneManager();
		~SceneManager();

	private:
		// don't Destoryed
		std::vector<hos::com::Scene*> Scenes;
		std::vector<hos::com::GameObject*> GameObjects;

		//Scene
		hos::com::Scene* NowScene;
		hos::com::Scene* PrevScene;

		ScriptFactory SF;

		bool IsLoopCondition;
	private:
		std::vector<hos::com::Scene*>& GetScenes();

	public:
		void ChangeScene(int buildIndex);
		void ChangeScene(hos::string_view name);
		hos::com::Scene* GetScene();
		void Quit();
		bool IsLoop() const;
		void CheckScene();
	};
}

extern hos::SceneManager* SceneManagement;

#endif // _SCENE_MANAGEMENT_H