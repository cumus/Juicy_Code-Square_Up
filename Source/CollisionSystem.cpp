#include "CollisionSystem.h"
#include "QuadTree.h"
#include "Collider.h"
#include "Gameobject.h"

#include <vector>

CollisionSystem::CollisionSystem()
{
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		for (int a = 0; a < MAX_COLLISION_LAYERS; a++)
		{
			collisionLayers[i][a] = false;
		}
	}
	collisionLayers[SCENE_LAYER][SCENE_LAYER] = true;
	collisionLayers[SCENE_LAYER][DEFAULT_LAYER] = true;
	collisionLayers[DEFAULT_LAYER][SCENE_LAYER] = true;
	collisionLayers[INPUT_LAYER][HUD_LAYER] = true;
}

CollisionSystem::~CollisionSystem()
{}

void CollisionSystem::SetLayerCollision(CollisionLayer one, CollisionLayer two, bool collide)
{
	collisionLayers[one][two] = collide; 
	collisionLayers[two][one] = collide;
}

void CollisionSystem::Add(std::vector<Gameobject*>& objects)
{
	for (std::vector<Gameobject*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it)
	{
		const Collider* col = (*it)->GetCollider();
		if (col != nullptr)
		{
			//CollisionLayer layer = col->GetLayer();
			//layerColliders[layer].push_back(col);
		}
	}
}

void CollisionSystem::ProcessRemovals()
{
	for (std::map<CollisionLayer, std::vector<Collider*>>::const_iterator itL = layerColliders.cbegin(); itL != layerColliders.cend(); ++itL)
	{
		for (std::vector<Collider*>::const_iterator itV = itL->second.cbegin(); itV != itL->second.cend(); ++itV)
		{
			if ((*itV)->GetGameobject()->BeingDestroyed())
			{
				layerColliders[itL->first].erase(itV);
			}
		}
	}
}

void CollisionSystem::Resolve()
{
	for (std::map<CollisionLayer, std::vector<Collider*>>::const_iterator itL = layerColliders.cbegin(); itL != layerColliders.cend(); ++itL)
	{
		if (collisionLayers[itL->first][DEFAULT_LAYER] == false && collisionLayers[itL->first][SCENE_LAYER] == false && 
			collisionLayers[itL->first][HUD_LAYER] == false && collisionLayers[itL->first][INPUT_LAYER] == false)
		{
			continue;
		}

		for (std::vector<Collider*>::const_iterator itV = itL->second.cbegin(); itV != itL->second.cend(); ++itV)
		{
			if ((*itV)->GetGameobject()->BeingDestroyed())
			{
				layerColliders[itL->first].erase(itV);
			}
		}
	}
}

void CollisionSystem::Update()
{
	//collisionTree.Clear();
	for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	{
		for (std::vector<Collider*>::iterator itV = itL->second.begin(); itV != itL->second.end(); ++itV)
		{
			//Comented due to compiler errors not declaring classes
			/*if (!(*itV)->GetGameobject()->GetStatic())//static object not collision resolve
			{
				std::vector<Collider*> collisions = collisionTree.Search(*(*itV));
				for (std::vector<Collider*>::iterator it = collisions.begin(); it != collisions.end(); ++itV)
				{
					if ((*itV)->GetID() != (*it)->GetID())
					{
						if (collisionLayers[(*itV)->GetCollLayer()][(*it)->GetCollLayer()])
						{
							Manifold m = (*itV)->Intersects(*it);
							if (m.colliding)
							{
								if ((*itV)->GetCollType() == TRIGGER)
								{
									//TODO: Trigger collider events
									//onEnter, onExit...
								}
								else
								{
									if ((*it)->GetGameobject()->GetStatic())
									{
										(*itV)->ResolveOverlap(m);
									}
									else
									{
										//TODO: Resolve non static collisions
										(*itV)->ResolveOverlap(m);
									}
								}
							}							
						}
					}
				}
			}*/
		}
	}
	Resolve();
}
