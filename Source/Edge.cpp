#include "Edge.h"
#include "Behaviour.h"
#include "Application.h"
#include "TextureManager.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"



Edge::Edge(Gameobject* go, ComponentType type) : B_Building(go, type)
{
	SetTexture();
}

Edge::Edge(const Edge& node) : B_Building(node.game_object, node.GetType())
{}

Edge::~Edge() 
{}

void Edge::RecieveEvent(const Event& e) 
{
	switch (e.type)
	{
		case GET_DAMAGE: GotDamaged(e.data1.AsInt()); break;
		case SELECTED: Selected(); break;
		case UNSELECTED: UnSelected(); break;
		case SPAWNED: 
			App->pathfinding.SetWalkabilityTile(game_object->GetTransform()->GetLocalPos().x, game_object->GetTransform()->GetLocalPos().y, false); 
			break;
	}
}

void Edge::Selected()
{
	selectionMark->section = { 0, 0, 64, 64 };
	selected = true;
}

void Edge::UnSelected()
{
	selectionMark->section = { 64, 0, 64, 64 };
	selected = false;
}

void Edge::SetTexture()
{
	textureID = App->tex.Load("textures/meta.png");
	building = new Sprite(this->game_object); 
	building->tex_id = textureID;//Temporal texture

	textureSelectionID = App->tex.Load("textures/selectionMark.png");
	selectionMark = new Sprite(this->game_object);
	selectionMark->tex_id = textureSelectionID;
	
	CheckSprite();
}

void Edge::CheckSprite()
{
	if (building == nullptr) SetTexture();

	switch (currentState)
	{
	case FULL:
		building->section = { 128, 0, 64, 64 };
		break;
	case HALF:
		building->section = { 0, 0, 64, 64 };
		break;
	case DESTROYED:
		building->section = { 64, 0, 64, 64 };
		break;
	}
}

void Edge::BuildingAction()
{

}

void Edge::FreeWalkability()
{ 
	App->pathfinding.SetWalkabilityTile(game_object->GetTransform()->GetLocalPos().x, game_object->GetTransform()->GetLocalPos().y, true);
}