#include "AudioSource.h"
#include "Application.h"
#include "Input.h"
#include "Map.h"
#include "Render.h"
#include "Gameobject.h"
#include "Log.h"
#include "SDL/include/SDL_scancode.h"

AudioSource::AudioSource(Gameobject* go, ComponentType type) : Component(type, go)
{
	if (go != nullptr) go->AddComponent(this);

	test_sound = App->audio->LoadFx("audio/Effects/Buildings/Select/select.wav");
}

AudioSource::~AudioSource()
{
	LOG("Deleting Audio Source");
	App->audio->UnloadFx(test_sound);
}

void AS_Object::RecieveEvent(const Event& e)
{
	/*if ((App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN))
		App->audio->PlayFx(test_sound);*/

	switch (e.type)
	{
	case ON_SELECT:
	{
		App->audio->PlayFx(test_sound);
		LOG("BEEP!");
		break;
	}
	default:
		break;
	}
};
