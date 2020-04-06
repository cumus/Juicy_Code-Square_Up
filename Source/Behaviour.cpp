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
	selection_highlight = new Sprite(go, App->tex.Load("textures/selectionMark.png"), { 0, 0, 64, 64 }, BACK_SCENE);
	selection_highlight->SetInactive();

	b_map.insert({ GetID(), this });
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
	current_life -= d;

	if (current_life <= 0)
		OnKill();
}

void Behaviour::OnKill()
{
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
{}

void B_Unit::Update()
{	
	if (path != nullptr && !path->empty()) 
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
		if (total_found > 0)
		{
			LOG("%d behaviours found neer right click (%f, %f):", total_found, pos.x, pos.y);
			for (std::map<float, Behaviour*>::iterator it = out.begin(); it != out.end(); ++it)
			{
				std::string name = it->second->GetGameobject()->GetName();
				LOG(" - %s, id: %d, type: %d, at %f distance", name.c_str(), it->second->GetID(), int(it->second->GetType()), it->first);
			}
		}
	}
}