#include "Bang/AudioSource.h"

#include "Bang/Gizmos.h"
#include "Bang/XMLNode.h"
#include "Bang/AudioClip.h"
#include "Bang/Transform.h"
#include "Bang/AudioClip.h"
#include "Bang/Texture2D.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"
#include "Bang/ICloneable.h"
#include "Bang/IconManager.h"
#include "Bang/AudioManager.h"

USING_NAMESPACE_BANG

AudioSource::AudioSource()
{
}

AudioSource::~AudioSource()
{
    Stop();
}

void AudioSource::OnStart()
{
    Component::OnStart();
    if (GetPlayOnStart() && !AudioManager::GetPlayOnStartBlocked())
    {
        Play();
    }
}

void AudioSource::OnRender(RenderPass rp)
{
    Component::OnRender(rp);
    if (rp == RenderPass::Overlay)
    {
        Gizmos::Reset();
        Gizmos::SetSelectable(GetGameObject());
        Gizmos::SetPosition( GetGameObject()->GetTransform()->GetPosition() );
        Gizmos::SetScale( Vector3(0.1f) );
        Gizmos::RenderIcon( IconManager::GetAudioIcon().Get(), true );
    }
}

void AudioSource::OnUpdate()
{
    Component::OnUpdate();

    if (GetAudioClip())
    {
        if (m_currentAudioClipALBufferId != GetAudioClip()->GetALBufferId())
        {
            SetAudioClip(GetAudioClip());
        }
    }

    Transform *tr = GetGameObject()->GetTransform();
    if (tr)
    {
        ALAudioSource::SetPosition(tr->GetPosition());
    }
}

void AudioSource::SetAudioClip(AudioClip *audioClip)
{
    p_audioClip.Set(audioClip);
    if (GetAudioClip())
    {
        SetALBufferId(audioClip->GetALBufferId());
        m_currentAudioClipALBufferId = audioClip->GetALBufferId();
    }
    else
    {
        SetALBufferId(0);
        m_currentAudioClipALBufferId = 0;
    }
}

void AudioSource::SetPlayOnStart(bool playOnStart)
{
    m_playOnStart = playOnStart;
}

void AudioSource::Play()
{
    Play(0.0f);
}

void AudioSource::Play(float delay)
{
    AudioManager::Play(GetAudioClip(), this, delay);
}

bool AudioSource::GetPlayOnStart() const { return m_playOnStart; }
AudioClip *AudioSource::GetAudioClip() const { return p_audioClip.Get(); }
float AudioSource::GetPlayProgress() const
{
    float secondsOffset;
    alGetSourcef(GetALSourceId(), AL_SEC_OFFSET, &secondsOffset);
    alGetSourcef(GetALSourceId(), AL_SEC_OFFSET, &secondsOffset);
    return secondsOffset / GetAudioClip()->GetLength();
}

void AudioSource::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    AudioSource *as = Cast<AudioSource*>(clone);
    as->SetAudioClip( GetAudioClip() );
    as->SetVolume( GetVolume()  );
    as->SetPitch( GetPitch() );
    as->SetRange( GetRange() );
    as->SetLooping( GetLooping() );
    as->SetPlayOnStart( GetPlayOnStart() );
}

void AudioSource::ImportXML(const XMLNode &xml)
{
    Component::ImportXML(xml);

    if (xml.Contains("AudioClip"))
    {
        RH<AudioClip> audioClip =
                    Resources::Load<AudioClip>(xml.Get<GUID>("AudioClip"));
        SetAudioClip(audioClip.Get());
    }

    if (xml.Contains("Volume"))
    { SetVolume(xml.Get<float>("Volume")); }

    if (xml.Contains("Pitch"))
    { SetPitch(xml.Get<float>("Pitch")); }

    if (xml.Contains("Range"))
    { SetRange(xml.Get<float>("Range")); }

    if (xml.Contains("Looping"))
    { SetLooping(xml.Get<bool>("Looping")); }

    if (xml.Contains("PlayOnStart"))
    { SetPlayOnStart(xml.Get<bool>("PlayOnStart")); }
}

void AudioSource::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);

    AudioClip *audioClip = GetAudioClip();
    GUID audioClipGUID = audioClip ? audioClip->GetGUID() : GUID::Empty();
    xmlInfo->Set("AudioClip",   audioClipGUID);
    xmlInfo->Set("Volume",      GetVolume());
    xmlInfo->Set("Pitch",       GetPitch());
    xmlInfo->Set("Range",       GetRange());
    xmlInfo->Set("Looping",     GetLooping());
    xmlInfo->Set("PlayOnStart", GetPlayOnStart());
}
