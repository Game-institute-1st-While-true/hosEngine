// 참조 헤더
#include "Object.h"
#include <DirectXMath.h>

// 자기 헤더
#include "BoneAnimation.h"

using namespace hos;

hos::BoneAnimation::BoneAnimation()
{
	KeyframeList.reserve(50);
}

hos::BoneAnimation::~BoneAnimation()
{
	KeyframeList.clear();
	KeyframeList.shrink_to_fit();
}

hos::F32 hos::BoneAnimation::_GetStartTime() const
{
	if (KeyframeList.size() == 0)
	{
		return 0.0f;
	}

	return KeyframeList.front().GetTimePos();
}

hos::F32 hos::BoneAnimation::_GetEndTime() const
{
	if (KeyframeList.size() == 0)
	{
		return 0.0f;
	}

	return KeyframeList.back().GetTimePos();
}

void hos::BoneAnimation::_Interpolate(U32 index, F32 time, Matrix* matrix, Vector3* vPos, Vector3* vScale, Quaternion* vQuat) const
{
	Matrix mScale, mRot, mTrans, mTM;

	if (KeyframeList.size() == 0)
	{
		if (vScale != nullptr)
		{
			vScale[index] = Vector3(1,1,1);
		}

		if (vPos != nullptr)
		{
			vPos[index] = Vector3::Zero;
		}

		if (vQuat != nullptr)
		{
			vQuat[index] = Quaternion::Identity;
		}

		if (matrix != nullptr)
		{
			matrix[index] = Matrix::Identity;
		}

		return;
	}

	if (time <= KeyframeList.front().GetTimePos())
	{
		Vector3 S = KeyframeList.front().GetScale();
		Vector3 P = KeyframeList.front().GetPos();
		Quaternion Q = KeyframeList.front().GetRotQuat();

		mScale = Matrix::CreateScale(S);
		mRot = Matrix::CreateFromQuaternion(Q);
		mTrans = Matrix::CreateTranslation(P);

		if (vScale != nullptr)
		{
			vScale[index] = S;
		}

		if (vPos != nullptr)
		{
			vPos[index] = P;
		}

		if (vQuat != nullptr)
		{
			vQuat[index] = Q;
		}

		if (matrix != nullptr)
		{
			mTM = mScale * mRot * mTrans;
			matrix[index] = mTM;
		}
	}
	else if (time >= KeyframeList.back().GetTimePos())
	{
		Vector3 S = KeyframeList.back().GetScale();
		Vector3 P = KeyframeList.back().GetPos();
		Quaternion Q = KeyframeList.back().GetRotQuat();

		mScale = Matrix::CreateScale(S);
		mRot = Matrix::CreateFromQuaternion(Q);
		mTrans = Matrix::CreateTranslation(P);

		if (vScale != nullptr)
		{
			vScale[index] = S;
		}

		if (vPos != nullptr)
		{
			vPos[index] = P;
		}

		if (vQuat != nullptr)
		{
			vQuat[index] = Q;
		}

		if (matrix != nullptr)
		{
			mTM = mScale * mRot * mTrans;
			matrix[index] = mTM;
		}
	}
	else
	{
		for (UINT i = 0; i < KeyframeList.size() - 1; ++i)
		{
			if (time >= KeyframeList[i].GetTimePos() && time <= KeyframeList[i + 1].GetTimePos())
			{
				float lerpPercent = (time - KeyframeList[i].GetTimePos()) / (KeyframeList[i + 1].GetTimePos() - KeyframeList[i].GetTimePos());

				Vector3 s0 = KeyframeList[i].GetScale();
				Vector3 s1 = KeyframeList[i + 1].GetScale();

				Vector3 p0 = KeyframeList[i].GetPos();
				Vector3 p1 = KeyframeList[i + 1].GetPos();

				Quaternion q0 = KeyframeList[i].GetRotQuat();
				Quaternion q1 = KeyframeList[i + 1].GetRotQuat();

				DirectX::SimpleMath::Vector3 S = DirectX::XMVectorLerp(s0, s1, lerpPercent);
				DirectX::SimpleMath::Vector3 P = XMVectorLerp(p0, p1, lerpPercent);
				DirectX::SimpleMath::Vector4 Q = XMQuaternionSlerp(q0, q1, lerpPercent);

				mScale = Matrix::CreateScale(S);
				mRot = Matrix::CreateFromQuaternion(Q);
				mTrans = Matrix::CreateTranslation(P);

				if (vScale != nullptr)
				{
					vScale[index] = S;
				}

				if (vPos != nullptr)
				{
					vPos[index] = P;
				}

				if (vQuat != nullptr)
				{
					vQuat[index] = Q;
				}

				if (matrix != nullptr)
				{
					mTM = mScale * mRot * mTrans;
					matrix[index] = mTM;
				}

				break;
			}
		}
	}
}

const std::vector<U8> hos::BoneAnimation::Serialize()
{
	flexbuffers::Builder builder;
	builder.Vector([&] {
		for (int i = 0; i < KeyframeList.size(); i++)
		{
			builder.TypedVector([&] {
				builder.Float(KeyframeList[i].GetTimePos());
				builder.Float(KeyframeList[i].GetPos().x);
				builder.Float(KeyframeList[i].GetPos().y);
				builder.Float(KeyframeList[i].GetPos().z);
				builder.Float(KeyframeList[i].GetScale().x);
				builder.Float(KeyframeList[i].GetScale().y);
				builder.Float(KeyframeList[i].GetScale().z);
				builder.Float(KeyframeList[i].GetRotQuat().x);
				builder.Float(KeyframeList[i].GetRotQuat().y);
				builder.Float(KeyframeList[i].GetRotQuat().z);
				builder.Float(KeyframeList[i].GetRotQuat().w);
				});
		}
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::BoneAnimation::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}
	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsVector();

	for (int i = 0; i < m.size(); i++)
	{
		auto keyframe = m[i].AsTypedVector();

		Keyframe key;
		key.SetTimePos(keyframe[0].AsFloat());
		Vector3 pos, scale;
		Vector4 rotQuat;
		pos.x = keyframe[1].AsFloat();
		pos.y = keyframe[2].AsFloat();
		pos.z = keyframe[3].AsFloat();
		scale.x = keyframe[4].AsFloat();
		scale.y = keyframe[5].AsFloat();
		scale.z = keyframe[6].AsFloat();
		rotQuat.x = keyframe[7].AsFloat();
		rotQuat.y = keyframe[8].AsFloat();
		rotQuat.z = keyframe[9].AsFloat();
		rotQuat.w = keyframe[10].AsFloat();
		key.SetTransform(pos, scale, rotQuat);

		AddKeyframe(key);
	}

	return true;
}
