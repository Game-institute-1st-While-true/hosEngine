#ifndef _AUDIO_SOURCE_H
#define _AUDIO_SOURCE_H

// [2020/12/10 ������]
// ����� �ҽ�
// �Ҹ��� ����ϴ� ������Ʈ

// LOG
// [2020/12/31] ������ : com���ӽ����̽� �߰�
// [2021/03/27] ������ : ���� ������ �߰�, ���� ���� ���� �Լ� �߰�
// [2021/03/29] ������ : SetAudioClip ���׼���

#include "Component.h"
#include "AudioClip.h"

namespace hos::com
{
	class AudioSource : public Component
	{
	public:
		static constexpr float DEFAULT_RANGE = 100.f;
		static constexpr float DEFAULT_VOLUME = 1.0f;
	private:
		AudioClip* Clip;
		DirectX::SoundEffectInstance* Source;
		DirectX::AudioEmitter Emitter;

		bool Mute;
		bool Loop;
		bool PlayOnAwake;
		float Volume;
		bool Is3D;
		float Range;

	public:
		AudioSource();
		~AudioSource();

	protected:
		AudioSource(const AudioSource& dest);

	public:
		void SetAudioClip(AudioClip* clip);
		void SetAudioClip(string_view name);
		void SetMute(bool b);
		void SetLoop(bool b);
		void SetPlayOnAwake(bool b);
		void SetIs3D(bool b);
		void SetVolume(float volume);
		void SetRange(float range);
		bool GetMute()const;
		bool GetLoop()const;
		bool GetPlayOnAwake()const;
		bool GetIs3D()const;
		float GetVolume()const;
		float GetRange()const;
		AudioClip* GetAudioClip() const { return Clip; }
		void Play();
		void Stop();
		void Pause();
		DirectX::SoundState GetState();
		void SetPitch(float pitch);

	public:
		AudioSource* Clone() const override;
		void Reset()override;

		const std::vector<U8> Serialize() override;
		bool Deserialize(mbstring_view) override;

	public:
		void Awake()override;
		void Update()override;
		void OnEnable()override;
		void OnDisable()override;
	};
}

#endif // !_AUDIO_SOURCE_H