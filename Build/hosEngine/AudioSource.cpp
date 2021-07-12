#include "AudioSource.h"
#include "Transform.h"
#include "GameObject.h"
#include "Scene.h"
#include "AudioListener.h"

using namespace hos;

//				FL      FR      C      LFE      BL      BR      SL     SR
//Degree(0~360)	334     26      0      360      260     100     217.5  142.5
static float EmitterAngle[] =
{
	5.8294f,		//Front Left
	0.453786f,		//Front Right
	0.f,			//Center
	X3DAUDIO_2PI,	//Low-frequency effects
	3.7960911f,		//Back Left
	1.74533f,		//Back Right
	4.53786f,		//Surround Left
	2.4870942f,		//Surround Right
};

hos::com::AudioSource::AudioSource() :
	Component(L"AudioSource")
{
	Reset();
}

hos::com::AudioSource::~AudioSource()
{
	Stop();
	SafeDelete(Source);
}

hos::com::AudioSource::AudioSource(const AudioSource& dest) :
	Component(dest),
	Clip(dest.Clip),
	Mute(dest.Mute),
	Loop(dest.Loop),
	PlayOnAwake(dest.PlayOnAwake),
	Volume(dest.Volume),
	Is3D(dest.Is3D),
	Range(dest.Range),
	Source(nullptr)
{
	if (Clip && Clip->Sound)
	{
		dest.Source->Stop();
		Source = Clip->Sound->CreateInstance(DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters).release();
		if (Source)
		{
			SetVolume(Volume);
			if (Is3D)
			{
				Emitter.ChannelCount = Clip->Sound->GetFormat()->nChannels;
				Emitter.pChannelAzimuths = EmitterAngle;
				Emitter.CurveDistanceScaler = Range;
				Emitter.pVolumeCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&X3DAudioDefault_LinearCurve);
			}
		}
	}
}

void hos::com::AudioSource::SetAudioClip(AudioClip* clip)
{
	Stop();
	SafeDelete(Source);
	Clip = clip;
	if (Clip)
	{
		Source = Clip->Sound->CreateInstance(DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters).release();
		if (Source)
		{
			if (Is3D)
			{
				Emitter.ChannelCount = Clip->Sound->GetFormat()->nChannels;
				Emitter.pChannelAzimuths = EmitterAngle;
				Emitter.CurveDistanceScaler = Range;
				Emitter.pVolumeCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&X3DAudioDefault_LinearCurve);
			}
		}
	}
}

void hos::com::AudioSource::SetAudioClip(string_view name)
{
	Stop();
	SafeDelete(Source);
	AudioClip* _Clip = g_DataManager->GetAudioClip(name);

	if (_Clip)
	{
		Clip = _Clip;
		Source = Clip->Sound->CreateInstance(DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters).release();
		if (Source)
		{
			if (Is3D)
			{
				Emitter.ChannelCount = Clip->Sound->GetFormat()->nChannels;
				Emitter.pChannelAzimuths = EmitterAngle;
				Emitter.CurveDistanceScaler = Range;
				Emitter.pVolumeCurve = const_cast<X3DAUDIO_DISTANCE_CURVE*>(&X3DAudioDefault_LinearCurve);
			}
		}
	}
}

void hos::com::AudioSource::SetMute(bool b)
{
	Mute = b;
	if (Mute)
	{
		if (Source)
		{
			Source->SetVolume(0);
		}
	}
	else
	{
		if (Source)
		{
			Source->SetVolume(Volume);
		}
	}
}

void hos::com::AudioSource::SetLoop(bool b)
{
	Loop = b;
	if (Source)
	{
		if (Source->GetState() == DirectX::SoundState::PLAYING)
		{
			Source->Stop();
			Source->Play(Loop);
		}
	}
}

void hos::com::AudioSource::SetPlayOnAwake(bool b)
{
	PlayOnAwake = b;
}

void hos::com::AudioSource::SetIs3D(bool b)
{
	Is3D = b;
}

void hos::com::AudioSource::SetVolume(float volume)
{
	Volume = Max(0.f, Min(volume, 1.f));
	if (!Mute)
	{
		if (Source)
		{
			Source->SetVolume(Volume);
		}
	}
}

void hos::com::AudioSource::SetRange(float range)
{
	Range = range;
	if (Source)
	{
		Emitter.CurveDistanceScaler = Range;
	}
}

bool hos::com::AudioSource::GetMute() const
{
	return Mute;
}

bool hos::com::AudioSource::GetLoop() const
{
	return Loop;
}

bool hos::com::AudioSource::GetPlayOnAwake() const
{
	return PlayOnAwake;
}

bool hos::com::AudioSource::GetIs3D() const
{
	return Is3D;
}

float hos::com::AudioSource::GetVolume() const
{
	return Volume;
}

float hos::com::AudioSource::GetRange() const
{
	return Range;
}

com::AudioSource* hos::com::AudioSource::Clone() const
{
	return new AudioSource(*this);
}

void hos::com::AudioSource::Reset()
{
	Clip = nullptr;
	Stop();
	SafeDelete(Source);
	Emitter = DirectX::AudioEmitter();
	Mute = false;
	Loop = false;
	PlayOnAwake = true;
	Volume = DEFAULT_VOLUME;
	Is3D = true;
	Range = DEFAULT_RANGE;
}

void hos::com::AudioSource::Awake()
{
	if (Clip)
	{
		if (Mute)
		{
			Source->SetVolume(0);
		}
		if(m_GameObject->GetActive() && GetActive())
		{
			if (PlayOnAwake)
			{
				Stop();
				Play();
			}
		}
	}

	Component::Awake();
}

void hos::com::AudioSource::Update()
{
	if (Is3D)
	{
		if (Source)
		{
			//Emitter.SetPosition(transform->GetPosition());
			Emitter.Update(m_GameObject->transform->GetPosition(), m_GameObject->transform->GetUp(), (float)Time->DeltaTime());
			Source->Apply3D(m_GameObject->m_Scene->GetAudioListener()->Get(), Emitter, false);
		}
	}
}

void hos::com::AudioSource::OnEnable()
{
}

void hos::com::AudioSource::OnDisable()
{
	Stop();
}

const std::vector<U8> hos::com::AudioSource::Serialize()
{
	mbstring name = ut::UTF16ToAnsi(GetName());
	mbstring clipName;
	if (Clip)
	{
		clipName = ut::UTF16ToAnsi(Clip->GetName());
	}

	flexbuffers::Builder builder;

	builder.Map([&] {
		builder.String("Name", name);
		builder.Bool("IsActive", GetActive());
		builder.Bool("Mute", Mute);
		builder.Bool("Loop", Loop);
		builder.Bool("PlayOnAwake", PlayOnAwake);
		builder.Bool("Is3D", Is3D);
		builder.Float("Volume", Volume);
		builder.Float("Range", Range);
		builder.String("AudioClip", clipName);
		});
	builder.Finish();

	return builder.GetBuffer();
}

bool hos::com::AudioSource::Deserialize(mbstring_view data)
{
	if (data.size() <= 0)
	{
		return false;
	}

	auto m = flexbuffers::GetRoot(reinterpret_cast<const uint8_t*>(data.data()), data.size()).AsMap();
	mbstring sn = m["Name"].AsString().str();
	string name = ut::AnsiToUTF16(sn);
	if (name != GetName())
	{
		Debug->LogConsole(L"DataManager", L"AudioSource try Deserialize to" + name);
		return false;
	}

	bool active = m["IsActive"].AsBool();
	SetActive(active);

	Mute = m["Mute"].AsBool();
	Loop = m["Loop"].AsBool();
	PlayOnAwake = m["PlayOnAwake"].AsBool();
	Is3D = m["Is3D"].AsBool();
	Volume = m["Volume"].AsFloat();
	Range = m["Range"].AsFloat();
	mbstring t = m["AudioClip"].AsString().str();
	string clipName = ut::AnsiToUTF16(t);
	Clip = g_DataManager->GetAudioClip(clipName);

	if (!Clip)
	{
		Clip = g_DataManager->LoadAudioClip(g_Path + AudioClip::FILE_PATH + clipName + AudioClip::FILE_EXTENSION);
	}

	if (Clip)
	{
		if (Clip->Sound)
			Source = Clip->Sound->CreateInstance(DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters).release();
	}

	return true;
}

void hos::com::AudioSource::Play()
{
	if (Source)
	{
		Source->SetVolume(Volume);
		if (Source->GetState() != DirectX::PLAYING)
		{
			Source->Play(Loop);
		}
	}
}

void hos::com::AudioSource::Stop()
{
	if (Source)
	{
		Source->Stop();
	}
}

void hos::com::AudioSource::Pause()
{
	if (Source)
	{
		if (Source->GetState() == DirectX::PLAYING)
		{
			Source->Pause();
		}
	}
}

DirectX::SoundState hos::com::AudioSource::GetState()
{
	if (Source)
	{
		return Source->GetState();
	}
	return DirectX::SoundState::STOPPED;
}

void hos::com::AudioSource::SetPitch(float pitch)
{
	if (Source)
	{
		Source->SetPitch(std::clamp(pitch, -1.f, 1.f));
	}
}
