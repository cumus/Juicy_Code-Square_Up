#include "AudioSource.h"
#include "Application.h"
#include "Input.h"
#include "Map.h"
#include "Render.h"
#include "Gameobject.h"
#include "Log.h"
#include "SDL/include/SDL_scancode.h"

AudioSource::AudioSource(Gameobject* go, int id) : Component(AUDIO_SOURCE, go), fx_id(id)
{
	if (go != nullptr) go->AddComponent(this);
}

AudioSource::~AudioSource()
{
	LOG("Deleting Audio Source");
	App->audio->UnloadFx(fx_id);
}

bool AudioSource::Play() const
{
	return false;
}

void AudioSource::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case ON_SELECT:
	{
		if (fx_id >= 0)
			App->audio->PlayFx(fx_id);

		LOG("BEEP!");
		break;
	}
	default:
		break;
	}
};
