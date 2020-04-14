#include "Spawner.h"
#include "Application.h"
#include "Gameobject.h"
#include "Render.h"
#include "Audio.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Log.h"
#include "Scene.h"

#include "EnemyMeleeUnit.h"

Spawner::Spawner(Gameobject* go) : Behaviour(go, SPAWNER, FULL_LIFE, B_SPAWNER)
{
	Transform* t = game_object->GetTransform();

	max_life = 10;
	current_life = max_life;

	create_bar();
	bar_go->SetInactive();
	CreatePanel();
	selectionPanel->SetInactive();

	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), false);
	}
}

Spawner::~Spawner()
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		App->pathfinding.SetWalkabilityTile(int(pos.x), int(pos.y), true);
	}
	b_map.erase(GetID());
}

void Spawner::SpawnMelee(float x,float y)
{
	Gameobject* melee_go = App->scene->AddGameobject("Melee enemy");
	melee_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(melee_go);
}

void SpawnRanged(float x, float y)
{
	Gameobject* melee_go = App->scene->AddGameobject("Ranged enemy");
	melee_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(melee_go);
}

void SpawnSuper(float x, float y)
{
	Gameobject* melee_go = App->scene->AddGameobject("Super Enemy");
	melee_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(melee_go);
}

void SpawnSpecial(float x, float y)
{
	Gameobject* melee_go = App->scene->AddGameobject("Special enemy");
	melee_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(melee_go);
}