#ifndef _LIGHT_H
#define _LIGHT_H

// [2020/10/28 ������]
// ����Ʈ
// �������� ������Ʈ�� ǥ���ϴ� ����Ʈ ������Ʈ

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/03/27] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�
// [2021/04/06] ������ : Serialize/Deserialize �߰�

#include "Component.h"

namespace hos::com
{
	class Light :public Component
	{
	public:
		enum class eType
		{
			DIRECTIONAL,
			POINT,
			SPOT,
			AMBIENT,
			MAX
		};
	private:
		Color SolidColor;

		float Range;
		float Angle;
		float Intensity;

		float Attenuation_Const;
		float Attenuation_Linear;
		float Attenuation_Quad;

		eType Type;
	public:
		Light(eType type = eType::DIRECTIONAL);
		~Light();

	protected:
		Light(const Light& dest);

	public:
		Color GetColor() const;
		void SetColor(Color color);
		float GetRange() const;
		void SetRange(float range);
		float GetAngle() const;
		void SetAngle(float angle);
		float GetIntensity() const;
		void SetIntensity(float intensity);

		eType GetType() const { return Type; };
		void ChangeType(eType type);

		float GetAttenuation_Const() const { return Attenuation_Const; }
		void SetAttenuation_Const(float val) { Attenuation_Const = val; }
		float GetAttenuation_Linear() const { return Attenuation_Linear; }
		void SetAttenuation_Linear(float val) { Attenuation_Linear = val; }
		float GetAttenuation_Quad() const { return Attenuation_Quad; }
		void SetAttenuation_Quad(float val) { Attenuation_Quad = val; }

	public:
		Light* Clone()const override;
		void Reset()override;
		void OnEnable()override;
		void OnDisable()override;

		void Update() override;

		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view data) override;
	};
}

#endif // !_LIGHT_H