#ifndef _AUDIO_LISTENER_H
#define _AUDIO_LISTENER_H

// [2020/12/10 ������]
// ����� ������
// ����� �ҽ��� �Ҹ��� ��� û���� ������Ʈ

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/03/26] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�, DirectX::AudioListenser�� ����ϰ��ִ� ��Ÿ ����

#include "Component.h"
#include "DXTK\DirectXTK.h"

namespace hos::com
{
	class AudioListener : public Component
	{
	private:
		DirectX::AudioListener Listener;
	public:
		AudioListener();
		~AudioListener();

	protected:
		//Only Used GameObject Copy
		AudioListener(const AudioListener& dest);

	public:
		DirectX::AudioListener& Get();

		void Bind(GameObject* gameObject) override;
		void UnBind(int index) override;
		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view);

		AudioListener* Clone() const override;
		void Reset() override;
		void Awake() override;

		void Update() override;

		void OnEnable() override;
		void OnDisable() override;
	};
}

#endif // !_AUDIO_LISTENER_H