#include "MeleeUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Collider.h"
#include "Log.h"

MeleeUnit::MeleeUnit(Gameobject* go) : B_Unit(go, UNIT_MELEE, IDLE, B_MELEE_UNIT)
{
	LOG("Unit init");
	//Stats	
	max_life = 100;
	current_life = max_life;
	atkTime = 1.0;
	speed = 3;
	damage = 5;
	attack_range = 2.0f;
	vision_range = 20.0f;
	providesVisibility = true;
	//SFX
	deathFX = MELEE_DIE_FX;
	attackFX = MELEE_ATK_FX;
	//SetColliders();
}

MeleeUnit::~MeleeUnit()
{
	
}


void MeleeUnit::SetColliders()
{
	LOG("Set colliders");
	//Colliders
	pos = game_object->GetTransform()->GetGlobalPosition();
	bodyColl = new Collider(game_object, { pos.x,pos.y,game_object->GetTransform()->GetLocalScaleX(),game_object->GetTransform()->GetLocalScaleY() }, NON_TRIGGER, PLAYER_TAG, { 0,Map::GetBaseOffset(),0,0 });
	visionColl = new Collider(game_object, { pos.x,pos.y,vision_range,vision_range }, TRIGGER, PLAYER_VISION_TAG, { vision_range * vision_range * Map::GetBaseOffset(),Map::GetBaseOffset(),0,0 });
	//attackColl = new Collider(game_object, { pos.x,pos.y,attack_range,attack_range }, TRIGGER, PLAYER_ATTACK_TAG, {});
}
