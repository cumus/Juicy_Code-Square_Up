#include "Behaviour.h"
#include "Application.h"
#include "TimeManager.h"
#include "PathfindingManager.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Sprite.h"
#include "AudioSource.h"
#include "Log.h"
#include "Vector3.h"

std::map<double, Behaviour*> Behaviour::b_map;

Behaviour::Behaviour(Gameobject* go, UnitType t, UnitState starting_state, ComponentType comp_type) :
	Component(comp_type, go),
	type(t),
	current_state(starting_state)
{
	current_life = max_life = damage = 10;
	attack_range = vision_range = 5.0f;

	audio = new AudioSource(game_object);
	new AnimatedSprite(this);
	selection_highlight = new Sprite(go, App->tex.Load("textures/selectionMark.png"), { 0, 0, 64, 64 }, BACK_SCENE, { 0, -32, 1.f, 1.f });
	selection_highlight->SetInactive();

	b_map.insert({ GetID(), this });
}

Behaviour::~Behaviour()
{
	b_map.erase(GetID());
}

void Behaviour::RecieveEvent(const Event& e)
{
	switch (e.type)
	{
	case ON_PLAY: break;
	case ON_PAUSE: break;
	case ON_STOP: break;
	case ON_SELECT: Selected(); break;
	case ON_UNSELECT: UnSelected(); break;
	case ON_DESTROY: OnDestroy(); break;
	case ON_RIGHT_CLICK: OnRightClick(e.data1.AsFloat(), e.data2.AsFloat()); break;
	case DAMAGE: OnDamage(e.data1.AsInt()); break;
	case IMPULSE: OnGetImpulse(e.data1.AsFloat(),e.data2.AsFloat()); break;
	}
}

void Behaviour::Selected()
{
	selection_highlight->SetActive();
}

void Behaviour::UnSelected()
{
	selection_highlight->SetInactive();
}

void Behaviour::OnDamage(int d)
{
	LOG("Got damage");
	current_life -= d;

	if (current_life <= 0)
		OnKill();
}

void Behaviour::OnKill()
{
	current_life = 0;
	current_state = DESTROYED;
	current_state = DESTROYED;
	game_object->Destroy(2.0f);
}

unsigned int Behaviour::GetBehavioursInRange(vec pos, float dist, std::map<float, Behaviour*>& res) const
{
	unsigned int ret = 0;

	for (std::map<double, Behaviour*>::iterator it = b_map.begin(); it != b_map.end(); ++it)
	{
		if (it->first != GetID())
		{
			Transform* t = it->second->game_object->GetTransform();
			if (t)
			{
				float d = t->DistanceTo(pos);
				if (d < dist)
				{
					ret++;
					res.insert({ d, it->second });
				}
			}
		}
	}

	return ret;
}


///////////////////////////
// UNIT BEHAVIOUR
///////////////////////////

B_Unit::B_Unit(Gameobject* go, UnitType t, UnitState s, ComponentType comp_type) :
	Behaviour(go, t, s, comp_type)
{
	//Depending on unit
	atkDelay = 1.0;
	speed = 5;//MAX SPEED 60
	attack_range = 3.0f;
	damage = 5;

	//Needed
	path = nullptr;
	nextTile;
	next = false;
	move = false;
	positiveX = false;
	positiveY = false;
	cornerNW = false;
	cornerNE = false;
	cornerSW = false;
	cornerSE = false;
	dirX = 0;
	dirY = 0;
	inRange = false;
	attackObjective = nullptr;
	msCount = 0;
}

void B_Unit::Update()
{	
	current_state = IDLE;
	if (attackObjective !=nullptr && attackObjective->GetState() != DESTROYED) //Attack
	{
		//LOG("FOUND");
		attackPos = attackObjective->GetGameobject()->GetTransform()->GetGlobalPosition();
		float d = game_object->GetTransform()->DistanceTo(attackPos);
		//LOG("Distance 1:%f",d);
		if (d <= attack_range) //Arriba izquierda
		{				
			cornerNW = true;
			inRange = true;
		}
		attackPos.x += attackObjective->GetGameobject()->GetTransform()->GetLocalScaleX();
		attackPos.y += attackObjective->GetGameobject()->GetTransform()->GetLocalScaleY();
		d = game_object->GetTransform()->DistanceTo(attackPos);
		//LOG("Distance 2:%f", d);
		if (d <= attack_range)//Abajo derecha
		{
			cornerSE = true;
			inRange = true;
		}
				
		attackPos.x -= attackObjective->GetGameobject()->GetTransform()->GetLocalScaleX();
		d = game_object->GetTransform()->DistanceTo(attackPos);
		//LOG("Distance 3:%f", d);
		if (d <= attack_range)//Abajo izquierda
		{
			cornerSW = true;
			inRange = true;
		}
					
		attackPos.x += attackObjective->GetGameobject()->GetTransform()->GetLocalScaleX();
		attackPos.y -= attackObjective->GetGameobject()->GetTransform()->GetLocalScaleY();
		d = game_object->GetTransform()->DistanceTo(attackPos);
		//LOG("Distance 4:%f", d);
		if (d <= attack_range)//Arriba derecha
		{
			cornerNE = true;
			inRange = true;
		}					
	}
	else
	{
		attackObjective = nullptr;
		cornerNW = false;
		cornerSE = false;
		cornerNE = false;
		cornerSW = false;
		inRange = false;
	}

	if (msCount < atkDelay)
	{
		msCount += App->time.GetGameDeltaTime();
	}

	if (inRange)
	{
		if (msCount >= atkDelay)
		{
			LOG("Do attack");
			DoAttack(attackPos);
			Event::Push(DAMAGE, attackObjective, damage);
			msCount = 0;
		}
	}		
	else if (path != nullptr && !path->empty()) //Movement
	{	
		vec pos = game_object->GetTransform()->GetGlobalPosition();
		fPoint actualPos = { pos.x, pos.y };		
		
		if(!next) 
		{		
			nextTile = path->front();
			next = true;
			move = true;
		}

		if (dirX == 1 && dirY == 1)
		{
			if (actualPos.x >= nextTile.x && actualPos.y >= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (dirX == -1 && dirY == -1)
		{
			if (actualPos.x <= nextTile.x && actualPos.y <= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (dirX == -1 && dirY == 1)
		{
			if (actualPos.x <= nextTile.x && actualPos.y >= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (dirX == 1 && dirY == -1)
		{
			if (actualPos.x >= nextTile.x && actualPos.y <= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if(dirX == 0 && dirY ==- 1)
		{
			if (actualPos.y <= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (dirX == 0 && dirY == 1)
		{
			if (actualPos.y >= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (dirX == 1 && dirY == 0)
		{
			if (actualPos.x >= nextTile.x)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (dirX == -1 && dirY == 0)
		{
			if (actualPos.x <= nextTile.x)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (dirX == 0 && dirY == 0)
		{
			path->erase(path->begin());
			next = false;
		}
	}	
	else
	{
		move = false;
	}

	if (move)
	{
		vec pos = game_object->GetTransform()->GetLocalPos();
		iPoint tilePos = { int(pos.x), int(pos.y) };
		if (nextTile.x > tilePos.x)
		{
			dirX = 1;
		}
		else if (nextTile.x < tilePos.x)
		{
			dirX = -1;
		}
		else dirX = 0;

		if (nextTile.y > tilePos.y)
		{
			dirY = 1;
		}
		else if (nextTile.y < tilePos.y)
		{
			dirY = -1;
		}
		else dirY = 0;

		game_object->GetTransform()->MoveX(dirX * speed * App->time.GetGameDeltaTime());//Move x
		game_object->GetTransform()->MoveY(dirY * speed * App->time.GetGameDeltaTime());//Move y

		//Change state to change sprite
		if (dirX == 0 && dirY == 0)
		{
			current_state = IDLE;
		}
		else if(dirX == 1 && dirY==1)//NE
		{
			current_state = MOVING_NE;
		}
		else if (dirX == -1 && dirY == -1)//SO
		{
			current_state = MOVING_SW;
		}
		else if (dirX == 1 && dirY == -1)//SE
		{
			current_state = MOVING_SE;
		}
		else if (dirX == -1 && dirY == 1)//NO
		{
			current_state = MOVING_NW;
		}
		else if (dirX == 0 && dirY == 1)//N
		{
			current_state = MOVING_N;
		}
		else if (dirX == 1 && dirY == 0)//E
		{
			current_state = MOVING_E;
		}
		else if (dirX == 0 && dirY == -1)//S
		{
			current_state = MOVING_S;
		}
		else if (dirX == -1 && dirY == 0)//O
		{
			current_state = MOVING_W;
		}			
	}

	//Colision check
	vec pos = game_object->GetTransform()->GetGlobalPosition();
	std::map<float, Behaviour*> out;
	unsigned int total_found = GetBehavioursInRange(pos, 1.4f, out);
	if (total_found > 0)
	{
		fPoint pos(0, 0);
		pos.x = game_object->GetTransform()->GetGlobalPosition().x;
		pos.y = game_object->GetTransform()->GetGlobalPosition().y;
		for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
		{
			vec otherPos = it->second->GetGameobject()->GetTransform()->GetGlobalPosition();
			fPoint separationSpd(0, 0);
			separationSpd.x = pos.x - otherPos.x;
			separationSpd.y = pos.y - otherPos.y;
			if (it->second->GetState() != DESTROYED)
			{
				Event::Push(IMPULSE, it->second->AsBehaviour(), -separationSpd.x, -separationSpd.y);
			}			
		}
	}	
}

void B_Unit::DoAttack(vec objectivePos)
{
	vec localPos = game_object->GetTransform()->GetLocalPos();

	if (cornerNW && cornerNE)//arriba
	{
		current_state = ATTACKING_N;
	}
	else if (cornerSW && cornerSE)//abajo
	{
		current_state = ATTACKING_S;
	}
	else if (cornerSW && cornerNW)//izquierda
	{
		current_state = ATTACKING_W;
	}
	else if (cornerNE && cornerSE)//derecha
	{
		current_state = ATTACKING_E;
	}
	else if (cornerNW && !cornerNE && !cornerSE && !cornerSW)//arriba izquierda
	{
		current_state = ATTACKING_NW;
	}
	else if (cornerNE && !cornerNW && !cornerSE && !cornerSW)//arriba derecha
	{
		current_state = ATTACKING_NE;
	}
	else if (cornerSW && !cornerSE && !cornerNW && !cornerNE)//abajo izquierda
	{
		current_state = ATTACKING_SW;
	}
	else if (cornerSE && !cornerSW && !cornerNW && !cornerNE)//abajo derecha
	{
		current_state = ATTACKING_SE;
	}
	cornerNW = false;
	cornerSE = false;
	cornerNE = false;
	cornerSW = false;
}

void B_Unit::OnDestroy()
{
	App->pathfinding.DeletePath(GetID());
}

void B_Unit::OnRightClick(float x, float y)
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();		
		next = false;
		move = false;

		audio->Play(HAMMER);

		std::map<float, Behaviour*> out;
		unsigned int total_found = GetBehavioursInRange(vec(x, y, 0.5f), 1.5f, out);
		float distance = 0;
		if (total_found > 0)
		{
			LOG("Unit cliked");
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				if (GetType() == GATHERER)
				{
					if (it->second->GetType() == EDGE)
					{
						if (distance == 0)//Chose closest
						{
							attackObjective = it->second;
							distance = it->first;
						}
						else
						{
							if (it->first < distance)
							{
								distance = it->first;
								attackObjective = it->second;
							}
						}

					}
				}
				else if (it->second->GetType() == ENEMY_MELEE || it->second->GetType() == ENEMY_RANGED || it->second->GetType() == EDGE)//Temporal
				{
					if (distance == 0)//Closest distance
					{
						attackObjective = it->second;
						distance = it->first;
					}
					else
					{
						if (it->first < distance)
						{
							distance = it->first;
							attackObjective = it->second;
						}
					}

				}
			}
			path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(x-1), int(y-1) }, GetID());
		}
		else
		{
			path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(x), int(y) }, GetID());
			attackObjective = nullptr;
		}
	}
}
 
void B_Unit::OnGetImpulse(float x, float y)
{
	float tempX = game_object->GetTransform()->GetGlobalPosition().x;
	float tempY = game_object->GetTransform()->GetGlobalPosition().y;
	if (App->pathfinding.ValidTile(int(tempX), int(tempY)) == false)
	{
		game_object->GetTransform()->MoveX(-6 * x * App->time.GetGameDeltaTime());//Move x
		game_object->GetTransform()->MoveY(-6 * y * App->time.GetGameDeltaTime());//Move y
	}
	else
	{
		game_object->GetTransform()->MoveX(6 * x * App->time.GetGameDeltaTime());//Move x
		game_object->GetTransform()->MoveY(6 * y * App->time.GetGameDeltaTime());//Move y
	}
}

