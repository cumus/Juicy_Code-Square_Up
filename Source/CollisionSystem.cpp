#include "CollisionSystem.h"
#include "QuadTree.h"
#include "Collider.h"

CollisionSystem::CollisionSystem()
{
	for (int i = 0; i < MAX_LAYERS; i++)
	{
		for (int a = 0; a < MAX_LAYERS; a++)
		{
			collisionLayers[i][a] = true;
		}
	}
	collisionLayers[SCENE_LAYER][HUD_LAYER] = false;
	collisionLayers[HUD_LAYER][SCENE_LAYER] = false;
}

CollisionSystem::~CollisionSystem()
{}

void CollisionSystem::SetLayerCollision(CollisionLayer one, CollisionLayer two, bool collide)
{
	collisionLayers[one][two] = collide; 
	collisionLayers[two][one] = collide;
}
