#ifndef _SKINNEDMESHRENDERER_H
#define _SKINNEDMESHRENDERER_H

// [2021/03/23 ��ȣ��]
// ��Ų �޽� ������ ������Ʈ

// LOG
// [2021/03/23] ��ȣ�� : ����.
// [2021/03/26] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�
// [2021/04/03] ������ : Serialize/Deserialize �߰�

#include "Component.h"

namespace hos
{
	namespace com
	{
		class MeshFilter;
	}
	class Material;
}

namespace hos::com
{

	class SkinnedMeshRenderer : public Component
	{
	private:
		hos::cg::RendererDesc::RenderModeList RenderMode;

		MeshFilter* MeshFilterRef;
		Material* MaterialRef;

		hos::Matrix mFinalTM[MAX_BONE];

	public:
		SkinnedMeshRenderer();
		~SkinnedMeshRenderer();

	protected:
		SkinnedMeshRenderer(const SkinnedMeshRenderer& dest);

	public:
		U32 GetDefaultBoneIndex();

		void Bind(GameObject* gameObject) override;
		void UnBind(int index) override;

		void SetRenderMode(hos::cg::RendererDesc::RenderModeList renderMode) { RenderMode = renderMode; };
		hos::cg::RendererDesc::RenderModeList GetRenderMode() const { return RenderMode; };

		void AddMeshFilter(MeshFilter* meshFilter);
		void RemoveMeshFilter();

		void SetMaterial(hos::string materialName);
		Material* GetMaterialRef() const;
		hos::string GetMaterialName() const { return MaterialRef->GetMaterialName(); };
		void RemoveMaterial();

		void Update();

		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view data) override;

		SkinnedMeshRenderer* Clone() const override;
		void Reset() override {};
		void OnEnable() override {};
		void OnDisable() override {};
	};

}
#endif //!_SKINNEDMESHRENDERER_H