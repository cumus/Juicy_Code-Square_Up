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
	App->pathfinding.SetWalkabilityTile(game_object->GetTransform()->GetLocalPos().x, game_object->GetTransform()->GetLocalPos().y,false);
}

Edge::Edge(const Edge& node) : B_Building(node.game_object, node.GetType())
{}

Edge::~Edge() 
{}

void Edge::RecieveEvent(const Event& e) 
{
	if (e.type == GET_DAMAGE) GotDamaged(e.data1.AsInt());	
}

void Edge::SetTexture()
{
	textureID = App->tex.Load("textures/meta.png");
	building = new Sprite(this->game_object); 
	building->tex_id = textureID;//Temporal texture

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