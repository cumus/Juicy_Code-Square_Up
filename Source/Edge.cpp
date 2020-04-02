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
	if (currentLife < (startingLife / 2))
	{
		if (currentLife > 0) building->section = { 0, 0, 64, 64 };		
		else building->section = { 64, 0, 64, 64 };		
	}
	else building->section = { 128, 0, 64, 64 };
}

void Edge::BuildingAction()
{

}