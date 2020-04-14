/*#include "Spawner.h"
#include "Application.h"
#include "Gameobject.h"
#include "Render.h"
#include "Audio.h"
#include "AudioSource.h"
#include "Transform.h"
#include "Log.h"
#include "Scene.h"

#include "EnemyMeleeUnit.h"

//#include <time.h>

Spawner::Spawner(Gameobject* go) : Behaviour(go, SPAWNER, FULL_LIFE, B_SPAWNER)
{
	Transform* t = game_object->GetTransform();

	max_life = 10;
	current_life = max_life;
	shoot = true; //Control if active or not
	damage = 5;//number of unit spawns 
	ms_counter = 0;
	cooldown = 5.0f;
	maxSpawns = 100;
	currentSpawns = 0;

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

void Spawner::ChangeValues(int spawns, float cd, int spawnPoints)
{
	damage = spawnPoints;
	cooldown = cd;
	maxSpawns = spawns;
}


void Spawner::SpawnMelee(float x,float y)
{
	Gameobject* melee_go = App->scene->AddGameobject("Melee enemy");
	melee_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(melee_go);
}

void Spawner::SpawnRanged(float x, float y)
{
	//Temporal
	Gameobject* melee_go = App->scene->AddGameobject("Ranged enemy");
	melee_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(melee_go);
}

void Spawner::SpawnSuper(float x, float y)
{
	//Temporal
	Gameobject* melee_go = App->scene->AddGameobject("Super Enemy");
	melee_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(melee_go);
}

void Spawner::SpawnSpecial(float x, float y)
{
	//Temporal
	Gameobject* melee_go = App->scene->AddGameobject("Special enemy");
	melee_go->GetTransform()->SetLocalPos({ x, y, 0.0f });

	new EnemyMeleeUnit(melee_go);
}

void Spawner::ResetSpawner()
{
	maxSpawns = 100;
	currentSpawns = 0;
}

void Spawner::Update()
{
	if (shoot && ms_counter > cooldown && currentSpawns < maxSpawns)
	{
		LOG("Spawn time");
		vec pos = game_object->GetTransform()->GetGlobalPosition();
		bool incX = false;
		for (int i = 0; i < damage; i++)
		{		
			if (incX)
			{
				pos.x++;
				incX = false;
			}
			else
			{
				pos.y++;
				incX = true;
			}

			int random = std::rand() % 100 + 1;
			LOG("Random %d", random);
			if (random < MELEE_RATE) //Spawn melee
			{
				LOG("melee");
				SpawnMelee(pos.x,pos.y);
			}
			else if (random < (MELEE_RATE+RANGED_RATE)) //Spawn ranged
			{
				LOG("ranged");
				SpawnRanged(pos.x, pos.y);
			}
			else if (random < (MELEE_RATE + RANGED_RATE+SUPER_RATE)) //Spawn super
			{
				LOG("super");
				SpawnSuper(pos.x, pos.y);
			}
			else //Spawn special
			{
				LOG("Special");
				SpawnSpecial(pos.x, pos.y);
			}
			currentSpawns++;
			LOG("Spawned one");
		}
		
		ms_counter = 0;
		std::srand(time(NULL));
	}

	if (ms_counter < cooldown)
	{
		ms_counter += App->time.GetGameDeltaTime();
	}
}*/