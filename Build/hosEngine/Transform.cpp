#include "Transform.h"
#include "Rigidbody.h"
#include <stdexcept>
#include "GameObject.h"

using namespace hos;

hos::com::Transform::Transform()
	: Component(L"Transform"), Root(this), Parent(nullptr), Forward(Vector3::Forward), Up(Vector3::Up), Left(Vector3::Left),
	Position(Vector3::Zero), Rotation(Rotator::Identity), Scale(Vector3::One),
	LocalPosition(Vector3::Zero), LocalRotation(Rotator::Identity), LocalScale(Vector3::One),
	TransformMatrix(Matrix::Identity)
{
	ExecutionOrder = I32_MIN;
}

hos::com::Transform::Transform(const Transform& dest) :
	Component(dest), Root(nullptr), Parent(nullptr),
	Forward(dest.Forward), Up(dest.Up), Left(dest.Left),
	Position(dest.Position), Rotation(dest.Rotation), Scale(dest.Scale),
	LocalPosition(dest.LocalPosition), LocalRotation(dest.LocalRotation), LocalScale(dest.LocalScale),
	TransformMatrix(dest.TransformMatrix)
{
	if (nullptr == dest.Parent)
	{
		Root = this;
	}
}

hos::com::Transform::~Transform()
{
}

void hos::com::Transform::UpdateRoot(Transform* root)
{
	for (Transform* child : Childs)
	{
		child->Root = root;
		child->UpdateRoot(root);
	}
}

void hos::com::Transform::UpdateTransformMatrix()
{
	LocalTransformMatrix = Matrix::CreateScale(LocalScale) * Matrix::CreateFromQuaternion(LocalRotation) * Matrix::CreateTranslation(LocalPosition);
	//부모가 있을경우 월드 데이터 갱신
	if (Parent)
	{
		//행열 갱신
		TransformMatrix = LocalTransformMatrix * Parent->TransformMatrix;
		Quaternion q;
		TransformMatrix.Decompose(Scale, q, Position);
		//소수점 보정
		Scale.x = std::roundf(Scale.x * 1000000.f) / 1000000.f;
		Scale.y = std::roundf(Scale.y * 1000000.f) / 1000000.f;
		Scale.z = std::roundf(Scale.z * 1000000.f) / 1000000.f;
		Position.x = std::roundf(Position.x * 1000000.f) / 1000000.f;
		Position.y = std::roundf(Position.y * 1000000.f) / 1000000.f;
		Position.z = std::roundf(Position.z * 1000000.f) / 1000000.f;
		Rotation = q;
	}
	else
	{
		TransformMatrix = LocalTransformMatrix;
	}

	//방향백터 갱신
	Forward = Vector3::Forward * Matrix::CreateFromQuaternion(LocalRotation);
	Up = Vector3::Up * Matrix::CreateFromQuaternion(LocalRotation);
	Left = Vector3::Left * Matrix::CreateFromQuaternion(LocalRotation);

	// physx 갱신
	if (m_GameObject)
	{
		if (m_GameObject->PhysicsActor)
		{
			physx::PxTransform t(Position, Rotation);
			m_GameObject->PhysicsActor->setGlobalPose(t);
		}
	}

	//자식들 갱신
	for (Transform* Child : Childs)
	{
		Child->UpdateTransformMatrix();
	}
}

hos::com::Transform* hos::com::Transform::GetRoot() const
{
	return Root;
}

hos::com::Transform* hos::com::Transform::GetParent() const
{
	return Parent;
}

void hos::com::Transform::SetParent(Transform* transform)
{
	if (Parent == transform)
	{
		return;
	}
	else
	{
		//현제 등록된 씬
		Scene* correctScene = nullptr;
		if (m_GameObject)
		{
			correctScene = m_GameObject->m_Scene;
		}

		//루트오브젝트로 변경될때
		if (transform == nullptr)
		{
			//씬의 루트오브젝트에 등록
			if (correctScene)
			{
				correctScene->RootGameObjects.push_back(m_GameObject);
			}

			LocalTransformMatrix = TransformMatrix;
			LocalPosition = Position;
			LocalRotation = Rotation;
			LocalScale = Scale;
		}
		else
		{
			//현제 루트 오브젝트일때
			if (Parent == nullptr)
			{
				//씬의 루트오브젝트에서 제거
				if (correctScene)
				{
					correctScene->RootGameObjects.erase(std::find(correctScene->RootGameObjects.begin(), correctScene->RootGameObjects.end(), m_GameObject));
				}
			}
			LocalTransformMatrix = TransformMatrix * transform->TransformMatrix.Invert();

			Quaternion q;
			LocalTransformMatrix.Decompose(LocalScale, q, LocalPosition);
			LocalRotation = q;
		}

		//부모 갱신
		Parent = transform;

		//루트 갱신
		if (Parent)
		{
			Root = Parent->Root;

			//부모의 자식으로 등록
			Parent->Childs.emplace_back(this);
		}
		else
		{
			Root = this;
		}

		UpdateRoot(Root);

		UpdateTransformMatrix();
	}
}

void hos::com::Transform::AttachChild(Transform* child)
{
	//이미 등록된 경우 리턴
	for (Transform* findChild : Childs)
	{
		if (child == findChild)
		{
			return;
		}
	}

	//자식의 부모로 등록
	child->SetParent(this);
}

void hos::com::Transform::DetachChild(int index, Transform* newParent)
{
	//인덱스 검사
	if (index < 0 || index >= Childs.size())
	{
		return;
	}
	//자식 찾기
	auto it = Childs.begin();
	for (int i = 0; i < index; i++)
	{
		++it;
		if (it == Childs.end())
		{
			return;
		}
	}

	Transform* findChild = (*it);
	//자식 제거
	Childs.erase(it);

	//부모 변경
	findChild->SetParent(newParent);
}

void hos::com::Transform::DetachChild(Transform* child, Transform* newParent)
{
	//자식 찾기
	for (auto it = Childs.begin(); it != Childs.end(); ++it)
	{
		if ((*it) == child)
		{
			Transform* findChild = (*it);
			//자식 제거
			Childs.erase(it);

			//부모 변경
			findChild->SetParent(newParent);

			return;
		}
	}
}

hos::com::Transform* hos::com::Transform::GetChild(int index) const
{
	if (index < 0 || index >= Childs.size())
	{
		return nullptr;
	}
	return Childs[index];
}

com::Transform* hos::com::Transform::Find(string_view name) const
{
	for (Transform* child : Childs)
	{
		if (child->m_GameObject->GetName() == name.data())
		{
			return child;
		}
	}
	return nullptr;
}

std::size_t hos::com::Transform::GetChildCount() const
{
	return Childs.size();
}

std::vector<hos::com::Transform*>& hos::com::Transform::GetChilds()
{
	return Childs;
}

void hos::com::Transform::SetForward(Vector3 forward)
{
	if (forward != Vector3::Zero)
	{
		//?????????
		//왼손좌표계를 사용하는데 RH를 써야 정면을 본다 LH로 하면 등을 보여준다
		Matrix rot = DirectX::SimpleMath::Matrix(DirectX::XMMatrixLookAtRH(Vector3::Zero, forward, Up));
		Vector3 scale, pos;
		Quaternion q;
		rot.Decompose(scale, q, pos);
		Rotator r = q;
		//??????????
		//RH를 하든 LH를 하든 회전방향이 반대로 돈다
		r.y = -r.y;
		SetRotation(r);
	}
}

void hos::com::Transform::SetUp(Vector3 up)
{
	Vector3 yz(0.f, up.y, up.z);
	yz.Normalize();
	Vector3 xz(up.x, 0.f, up.z);
	xz.Normalize();

	float xTheta = acosf(yz.Dot(Vector3::Forward));
	float yTheta = acosf(xz.Dot(Vector3::Forward));
	float zTheta = 0;

	LocalRotation = Rotator(RadToDegree(xTheta), RadToDegree(yTheta), RadToDegree(zTheta));

	Forward = Vector3::Forward * Matrix::CreateFromQuaternion(LocalRotation);
	Up = Vector3::Up * Matrix::CreateFromQuaternion(LocalRotation);
	Left = Vector3::Left * Matrix::CreateFromQuaternion(LocalRotation);
}

void hos::com::Transform::SetLeft(Vector3 right)
{
	Vector3 yz(0.f, right.y, right.z);
	yz.Normalize();
	Vector3 xz(right.x, 0.f, right.z);
	xz.Normalize();

	float xTheta = acosf(yz.Dot(Vector3::Forward));
	float yTheta = acosf(xz.Dot(Vector3::Forward));
	float zTheta = 0;

	LocalRotation = Vector3(RadToDegree(xTheta), RadToDegree(yTheta), RadToDegree(zTheta));

	Forward = Vector3::Forward * Matrix::CreateFromQuaternion(LocalRotation);
	Up = Vector3::Up * Matrix::CreateFromQuaternion(LocalRotation);
	Left = Vector3::Left * Matrix::CreateFromQuaternion(LocalRotation);
}

void hos::com::Transform::SetPosition(Vector3 position)
{
	//월드 갱신
	Position = position;

	//로컬 갱신
	if (Parent)
	{
		TransformMatrix = Matrix::CreateScale(Scale) * Matrix::CreateFromQuaternion(Rotation) * Matrix::CreateTranslation(Position);
		LocalTransformMatrix = TransformMatrix * Parent->TransformMatrix.Invert();
		Quaternion q;
		Vector3 s;
		LocalTransformMatrix.Decompose(s, q, LocalPosition);
	}
	else
	{
		LocalPosition = Position;
	}

	if (m_GameObject)
	{
		if (m_GameObject->rigidbody)
		{
			m_GameObject->rigidbody->ClearForce();
			m_GameObject->rigidbody->ClearTorque();
		}
	}

	//Transform Matrix 갱신
	UpdateTransformMatrix();
}

void hos::com::Transform::SetRotation(Rotator rotation)
{
	//월드 변경
	Rotation = rotation;

	//로컬 갱신
	if (Parent)
	{
		TransformMatrix = Matrix::CreateScale(Scale) * Matrix::CreateFromQuaternion(Rotation) * Matrix::CreateTranslation(Position);
		LocalTransformMatrix = TransformMatrix * Parent->TransformMatrix.Invert();
		Quaternion q;
		Vector3 s, p;
		LocalTransformMatrix.Decompose(s, q, p);
		LocalRotation = q;
	}
	else
	{
		LocalRotation = Rotation;
	}

	if (m_GameObject)
	{
		if (m_GameObject->rigidbody)
		{
			m_GameObject->rigidbody->ClearForce();
			m_GameObject->rigidbody->ClearTorque();
		}
	}

	//Transform Matrix 갱신
	UpdateTransformMatrix();
}

void hos::com::Transform::SetScale(Vector3 scale)
{
	//월드 갱신
	Scale = scale;

	//로컬 갱신
	if (Parent)
	{
		TransformMatrix = Matrix::CreateScale(Scale) * Matrix::CreateFromQuaternion(Rotation) * Matrix::CreateTranslation(Position);
		LocalTransformMatrix = TransformMatrix * Parent->TransformMatrix.Invert();
		Quaternion q;
		Vector3 p;
		LocalTransformMatrix.Decompose(LocalScale, q, p);
	}
	else
	{
		LocalScale = Scale;
	}

	//Transform Matrix 갱신
	UpdateTransformMatrix();
}

void hos::com::Transform::SetLocalPosition(Vector3 position)
{
	//로컬 갱신
	LocalPosition = position;

	if (nullptr == Parent)
	{
		Position = LocalPosition;
	}

	if (m_GameObject)
	{
		if (m_GameObject->rigidbody)
		{
			m_GameObject->rigidbody->ClearForce();
			m_GameObject->rigidbody->ClearTorque();
		}
	}

	//월드 갱신, PhysX갱신, Matrix갱신
	UpdateTransformMatrix();
}

void hos::com::Transform::SetLocalRotation(Rotator rotation)
{
	//로컬 갱신
	LocalRotation = rotation;

	if (nullptr == Parent)
	{
		Rotation = LocalRotation;
	}

	if (m_GameObject)
	{
		if (m_GameObject->rigidbody)
		{
			m_GameObject->rigidbody->ClearForce();
			m_GameObject->rigidbody->ClearTorque();
		}
	}

	//월드 갱신, PhysX갱신, Matrix갱신
	UpdateTransformMatrix();
}

void hos::com::Transform::SetLocalScale(Vector3 scale)
{
	//로컬 갱신
	LocalScale = scale;

	if (nullptr == Parent)
	{
		Scale = LocalScale;
	}

	//월드 갱신, PhysX갱신, Matrix갱신
	UpdateTransformMatrix();
}

void hos::com::Transform::SetTransform(Vector3 position, Rotator rotation, Vector3 scale)
{
	//월드 갱신
	Position = position;
	Rotation = rotation;
	Scale = scale;

	//로컬 갱신
	if (Parent)
	{
		TransformMatrix = Matrix::CreateScale(Scale) * Matrix::CreateFromQuaternion(Rotation) * Matrix::CreateTranslation(Position);
		LocalTransformMatrix = TransformMatrix * Parent->TransformMatrix.Invert();
		Quaternion q;
		LocalTransformMatrix.Decompose(LocalScale, q, LocalPosition);
		//소수점 보정
		LocalScale.x = std::roundf(LocalScale.x * 1000000.f) / 1000000.f;
		LocalScale.y = std::roundf(LocalScale.y * 1000000.f) / 1000000.f;
		LocalScale.z = std::roundf(LocalScale.z * 1000000.f) / 1000000.f;
		LocalPosition.x = std::roundf(LocalPosition.x * 1000000.f) / 1000000.f;
		LocalPosition.y = std::roundf(LocalPosition.y * 1000000.f) / 1000000.f;
		LocalPosition.z = std::roundf(LocalPosition.z * 1000000.f) / 1000000.f;
		LocalRotation = q;
	}
	else
	{
		LocalPosition = Position;
		LocalRotation = Rotation;
		LocalScale = Scale;
	}

	//Transform Matrix 갱신
	UpdateTransformMatrix();
}

void hos::com::Transform::SetLocalTransform(Vector3 position, Rotator rotation, Vector3 scale)
{
	LocalPosition = position;
	LocalRotation = rotation;
	LocalScale = scale;

	if (nullptr == Parent)
	{
		Position = LocalPosition;
		Rotation = LocalRotation;
		Scale = LocalScale;
	}

	//월드 갱신, PhysX갱신, Matrix갱신
	UpdateTransformMatrix();
}

Vector3 hos::com::Transform::GetForward() const
{
	return Forward;
}

Vector3 hos::com::Transform::GetUp() const
{
	return Up;
}

Vector3 hos::com::Transform::GetLeft() const
{
	return Left;
}

Vector3 hos::com::Transform::GetPosition() const
{
	//부모가 없는경우 World = Local;
	return Position;
}

Rotator hos::com::Transform::GetRotation() const
{
	//부모가 없는경우 World = Local;
	return Rotation;
}

Vector3 hos::com::Transform::GetScale() const
{
	//부모가 없는경우 World = Local;
	return Scale;
}

Vector3 hos::com::Transform::GetLocalPosition() const
{
	return LocalPosition;
}

Rotator hos::com::Transform::GetLocalRotation() const
{
	return LocalRotation;
}

Vector3 hos::com::Transform::GetLocalScale() const
{
	return LocalScale;
}

Matrix hos::com::Transform::GetLocalTransformMatrix() const
{
	//SRT순서
	//부모 Transform이 없다면 Local = World이다.
	return LocalTransformMatrix;
}

Matrix hos::com::Transform::GetTransformMatrix() const
{
	return TransformMatrix;
}

void hos::com::Transform::Translate(Vector3 translation)
{
	if (m_GameObject)
	{
		if (m_GameObject->rigidbody)
		{
			m_GameObject->rigidbody->AddForce(translation, Rigidbody::FORCE_MODE_VELOCITYCHANGE);
		}
	}
}

void hos::com::Transform::Rotate(Rotator rotate)
{
	if (m_GameObject)
	{
		if (m_GameObject->rigidbody)
		{
			Vector3 torque = Forward * Matrix::CreateFromQuaternion(rotate);
			m_GameObject->rigidbody->AddTorque(torque);
		}
	}
}

void hos::com::Transform::Bind(GameObject* gameObject)
{
	if (m_GameObject)
	{
		throw std::invalid_argument("This Component is bound");
	}

	if (gameObject == nullptr)
	{
		throw std::invalid_argument("NullObject try to bind Component");
	}

	if (gameObject->transform)
	{
		throw std::invalid_argument("this Object already has Transform");
	}

	m_GameObject = gameObject;
	gameObject->transform = this;
}

const std::vector<U8> hos::com::Transform::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());

	flexbuffers::Builder builder;
	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		builder.TypedVector("Position", [&] {
			builder.Float(Position.x);
			builder.Float(Position.y);
			builder.Float(Position.z);
			});
		builder.TypedVector("EulerAngle", [&] {
			builder.Float(Rotation.x);
			builder.Float(Rotation.y);
			builder.Float(Rotation.z);
			});
		builder.TypedVector("Scale", [&] {
			builder.Float(Scale.x);
			builder.Float(Scale.y);
			builder.Float(Scale.z);
			});
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::Transform::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}
	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring sn = m["Name"].AsString().str();
	string name = ut::AnsiToUTF16(sn);
	if (name != GetName())
	{
		Debug->LogConsole(L"DataManager", L"Transform try Deserialize to" + name);
		return false;
	}

	float x, y, z;
	x = m["Position"].AsTypedVector()[0].AsFloat();
	y = m["Position"].AsTypedVector()[1].AsFloat();
	z = m["Position"].AsTypedVector()[2].AsFloat();
	Position = Vector3(x, y, z);
	x = m["EulerAngle"].AsTypedVector()[0].AsFloat();
	y = m["EulerAngle"].AsTypedVector()[1].AsFloat();
	z = m["EulerAngle"].AsTypedVector()[2].AsFloat();
	Rotation = Rotator(x, y, z);
	x = m["Scale"].AsTypedVector()[0].AsFloat();
	y = m["Scale"].AsTypedVector()[1].AsFloat();
	z = m["Scale"].AsTypedVector()[2].AsFloat();
	Scale = Vector3(x, y, z);

	TransformMatrix = Matrix::CreateScale(Scale) * Matrix::CreateFromQuaternion(Rotation) * Matrix::CreateTranslation(Position);

	SetTransform(Position, Rotation, Scale);

	return true;
}

com::Transform* hos::com::Transform::Clone() const
{
	return new Transform(*this);
}

void hos::com::Transform::Reset()
{
	Forward = Vector3::Forward;
	Up = Vector3::Up;
	Left = Vector3::Left;

	Position = Vector3::Zero;
	Rotation = Vector3::Zero;
	Scale = Vector3::One;

	LocalPosition = Vector3::Zero;
	LocalRotation = Vector3::Zero;
	LocalScale = Vector3::One;

	TransformMatrix = Matrix::Identity;
}

void hos::com::Transform::LateUpdate()
{
	if (m_GameObject && m_GameObject->PhysicsActor)
	{
		physx::PxTransform pt = m_GameObject->PhysicsActor->getGlobalPose();
		SetTransform(pt.p, pt.q, Scale);
	}
}

void hos::com::Transform::OnEnable()
{
}

void hos::com::Transform::OnDisable()
{
}