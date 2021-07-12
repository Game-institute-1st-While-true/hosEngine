#ifndef _AUDIO_CLIP_H
#define _AUDIO_CLIP_H

// [2021/1/15 ������]
// ����� Ŭ��(����)

// LOG
// [2021/03/11] ������ : ������ string���� string_veiw�� ����

#include "hosUtility\hosUtility.h"
#include "DXTK\DirectXTK.h"

namespace hos
{
	class AudioClip
	{
	public:
		static constexpr string_literal WAV_FILE = L".wav";
		static constexpr string_literal FILE_EXTENSION = L".sclip";
		static constexpr string_literal FILE_PATH = L"\\Assets\\Sound\\";

	public:
		DirectX::SoundEffect* Sound;

	private:
		string Name;

	public:
		AudioClip();
		~AudioClip();

		string GetName() const { return Name; }
		void SetName(string_view name);

		const std::vector<U8> Serialize();
		bool Deserialize(mbstring_view data);
	};
}

#endif // !_AUDIO_CLIP_H