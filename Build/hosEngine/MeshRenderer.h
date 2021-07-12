#ifndef _MESHRENDERER_H
#define _MESHRENDERER_H

// [2021/03/18 ��ȣ��]
// �޽� ������ ������Ʈ

// LOG
// [2021/03/18] ��ȣ�� : ����.
// [2021/03/27] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�
// [2021/03/29] �ڿ��� : �����Ϳ��� ���� ���� RenderMode, MaterialRef�� Get �Լ� �߰�
// [2021/04/03] ������ : ��������ʴ� mFinalTM ���� Serialize/Deserialize�߰� bind ����

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

	class MeshRenderer: public Component
	{
	private:
		hos::cg::RendererDesc::RenderModeList RenderMode;

		MeshFilter* MeshFilterRef;
		Material* MaterialRef;

		bool bEffect;

	public:
		MeshRenderer();
		~MeshRenderer();

	protected:
		MeshRenderer(const MeshRenderer& dest);

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

		void SetIsEffect(bool isBool) { this->bEffect = isBool; };

		void Update();

		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view data) override;

		MeshRenderer* Clone()const override;
		void Reset() override {};
		void OnEnable() override {};
		void OnDisable() override {};
	};

}
#endif //!_MESHRENDERER_H