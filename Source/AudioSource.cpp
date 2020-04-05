#include "AudioSource.h"
#include "Application.h"
#include "Input.h"
#include "Map.h"
#include "Render.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Event.h"
#include "Log.h"
#include "SDL/include/SDL_scancode.h"
#include "SDL2_mixer-2.0.4/include/SDL_mixer.h"

AudioSource::AudioSource(Gameobject* go, Audio_FX fx, int loops) :
	Component(AUDIO_SOURCE, go),
	fx(fx),
	loops(loops)
{}

AudioSource::~AudioSource()
{
	LOG("Deleting Audio Source");
	App->audio->StopFXChannel(GetID());
}

bool AudioSource::Play()
{
	Transform* t = game_object->GetTransform();
	return t ?
		App->audio->PlaySpatialFx(fx, GetID(), Map::F_MapToWorld(t->GetGlobalPosition()), loops) :
		App->audio->PlayFx(fx, loops);
}

void AudioSource::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case ON_SELECT:
	{
		Play();
		break;
	}
	case TRANSFORM_MODIFIED:
	{
		Event::Push(TRANSFORM_MODIFIED, App->audio, GetID(), e.data1);
		break;
	}
	default:
		break;
	}
};