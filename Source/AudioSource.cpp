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

AudioSource::AudioSource(Gameobject* go) :
	Component(AUDIO_SOURCE, go)
{}

AudioSource::~AudioSource()
{
	App->audio->StopFXChannel(GetID());
}

bool AudioSource::Play(Audio_FX fx, int loops)
{
	Transform* t = game_object->GetTransform();
	return t ?
		App->audio->PlaySpatialFx(fx, GetID(), Map::F_MapToWorld(t->GetGlobalPosition()), loops) :
		App->audio->PlayFx(fx, loops);
}

void AudioSource::Halt()
{
	Event::Push(HALT_FX, App->audio, GetID());
}

void AudioSource::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case TRANSFORM_MODIFIED:
	{
		std::pair<float, float> pos = Map::F_MapToWorld(e.data1.AsVec());
		Event::Push(TRANSFORM_MODIFIED, App->audio, GetID(), vec(pos.first, pos.second));
		break;
	}
	default:
		break;
	}
};