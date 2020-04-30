#include "CollisionSystem.h"
#include "QuadTree.h"
#include "Collider.h"
#include "Gameobject.h"
#include "Map.h"
#include "Event.h"

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
	collisionTree.Init(5, 5, 0, { 0.0f,0.0f,Map::GetMapSize_F().first,Map::GetMapSize_F().second }, nullptr);
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
			CollisionLayer layer = col->AsCollider()->GetCollLayer();
			layerColliders[layer].push_back(col->AsCollider());
		}
	}
}

void CollisionSystem::Add(Gameobject* obj)
{
	const Collider* col = obj->GetCollider();
	if (col != nullptr)
	{
		CollisionLayer layer = col->AsCollider()->GetCollLayer();
		layerColliders[layer].push_back(col->AsCollider());
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

void CollisionSystem::ProcessRemovals(Gameobject* obj)
{
	bool exit=false;
	for (std::map<CollisionLayer, std::vector<Collider*>>::const_iterator itL = layerColliders.cbegin(); itL != layerColliders.cend(); ++itL)
	{
		for (std::vector<Collider*>::const_iterator itV = itL->second.cbegin(); itV != itL->second.cend(); ++itV)
		{
			if ((*itV)->GetGameobject()->GetID() == obj->GetID())
			{
				layerColliders[itL->first].erase(itV);
				exit = true;
			}
			if (exit) break;
		}
		if (exit) break;
	}
}

//void CollisionSystem::ProcessCollisions()
//{}

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
			if (!(*itV)->GetGameobject()->GetStatic())//static object not collision resolve
			{
				//Comented due to compiler errors not declaring classes
				std::vector<Collider*> collisions = collisionTree.Search(*(*itV));
				if (!collisions.empty())
				{
					for (std::vector<Collider*>::iterator it = collisions.begin(); it != collisions.end(); ++it)
					{
						if ((*itV)->GetID() != (*it)->GetID())
						{
							if (collisionLayers[(*itV)->GetCollLayer()][(*it)->GetCollLayer()])
							{
								Manifold m = (*itV)->Intersects(*it);
								if (m.colliding)
								{
									//Save collision
									if (!(*itV)->GetCollisionState((*it)->GetID()))//First collision
									{
										(*itV)->SaveCollision((*it)->GetID());
										//Event::Push(ON_COLL_ENTER,(*itV)->GetGameobject(),(*it));
										Event::Push(ON_COLL_ENTER,(*itV)->GetGameobject(),(*it)->GetID());
									}
									else //Already collisioning
									{
										//Event::Push(ON_COLL_STAY, (*itV)->GetGameobject(), (*it));
										Event::Push(ON_COLL_STAY, (*itV)->GetGameobject(), (*it)->GetID());
									}

									if ((*itV)->GetCollType() != TRIGGER)
									{
										if ((*it)->GetGameobject()->GetStatic())
										{
											(*itV)->ResolveOverlap(m);
										}
										else
										{
											//Non static collision
											(*itV)->ResolveOverlap(m);
										}
									}
								}
								else
								{
									if ((*itV)->GetCollisionState((*it)->GetID()))//First collision
									{
										//Event::Push(ON_COLL_EXIT, (*itV)->GetGameobject(), (*it));
										Event::Push(ON_COLL_EXIT, (*itV)->GetGameobject(), (*it)->GetID());
										(*itV)->DeleteCollision((*it)->GetID());
									}
								}
							}
						}
					}
				}
			}
		}	
	}
}


void CollisionSystem::Update()
{
	collisionTree.Clear();
	for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	{
		for (std::vector<Collider*>::iterator itV = itL->second.begin(); itV != itL->second.end(); ++itV)
		{
			collisionTree.Insert(*itV);
		}
	}
	Resolve();
}