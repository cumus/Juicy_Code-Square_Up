#include "Edge.h"
#include "Behaviour.h"
#include "Application.h"
#include "TextureManager.h"


Edge::~Edge() 
{}

void Edge::SetTexture()
{
	textureID = App->tex.Load("textures/meta.png");
	building = new Sprite(this->game_object); //Full life sprite
	building->tex_id = textureID;//Temporal texture

	CheckSprite();
}

void Edge::CheckSprite()
{
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