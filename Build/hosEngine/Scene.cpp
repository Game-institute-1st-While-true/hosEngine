#include "Scene.h"
#include "Transform.h"
#include "AudioListener.h"
#include "Camera.h"
#include "Collision.h"
#include "../GzzNet/Note.h"

using namespace hos;

bool g_Layer[com::Scene::MAX_LAYER][com::Scene::MAX_LAYER] = { true, };

physx::PxFilterFlags SimulateFilterShader(
	physx::PxFilterObjectAttributes attributes0,
	physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1,
	physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags,
	const void* constantBlock,
	physx::PxU32 constantBlockSize)
{
	if (!g_Layer[filterData0.word0][filterData1.word0])
	{
		return physx::PxFilterFlag::eSUPPRESS;
	}

	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlags();
	}

	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS | physx::PxPairFlag::eDETECT_CCD_CONTACT;

	return physx::PxFilterFlags();
}

hos::com::Scene::Scene() :IsLoaded(true), BuildIndex(VALID), Listener(nullptr), MainCamera(nullptr),
PxScene(nullptr), PxSceneCallback(nullptr), Gravity(DEFAULT_GRAVITY), SceneTime(nullptr)
{
	if (g_Px)
	{
		PxSceneCallback = new CollisionCallback();
		physx::PxSceneDesc sceneDesc(g_Px->Physics->getTolerancesScale());

		sceneDesc.gravity = Gravity;
		sceneDesc.cpuDispatcher = g_Px->Dispatcher;
		sceneDesc.filterShader = SimulateFilterShader;
		sceneDesc.simulationEventCallback = PxSceneCallback;
		sceneDesc.bounceThresholdVelocity = 2.0f;
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;

		PxScene = g_Px->Physics->createScene(sceneDesc);

		physx::PxPvdSceneClient* pvdClient = PxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}

	for (int i = 0; i < MAX_LAYER; i++)
	{
		for (int j = 0; j < MAX_LAYER; j++)
		{
			Layer[i][j] = true;
		}
	}

	LayerName[DEFAULT_LAYER] = L"Default";
	LayerName[1] = L"TransparentFX";
	LayerName[2] = L"Ignore RayCast";
	LayerName[5] = L"UI";
	LayerName[7] = L"Reserved Layer";

	SceneTime = new Clock();
}

hos::com::Scene::Scene(string_view name) : Scene()
{
	SetName(name);
}

hos::com::Scene::Scene(const Scene& dest) : Name(dest.Name), IsLoaded(dest.IsLoaded), BuildIndex(dest.BuildIndex), Listener(nullptr), MainCamera(nullptr),
PxScene(nullptr), PxSceneCallback(nullptr), Gravity(dest.Gravity), SceneTime(nullptr)
{
	if (g_Px)
	{
		PxSceneCallback = new CollisionCallback();
		physx::PxSceneDesc sceneDesc(g_Px->Physics->getTolerancesScale());

		sceneDesc.gravity = Gravity;
		sceneDesc.cpuDispatcher = g_Px->Dispatcher;
		sceneDesc.filterShader = SimulateFilterShader;
		sceneDesc.simulationEventCallback = PxSceneCallback;
		sceneDesc.bounceThresholdVelocity = 2.0f;

		PxScene = g_Px->Physics->createScene(sceneDesc);

		physx::PxPvdSceneClient* pvdClient = PxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}

	for (int i = 0; i < MAX_LAYER; i++)
	{
		for (int j = 0; j < MAX_LAYER; j++)
		{
			Layer[i][j] = dest.Layer[i][j];
		}
		LayerName[i] = dest.LayerName[i];
	}

	SceneTime = new Clock();

	for (GameObject* obj : dest.RootGameObjects)
	{
		GameObject* copy = new GameObject(*obj);
		AddGameObjectRecursive(copy);
	}

	ExecutionOrderRebuild();
}

hos::com::Scene::~Scene()
{
	for (GameObject* gameObject : GameObjects)
	{
		delete gameObject;
	}
	GameObjects.clear();
	RootGameObjects.clear();
	ScriptExecution.clear();
	SafeDelete(PxSceneCallback);
	SafeRelease(PxScene);
	SafeDelete(SceneTime);
}

com::AudioListener* hos::com::Scene::GetAudioListener()
{
	return Listener;
}

void hos::com::Scene::AddCollision(Collision* collision)
{
	if (g_Px)
	{
		PxSceneCallback->Collisions.insert(std::make_pair(collision->GetShape(), collision));
	}
}

void hos::com::Scene::RemoveCollision(Collision* collision)
{
	if (PxSceneCallback)
	{
		if (PxSceneCallback->Collisions.size() > 0)
		{
			std::map<physx::PxShape*, Collision*>::iterator t = PxSceneCallback->Collisions.find(collision->GetShape());
			if (t != PxSceneCallback->Collisions.end())
			{
				PxSceneCallback->Collisions.erase(t);
			}
		}
	}
}

string hos::com::Scene::GetName()
{
	return Name;
}

void hos::com::Scene::SetName(string_view name)
{
	Name = name;
}

bool hos::com::Scene::GetIsLoaded()
{
	return IsLoaded;
}

void hos::com::Scene::SetIsLoaded(bool b)
{
	if (!IsValid())
	{
		IsLoaded = b;
	}
}

int hos::com::Scene::GetBuildIndex()
{
	return BuildIndex;
}

void hos::com::Scene::SetBuildIndex(int index)
{
	BuildIndex = index;
}

bool hos::com::Scene::IsValid()
{
	return IsLoaded ? BuildIndex == VALID : true;
}

string hos::com::Scene::ObjectNameDuplicateCheck(string_view name)
{
	// TODO: 중복된 이름에 문제가 있음..
	//static int index = 0;
	string _name(name.data());

	for (auto obj : GameObjects)
	{
		if (name.data() == obj->GetName())
		{

			//index++;
			//_name += L" (" + std::to_wstring(index) + L")";

			// 끝에 괄호와 번호를 확인하자
			string::size_type _Numbering = _name.find_last_of(L"(");

			if (_Numbering > _name.length())
			{
				// 괄호와 번호가 없다면 (1) 을 붙이자
				_name += L"(" + std::to_wstring(1) + L")";
			}
			else
			{
				// 있다면 ('') 에서 ''에 해당하는 숫자를 증가시켜 넣는다.
				string _GameObjectName = _name.substr(0, _Numbering + 1);
				string _ObjectNumber = _name.substr(_Numbering + 1);

				// 맨 끝에 괄호도 빼야 함
				string::size_type _LastBracketRemove = _ObjectNumber.find_last_of(L")");

				string _RealObjectNumber = _ObjectNumber.substr(0, _LastBracketRemove);

				int _RealNumber = stoi(_RealObjectNumber);

				_RealNumber++;

				string _ChangedNumber = std::to_wstring(_RealNumber) + L")";

				_GameObjectName = _GameObjectName + _ChangedNumber;

				_name = _GameObjectName;

			}

			_name = ObjectNameDuplicateCheck(_name);

			break;
		}
	}
	return _name;
}

void hos::com::Scene::ExecutionOrderRebuild()
{
	ScriptExecution.clear();
	for (GameObject* obj : GameObjects)
	{
		for (Component* com : obj->GetComponents())
		{
			ScriptExecution.insert({ com->ExecutionOrder, com });
		}
	}
}

void hos::com::Scene::AddGameObject(GameObject* obj, GameObject* parent)
{
	if (parent)
	{
		parent->transform->AttachChild(obj->transform);
	}
	else
	{
		RootGameObjects.emplace_back(obj);
	}

	obj->Bind(this);
	obj->SetName(obj->GetName());
	GameObjects.emplace_back(obj);
}

void hos::com::Scene::AddGameObjectRecursive(GameObject* obj, GameObject* rootParent)
{
	AddGameObject(obj, rootParent);

	std::vector<Transform*> v = obj->transform->GetChilds();
	for (int i = 0; i < v.size(); i++)
	{
		AddGameObjectRecursive(v[i]->m_GameObject, obj);
	}
}

void hos::com::Scene::RemoveGameObject(GameObject* obj)
{
	int index = 0;
	for (auto it = GameObjects.begin(); it != GameObjects.end(); ++it, index++)
	{
		if (obj == *it)
		{
			// 최상위 오브젝트라면 최상위 오브젝트 벡터에서 없애야 함
			if (nullptr == obj->GetParent())
			{
				RemoveRootGameObject(obj);
			}
			else
			{
				// 부모가 있으면 부모에서 연결을 해제해야 함
				obj->GetParent()->transform->DetachChild(obj->transform);
				RemoveRootGameObject(obj);
			}

			// 하위 오브젝트들이 있다면 없애야 함
			while (obj->transform->GetChildCount() != 0)
			{
				GameObject* _RemoveChild = obj->transform->GetChild(0)->m_GameObject;
				obj->transform->DetachChild(0);
				RemoveGameObject(_RemoveChild);
			}

			//if (obj->GetComponent<Transform>()->GetParent() && obj->GetComponent<Transform>()->GetParent()->GetChildCount() > 0)
			//{
			//	obj->GetComponent<Transform>()->GetParent()->DetachChild(0);
			//}

			//obj->UnBind(index);
			GameObject* _DeleteGameObject = *it;
			GameObjects.erase(it);
			SafeDelete(_DeleteGameObject);

			return;
		}
	}
}

std::vector<com::GameObject*>& hos::com::Scene::GetObjects()
{
	return GameObjects;
}

std::vector<com::GameObject*>& hos::com::Scene::GetRoots()
{
	return RootGameObjects;
}

unsigned long long hos::com::Scene::GetHashCode()
{
	return (reinterpret_cast<long long>(this));
}

void hos::com::Scene::SetLayerName(U32 Layer, string_view name)
{
	if (Layer >= BUILTIN_LAYER_COUNT && Layer < MAX_LAYER)
	{
		LayerName[Layer] = name;
	}
}

string hos::com::Scene::GetLayerName(U32 Layer) const
{
	if (Layer < MAX_LAYER)
	{
		return LayerName[Layer];
	}

	return string();
}

void hos::com::Scene::SetLayerCollision(U32 Layer1, U32 Layer2, bool enable)
{
	if (Layer1 < MAX_LAYER && Layer2 < MAX_LAYER)
	{
		Layer[Layer1][Layer2] = enable;
		Layer[Layer2][Layer1] = enable;
	}
}

bool hos::com::Scene::GetLayerCollision(U32 Layer1, U32 Layer2) const
{
	if (Layer1 < MAX_LAYER && Layer2 < MAX_LAYER)
	{
		return Layer[Layer1][Layer2];
	}

	return false;
}

Vector3 hos::com::Scene::GetGravity() const
{
	return Gravity;
}

void hos::com::Scene::SetGravity(Vector3 gravity)
{
	Gravity = gravity;
}

void hos::com::Scene::PhysicsUpdate()
{
	//PhysX Calculate
	float deltatime = (float)Time->DeltaTime();
	PxScene->simulate(deltatime);
	PxScene->fetchResults(true);
}

void hos::com::Scene::SetAudioListener(AudioListener* listener)
{
	if (listener)
	{
		Listener = listener;
	}
	else
	{
		Debug->LogConsole(L"Scene", L"Scene is not include MainAudioListener");
	}
}

void hos::com::Scene::SetMainCamera(Camera* cam)
{
	if (cam)
	{
		MainCamera = cam;
	}
	else
	{
		Debug->LogConsole(L"Scene", L"Scene is not include MainCamera");
	}
}

com::Camera* hos::com::Scene::GetMainCamera() const
{
	return MainCamera;
}

bool hos::com::Scene::RayCast(Vector3 pos, Vector3 dir, float distance, eRaycastMode mode, eRaycastType type, U32 num)
{
	dir.Normalize();
	physx::PxRaycastBuffer hit;
	physx::PxQueryFilterData fd;

	switch (type)
	{
	case hos::com::eRaycastType::All:
		fd.flags |= physx::PxQueryFlag::eDYNAMIC;
		fd.flags |= physx::PxQueryFlag::eSTATIC;
		break;
	case hos::com::eRaycastType::DYNAMIC:
		fd.flags |= physx::PxQueryFlag::eDYNAMIC;
		break;
	case hos::com::eRaycastType::STATIC:
		fd.flags |= physx::PxQueryFlag::eSTATIC;
		break;
	}

	for (int i = 0; i < MAX_LAYER; i++)
	{
		//2번레이어가 RaycastLayer
		if (Layer[2][i])
		{
			fd.data.word0 |= (1 << i);
		}
	}

	bool isHit = false;

	switch (mode)
	{
	default:
	case hos::com::eRaycastMode::CLOSEST:
	{
		//physx::PxRaycastHit* rayhit = new physx::PxRaycastHit[1];
		//hit = physx::PxRaycastBuffer(rayhit, 1);
	}
	break;
	case hos::com::eRaycastMode::ANY:
		fd.flags |= physx::PxQueryFlag::eANY_HIT;
		break;
	case hos::com::eRaycastMode::MULTIPLE:
	{
		physx::PxU32 n = num;
		physx::PxRaycastHit* rayhit = new physx::PxRaycastHit[n];
		hit = physx::PxRaycastBuffer(rayhit, n);
	}
	break;
	}
	isHit = PxScene->raycast(pos, dir, distance, hit, physx::PxHitFlag::eDEFAULT, fd);

	return isHit;
}

std::vector<const physx::PxRaycastHit*> hos::com::Scene::RayCastAll(Vector3 pos, Vector3 dir, eRaycastType type)
{
	dir.Normalize();
	RaycastInfinityCallBack hit(this);
	physx::PxQueryFilterData fd;
	switch (type)
	{
	case hos::com::eRaycastType::All:
		fd.flags |= physx::PxQueryFlag::eDYNAMIC;
		fd.flags |= physx::PxQueryFlag::eSTATIC;
		break;
	case hos::com::eRaycastType::DYNAMIC:
		fd.flags |= physx::PxQueryFlag::eDYNAMIC;
		break;
	case hos::com::eRaycastType::STATIC:
		fd.flags |= physx::PxQueryFlag::eSTATIC;
		break;
	}

	for (int i = 0; i < MAX_LAYER; i++)
	{
		//2번레이어가 RaycastLayer
		if (Layer[2][i])
		{
			fd.data.word0 |= (1 << i);
		}
	}
	fd.flags |= physx::PxQueryFlag::eNO_BLOCK;

	if (PxScene->raycast(pos, dir, std::numeric_limits<float>::infinity(), hit, physx::PxHitFlag::eDEFAULT, fd))
	{
		return hit.Collisions;
	}

	return std::vector<const physx::PxRaycastHit*>();
}

void hos::com::Scene::Awake()
{
	for (int i = 0; i < MAX_LAYER; i++)
	{
		for (int j = 0; j < MAX_LAYER; j++)
		{
			g_Layer[i][j] = Layer[i][j];
		}
	}

	for (auto [order, com] : ScriptExecution)
	{
		//Awake는 Active 상태를 체크하면 안된다
		com->Awake();
	}

	for (GameObject* obj : GameObjects)
	{
		if (obj->listener)
		{
			if (nullptr == Listener)
			{
				Listener = obj->listener;
			}
			else
			{
				Debug->LogConsole(L"Scene", L"There are multiple audio listeners.");
			}
		}

		Camera* mainCamera = obj->GetComponent<Camera>();
		if (nullptr == MainCamera)
		{
			MainCamera = mainCamera;
		}
	}
}

void hos::com::Scene::Start()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->Start();
			}
		}
	}
}

void hos::com::Scene::FixedUpdate()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->FixedUpdate();
			}
		}
	}
}

void hos::com::Scene::OnTriggerEnter(Collision* other)
{
}

void hos::com::Scene::OnTriggerStay(Collision* other)
{
}

void hos::com::Scene::OnTriggerExit(Collision* other)
{
}

void hos::com::Scene::OnCollisionEnter(Collision* other)
{
}

void hos::com::Scene::OnCollisionStay(Collision* other)
{
}

void hos::com::Scene::OnCollisionExit(Collision* other)
{
}

void hos::com::Scene::Update()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->Update();
			}
		}
	}
}

void hos::com::Scene::LateUpdate()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->LateUpdate();
			}
		}
	}
}

void hos::com::Scene::OnStateMachine()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->OnStateMachine();
			}
		}
	}
}

void hos::com::Scene::OnAnimation()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->OnAnimation();
			}
		}
	}
}

void hos::com::Scene::PreRender()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->PreRender();
			}
		}
	}
}

void hos::com::Scene::Render()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->Render();
			}
		}
	}
}

void hos::com::Scene::PostRender()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->PostRender();
			}
		}
	}
}

void hos::com::Scene::RenderImage()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->RenderImage();
			}
		}
	}
}

void hos::com::Scene::OnGUI(float aspect_x, float aspect_y)
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->OnGUI(aspect_x, aspect_y);
			}
		}
	}
}

void hos::com::Scene::OnPause()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->OnPause();
			}
		}
	}
}

void hos::com::Scene::OnClose()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->OnClose();
			}
		}
	}
}

void hos::com::Scene::OnQuit()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->OnQuit();
			}
		}
	}
}

void hos::com::Scene::OnDestory()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->m_GameObject->GetActive())
		{
			if (com->GetActive())
			{
				com->OnDestory();
			}
		}
	}
}

void hos::com::Scene::OnDisable()
{
	for (auto [order, com] : ScriptExecution)
	{
		if (com->GetActive())
		{
			com->OnDisable();
		}
	}
}

void hos::com::Scene::RemoveRootGameObject(GameObject* obj)
{
	for (auto it = RootGameObjects.begin(); it != RootGameObjects.end(); ++it)
	{
		if (obj == *it)
		{
			//obj->UnBind();
			RootGameObjects.erase(it);
			return;
		}
	}
}

hos::com::GameObject* hos::com::Scene::FindGameObjectWithName(string name)
{
	for (int i = 0; i < GameObjects.size(); i++)
	{
		if (GameObjects[i]->GetName() == name)
		{
			return GameObjects[i];
		}
	}

	return nullptr;
}

const std::vector<U8> hos::com::Scene::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());

	flexbuffers::Builder builder;

	std::vector<uint8_t> temp;

	builder.Map([&] {
		builder.String("Name", name);
		builder.TypedVector("Gravity", [&] {
			builder.Float(Gravity.x);
			builder.Float(Gravity.y);
			builder.Float(Gravity.z);
			});
		builder.Vector("Layer", [&] {
			for (int i = 0; i < MAX_LAYER; i++)
			{
				builder.Vector([&] {
					for (int j = 0; j < MAX_LAYER; j++)
					{
						builder.Bool(Layer[i][j]);
					}
					});
			}
			});
		builder.Vector("LayerName", [&] {
			mbstring ln;
			for (int i = 0; i < MAX_LAYER; i++)
			{
				ln = ut::UTF16ToAnsi(LayerName[i]);
				builder.String(ln);
			}
			});
		// 루트 게임오브젝트들을 Serialization
		builder.Vector("RootGameObjects", [&] {

			for (int i = 0; i < RootGameObjects.size(); i++)
			{
				temp = RootGameObjects[i]->Serialize();
				builder.Blob(temp);
			}

			});
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::Scene::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring sn = m["Name"].AsString().str();
	string name = ut::AnsiToUTF16(sn);

	SetName(name);

	auto gravity = m["Gravity"].AsTypedVector();
	Gravity.x = gravity[0].AsFloat();
	Gravity.y = gravity[1].AsFloat();
	Gravity.z = gravity[2].AsFloat();

	auto lv = m["Layer"].AsVector();
	for (int i = 0; i < MAX_LAYER; i++)
	{
		auto lvv = lv[i].AsVector();
		for (int j = 0; j < MAX_LAYER; j++)
		{
			Layer[i][j] = lvv[j].AsBool();
		}
	}

	auto lnv = m["LayerName"].AsVector();
	mbstring ln;
	for (int i = 0; i < MAX_LAYER; i++)
	{
		ln = lnv[i].AsString().str();
		LayerName[i] = ut::AnsiToUTF16(ln);
	}

	auto temp = m["RootGameObjects"].AsVector();

	for (int i = 0; i < temp.size(); i++)
	{
		if (!temp[i].AsBlob().IsTheEmptyBlob())
		{
			GameObject* _GameObject = new GameObject();
			mbstring _GameObjectData(reinterpret_cast<const char*>(temp[i].AsBlob().data()), temp[i].AsBlob().size());
			bool b = _GameObject->Deserialize(_GameObjectData);
			if (b)
			{
				AddGameObjectRecursive(_GameObject);
			}
			else
			{
				SafeDelete(_GameObject);
			}

		}
	}

	return true;
}

hos::com::RaycastInfinityCallBack::RaycastInfinityCallBack(Scene* scene) :
	physx::PxRaycastCallback(nullptr, 1), m_Scene(scene)
{
}

hos::com::RaycastInfinityCallBack::~RaycastInfinityCallBack()
{
}

physx::PxAgain hos::com::RaycastInfinityCallBack::processTouches(const physx::PxRaycastHit* buffer, physx::PxU32 nbHits)
{
	Collisions.clear();
	maxNbTouches = nbHits;
	touches = const_cast<physx::PxRaycastHit*>(buffer);
	for (physx::PxU32 i = 0; i < nbHits; i++)
	{
		Collisions.push_back(&buffer[i]);
	}

	std::sort(Collisions.begin(), Collisions.end(),
		[](const physx::PxRaycastHit* b1, const physx::PxRaycastHit* b2)
		{
			return b1->distance < b2->distance;
		});
	return true;
}

void hos::com::RaycastInfinityCallBack::finalizeQuery()
{
	if (touches)
	{
		maxNbTouches = 1;
		touches = nullptr;
	}
}

hos::com::CollisionCallback::CollisionCallback()
{
}

hos::com::CollisionCallback::~CollisionCallback()
{
}

void hos::com::CollisionCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	for (physx::PxU32 i = 0; i < nbPairs; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (physx::PxContactPairFlag::eREMOVED_SHAPE_0 | physx::PxContactPairFlag::eREMOVED_SHAPE_1))
		{
			continue;
		}

		auto iter1 = Collisions.find(pairs[i].shapes[0]);
		auto iter2 = Collisions.find(pairs[i].shapes[1]);
		if (iter1 != Collisions.end() && iter2 != Collisions.end())
		{
			com::Collision* col1 = (*iter1).second;
			com::Collision* col2 = (*iter2).second;
			if (col1 && col2)
			{
				if (col1->m_GameObject->GetActive() && col2->m_GameObject->GetActive())
				{
					if (pairs[i].events == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						col1->m_GameObject->OnCollisionEnter(col2);
						col2->m_GameObject->OnCollisionEnter(col1);
					}
					else if (pairs[i].events == physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
					{
						col1->m_GameObject->OnCollisionStay(col2);
						col2->m_GameObject->OnCollisionStay(col1);
					}
				}

				if (pairs[i].events == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					col1->m_GameObject->OnCollisionExit(col2);
					col2->m_GameObject->OnCollisionExit(col1);
				}
			}
		}
	}
}

void hos::com::CollisionCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
		{
			continue;
		}

		auto iter1 = Collisions.find(pairs[i].triggerShape);
		auto iter2 = Collisions.find(pairs[i].otherShape);
		if (iter1 != Collisions.end() && iter2 != Collisions.end())
		{
			com::Collision* col1 = (*iter1).second;
			com::Collision* col2 = (*iter2).second;
			if (col1 && col2)
			{
				if (col1->m_GameObject->GetActive() && col2->m_GameObject->GetActive())
				{
					if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
					{
						col2->m_GameObject->OnTriggerEnter(col1);
						col1->m_GameObject->OnTriggerEnter(col2);
					}
					else
					{
						col1->m_GameObject->OnTriggerStay(col2);
						col2->m_GameObject->OnTriggerStay(col1);
					}
				}

				if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					col1->m_GameObject->OnTriggerExit(col2);
					col2->m_GameObject->OnTriggerExit(col1);
				}
			}
		}
	}
}