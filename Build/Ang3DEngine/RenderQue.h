// <Concept>
// Source Name		: RenderQue.h
// Creator			: Hosik_Shin / shin0394g@gmail.com
// Create Date		: 2021.02.15

// <Explanation>
// ������ ��ü�� ���ҽ� ������ ������ ����صδ� Que.

// <Working Log>
// 2021.02.15.Hosik	: ����
// 2021.03.16.Hosik : Resource�� ���̺귯�� �ٱ����� �̵���Ű�鼭 RenderQue�� ���� ���� �� ��� ����ü ����.
// 2021.03.17.Hosik : vector list�� push_back�� ������� �ʰ� resize�� index�� �����ϵ��� ���� ����.


// <Memo>
// 2021.03.17.Hosik	: ������ Vector�� �ƴ϶� �Ϲ� �迭�� �ص� �� �� ����.

#pragma once

#include <vector>
#include <string>
#include <algorithm>

#include "DXTK\SimpleMath.h"
#include "RendererDesc.h"

// �⺻ ����ť ũ��
#define DEFAULT_RENDERQUE_COUNT 1000

namespace hos::cg
{

	class RenderQue
	{

	public:

		struct RenderUIObejct
		{
			enum Alignment { LeftBottom = 1, Bottom = 2, RightBottom = 3, Left = 4, Center = 5, Right = 6, LeftTop = 7, Top = 8, RightTop = 9 };

			bool bFont = false;
			std::wstring FontName = L"";
			std::wstring Text = L"";
			unsigned int FontSize = 10;
			unsigned int Width = 0;
			unsigned int Height = 0;
			float FontRot = 0.0f;
			int Align = LeftTop;
			DirectX::SimpleMath::Vector2 Origin = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
			
			DirectX::SimpleMath::Vector2 Aspect = DirectX::SimpleMath::Vector2(1.0f, 1.0f);

			DirectX::SimpleMath::Vector3 Pos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
			DirectX::SimpleMath::Vector3 Rot = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
			DirectX::SimpleMath::Vector3 Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

			bool bAni = false;
			DirectX::SimpleMath::Matrix AniTM;
			DirectX::SimpleMath::Vector3 vAniPos = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
			DirectX::SimpleMath::Vector3 vAniScale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
			DirectX::SimpleMath::Vector3 vAniRot = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

			DirectX::SimpleMath::Vector4 Color = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
			DirectX::SimpleMath::Vector4 OffTill = DirectX::SimpleMath::Vector4(0, 1, 0, 1);
			
			int LayerOrder = 0;

			bool bTexture = false;
			std::wstring TextureName = L"";

			bool bFollow = false;
			int CoordU = 0;
			int CoordV = 0;

			bool operator < (const RenderUIObejct& rhs)
			{
				return (LayerOrder < rhs.LayerOrder);
			}
		};

		enum class RenderPrimitiveType
		{
			BOX,
			SPHERE,
			CAPSULE,
		};

		struct RenderMaterial
		{
			// Material
			DirectX::SimpleMath::Vector4 DiffColor = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
			DirectX::SimpleMath::Vector4 AmbColor = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
			DirectX::SimpleMath::Vector4 SpecColor = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
			float SpecPower = 0.0f;

			// texture
			bool bAlbedo = false;
			bool bNormal = false;
			bool bARM = false;
			bool bEmiss = false;
			std::wstring TexNameAlbedo = L"";
			std::wstring TexNameNormal = L"";
			std::wstring TexNameARM = L"";
			std::wstring TexNameEmiss = L"";
		};

		struct RenderObject
		{
			// User TM
			bool InView = true;
			bool ViewCullSkip = false;
			DirectX::SimpleMath::Vector3 vPos;
			DirectX::SimpleMath::Vector3 vBBox[9];

			DirectX::SimpleMath::Matrix mTM;
			float ViewDistance = -1.0f;

			// Animation
			bool bAnimation = false;
			bool bSkinning = false;
			unsigned int BoneCount = 0;
			DirectX::SimpleMath::Matrix* mSkinnedAni;
			DirectX::SimpleMath::Matrix mRootBone;
			DirectX::SimpleMath::Matrix mNode;

			// Mesh, Vertex Data
			std::wstring MeshName = L"";

			// Render Option
			RendererDesc::RenderModeList RenderMode = RendererDesc::RenderModeList::RENDER_DEFAULT;
			/// DrawMode ����Ʈ(��ο� Ÿ��?) ���ĺ���, ����Ʈ���̽� �ø� ��

			RenderMaterial MaterialInfo;

			RenderPrimitiveType PrimitiveType = RenderPrimitiveType::BOX;
			bool CollisionState = false;

			bool operator < (const RenderObject& rhs)
			{
				return (ViewDistance > rhs.ViewDistance);
			}
		};

	private:
		
		std::vector<RenderObject> RenderObjectList;
		unsigned int NowObjectCount;
		unsigned int MaxObjectCount;

		std::vector<RenderObject> AlphaObjectList;
		unsigned int NowAlphaCount;
		unsigned int MaxAlphaCount;

		std::vector<RenderObject> EffectList;
		unsigned int NowEffectCount;
		unsigned int MaxEffectCount;

		std::vector<RenderObject> DebugObjectList;
		std::vector<RenderUIObejct> UIObjectList;

		// �������ҿ�
		DirectX::SimpleMath::Plane mPlane[6];

	public:
		RenderQue();
		~RenderQue();

		// �ܺ� ���� �Լ�
	public:
		// ����ť�� ������ ��ü�� ����մϴ�.
		void AddRenderObejct(RenderObject& objectInfo);
		void AddAlphaObject(RenderObject& objectInfo);
		void AddEffectObject(RenderObject& objectInfo);
		void AddDebugObject(RenderObject& objectInfo);
		void AddUIObject(RenderUIObejct& objectInfo);

		unsigned int GetRenderObjectListSize() { return NowObjectCount; };
		unsigned int GetAlphaObjectListSize() { return NowAlphaCount; };
		unsigned int GetEffectListSize() { return NowEffectCount; };
		unsigned int GetDebugObjectListSize() { return (unsigned int)DebugObjectList.size(); };
		unsigned int GetUIObjectListSize() { return (unsigned int)UIObjectList.size(); };

		std::vector<RenderObject>& GetRenderObjectList() { return RenderObjectList; };
		std::vector<RenderObject>& GetAlphaObjectList() { return AlphaObjectList; };
		std::vector<RenderObject>& GetEffectList() { return EffectList; };
		std::vector<RenderObject>& GetDebugObjectList() { return DebugObjectList; };
		std::vector<RenderUIObejct>& GetUIObjectList() { return UIObjectList; };

		void ClearObjectRenderQue();		// ����ť�� ����ݴϴ�.
		void ClearUIRenderQue();

		void CheckMaxSize();
		void CheckMaxSize_Alpha();
		void CheckMaxSize_Effect();

		// ����ť ���� �Լ�
		void SortUIQue_LayerOrder();

		// ����ť ���� �Լ�
		void CalculateViewDistanceInAlphaQue(DirectX::SimpleMath::Vector3 eyepos);

		// ���ν��� �Լ�
		void ViewFrustum(float screenDepth, DirectX::SimpleMath::Matrix projMatrix, DirectX::SimpleMath::Matrix viewMatrix, bool bUI = false);
		bool CheckFrustum_Point(DirectX::SimpleMath::Vector3 vPos);

	private:

	

	};

}
