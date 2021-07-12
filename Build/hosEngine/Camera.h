#ifndef _CAMERA_H
#define _CAMERA_H

// [2020/10/28 ������]
// ī�޶�
// ������Ʈ���� ȭ��� ���ϼ� �ֵ��� �ϴ� ������Ʈ

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/03/27] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�, GetProjMatrix ��Ÿ����
// [2021/04/20] ��ȣ�� : ViewPort���� ���� 2�� => 4���� ����, Serialize ���� �κ� ����, GetSet �Լ� ����.

#include "Component.h"

//Warning : Windows.h�� ������ũ�� ����
#ifdef far
#undef far
#endif // far

#ifdef near
#undef near
#endif // near

namespace hos::com
{
	class Camera : public Component
	{
	public:
		enum class eMode
		{
			DEFAULT,
			FOLLOW,
		};
		enum class eViewMode
		{
			PERSPECTIVE,
			ORTHOGRAPHIC,
		};
		enum class eClearFlag
		{
			SKYBOX,
			SOLID_COLOR,
			NONE
		};
	private:
		float ViewPortLeft;
		float ViewPortRight;
		float ViewPortTop;
		float ViewPortBottom;

		float AspectRatio;
		U32 ScreenWidth;
		U32 ScreenHeight;

		float FOV;
		float Near;
		float Far;

		Vector3 FollowTarget;
		Color ClearColor;

		Matrix ViewMatrix;
		Matrix ProjMatrix;

		eMode Mode;
		eViewMode ViewMode;
		eClearFlag ClearFlag;

		hos::string SkyBoxName;
		
		hos::U32 MortionBlurSampleCount;
		bool RadialBlurState;

		bool bViewCull;

	public:
		Camera() = delete;
		//�ȷο� ī�޶� ���鶧 Ÿ���� �������� ������ ������ �ٶ󺸰� �˴ϴ�.
		Camera(eMode mode = eMode::DEFAULT, Vector3 followTarget = Vector3(), eViewMode view = eViewMode::PERSPECTIVE, eClearFlag flag = eClearFlag::SOLID_COLOR, float fov = 60, float far = 500, float near = 1);
		~Camera();

	protected:
		Camera(const Camera& dest);

	public:
		float GetViewPortLeft() const;
		float GetViewPortRight() const;
		float GetViewPortTop() const;
		float GetViewPortBottom() const;
		void SetViewPortLeft(float ratioValue);
		void SetViewPortRight(float ratioValue);
		void SetViewPortTop(float ratioValue);
		void SetViewPortBottom(float ratioValue);
		void SetViewPort(float left, float right, float top, float bottom);

		float GetFOV() const;
		void SetFOV(float fov);
		float GetNear() const;
		void SetNear(float near);
		float GetFar() const;
		void SetFar(float far);

		Vector3 GetFollowTarget() const;
		void SetFollowTarget(const Vector3& followTarget);
		Color GetClearColor() const;
		void SetClearColor(const Color& color);

		Matrix GetViewMatrix() const;
		Matrix GetProjMatrix() const;

		void ChangeMode(eMode mode);
		void ChangeView(eViewMode view);
		void ChangeClearFlag(eClearFlag flag);

		hos::string GetSkyBoxName() const;
		void SetSkyBox(hos::string_view skyBoxname);

		eMode GetMode() const { return Mode; };
		eViewMode GetViewMode() const { return ViewMode; };
		eClearFlag GetClearFlag() const { return ClearFlag; };

		// MortionBlur On/Off
		void SetMortionBlurState(unsigned int sampleCount) { this->MortionBlurSampleCount = sampleCount; };
		void SetRadialBlurState(bool bRadial, float uv_x = 0.5f, float uv_y = 0.5f, float distance = 0.3f, float strength = 3.0f);

	public:
		Camera* Clone() const override;
		void Reset()override;
		void Bind(GameObject* gameObject) override;

		//ICallable
		void Start() override;
		void PreRender() override;
		void Render() override;
		void PostRender() override;
		void FixedUpdate() override;
		void OnEnable() override;
		void OnDisable() override;

		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view) override;
	};
}

#endif // !_CAMERA_H