#pragma once
#include "hosEngine/COM_Client.h"
#include "hosEngine/DataManager.h"

//class Graphics;

extern int mousePosX;
extern int mousePosY;
extern bool bLBTDOWN;
extern bool bStaticClicked;

class EditorManager
{
	SINGLETON_CLASS(EditorManager);
private:
	hos::com::Scene* NowScene = nullptr;
	hos::com::Scene* firstScene = nullptr;

	//hos::com::COM_System* ComSystem = nullptr;

	// 현재 선택된 오브젝트 정보
	hos::com::GameObject* NowGameObject = nullptr;


public:
	void Initialize(HWND hwnd);
	void Initialize();
	void Finalize();
	CString CreateGameObject(CString* gameObjectPerent = nullptr);
	void RemoveGameObject(CString* gameObjectName);
	CString ChangeGameObjectName(CString* gameObject, CString* changeName);
	void AddComponent(CString* gameObject, CString* componentName, int index);
	void RemoveComponent(CString* gameObject, CString* componentName, int index);
	bool LoadScene(CString* scenePath);
	bool SaveScene(CString* scenePath);
	void SavePrefab(CString* prefabPath, CString* gameObjectName);
	bool LoadPrefab(CString* prefabPath, CString* parentGameObjectName);
	CString AddPrefab(CString* prefabName, CString* gameObjectPerent = nullptr);

	void LoadGameData(CString* dataPath);

	void SaveDataManager(CString* savePath);

	hos::com::Scene* GetNowScene() const { return NowScene; }
	void SetNowScene(hos::com::Scene* val) { NowScene = val; }
	hos::com::GameObject* GetNowGameObject() const { return NowGameObject; }
	void SetNowGameObject(hos::com::GameObject* val) { NowGameObject = val; }
	void SetNowGameObject(CString* name);

	hos::cg::Renderer* GetRenderer() const { return g_Renderer; }
	hos::com::DataManager* GetDataManager() const { return g_DataManager; }

	void EditorLoop();

};

