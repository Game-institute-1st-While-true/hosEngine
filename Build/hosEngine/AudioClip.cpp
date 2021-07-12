#include "Object.h"
#include "AudioClip.h"

using namespace hos;

hos::AudioClip::AudioClip() :
	Name(string()), Sound(nullptr)
{
}

hos::AudioClip::~AudioClip()
{
	SafeDelete(Sound);
}

void hos::AudioClip::SetName(string_view name)
{
	Name = name;
}

const std::vector<U8> hos::AudioClip::Serialize()
{
	XAUDIO2_BUFFER buf;
	XAUDIO2_BUFFER_WMA wmabuf;
	Sound->FillSubmitBuffer(buf, wmabuf);

	const WAVEFORMATEX* format = Sound->GetFormat();

	std::string data(reinterpret_cast<const char*>(buf.pAudioData), buf.AudioBytes);

	mbstring pathName = ut::UTF16ToAnsi(Name);

	flexbuffers::Builder builder;
	builder.Vector([&] {
		builder.String(data);
		builder.Int(format->wFormatTag);
		builder.Int(format->nChannels);
		builder.Int(format->nSamplesPerSec);
		builder.Int(format->nAvgBytesPerSec);
		builder.Int(format->nBlockAlign);
		builder.Int(format->wBitsPerSample);
		builder.Int(format->cbSize);
		builder.String(pathName);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::AudioClip::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsVector();
	mbstring tmpData = m[0].AsString().str();
	std::unique_ptr<uint8_t[]> audioData(new uint8_t[tmpData.size() + sizeof(WAVEFORMATEX)]);

	uint8_t* startAudio = audioData.get() + sizeof(WAVEFORMATEX);
	WAVEFORMATEX* wfx = reinterpret_cast<WAVEFORMATEX*>(audioData.get());
	wfx->wFormatTag = m[1].AsInt16();
	wfx->nChannels = m[2].AsInt16();
	wfx->nSamplesPerSec = m[3].AsInt32();
	wfx->nAvgBytesPerSec = m[4].AsInt32();
	wfx->nBlockAlign = m[5].AsInt16();
	wfx->wBitsPerSample = m[6].AsInt16();
	wfx->cbSize = m[7].AsInt16();

	memcpy(startAudio, tmpData.data(), tmpData.size());

	SafeDelete(Sound);

	if (g_Audio)
	{
		Sound = new DirectX::SoundEffect(g_Audio, audioData, wfx, startAudio, tmpData.size());
	}

	mbstring pathName = m[8].AsString().str();
	Name = ut::AnsiToUTF16(pathName);

	return true;
}