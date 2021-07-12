#ifndef _MATERIAL_H
#define _MATERIAL_H

// [2021/03/15 ��ȣ��]
// ���׸��� ������

// LOG
// [2021/03/15 ��ȣ��] Graphics Engine Mk2���� ���� Ŭ���̾�Ʈ ���η� �̵�.
// [2021/04/06] ������ : Serialize/Deserialize �߰�

#include "hosUtility\hosUtility.h"
#include "hosUtility\hosMath.h"
#include "Ang3DEngine\RenderQue.h"

namespace hos
{
	class Material
	{
		friend class FbxLoader;

	public:
		static constexpr string_literal FILE_EXTENSION = L".mat";
		
		enum class MATERIAL_TYPE
		{
			MATERIAL_LEGACY_LAMBERT,	// ������ ������, ����ŧ�� ����
			MATERIAL_LEGACY_PHONG,		// ����Ʈ + ����ŧ��
			MATERIAL_STANDARD_PBR,		// PBR

			MATERIAL_MAX,
		};

	private:
		hos::string MaterialName;

		MATERIAL_TYPE MaterialType;

		Vector4 DiffuseColor;
		Vector4 AmbientColor;
		Vector4 SpecularColor;
		F32 SpecularPower;

		bool bTexAlbedo;
		bool bTexNormal;
		bool bTexARM;	// Ambient Occlusion(R) - Roughness(G) - Metaclic(B)
		bool bTexEmiss;

		hos::string TexNameAlbedo;
		hos::string TexNameNormal;
		hos::string TexNameARM;
		hos::string TexNameEmiss;

	public:
		Material();
		~Material();

	public:
		void CopyMaterialValue(hos::cg::RenderQue::RenderMaterial& material) const;

		const bool IsThereAlbedo() const { return bTexAlbedo; };
		const bool IsThereNormal() const { return bTexNormal; };
		const bool IsThereARM() const { return bTexARM; };
		const bool IsThereEmiss() const { return bTexEmiss; };

		MATERIAL_TYPE GetMaterialType() const { return MaterialType; };

		hos::string GetMaterialName() const { return MaterialName; };
		hos::string GetTexNameAlbedo() const { return TexNameAlbedo; };
		hos::string GetTexNameNormal() const { return TexNameNormal; };
		hos::string GetTexNameARM() const { return TexNameARM; };
		hos::string GetTexNameEmiss() const { return TexNameEmiss; };

		void SetDiffuseColor(hos::Vector4 color) { DiffuseColor = color; };
		void SetAmbientColor(hos::Vector4 color) { AmbientColor = color; };
		void SetSpecularColor(hos::Vector4 color, hos::F32 power) { SpecularColor = color; SpecularPower = power; };

		void SetAlbedoTex(hos::string texName);
		void SetNormalTex(hos::string texName);
		void SetARMTex(hos::string texName);
		void SetEmissTex(hos::string texName);

		void ChangeAlbedoTex(hos::string texName);
		void ChangeNormalTex(hos::string texName);
		void ChangeARMTex(hos::string texName);
		void ChangeEmissTex(hos::string texName);

		void RemoveAlbedoTex();
		void RemoveNormalTex();
		void RemoveARMTex();
		void RemoveEmissTex();

		const std::vector<U8> Serialize();
		bool Deserialize(mbstring_view data);
	};
}

#endif // !_MATERIAL_H