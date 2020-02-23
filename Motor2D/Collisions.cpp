#include "Collisions.h"
#include "Application.h"
#include "Render.h"
#include "Input.h"
#include "Log.h"

Collisions::Collisions() : Module("collisions")
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	/*matrix[COLLIDER][COLLIDER] = false;
	matrix[COLLIDER][NEXTLVL] = true;
	matrix[COLLIDER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER][COLLIDER_DAMAGE] = false;

	matrix[COLLIDER_PLAYER][COLLIDER] = true;
	matrix[COLLIDER_PLAYER][NEXTLVL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_DAMAGE] = true;
		
	matrix[NEXTLVL][COLLIDER] = false;
	matrix[NEXTLVL][NEXTLVL] = false;
	matrix[NEXTLVL][COLLIDER_PLAYER] = true;
	matrix[NEXTLVL][COLLIDER_DAMAGE] = false;

	matrix[COLLIDER_DAMAGE][COLLIDER] = false;
	matrix[COLLIDER_DAMAGE][NEXTLVL] = false;
	matrix[COLLIDER_DAMAGE][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_DAMAGE][COLLIDER_DAMAGE] = false;*/
}


Collisions::~Collisions()
{}

bool Collisions::PreUpdate()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	return true;
}


bool Collisions::Update()
{

	// To be implemented debug capabilities for painting colliders and godmode
	DebugDraw();
	if (App->input->GetKey(SDL_SCANCODE_F10))
		god_mode = !god_mode;
		

	if (god_mode)
	{
		matrix[COLLIDER_PLAYER][COLLIDER_DAMAGE] = false;
		matrix[COLLIDER_DAMAGE][COLLIDER_PLAYER] = false;

	}
	else
	{
		matrix[COLLIDER_PLAYER][COLLIDER_DAMAGE] = true;
		matrix[COLLIDER_DAMAGE][COLLIDER_PLAYER] = true;

	}

	return true;

}

void Collisions::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE:
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER:
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case NEXTLVL:
			App->render->DrawQuad(colliders[i]->rect, 100, 100, 100, alpha);
			break;
		case COLLIDER_PLAYER:
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_DAMAGE:
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		}
	}
}

bool Collisions::CleanUp()
{
	LOG("Freeing all colliders");
	
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}
	return true;

}

Collider* Collisions::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}

