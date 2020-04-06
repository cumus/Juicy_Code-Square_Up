#include "Behaviour.h"
#include "Application.h"
#include "TimeManager.h"
#include "PathfindingManager.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Sprite.h"
#include "AudioSource.h"
#include "Log.h"

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
	case ON_DESTROY: break;
	case ON_RIGHT_CLICK: OnRightClick(e.data1.AsFloat(), e.data2.AsFloat()); break;
	case DAMAGE: OnDamage(e.data1.AsInt()); break;
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
	game_object->Destroy();
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
	speed = 2;
	aux_speed = speed;
	attackRange = 1;
	damage = 1;
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
	objectiveID = 0;
	direction = NONE;
}

void B_Unit::Update()
{	
	if (objectiveID != 0)
	{
		std::map<double, Behaviour*>::iterator it;
		it = b_map.find(objectiveID);
		if (b_map.empty() == false && it != b_map.end())
		{
			vec pos = it->second->GetGameobject()->GetTransform()->GetLocalPos();
			float d = game_object->GetTransform()->DistanceTo(pos);
			
			if (d <= attackRange) //Arriba izquierda
			{
				cornerNW = true;
				DoAttack(pos);
				Event::Push(DAMAGE, it->second, damage);				
			}
			
			pos.x += it->second->GetGameobject()->GetTransform()->GetLocalScaleX();
			pos.y += it->second->GetGameobject()->GetTransform()->GetLocalScaleY();
			d = game_object->GetTransform()->DistanceTo(pos);
			if (d <= attackRange)//Abajo derecha
			{
				cornerSE = true;
				DoAttack(pos);
				Event::Push(DAMAGE, it->second, damage);
			}
				
			pos.x -= it->second->GetGameobject()->GetTransform()->GetLocalScaleX();
			d = game_object->GetTransform()->DistanceTo(pos);
			if (d <= attackRange)//Abajo izquierda
			{
				cornerSW = true;
				DoAttack(pos);
				Event::Push(DAMAGE, it->second, damage);
			}
					
			pos.x += it->second->GetGameobject()->GetTransform()->GetLocalScaleX();
			pos.y -= it->second->GetGameobject()->GetTransform()->GetLocalScaleY();
			d = game_object->GetTransform()->DistanceTo(pos);
			if (d <= attackRange)//Arriba derecha
			{
				cornerNE = true;
				DoAttack(pos);
				Event::Push(DAMAGE, it->second, damage);
			}
									
		}
	}
	else if (path != nullptr && !path->empty()) 
	{	
		vec pos = game_object->GetTransform()->GetGlobalPosition();
		fPoint actualPos = { pos.x, pos.y };		
		
		if(!next) 
		{		
			nextTile = path->front();
			//LOG("Tile coords X:%d, Y:%d",nextTile.x,nextTile.y);
			next = true;
			move = true;

			//LOG("X: %d, Y: %d", pathbegin.x, pathbegin.y);
			//LOG("X: %f, Y: %f", game_object->GetTransform()->GetGlobalPosition().x, game_object->GetTransform()->GetGlobalPosition().y);
		}

		if (positiveX && positiveY)
		{
			if (actualPos.x >= nextTile.x && actualPos.y >= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (!positiveX && !positiveY)
		{
			if (actualPos.x <= nextTile.x && actualPos.y <= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (!positiveX && positiveY)
		{
			if (actualPos.x <= nextTile.x && actualPos.y >= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
		else if (positiveX && !positiveY)
		{
			if (actualPos.x >= nextTile.x && actualPos.y <= nextTile.y)
			{
				path->erase(path->begin());
				next = false;
			}
		}
	}	
	else
	{
		move = false;
	}

	if (move)
	{
		vec pos = game_object->GetTransform()->GetGlobalPosition();
		iPoint tilePos = { int(pos.x), int(pos.y) };

		if (nextTile.x > tilePos.x) 
		{
			game_object->GetTransform()->MoveX(+speed * App->time.GetGameDeltaTime());
			positiveX = true;
		}
		else
		{
			game_object->GetTransform()->MoveX(-speed * App->time.GetGameDeltaTime());
			positiveX = false;			
		}				

		if (nextTile.y > tilePos.y) 
		{
			game_object->GetTransform()->MoveY(+speed * App->time.GetGameDeltaTime());
			positiveY = true;
		}
		else
		{
			game_object->GetTransform()->MoveY(-speed * App->time.GetGameDeltaTime());
			positiveY = false;
		}	

		if (positiveX && positiveY)//NE
		{
			current_state = MOVING_NE;
		}
		else if (!positiveX && !positiveY)//SW
		{
			current_state = MOVING_SW;
		}
		else if (!positiveX && !positiveY)//SW
		{
			current_state = MOVING_SW;
		}
		else if (!positiveX && !positiveY)//SW
		{
			current_state = MOVING_SW;
		}
	}	
}

void B_Unit::DoAttack(vec objectivePos)
{
	LOG("Do attack");
	vec localPos = game_object->GetTransform()->GetLocalPos();

	if (cornerNW && cornerNE)//arriba
	{
		direction = ATTACKING_N;
	}
	else if (cornerSW && cornerSE)//abajo
	{
		direction = ATTACKING_S;
	}
	else if (cornerSW && cornerNW)//izquierda
	{
		direction = ATTACKING_W;
	}
	else if (cornerNE && cornerSE)//derecha
	{
		direction = ATTACKING_E;
	}
	else if (cornerNW && !cornerNE && !cornerSE && !cornerSW)//arriba izquierda
	{
		direction = ATTACKING_NW;
	}
	else if (cornerNE && !cornerNW && !cornerSE && !cornerSW)//arriba derecha
	{
		direction = ATTACKING_NE;
	}
	else if (cornerSW && !cornerSE && !cornerNW && !cornerNE)//abajo izquierda
	{
		direction = ATTACKING_SW;
	}
	else if (cornerSE && !cornerSW && !cornerNW && !cornerNE)//abajo derecha
	{
		direction = ATTACKING_SE;
	}
	cornerNW = false;
	cornerNE = false;
	cornerSW = false;
	cornerSE = false;

	switch (direction)
	{
		
	}
}

void B_Unit::OnRightClick(float x, float y)
{
	Transform* t = game_object->GetTransform();
	if (t)
	{
		vec pos = t->GetGlobalPosition();
		path = App->pathfinding.CreatePath({ int(pos.x), int(pos.y) }, { int(x), int(y) }, GetID());
		next = false;
		move = false;

		audio->Play(HAMMER);

		std::map<float, Behaviour*> out;
		unsigned int total_found = GetBehavioursInRange(vec(x, y, 0.5f), 1.5f, out);
		float distance = 0;
		if (total_found > 0)
		{
			//LOG("%d behaviours found neer right click (%f, %f):", total_found, pos.x, pos.y);
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				if (GetType() == GATHERER)
				{
					if (it->second->GetType() == EDGE)
					{
						if (distance == 0)
						{
							objectiveID = it->second->GetID();
							distance = it->first;
						}
						else
						{
							if (it->first < distance)
							{
								distance = it->first;
								objectiveID = it->second->GetID();
							}
						}
						
					}
				}
				else if(it->second->GetType() == ENEMY_MELEE || it->second->GetType() == ENEMY_RANGED)
				{
					if (distance == 0)
					{
						objectiveID = it->second->GetID();
						distance = it->first;
					}
					else
					{
						if (it->first < distance)
						{
							distance = it->first;
							objectiveID = it->second->GetID();
						}
					}
				
				}
				//std::string name = it->second->GetGameobject()->GetName();
				//LOG(" - %s, id: %d, type: %d, at %f distance", name.c_str(), it->second->GetID(), int(it->second->GetType()), it->first);
			}
		}
	}
}