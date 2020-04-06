#include "EnemyMeleeUnit.h"
#include "Behaviour.h"
#include "Application.h"
#include "TextureManager.h"
#include "Gameobject.h"
#include "Component.h"
#include "Log.h"


/*
EnemyMeleeUnit::EnemyMeleeUnit(Gameobject* go, UnitType type) : B_Unit(go, type)
{
	SetTexture();
	App->pathfinding.SetWalkabilityTile(game_object->GetTransform()->GetLocalPos().x, game_object->GetTransform()->GetLocalPos().y, false);

}

EnemyMeleeUnit::EnemyMeleeUnit(const EnemyMeleeUnit& node) : B_Unit(node.game_object, node.GetType())
{}

EnemyMeleeUnit::~EnemyMeleeUnit()
{}

void EnemyMeleeUnit::RecieveEvent(const Event& e)
{
	if (current_state != DEAD)
	{
		current_life -= d;

		LOG("Current life: %d", current_life);

		if (current_life <= 0)
			OnKill();
	}

	void EnemyMeleeUnit::SetTexture()
	{
		textureID = App->tex.Load("textures/meta.png");
		unitsprite = new Sprite(this->game_object);
		unitsprite->tex_id = textureID;

		CheckSprite();
	}

	void EnemyMeleeUnit::CheckSprite()
	{
		if (unitsprite == nullptr) SetTexture();

		switch (unitstate)
		{
		case IDLE:

			break;
		case MOVING_E:

			break;
		case MOVING_W:

			break;
		case MOVING_S:

			break;
		case MOVING_N:

			break;
		case MOVING_SE:

			break;
		case MOVING_NE:

			break;
		case MOVING_SW:

			break;
		case MOVING_NW:

			break;
		case ATTACKING_E:

			break;
		case ATTACKING_W:

			break;
		case ATTACKING_S:

			break;
		case ATTACKING_N:

			break;
		case ATTACKING_SE:

			break;
		case ATTACKING_NE:

			break;
		case ATTACKING_SW:

			break;
		case ATTACKING_NW:

			break;
		case DEAD:

			break;
		}
	}

	void MeleeUnit::OnKill()
	{
		current_state = DEAD;
		game_object->Destroy(5.0f);
	}
	*/