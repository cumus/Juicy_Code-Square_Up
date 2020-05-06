#include "CollisionSystem.h"
#include "Application.h"
#include "Render.h"
#include "QuadTree.h"
#include "Collider.h"
#include "Gameobject.h"
#include "Map.h"
#include "Event.h"
#include "Log.h"

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
	collisionTree = new Quadtree(10, 5, 0, { 0.0f,0.0f,Map::GetMapSize_F().first,Map::GetMapSize_F().second }, nullptr);
	debug = false;
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

void CollisionSystem::Add(Collider* coll)
{
	if (coll != nullptr)
	{
		layerColliders[coll->GetCollLayer()].push_back(coll);
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
	//LOG("Resolve");
	for (std::map<CollisionLayer, std::vector<Collider*>>::const_iterator itL = layerColliders.cbegin(); itL != layerColliders.cend(); ++itL)//for each layer
	{
		//LOG("Layer");
		if (collisionLayers[itL->first][DEFAULT_LAYER] == false && collisionLayers[itL->first][SCENE_LAYER] == false && 
			collisionLayers[itL->first][HUD_LAYER] == false && collisionLayers[itL->first][INPUT_LAYER] == false)
		{
			LOG("Layers not colliding");
			continue;
		}

		for (std::vector<Collider*>::const_iterator itV = itL->second.cbegin(); itV != itL->second.cend(); ++itV)//for each collider in layer
		{		
			if (!(*itV)->GetGameobject()->GetStatic())//static object not collision resolve
			{
				std::vector<Collider*> collisions = collisionTree->Search(*(*itV));
				if (!collisions.empty())
				{
					LOG("Got collisions");
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
										Event::Push(ON_COLL_ENTER, (*itV)->GetGameobject(), (*itV)->GetID(), (*it)->GetID());
										LOG("Coll enter");
									}
									else //Already collisioning
									{
										//Event::Push(ON_COLL_STAY, (*itV)->GetGameobject(), (*it));
										Event::Push(ON_COLL_STAY, (*itV)->GetGameobject(), (*itV)->GetID(),(*it)->GetID());
										LOG("Coll stay");
									}

									if ((*itV)->GetCollType() != TRIGGER && (*it)->GetCollType() != TRIGGER)
									{
										if (!(*itV)->GetGameobject()->GetStatic())
										{
											(*itV)->ResolveOverlap(m);
											LOG("Reolve overlap");
										}
										/*else
										{
											//Non static collision
											(*itV)->ResolveOverlap(m);
										}*/
									}
								}
								else
								{
									if ((*itV)->GetCollisionState((*it)->GetID()))//First collision
									{
										//Event::Push(ON_COLL_EXIT, (*itV)->GetGameobject(), (*it));
										Event::Push(ON_COLL_EXIT, (*itV)->GetGameobject(), (*itV)->GetID(), (*it)->GetID());
										LOG("Coll exit");
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
	collisionTree->Clear();
	//LOG("Tree clear");
	for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	{
		if (!itL->second.empty())
		{
			//LOG("colliders length: %d", itL->second.size());
			for (std::vector<Collider*>::iterator itV = itL->second.begin(); itV != itL->second.end(); ++itV)
			{
				(*itV)->SetPosition();
				collisionTree->Insert(*itV);
			}
		}
	}
	Resolve();

	if (debug)
	{
		for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
		{
			if (!itL->second.empty())
			{
				//LOG("colliders length: %d", itL->second.size());
				for (std::vector<Collider*>::iterator itV = itL->second.begin(); itV != itL->second.end(); ++itV)
				{
					(*itV)->SetPosition();
					RectF rect = (*itV)->GetISOColliderBounds();
					LOG("Bounds X:%f/Y:%f/W:%f/H:%f",rect.x,rect.y,rect.w,rect.h);
					App->render->DrawQuad({int(rect.x),int(rect.y),int(rect.w),int(rect.h)}, {0,255,0,255},true, DEBUG_SCENE,true);
				}
			}
		}
	}
}

void CollisionSystem::SetDebug()
{
	//LOG("Toggle coll debug");
	debug = !debug;
}