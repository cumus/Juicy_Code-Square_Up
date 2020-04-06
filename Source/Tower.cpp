#include "Tower.h"
#include "Application.h"
#include "Gameobject.h"
#include "Audio.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Log.h"
#include "MapContainer.h"

Tower::Tower(Gameobject* go) : Behaviour(go, TOWER, FULL_LIFE, B_TOWER)
{
	current_life = max_life = 50;
}

Tower::~Tower()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}
}



void Tower::OnDamage(int d)
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

void Tower::OnKill()
{
	App->audio->PlayFx(B_DESTROYED);
	current_state = DESTROYED;
	game_object->Destroy(1.0f);

}


void Tower::Upgrade()
{
	if (t_lvl < t_max_lvl) {
		
		t_lvl += 1;
		max_life += 25;
		t_damage += 2;
		App->audio->PlayFx(B_BUILDED);
		LOG("LIFE AFTER UPGRADE: %d", max_life);
		LOG("Tower LEVEL: %d", t_lvl);
	}

}

void Tower::OnRightClick(float x, float y)
{
	Upgrade();
	//OnDamage(10);
}

void Tower::DoAttack(vec pos)
{
	
	
}



