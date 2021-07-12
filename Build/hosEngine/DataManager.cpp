#include "Object.h"
#include "DataManager.h"

#include <filesystem>
#include <functional>
#include <algorithm>
#include <vector>

#include "GameObject.h"
#include "AudioClip.h"
#include "PhysicMaterial.h"
#include "Material.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "Transform.h"
#include "Animation.h"
#include "intsafe.h"

using namespace hos;

#ifdef _DEBUG
bool bDebug = true;
#elif NDEBUG
bool bDebug = false;
#endif

bool bClient = true;

hos::com::DataManager::DataManager()
	:FbxLoaderInstance(nullptr), DefaultMaterial(nullptr)
{
	if (g_Renderer)
	{
		DefaultMaterial = new Material();

		Mesh* t;
		ModelPrefab* t2;

		// Editor���
		if (bClient == false)
		{
			// FbxLoader ����
			FbxLoaderInstance = new hos::FbxLoader(ModelList, MeshList, AnimationList, MaterialList, TextureList, UIAnimationList);

			// Box
			t2 = FbxLoaderInstance->LoadFbxFile("../../Resource/DebugBox.fbx");
			if (t2 == nullptr)
			{
				t2 = FbxLoaderInstance->LoadFbxFile("../Resource/DebugBox.fbx");
				if (t2 == nullptr)
				{
					t = this->LoadMesh(L"./Assets/Mesh/DebugBox.mesh");
					if (t == nullptr)
					{
						Debug->LogConsole(L"DataManager", L"DebugBox Mesh ������ �����ϴ�.");
					}
				}
			}

			// Sphere
			t2 = FbxLoaderInstance->LoadFbxFile("../../Resource/DebugSphere.fbx");
			if (t2 == nullptr)
			{
				t2 = FbxLoaderInstance->LoadFbxFile("../Resource/DebugSphere.fbx");
				if (t2 == nullptr)
				{
					t = this->LoadMesh(L"./Assets/Mesh/DebugSphere.mesh");
					if (t == nullptr)
					{
						Debug->LogConsole(L"DataManager", L"DebugSphere Mesh ������ �����ϴ�.");
					}
				}
			}

			// Capsule
			t2 = FbxLoaderInstance->LoadFbxFile("../../Resource/DebugCapsule.fbx");
			if (t2 == nullptr)
			{
				t2 = FbxLoaderInstance->LoadFbxFile("../Resource/DebugCapsule.fbx");
				if (t2 == nullptr)
				{
					t = this->LoadMesh(L"./Assets/Mesh/DebugCapsule.mesh");
					if (t == nullptr)
					{
						Debug->LogConsole(L"DataManager", L"DebugCapsule Mesh ������ �����ϴ�.");
					}
				}
			}
		}
		// Client ���
		else
		{
			t = this->LoadMesh(L"./Assets/Mesh/DebugBox.mesh");
			if (t == nullptr)
			{
				Debug->LogConsole(L"DataManager", L"DebugBox Mesh ������ �����ϴ�.");
			}

			t = this->LoadMesh(L"./Assets/Mesh/DebugSphere.mesh");
			if (t == nullptr)
			{
				Debug->LogConsole(L"DataManager", L"DebugBox Sphere ������ �����ϴ�.");
			}

			t = this->LoadMesh(L"./Assets/Mesh/DebugCapsule.mesh");
			if (t == nullptr)
			{
				Debug->LogConsole(L"DataManager", L"DebugCapsule Mesh ������ �����ϴ�.");
			}
		}

	}

	// Data
	TextureList.reserve(100);
	SkyBoxList.reserve(10);
}

hos::com::DataManager::~DataManager()
{
	SkyBoxList.clear();

	TextureList.clear();

	for (auto [name, uiani] : UIAnimationList)
	{
		SafeDelete(uiani);
	}
	UIAnimationList.clear();

	for (auto [name, anim] : AnimationList)
	{
		SafeDelete(anim);
	}
	AnimationList.clear();

	for (auto [name, material] : MaterialList)
	{
		SafeDelete(material);
	}
	MaterialList.clear();

	for (auto [name, mesh] : MeshList)
	{
		SafeDelete(mesh);
	}
	MeshList.clear();

	for (auto [name, model] : ModelList)
	{
		SafeDelete(model);
	}
	ModelList.clear();

	for (auto [name, font] : Fonts)
	{
		SafeDelete(font);
	}
	Fonts.clear();

	for (auto [name, physicMaterial] : PhysicMaterials)
	{
		SafeDelete(physicMaterial);
	}
	PhysicMaterials.clear();

	for (auto [name, audio] : AudioClips)
	{
		SafeDelete(audio);
	}
	AudioClips.clear();

	for (auto [name, prefab] : Prefabs)
	{
		SafeDelete(prefab);
	}
	Prefabs.clear();

	for (auto [name, scene] : Scenes)
	{
		SafeDelete(scene);
	}
	Scenes.clear();

	SafeDelete(DefaultMaterial);
	SafeDelete(FbxLoaderInstance);
}

bool hos::com::DataManager::SaveScene(string_view path, Scene* scene)
{
	if (scene)
	{
		// �� ���� ����
		string _CheckFolder = path.data();

		_CheckFolder.append(L"Scenes\\");

		CreateDirectory(_CheckFolder.c_str(), nullptr);

		string fullPath = _CheckFolder + scene->GetName() + Scene::FILE_EXTENSION;
		mbstring _path = ut::UTF16ToAnsi(fullPath);
		std::vector<U8> data;
		data = scene->Serialize();
		return flatbuffers::SaveFile(_path.c_str(), reinterpret_cast<const char*>(data.data()), data.size(), true);
	}

	return false;
}

com::Scene* hos::com::DataManager::LoadScene(string_view pathName)
{
	static U32 buildIndex = 0;
	Scene* newScene = nullptr;
	mbstring _path = ut::UTF16ToAnsi(pathName.data());
	mbstring data;
	size_t prevPos = pathName.find_last_of('\\') + 1;
	size_t postPos = Min(pathName.find_last_of('.'), pathName.length());

	string name = pathName.data();
	string extension = ut::ToLower(name.substr(postPos, MAX_FILE_EXETENSION_LENGTH));
	name = name.substr(prevPos, postPos - prevPos);

	//Ȯ���� �˻�
	if (extension == Scene::FILE_EXTENSION)
	{
		//������ �ִ°�
		if (flatbuffers::LoadFile(_path.c_str(), true, &data))
		{
			newScene = new Scene();
			//��ø�������� ��°�
			if (newScene->Deserialize(data))
			{
				std::pair<std::map<string, com::Scene*>::iterator, bool> b;
				b = Scenes.insert({ name, newScene });
				if (b.second == false)
				{
					//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
					SafeDelete(newScene);
					newScene = GetScene(name);
				}
				else
				{
					newScene->SetBuildIndex(buildIndex++);
				}
			}
			else
			{
				//deserialize ���н�
				SafeDelete(newScene);
			}
		}
	}

	return newScene;
}

com::Scene* hos::com::DataManager::GetScene(string_view name) const
{
	try
	{
		Scene* s = Scenes.at(name.data());
		return new Scene(*s);
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid Scene : ";
		log.append(name);
		Debug->LogConsole(L"DataManager", log);
		return nullptr;
	}
}

int hos::com::DataManager::GetSceneBulidIndex(string_view name) const
{
	try
	{
		Scene* s = Scenes.at(name.data());
		return s->GetBuildIndex();
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid Scene : ";
		log.append(name);
		Debug->LogConsole(L"DataManager", log);
		return -1;
	}
}

com::Scene* hos::com::DataManager::GetScene(U32 index) const
{
	for (auto [name, scene] : Scenes)
	{
		if (scene->GetBuildIndex() == index)
		{
			return new Scene(*scene);
		}
	}

	return nullptr;
}

bool hos::com::DataManager::RemoveScene(U32 index)
{
	Scene* s = nullptr;
	for (auto [name, scene] : Scenes)
	{
		if (scene->GetBuildIndex() == index)
		{
			s = scene;
		}
	}
	if (s)
	{
		Scenes.erase(s->GetName());
		SafeDelete(s);

		return true;
	}

	return false;
}

bool hos::com::DataManager::RemoveScene(string_view name)
{
	Scene* s = GetScene(name);
	if (s)
	{
		Scenes.erase(s->GetName());
		SafeDelete(s);

		return true;
	}

	return false;
}

bool hos::com::DataManager::AddScene(Scene* scene)
{
	std::pair<std::map<string, com::Scene*>::iterator, bool> b;
	b = Scenes.insert({ scene->GetName(), scene });

	return b.second;
}

bool hos::com::DataManager::SavePrefab(string_view path, GameObject* prefab)
{
	if (prefab)
	{
		string fullPath = path.data() + prefab->GetName() + GameObject::FILE_EXTENSION;
		mbstring _path = ut::UTF16ToAnsi(fullPath);
		std::vector<U8> data;
		data = prefab->Serialize();
		return flatbuffers::SaveFile(_path.c_str(), reinterpret_cast<const char*>(data.data()), data.size(), true);
	}

	return false;
}

com::GameObject* hos::com::DataManager::LoadPrefab(string_view pathName)
{
	GameObject* newPrefab = nullptr;
	mbstring data;
	bool bAnimation = false;
	hos::string FileRoot, FileName, ModelName, ClipName;

	// ���� ��Ʈ ȹ��
	FileRoot = pathName.data();

	for (int i = 0; i < FileRoot.length(); i++)
	{
		if (FileRoot[i] == '\\')
		{
			FileRoot[i] = '/';
		}
	}

	// ���ϸ� ȹ��
	size_t pos1 = FileRoot.find_last_of('/');
	FileName = FileRoot.substr(pos1 + 1);

	// �𵨸� ȹ��
	size_t pos2 = FileName.find_last_of('@');
	ModelName = FileName.substr(0, pos2);

	// ���ϸ� '@' ������ ���η� �ִϸ��̼� ������ �Ǵ��Ѵ�.
	if (pos2 != SIZE_T_MAX)
	{
		bAnimation = true;
	}

	// Clip�� ȹ��
	size_t pos3 = Min(FileName.find_last_of('.'), FileName.length());
	ClipName = FileName.substr(pos2 + 1, (pos3 - 1) - pos2);

	// Ȯ���� ȹ��
	hos::string extension = ut::ToLower(FileName.substr(pos3));

	// LoadFile�Լ� ��� ���� FileRoot�� Multi Byte�� ��ȯ
	mbstring mbFileRoot = ut::UTF16ToAnsi(FileRoot);
	//Ȯ���� �˻�
	if (extension == GameObject::FILE_EXTENSION)
	{
		//������ �ִ°�
		if (flatbuffers::LoadFile(mbFileRoot.c_str(), true, &data))
		{
			newPrefab = new GameObject();
			//��ø�������� ��°�
			if (newPrefab->Deserialize(data))
			{
				std::pair<std::map<string, com::GameObject*>::iterator, bool> b;
				b = Prefabs.insert({ ModelName,newPrefab });
				if (b.second == false)
				{
					//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
					SafeDelete(newPrefab);
					newPrefab = GetPrefab(ModelName);
				}
			}
			else
			{
				//newPrefab = nullptr;
				SafeDelete(newPrefab);
			}
		}
	}
	else if (extension == GameObject::FBX_FILE)
	{
		size_t t = pathName.find_last_of('\\');
		FBX_Material_Path = pathName.substr(0, t);
		ModelPrefab* fbx = FbxLoaderInstance->LoadFbxFile(mbFileRoot.c_str());

		if (fbx)
		{
			Node* rootbone = fbx->GetRootNode();
			if (rootbone)
			{
				newPrefab = new GameObject(ModelName);

				_FBXMakeHierarchyFromBone(fbx, rootbone, newPrefab);

				//// �ִϸ��̼� �ӽ� ����
				if (bAnimation)
				{
					Animation* ani = new Animation();
					ani->SetAnimation(ModelName, ClipName);
					newPrefab->AddComponent(ani);
				}
			}

			std::pair<std::map<string, com::GameObject*>::iterator, bool> b;
			b = Prefabs.insert({ FileName,newPrefab });
			if (b.second == false)
			{
				//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
				SafeDelete(newPrefab);
				newPrefab = GetPrefab(FileName);
			}
		}
		FBX_Material_Path = g_Path;
	}

	return newPrefab;
}

com::GameObject* hos::com::DataManager::GetPrefab(string_view name) const
{
	//���ӿ�����Ʈ�� �ν��Ͻ��� ���� �����ؼ� �����Ѵ�
	try
	{
		GameObject* prefab = Prefabs.at(name.data());
		return new GameObject(*prefab);
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid Prefab : ";
		log.append(name);
		Debug->LogConsole(L"DataManager", log);
		return nullptr;
	}
}

bool hos::com::DataManager::SaveAudioClip(string_view path, AudioClip* clip)
{
	if (clip)
	{
		string fullPath = path.data() + clip->GetName() + AudioClip::FILE_EXTENSION;
		mbstring _path = ut::UTF16ToAnsi(fullPath);
		std::vector<U8> data;
		data = clip->Serialize();
		return flatbuffers::SaveFile(_path.c_str(), reinterpret_cast<const char*>(data.data()), data.size(), true);
	}

	return false;
}

AudioClip* hos::com::DataManager::LoadAudioClip(string_view pathName)
{
	AudioClip* newClip = nullptr;
	mbstring _path = ut::UTF16ToAnsi(pathName.data());
	mbstring data;
	size_t prevPos = pathName.find_last_of('\\') + 1;
	size_t postPos = Min(pathName.find_last_of('.'), pathName.length());

	string name = pathName.data();
	string extension = ut::ToLower(name.substr(postPos, MAX_FILE_EXETENSION_LENGTH));
	name = name.substr(prevPos, postPos - prevPos);

	if (extension == AudioClip::FILE_EXTENSION)
	{
		if (flatbuffers::LoadFile(_path.c_str(), true, &data))
		{
			newClip = new AudioClip();
			newClip->SetName(name);
			if (newClip->Deserialize(data))
			{
				std::pair<std::map<string, AudioClip*>::iterator, bool> b;
				b = AudioClips.insert({ name,newClip });
				if (b.second == false)
				{
					//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
					SafeDelete(newClip);
					newClip = GetAudioClip(name);
				}
			}
			else
			{
				SafeDelete(newClip);
			}
		}
	}
	else if (extension == AudioClip::WAV_FILE)
	{
		newClip = new AudioClip();
		if (g_Audio)
		{
			newClip->Sound = new DirectX::SoundEffect(g_Audio, pathName.data());
		}

		if (newClip->Sound)
		{
			newClip->SetName(name);
			std::pair<std::map<string, AudioClip*>::iterator, bool> b;
			b = AudioClips.insert({ name,newClip });
			if (b.second == false)
			{
				//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
				SafeDelete(newClip);
				newClip = GetAudioClip(name);
			}
		}
		else
		{
			SafeDelete(newClip);
		}
	}

	return newClip;
}

AudioClip* hos::com::DataManager::GetAudioClip(string_view name) const
{
	try
	{
		return AudioClips.at(name.data());
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid AudioClip : ";
		log.append(name);
		Debug->LogConsole(L"DataManager", log);
		return nullptr;
	}
}

bool hos::com::DataManager::SavePhysicMaterial(string_view path, PhysicMaterial* material)
{
	if (material)
	{
		string fullPath = path.data() + material->GetName() + PhysicMaterial::FILE_EXTENSION;
		mbstring _path = ut::UTF16ToAnsi(fullPath);
		std::vector<U8> data;
		data = material->Serialize();
		return flatbuffers::SaveFile(_path.c_str(), reinterpret_cast<const char*>(data.data()), data.size(), true);
	}

	return false;
}

PhysicMaterial* hos::com::DataManager::LoadPhysicMaterial(string_view pathName)
{
	PhysicMaterial* newMaterial = nullptr;
	mbstring _path = ut::UTF16ToAnsi(pathName.data());
	mbstring data;
	size_t prevPos = pathName.find_last_of('\\') + 1;
	size_t postPos = Min(pathName.find_last_of('.'), pathName.length());

	string name = pathName.data();
	string extension = ut::ToLower(name.substr(postPos, MAX_FILE_EXETENSION_LENGTH));
	name = name.substr(prevPos, postPos - prevPos);

	if (extension == PhysicMaterial::FILE_EXTENSION)
	{
		if (flatbuffers::LoadFile(_path.c_str(), true, &data))
		{
			newMaterial = new PhysicMaterial();
			if (newMaterial->Deserialize(data))
			{
				std::pair<std::map<string, PhysicMaterial*>::iterator, bool> b;
				b = PhysicMaterials.insert({ name,newMaterial });
				if (b.second == false)
				{
					//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
					SafeDelete(newMaterial);
					newMaterial = GetPhysicMaterial(name);
				}
			}
			else
			{
				SafeDelete(newMaterial);
			}
		}
	}

	return newMaterial;
}

PhysicMaterial* hos::com::DataManager::GetPhysicMaterial(string_view name)
{
	try
	{
		return PhysicMaterials.at(name.data());
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid PhysicMaterial : ";
		log.append(name);
		Debug->LogConsole(L"DataManager", log);
		return nullptr;
	}
}

bool hos::com::DataManager::SaveFont(string_view path, FontData* font)
{
	if (font)
	{
		string fullPath = path.data() + font->GetName() + FontData::FILE_EXTENSION;
		mbstring _path = ut::UTF16ToAnsi(fullPath);
		std::vector<U8> data;
		data = font->Serialize();
		return flatbuffers::SaveFile(_path.c_str(), reinterpret_cast<const char*>(data.data()), data.size(), true);
	}

	return false;
}

FontData* hos::com::DataManager::LoadFont(string_view pathName)
{
	FontData* newFont = nullptr;
	mbstring _path = ut::UTF16ToAnsi(pathName.data());
	mbstring data;
	size_t prevPos = pathName.find_last_of('\\') + 1;
	size_t postPos = Min(pathName.find_last_of('.'), pathName.length());

	string name = pathName.data();
	string extension = ut::ToLower(name.substr(postPos, MAX_FILE_EXETENSION_LENGTH));
	name = name.substr(prevPos, postPos - prevPos);

	if (extension == FontData::FILE_EXTENSION)
	{
		if (flatbuffers::LoadFile(_path.c_str(), true, &data))
		{
			newFont = new FontData();
			if (newFont->Deserialize(data))
			{
				std::pair<std::map<string, FontData*>::iterator, bool> b;
				b = Fonts.insert({ name,newFont });
				if (b.second == false)
				{
					//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
					SafeDelete(newFont);
					newFont = GetFont(name);
				}
			}
			else
			{
				SafeDelete(newFont);
			}
		}
	}
	else if (extension == FontData::SPRITE_FONT_FILE)
	{
		newFont = new FontData();
		newFont->SetName(name);

		std::ifstream file(_path, std::ios::binary);
		if (file.is_open())
		{
			std::stringstream ss;
			ss << file.rdbuf();

			newFont->Blob = ss.str();

			file.close();
		}
		else
		{
			SafeDelete(newFont);
		}

		bool b = g_Renderer->CreateSpriteFont(name, reinterpret_cast<uint8_t*>(newFont->Blob.data()), newFont->Blob.size());
		if (b)
		{
			std::pair<std::map<string, FontData*>::iterator, bool> b;
			b = Fonts.insert({ name, newFont });
			if (b.second == false)
			{
				//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
				SafeDelete(newFont);
				newFont = GetFont(name);
			}
		}
	}

	return newFont;
}

FontData* hos::com::DataManager::GetFont(string_view name)
{
	try
	{
		return Fonts.at(name.data());
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid Font : ";
		log.append(name);
		Debug->LogConsole(L"DataManager", log);
		return nullptr;
	}
}

bool hos::com::DataManager::SaveMesh(string_view path, Mesh* mesh)
{
	if (mesh)
	{
		string fullPath = path.data() + mesh->GetMeshName() + Mesh::FILE_EXTENSION;
		mbstring _path = ut::UTF16ToAnsi(fullPath);
		std::vector<U8> data;
		data = mesh->Serialize();
		return flatbuffers::SaveFile(_path.c_str(), reinterpret_cast<const char*>(data.data()), data.size(), true);
	}

	return false;
}

Mesh* hos::com::DataManager::LoadMesh(string_view pathName)
{
	Mesh* newMesh = nullptr;
	mbstring _path = ut::UTF16ToAnsi(pathName.data());
	mbstring data;
	size_t prevPos = pathName.find_last_of('\\') + 1;
	size_t postPos = Min(pathName.find_last_of('.'), pathName.length());

	string name = pathName.data();
	string extension = ut::ToLower(name.substr(postPos, MAX_FILE_EXETENSION_LENGTH));
	name = name.substr(prevPos, postPos - prevPos);

	if (extension == Mesh::FILE_EXTENSION)
	{
		if (flatbuffers::LoadFile(_path.c_str(), true, &data))
		{
			newMesh = new Mesh();
			if (newMesh->Deserialize(data))
			{
				std::pair<std::unordered_map<string, Mesh*>::iterator, bool> b;
				b = MeshList.insert({ name,newMesh });
				if (b.second == false)
				{
					//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
					SafeDelete(newMesh);
					newMesh = GetMesh(name);
				}
				else
				{
					if (g_Renderer)
					{
						g_Renderer->CreateMeshVariable(newMesh->GetMeshName(), newMesh->GetVtxDataAdress(), newMesh->GetVtxCount(), newMesh->GetIndexDataAdress(), newMesh->GetIndexCount());
					}
				}
			}
			else
			{
				SafeDelete(newMesh);
			}
		}
	}

	return newMesh;
}

Mesh* hos::com::DataManager::GetMesh(string_view name)
{
	try
	{
		return MeshList.at(name.data());
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid Mesh : ";
		log.append(name);
		Debug->LogConsole(L"DataManager", log);
		return nullptr;
	}
}

bool hos::com::DataManager::SaveAnimationBundle(string_view path, AnimationBundle* bundle)
{
	if (bundle)
	{
		string fullPath = path.data() + bundle->GetName() + AnimationBundle::FILE_EXTENSION;
		mbstring _path = ut::UTF16ToAnsi(fullPath);
		std::vector<U8> data;
		data = bundle->Serialize();
		return flatbuffers::SaveFile(_path.c_str(), reinterpret_cast<const char*>(data.data()), data.size(), true);
	}

	return false;
}

AnimationBundle* hos::com::DataManager::LoadAnimationBundle(string_view pathName)
{
	AnimationBundle* newBundle = nullptr;
	mbstring _path = ut::UTF16ToAnsi(pathName.data());
	mbstring data;
	size_t prevPos = pathName.find_last_of('\\') + 1;
	size_t postPos = Min(pathName.find_last_of('.'), pathName.length());

	string name = pathName.data();
	string extension = ut::ToLower(name.substr(postPos, MAX_FILE_EXETENSION_LENGTH));
	name = name.substr(prevPos, postPos - prevPos);

	if (extension == AnimationBundle::FILE_EXTENSION)
	{
		if (flatbuffers::LoadFile(_path.c_str(), true, &data))
		{
			newBundle = new AnimationBundle(name);
			if (newBundle->Deserialize(data))
			{
				std::pair<std::unordered_map<string, AnimationBundle*>::iterator, bool> b;
				b = AnimationList.insert({ name,newBundle });
				if (b.second == false)
				{
					//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
					SafeDelete(newBundle);
					newBundle = GetAnimationBundle(name);
				}
			}
			else
			{
				SafeDelete(newBundle);
			}
		}
	}

	return newBundle;
}

AnimationBundle* hos::com::DataManager::GetAnimationBundle(string_view name)
{
	// �ϳ��� �ִٸ�
	if (AnimationList.count(name.data()) != 0)
	{
		return AnimationList.at(name.data());
	}

	return nullptr;
}

bool hos::com::DataManager::SaveMaterial(string_view path, Material* material)
{
	if (material)
	{
		string fullPath = path.data() + material->GetMaterialName() + Material::FILE_EXTENSION;
		mbstring _path = ut::UTF16ToAnsi(fullPath);
		std::vector<U8> data;
		data = material->Serialize();
		return flatbuffers::SaveFile(_path.c_str(), reinterpret_cast<const char*>(data.data()), data.size(), true);
	}

	return false;
}

Material* hos::com::DataManager::LoadMaterial(string_view pathName)
{
	Material* newMaterial = nullptr;
	mbstring _path = ut::UTF16ToAnsi(pathName.data());
	mbstring data;
	size_t prevPos = pathName.find_last_of('\\') + 1;
	size_t postPos = Min(pathName.find_last_of('.'), pathName.length());

	string name = pathName.data();
	string extension = ut::ToLower(name.substr(postPos, MAX_FILE_EXETENSION_LENGTH));
	name = name.substr(prevPos, postPos - prevPos);

	if (extension == Material::FILE_EXTENSION)
	{
		if (flatbuffers::LoadFile(_path.c_str(), true, &data))
		{
			newMaterial = new Material();
			if (newMaterial->Deserialize(data))
			{
				std::pair<std::unordered_map<string, Material*>::iterator, bool> b;
				b = MaterialList.insert({ name,newMaterial });
				if (b.second == false)
				{
					//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
					SafeDelete(newMaterial);
					newMaterial = GetMaterial(name);
				}
			}
			else
			{
				SafeDelete(newMaterial);
			}
		}
	}

	return newMaterial;
}

bool hos::com::DataManager::SaveUIAnimation(string_view pathName, BoneAnimation* anim)
{
	if (anim)
	{
		string fullPath = pathName.data();
		fullPath.append(BoneAnimation::FILE_EXTENSION);
		mbstring _path = ut::UTF16ToAnsi(fullPath);
		std::vector<U8> data;
		data = anim->Serialize();
		return flatbuffers::SaveFile(_path.c_str(), reinterpret_cast<const char*>(data.data()), data.size(), true);
	}

	return false;
}

BoneAnimation* hos::com::DataManager::LoadUIAnimation(string_view pathName)
{
	BoneAnimation* newAni = nullptr;
	mbstring _path = ut::UTF16ToAnsi(pathName.data());
	mbstring data;
	size_t prevPos = pathName.find_last_of('\\') + 1;
	size_t postPos = Min(pathName.find_last_of('.'), pathName.length());

	string name = pathName.data();
	string extension = ut::ToLower(name.substr(postPos, MAX_FILE_EXETENSION_LENGTH));
	name = name.substr(prevPos, postPos - prevPos);

	if (extension == BoneAnimation::FILE_EXTENSION)
	{
		if (flatbuffers::LoadFile(_path.c_str(), true, &data))
		{
			newAni = new BoneAnimation();
			if (newAni->Deserialize(data))
			{
				std::pair<std::unordered_map<string, BoneAnimation*>::iterator, bool> b;
				b = UIAnimationList.insert({ name,newAni });
				if (b.second == false)
				{
					//���� �̸��� �����Ͱ� �̹� �ε�Ǿ�������
					SafeDelete(newAni);
					newAni = GetUIAnimation(name);
				}
			}
			else
			{
				SafeDelete(newAni);
			}
		}
	}
	else if (extension == BoneAnimation::UI_ANI_FBX)
	{
		LoadUIFile(pathName);
	}

	return newAni;
}

void hos::com::DataManager::LoadData(string_view pathName)
{
	string path = hos::ut::ToLower(pathName.data());
	size_t postPos = pathName.find_last_of('.');
	string extension = path.substr(postPos, MAX_FILE_EXETENSION_LENGTH).data();

	if (extension == Scene::FILE_EXTENSION)
	{
		LoadScene(path);
	}
	else if (extension == GameObject::FILE_EXTENSION || extension == GameObject::FBX_FILE)
	{
		LoadPrefab(path);
	}
	else if (extension == AudioClip::FILE_EXTENSION || extension == AudioClip::WAV_FILE)
	{
		LoadAudioClip(path);
	}
	else if (extension == PhysicMaterial::FILE_EXTENSION)
	{
		LoadPhysicMaterial(path);
	}
	else if (extension == FontData::FILE_EXTENSION || extension == FontData::SPRITE_FONT_FILE)
	{
		LoadFont(path);
	}
	else if (extension == Mesh::FILE_EXTENSION)
	{
		LoadMesh(path);
	}
	else if (extension == Material::FILE_EXTENSION)
	{
		LoadMaterial(path);
	}
	else if (extension == AnimationBundle::FILE_EXTENSION)
	{
		LoadAnimationBundle(path);
	}
	else if (extension == L".png" || extension == L".jpg")
	{
		LoadTextureOnly(path);
	}
	else if (extension == BoneAnimation::FILE_EXTENSION || extension == BoneAnimation::UI_ANI_FBX)
	{
		LoadUIAnimation(path);
	}
	else if (extension == L".dds")
	{
		LoadSkyBox(path);
	}
}

void hos::com::DataManager::LoadAssets()
{
	std::filesystem::path p(g_Path);
	std::vector<string> paths;
	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path() / "Assets"))
	{
		string path = entry.path();
		LoadData(path);
	}
}

void hos::com::DataManager::LoadFbxFile(const CHAR* fileRoot)
{
	FbxLoaderInstance->LoadFbxFile(fileRoot);
}

BoneAnimation* hos::com::DataManager::LoadUIFile(hos::string_view fileRoot)
{
	// mbstring���� ��ȯ(fbxLoader�� ����)
	hos::mbstring _fileRoot = ut::UTF16ToAnsi(fileRoot);

	return FbxLoaderInstance->LoadFbxFile_UIAnimation(_fileRoot);
}

//void hos::com::DataManager::CreateALLRenderVariable_InDataManager()
//{
//	for (auto elem : MeshList)
//	{
//		Mesh& _mesh = *(MeshList[elem.first]);
//
//		g_Renderer->CreateMeshVariable(_mesh.GetMeshName(), _mesh.GetVtxDataAdress(), _mesh.GetVtxCount(), _mesh.GetIndexDataAdress(), _mesh.GetIndexCount());
//	}
//
//	for (auto elem : MaterialList)
//	{
//		Material& _material = *(MaterialList[elem.first]);
//
//		if (_material.IsThereAlbedo())
//		{
//			g_Renderer->CreateTextureVariable(_material.GetTexNameAlbedo(), L"../../Resource/Texture/");
//		}
//
//		if (_material.IsThereNormal())
//		{
//			g_Renderer->CreateTextureVariable(_material.GetTexNameNormal(), L"../../Resource/Texture/");
//		}
//
//		if (_material.IsThereARM())
//		{
//			g_Renderer->CreateTextureVariable(_material.GetTexNameARM(), L"../../Resource/Texture/");
//		}
//
//		if (_material.IsThereEmiss())
//		{
//			g_Renderer->CreateTextureVariable(_material.GetTexNameEmiss(), L"../../Resource/Texture/");
//		}
//	}
//}

void hos::com::DataManager::ReleaseAllRenderVariable_InDataManager()
{
	g_Renderer->AllClearMeshVariableList();
	g_Renderer->AllClearTextureVariableList();
}

hos::Material* hos::com::DataManager::GetMaterial(hos::string materialName)
{
	try
	{
		return this->MaterialList.at(materialName);
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid RenderMaterial : ";
		log.append(materialName);
		Debug->LogConsole(L"DataManager", log);
		return this->DefaultMaterial;
	}
};


hos::BoneAnimation* hos::com::DataManager::GetUIAnimation(hos::string_view aniName)
{
	try
	{
		return this->UIAnimationList.at(aniName.data());
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid UI_Animation : ";
		log.append(aniName);
		Debug->LogConsole(L"DataManager", log);
		return nullptr;
	}
}

void hos::com::DataManager::LoadTexture(hos::string texName)
{
	bool bAlpha = false;

	if (!g_Renderer)
	{
		return;
	}
	if (texName.size() > 0)
	{
		hos::string _finalRoot = FBX_Material_Path + L"\\Texture\\" + texName;

		// ���ҽ� ���� �õ� ��, ���� ���θ� üũ�Ѵ�.
		bool b = g_Renderer->CreateTextureVariable(texName, _finalRoot, &bAlpha);

		if (b == false)
		{
			_finalRoot = g_Path + L"\\Assets\\Texture\\" + texName;

			b = g_Renderer->CreateTextureVariable(texName, _finalRoot, &bAlpha);
		}

		if (b)
		{
			// �ؽ��� ����Ʈ ���� �ش� name���� ��ϵ� Texture�� ���ٸ�
			if (TextureList.count(texName) == 0)
			{
				TextureList.insert({ texName , bAlpha });
			}

			// �ؽ��� �ε忡�� ���� �ߴµ�, �ؽ��� ����Ʈ�� �̹� �ִ� ���
			else
			{
			#ifdef _DEBUG
				MessageBoxW(NULL, L"Resource ������ ġ������ ������ �߻��߽��ϴ�. Renderer���� DataManager�� �̹� �����Ǿ� �ִ� Texture�� �̸��� ������ �ؽ����� ������ �����߽��ϴ�.", L"Critical Error", MB_OK);
				ASSERT(1);
			#endif
			}
		}
	}
}

void hos::com::DataManager::LoadTextureOnly(hos::string filePath)
{
	bool bAlpha = false;
	size_t extentionCut = filePath.find_last_of(L"\\");
	hos::string _fileName = filePath.substr(extentionCut + 1);

	// ���ҽ� ���� �õ� ��, ���� ���θ� üũ�Ѵ�.
	bool b = g_Renderer->CreateTextureVariable(_fileName, filePath);

	if (b == true)
	{
		{
			// �ؽ��� ����Ʈ ���� �ش� name���� ��ϵ� Texture�� ���ٸ�
			if (TextureList.count(_fileName) == 0)
			{
				TextureList.insert({ _fileName , bAlpha });
			}

			// �ؽ��� �ε忡�� ���� �ߴµ�, �ؽ��� ����Ʈ�� �̹� �ִ� ���
			else
			{
			#ifdef _DEBUG
				MessageBoxW(NULL, L"Resource ������ ġ������ ������ �߻��߽��ϴ�. Renderer���� DataManager�� �̹� �����Ǿ� �ִ� Texture�� �̸��� ������ �ؽ����� ������ �����߽��ϴ�.", L"Critical Error", MB_OK);
				ASSERT(1);
			#endif
			}
		}
	}
}

void hos::com::DataManager::ReleaseTexture(hos::string textureName)
{
	// ���� �ؽ��ĸ� �����ַ� �մϴ�.
	if (TextureList.count(textureName) == 0)
	{
		Debug->LogConsole(L"DataManager", L"�ؽ��� ����Ʈ�� ���� �ؽ��ĸ� ���� �� �����ϴ�.");
	}

	// �ִ� �ؽ����Դϴ�.
	else
	{
		g_Renderer->ReleaseTextureVariable(textureName);
	}
}

bool hos::com::DataManager::HasTexture(string_view name)
{
	if (TextureList.count(name.data()) == 0)
	{
		return false;
	}

	return true;
}

bool hos::com::DataManager::IsAlpha(string_view name)
{
	if (TextureList.count(name.data()) == 0)
	{
		//Debug->LogConsole(L"DataManager", L"�ؽ��ĸ���Ʈ�� ���� �ؽ����� Alpha ���θ� ��û�߽��ϴ�.");
	}

	else
	{
		return TextureList.at(name.data());
	}

	return false;
}

void hos::com::DataManager::LoadSkyBox(hos::string filePath)
{
	size_t extentionCut = filePath.find_last_of(L"\\");
	hos::string _fileName = filePath.substr(extentionCut + 1);

	// ���ҽ� ���� �õ� ��, ���� ���θ� üũ�Ѵ�.
	bool b = false;

	if (g_Renderer)
	{
		b = g_Renderer->LoadSkyBox(_fileName, filePath);
	}

	if (b == true)
	{
		{
			// ���� �� �ؽ��� ����Ʈ �˻�.
			auto it = find(SkyBoxList.begin(), SkyBoxList.end(), _fileName);
			if (it == SkyBoxList.end())
			{
				// �ؽ��� ����Ʈ�� �����Ƿ� ����Ʈ�� �߰�(�̻����� ���)
				SkyBoxList.push_back(_fileName);
			}

			// ���� �ߴµ�, �ؽ��� ����Ʈ�� �ִٰ�..?
			else
			{
				/// �ϴ� �ƹ� �͵� ������ ����.
			}
		}
	}
}

bool hos::com::DataManager::HasSkyBox(string_view name)
{
	for (string texture : SkyBoxList)
	{
		if (texture == name)
		{
			return true;
		}
	}
	return false;
}

HCURSOR hos::com::DataManager::LoadMouseCursor(string pathName)
{
	size_t prevPos = pathName.find_last_of('\\') + 1;
	size_t postPos = Min(pathName.find_last_of('.'), pathName.length());

	string name = pathName.data();
	name = name.substr(prevPos, postPos - prevPos);
	HCURSOR cur = LoadCursorFromFileW(pathName.c_str());
	if (cur != NULL)
	{
		MouseCursors.insert({ name,cur });
	}
	else
	{
		cur = nullptr;
	}
	return cur;
}

HCURSOR hos::com::DataManager::GetMouseCursor(string name)
{
	try
	{
		return MouseCursors.at(name.data());
	}
	catch (std::out_of_range e)
	{
		string log = L"Invalid MouseCursor : " + name;
		Debug->LogConsole(L"DataManager", log);
		return nullptr;
	}
}

void hos::com::DataManager::FBXMakeHierarchyFromBone(Node* parent, GameObject* parentObj)
{
	for (Node* child : parent->GetChilds())
	{
		GameObject* obj = new GameObject(child->GetName());
		parentObj->transform->AttachChild(obj->transform);

		AnimationBundle* _aniBundle = g_DataManager->GetAnimationBundle(L"box");

		I32 ppap = _aniBundle->GetBoneIndexByName(child->GetName());
		hos::Vector3 vPos = _aniBundle->GetBoneTransformList()[ppap].vPos;
		hos::Vector3 vRot = _aniBundle->GetBoneTransformList()[ppap].vRot;
		hos::Vector3 vScale = _aniBundle->GetBoneTransformList()[ppap].vScale;
		hos::Rotator RotRot(vRot.x, vRot.y, vRot.z);
		obj->transform->SetPosition(vPos);
		obj->transform->SetRotation(RotRot);
		obj->transform->SetScale(vScale);

		FBXMakeHierarchyFromBone(child, obj);
	}
}

void hos::com::DataManager::_FBXMakeHierarchyFromBone(ModelPrefab* fbx, Node* parent, GameObject* parentObj)
{
	std::vector<Node*>& childs = parent->GetChilds();
	int childCount = (int)childs.size();

	for (int i = 0; i < childCount; i++)
	{
		Node* child = childs[i];

		GameObject* obj = new GameObject(child->GetName());
		parentObj->transform->AttachChild(obj->transform);

		hos::Vector3 vPos = child->GetPos();
		hos::Vector3 vRot = child->GetRot();
		hos::Vector3 vScale = child->GetScale();
		hos::Rotator vRotator(vRot.x, vRot.y, vRot.z);
		obj->transform->SetPosition(vPos);
		obj->transform->SetRotation(vRotator);
		obj->transform->SetScale(vScale);

		// Mesh
		if (child->GetNodeType() == Node::NODE_TYPE::MESH)
		{
			Mesh* _mesh = child->GetMesh();

			if (_mesh)
			{
				MeshFilter* _meshFilter = new MeshFilter();
				_meshFilter->SetMeshData(_mesh);
				obj->AddComponent(_meshFilter);

				if (fbx->IsBoolSkin())
				{
					SkinnedMeshRenderer* mr = new SkinnedMeshRenderer();
					mr->SetMaterial(_mesh->GetMaterialName());
					obj->AddComponent(mr);
				}
				else
				{
					MeshRenderer* mr = new MeshRenderer();
					mr->SetMaterial(_mesh->GetMaterialName());
					obj->AddComponent(mr);
				}
			}
		}

		// ���� child�� child������ 0�� �ƴ϶�� ��� ����
		if (child->GetChilds().size() != 0)
		{
			_FBXMakeHierarchyFromBone(fbx, child, obj);
		}
	}
}

void hos::com::DataManager::SaveAll(string_view path)
{
	string _CheckFolder;

	//////////////////////////////////////////////////////////////////////////
	/// �ؽ��� ����

	_CheckFolder = path.data();

	_CheckFolder.append(L"Texture\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	//////////////////////////////////////////////////////////////////////////
	/// ����� Ŭ�� ����

	_CheckFolder = path.data();

	_CheckFolder.append(L"Sound\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	for (auto [name, data] : AudioClips)
	{
		SaveAudioClip(_CheckFolder, data);
	}

	//////////////////////////////////////////////////////////////////////////
	/// �Ž� ����

	_CheckFolder = path.data();

	_CheckFolder.append(L"Mesh\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	for (auto [name, data] : MeshList)
	{
		SaveMesh(_CheckFolder, data);
	}

	//////////////////////////////////////////////////////////////////////////
	/// �ִϸ��̼� Ŭ��? ����? ����

	_CheckFolder = path.data();

	_CheckFolder.append(L"Anim\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	for (auto [name, data] : AnimationList)
	{
		SaveAnimationBundle(_CheckFolder, data);
	}

	//////////////////////////////////////////////////////////////////////////
	/// ��Ʈ ����

	_CheckFolder = path.data();

	_CheckFolder.append(L"Font\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	for (auto [name, data] : Fonts)
	{
		SaveFont(_CheckFolder, data);
	}

	//////////////////////////////////////////////////////////////////////////
	/// ���̴� ����

	_CheckFolder = path.data();

	_CheckFolder.append(L"Shader\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	//////////////////////////////////////////////////////////////////////////
	/// ���� ���׸��� ����

	_CheckFolder = path.data();

	_CheckFolder.append(L"PhysMat\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	for (auto [name, data] : PhysicMaterials)
	{
		SavePhysicMaterial(_CheckFolder, data);
	}

	//////////////////////////////////////////////////////////////////////////
	/// ������ ����

	/*_CheckFolder = path.data();

	_CheckFolder.append(L"Prefabs\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	for (auto [name, data] : Prefabs)
	{
		SavePrefab(_CheckFolder, data);
	}*/

	//////////////////////////////////////////////////////////////////////////
	/// ���׸��� ����

	_CheckFolder = path.data();

	_CheckFolder.append(L"Material\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	for (auto [name, data] : MaterialList)
	{
		SaveMaterial(_CheckFolder, data);
	}

	//////////////////////////////////////////////////////////////////////////
	/// UI ���ϸ��̼�

	_CheckFolder = path.data();

	_CheckFolder.append(L"Anim\\");

	CreateDirectory(_CheckFolder.c_str(), nullptr);

	for (auto [name, data] : UIAnimationList)
	{
		SaveUIAnimation(_CheckFolder + name, data);
	}
}

