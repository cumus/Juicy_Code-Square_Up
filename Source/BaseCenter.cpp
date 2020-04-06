#include "BaseCenter.h"
#include "Application.h"
#include "Gameobject.h"
#include "Render.h"
#include "Audio.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Log.h"
#include "Input.h"
#include "SDL/include/SDL_scancode.h"
#include "Scene.h"



Base_Center::Base_Center(Gameobject* go) : Behaviour(go, BASE_CENTER, FULL_LIFE, B_BASE_CENTER)
{
	current_life = max_life = 100;
}

Base_Center::~Base_Center()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}
}


void Base_Center::OnDamage(int d)
{
	if (current_state != DESTROYED)
	{
		current_life -= d;

		LOG("Current life: %d", current_life);

		if (current_life <= 0)
			OnKill();
		else if (current_life >= max_life * 0.5f)
			current_state = FULL_LIFE;
		else
			current_state = HALF_LIFE;
	}
}

void Base_Center::OnKill()
{
	App->audio->PlayFx(B_DESTROYED);
	current_state = DESTROYED;
	game_object->Destroy(1.0f);

}

void Base_Center::Upgrade()
{
	if (bc_lvl < bc_max_lvl) {

		max_life += 50;
		bc_lvl += 1;
		App->audio->PlayFx(B_BUILDED);
		LOG("LIFE AFTER UPGRADE: %d", max_life);
		LOG("BC LEVEL: %d", bc_lvl);
	}

}


void Base_Center::OnRightClick(float x, float y)
{
	//Upgrade();
	SpawnUnit(x,y);
}

void Base_Center::SpawnUnit(float x,float y) 
{
		
	Gameobject* unit_go = App->scene->AddGameobject("Game Unit - son of root");
	unit_go->GetTransform()->SetLocalPos({x, y, 0.0f });
	new B_Unit(unit_go, UNIT_MELEE, IDLE);
	
}