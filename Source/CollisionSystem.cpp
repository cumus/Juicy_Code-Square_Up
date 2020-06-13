#include "CollisionSystem.h"
#include "Application.h"
#include "Render.h"
#include "QuadTree.h"
#include "Collider.h"
#include "Gameobject.h"
#include "Map.h"
#include "Event.h"
#include "Log.h"
#include "Behaviour.h"

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
	//Self layer collisions
	collisionLayers[SCENE_COLL_LAYER][SCENE_COLL_LAYER] = true;
	collisionLayers[DEFAULT_COLL_LAYER][DEFAULT_COLL_LAYER] = true;
	collisionLayers[BODY_COLL_LAYER][BODY_COLL_LAYER] = true;

	//Layers collisions
	collisionLayers[SCENE_COLL_LAYER][DEFAULT_COLL_LAYER] = true;
	collisionLayers[DEFAULT_COLL_LAYER][SCENE_COLL_LAYER] = true;
	collisionLayers[BODY_COLL_LAYER][SCENE_COLL_LAYER] = true;
	collisionLayers[SCENE_COLL_LAYER][BODY_COLL_LAYER] = true;
	collisionLayers[BODY_COLL_LAYER][VISION_COLL_LAYER] = true;
	collisionLayers[VISION_COLL_LAYER][BODY_COLL_LAYER] = true;
	collisionLayers[BODY_COLL_LAYER][ATTACK_COLL_LAYER] = true;
	collisionLayers[ATTACK_COLL_LAYER][BODY_COLL_LAYER] = true;
	collisionTree = new Quadtree(10, 5, 0, { 0,0,14500,9000 }, nullptr);
	debug = false;
}

CollisionSystem::~CollisionSystem()
{}

void CollisionSystem::Clear()
{
	collisionTree->Clear();
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		layerColliders[i].clear();
	}
}

Quadtree* CollisionSystem::GetQuadTree() { return collisionTree; }

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
			layerColliders[int(layer)].push_back(col->AsCollider());
		}
	}
}

void CollisionSystem::Add(Gameobject* obj)
{
	const Collider* col = obj->GetCollider();
	if (col != nullptr)
	{
		CollisionLayer layer = col->AsCollider()->GetCollLayer();
		layerColliders[int(layer)].push_back(col->AsCollider());
	}
}

void CollisionSystem::Add(Collider* coll)
{
	if (coll != nullptr)
	{
		int a = coll->GetCollLayer();
		layerColliders[a].push_back(coll);
	}
}

void CollisionSystem::ProcessRemovals()
{
	std::vector<Collider*> cache;
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if ((*it)->GetGameobject()->GetBehaviour()->IsDestroyed() == false) cache.push_back((*it)); 
			}
			layerColliders[i].clear();
			if (!cache.empty()) { layerColliders[i] = cache; cache.clear(); }			
		}
	}
}

void CollisionSystem::ProcessRemovals(double id)
{
	std::vector<Collider*> cache;
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if ((*it)->GetGoID() != id) cache.push_back(*it);
				else (*it)->SetInactive();
			}
			if (!cache.empty()) layerColliders[i] = cache;
			cache.clear();
		}
	}
}

void CollisionSystem::Resolve()
{
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (layerColliders[i].empty()) continue;

		for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)//for each collider in layer
		{
			if (!(*it)->GetGameobject()->GetStatic())//static object not collision resolve
			{
				std::vector<Collider*> collisions = collisionTree->Search(*(*it));
				if (!collisions.empty())
				{
					//LOG("Got collisions: %d",collisions.size());
					for (std::vector<Collider*>::iterator itColls = collisions.begin(); itColls != collisions.end(); ++itColls)//For each posible detection in quad tree
					{
						if ((*it)->GetID() != (*itColls)->GetID() && (*it)->GetGoID() != (*itColls)->GetGoID())
						{
							//LOG("Check if collides");
							if (collisionLayers[(*it)->GetCollLayer()][(*itColls)->GetCollLayer()])
							{
								Manifold m = (*it)->Intersects(*itColls);
								if (m.colliding)
								{
									//LOG("Collides");
									Event::Push(ON_COLLISION, (*it)->parentGo, (*it)->GetID(), (*itColls)->GetID());
									Event::Push(ON_COLLISION, (*itColls)->parentGo, (*itColls)->GetID(), (*it)->GetID());

									if ((*it)->GetCollType() != TRIGGER && (*itColls)->GetCollType() != TRIGGER)
									{									
										(*it)->ResolveOverlap(m);
										//LOG("Resolve overlap");
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
	ProcessRemovals();
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if ((*it)->IsActive() && (*it)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED)
				{
					(*it)->SetPosition();
					collisionTree->Insert(*it);
				}
			}
		}
	}

	Resolve();

	if (debug)
	{
		for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
		{
			if (!layerColliders[i].empty())
			{
				for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
				{
					//Coll iso
					IsoLinesCollider lines = (*it)->GetIsoPoints();
					SDL_Rect quadTreeRect = (*it)->GetColliderBounds();
					App->render->DrawLine({ int(lines.top.first), int(lines.top.second) }, { int(lines.left.first), int(lines.left.second) }, { 0,255,0,255 }, DEBUG_SCENE);
					App->render->DrawLine({ int(lines.top.first), int(lines.top.second) }, { int(lines.right.first), int(lines.right.second) }, { 0,255,0,255 }, DEBUG_SCENE);
					App->render->DrawLine({ int(lines.bot.first), int(lines.bot.second) }, { int(lines.left.first), int(lines.left.second) }, { 0,255,0,255 }, DEBUG_SCENE);
					App->render->DrawLine({ int(lines.bot.first), int(lines.bot.second) }, { int(lines.right.first), int(lines.right.second) }, { 0,255,0,255 }, DEBUG_SCENE);
					App->render->DrawQuad(quadTreeRect, { 234,254,30,255 }, false, DEBUG_SCENE);					
				}
			}
		}
		collisionTree->DebugDrawBounds();
	}
}

void CollisionSystem::SetDebug() { debug = !debug; }