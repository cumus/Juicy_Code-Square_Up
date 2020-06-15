#include "Spawner.h"
#include "Application.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Log.h"
#include "Scene.h"
#include "EnemyMeleeUnit.h"

#include <time.h>

Spawner::Spawner(Gameobject* go) : Behaviour(go, SPAWNER, NO_UPGRADE, B_SPAWNER)
{
	Transform* t = game_object->GetTransform();

	max_life = 50;
	current_life = max_life;
	shoot = true; //Control if active or not
	damage = 1;//number of unit spawns 
	ms_counter = 0;
	cooldown = 15.0f;
	maxSpawns = 500;
	currentSpawns = 0;
	providesVisibility = false;
	
	SetColliders();
}

Spawner::~Spawner()
{}

void Spawner::ChangeValues(int spawns, float cd, int spawnPoints)
{
	damage = spawnPoints;
	cooldown = cd;
	maxSpawns = spawns;
}

void Spawner::ResetSpawner()
{
	maxSpawns = 500;
	currentSpawns = 0;
}

void Spawner::Update()
{
	CheckFoWMap();
	if (shoot && ms_counter > cooldown && currentSpawns < maxSpawns)
	{
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
			if (random < MELEE_RATE) //Spawn melee
				Event::Push(SPAWN_UNIT, App->scene, ENEMY_MELEE, pos);
			else if (random < (MELEE_RATE+RANGED_RATE)) //Spawn ranged
				Event::Push(SPAWN_UNIT, App->scene, ENEMY_RANGED, pos);
			else if (random < (MELEE_RATE + RANGED_RATE+SUPER_RATE)) //Spawn super
				Event::Push(SPAWN_UNIT, App->scene, ENEMY_SUPER, pos);

			currentSpawns++;
		}

		ms_counter = 0;
		std::srand(time(NULL));
	}

	if (currentSpawns > 3 && damage < 2) damage = 2;
	if (currentSpawns > 8 && damage < 3) { damage = 3; App->scene->difficultyLvl = 1; }
	if (currentSpawns > 20 && damage < 4) { damage = 4; App->scene->difficultyLvl = 3; }
	if (currentSpawns > 40 && damage < 5) { damage = 5; App->scene->difficultyLvl = 4; }
	if (currentSpawns > 60 && damage < 6) { damage = 6; App->scene->difficultyLvl = 5; }

	if (ms_counter < cooldown)
		ms_counter += App->time.GetGameDeltaTime();
}