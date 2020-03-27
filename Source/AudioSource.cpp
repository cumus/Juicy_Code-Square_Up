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

AudioSource::AudioSource(Gameobject* go, int id) : Component(AUDIO_SOURCE, go), fx_id(id)
{
	if (go != nullptr) go->AddComponent(this);
}

AudioSource::~AudioSource()
{
	LOG("Deleting Audio Source");
	App->audio->UnloadFx(fx_id);
}

bool AudioSource::Play()
{
	vec position = game_object->GetTransform()->GetGlobalPosition();

	Map::I_MapToWorld(position.x, position.y);
	std::pair<int, int> source_pos = Map::I_MapToWorld(position.x, position.y);
	channel = App->audio->PlaySpatialFx(fx_id, source_pos, 0);
	LOG("BEEP! from x = %d, y = %d", source_pos.first, source_pos.second);
	return channel != 1;
}

void AudioSource::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case ON_SELECT:
	{
		Mix_HaltChannel(-1);
		Play();
		break;
	}
	default:
		break;
	}
};