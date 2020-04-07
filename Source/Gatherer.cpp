#include "Gatherer.h"
#include "Behaviour.h"
#include "Application.h"
#include "Sprite.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"


Gatherer::Gatherer(Gameobject* go) : B_Unit(go, GATHERER, IDLE, B_UNIT)
{
	atkDelay = 2.0;
	speed = 3;
	damage = 5;
	current_life = max_life = 20;
	attack_range = 2.0f;
	vision_range = 10.0f;
}

Gatherer::~Gatherer(){}
