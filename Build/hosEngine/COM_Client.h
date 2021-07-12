#ifndef _COM_SYSTEM_H
#define _COM_SYSTEM_H

// [2020/11/23 ������]
// Component Object Model System
// ������Ʈ ���̺귯���� ������ ����� Ŭ����, ���̺귯���� ���� ���

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/03/10] ������ : ���̺귯�� ���� �߰������ʵ��� ����� ��ũ�� �߰�
// [2021/03/10] ������ : ����2�� ���ؼ� ������ �ӽ� ����
// [2021/04/16] ��ȿ�� : Scene Seed �߰�

#include <thread>
#include "NetworkManager.h"
#include "Navigation.h"
#include "DXTK/DirectXTK.h"
#include "PhysX.h"
#include "Scene.h"

//platform
#include "WindowsApplication.h"

//Components
#include "Transform.h"
#include "BoxCollision.h"
#include "CapsuleCollision.h"
#include "MeshCollision.h"
#include "SphereCollision.h"
#include "Rigidbody.h"
#include "Camera.h"
#include "Light.h"
#include "AudioListener.h"
#include "AudioSource.h"
#include "Script.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "NavAgent.h"
#include "Networkcomponent.h"
#include "Animation.h"
#include "UIImage.h"
#include "UI_Text.h"
#include "UIButton.h"
#include "UI_InputField.h"

#if defined(_DEBUG)||defined(DEBUG)
#if _WIN64
#pragma comment(lib,"hosEngine_x64D")
#else
#pragma comment(lib,"hosEngine_x86D")
#endif // _WIN64
#else
#if _WIN64
#pragma comment(lib,"hosEngine_x64R")
#else
#pragma comment(lib,"hosEngine_x86R")
#endif // _WIN64
#endif // defined(_DEBUG)||defined(DEBUG)

namespace hos::com
{
	class COM_Client
	{
	private:
		std::thread MainThread;
		WindowsApplication* App;
	public:

	public:
		COM_Client(HINSTANCE ins);
		~COM_Client();

	public:
		void Load();
		bool Update();
		WindowsApplication* GetApp()const;

	public:
		ut::Delegate<void> OnIdleProc;
	};
}

#endif // !_COM_SYSTEM_H