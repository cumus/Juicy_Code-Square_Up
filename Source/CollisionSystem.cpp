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
	collisionLayers[INPUT_COLL_LAYER][INPUT_COLL_LAYER] = true;
	collisionLayers[HUD_COLL_LAYER][HUD_COLL_LAYER] = true;
	collisionLayers[BODY_COLL_LAYER][BODY_COLL_LAYER] = true;

	//Layers collisions
	collisionLayers[SCENE_COLL_LAYER][DEFAULT_COLL_LAYER] = true;
	collisionLayers[DEFAULT_COLL_LAYER][SCENE_COLL_LAYER] = true;
	collisionLayers[INPUT_COLL_LAYER][HUD_COLL_LAYER] = true;
	collisionLayers[HUD_COLL_LAYER][INPUT_COLL_LAYER] = true;
	collisionLayers[BODY_COLL_LAYER][ATTACK_COLL_LAYER] = true;
	collisionLayers[ATTACK_COLL_LAYER][BODY_COLL_LAYER] = true;
	collisionLayers[BODY_COLL_LAYER][VISION_COLL_LAYER] = true;
	collisionLayers[VISION_COLL_LAYER][BODY_COLL_LAYER] = true;
	collisionLayers[BODY_COLL_LAYER][SCENE_COLL_LAYER] = true;
	collisionLayers[SCENE_COLL_LAYER][BODY_COLL_LAYER] = true;
	collisionTree = new Quadtree(25, 5, 0, { 0.0f,0.0f,17000,9500 }, nullptr);
	debug = false;
}

CollisionSystem::~CollisionSystem()
{}

void CollisionSystem::Clear()
{
	collisionTree->Clear();
	for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	{
		itL->second.clear();		
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
			if ((*itV)->GetGoID() == obj->GetID())
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
		/*if (collisionLayers[itL->first][DEFAULT_COLL_LAYER] == false && collisionLayers[itL->first][SCENE_COLL_LAYER] == false &&
			collisionLayers[itL->first][HUD_COLL_LAYER] == false && collisionLayers[itL->first][INPUT_COLL_LAYER] == false &&
			collisionLayers[itL->first][VISION_COLL_LAYER] == false && collisionLayers[itL->first][BODY_COLL_LAYER] == false &&
			collisionLayers[itL->first][UNIT_SELECTION_LAYER] == false && collisionLayers[itL->first][ATTACK_COLL_LAYER] == false)
		{

			//LOG("Layers not colliding");
			continue;
		}*/

		if (itL->first == UNIT_SELECTION_LAYER)
		{
			//LOG("Layers not colliding");
			continue;
		}

		for (std::vector<Collider*>::const_iterator itV = itL->second.cbegin(); itV != itL->second.cend(); ++itV)//for each collider in layer
		{		
			if (!(*itV)->GetGameobject()->GetStatic())//static object not collision resolve
			{
				std::vector<Collider*> collisions = collisionTree->Search(*(*itV));
				if (!collisions.empty())
				{
					//LOG("Got collisions");
					for (std::vector<Collider*>::iterator it = collisions.begin(); it != collisions.end(); ++it)
					{
						//LOG("This go id: %lf", (*itV)->GetGameobject()->GetID());
						//LOG("Other go id: %lf", (*it)->GetGameobject()->GetID());
						if ((*itV)->GetID() != (*it)->GetID() && (*itV)->GetGoID() != (*it)->GetGoID())
						{
							//LOG("Check if collides");
							if (collisionLayers[(*itV)->GetCollLayer()][(*it)->GetCollLayer()])
							{
								Manifold m = (*itV)->Intersects(*it);
								if (m.colliding)
								{
									/*if ((*itV)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED && (*it)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED)
									{
										Event::Push(ON_COLL_ENTER, (*itV)->GetGameobject(), (*itV)->GetID(), (*it)->GetID());
										Event::Push(ON_COLL_ENTER, (*it)->GetGameobject(), (*it)->GetID(), (*itV)->GetID());
									}*/
									//LOG("Save collision");
									if (!(*itV)->GetCollisionState((*it)->GetID()))//First collision
									{
										(*itV)->SaveCollision((*it)->GetID());
										if((*itV)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED && (*it)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED)
										{
											Event::Push(ON_COLL_ENTER, (*itV)->GetGameobject(), (*itV)->GetID(), (*it)->GetID());
											Event::Push(ON_COLL_ENTER, (*it)->GetGameobject(), (*it)->GetID(), (*itV)->GetID());
										}
										//LOG("Coll enter");
									}
									else //Already collisioning
									{
										if ((*itV)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED && (*it)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED)
										{
											Event::Push(ON_COLL_STAY, (*itV)->GetGameobject(), (*itV)->GetID(), (*it)->GetID());
											Event::Push(ON_COLL_STAY, (*it)->GetGameobject(), (*it)->GetID(), (*itV)->GetID());
										}
										//LOG("Coll stay");
									}

									if ((*itV)->GetCollType() != TRIGGER && (*it)->GetCollType() != TRIGGER)
									{
										if (!(*itV)->GetGameobject()->GetStatic())
										{
											(*itV)->ResolveOverlap(m);
											//LOG("Reolve overlap");
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
									if ((*itV)->GetCollisionState((*it)->GetID()))//Last collision
									{
										if ((*itV)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED && (*it)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED) 
										{
											Event::Push(ON_COLL_EXIT, (*itV)->GetGameobject(), (*itV)->GetID(), (*it)->GetID());
											Event::Push(ON_COLL_EXIT, (*it)->GetGameobject(), (*it)->GetID(), (*itV)->GetID());
										}
										//LOG("Coll exit");
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
				if (!(*itV)->GetGameobject()->BeingDestroyed())
				{
					(*itV)->SetPosition();
					collisionTree->Insert(*itV);
				}
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
					//Coll iso
					IsoLinesCollider lines = (*itV)->GetIsoPoints();
					App->render->DrawLine({ int(lines.top.first), int(lines.top.second) }, { int(lines.left.first), int(lines.left.second) }, {0,255,0,255}, DEBUG_SCENE);
					App->render->DrawLine({ int(lines.top.first), int(lines.top.second) }, { int(lines.right.first), int(lines.right.second) }, { 0,255,0,255 }, DEBUG_SCENE);
					App->render->DrawLine({ int(lines.bot.first), int(lines.bot.second) }, { int(lines.left.first), int(lines.left.second) }, { 0,255,0,255 }, DEBUG_SCENE);
					App->render->DrawLine({ int(lines.bot.first), int(lines.bot.second) }, { int(lines.right.first), int(lines.right.second) }, { 0,255,0,255 }, DEBUG_SCENE);
					//LOG("Bot X:%f/Y:%f",lines.bot.first,lines.bot.second);
					//LOG("Top X:%f/Y:%f", lines.top.first, lines.top.second);
					//LOG("Left X:%f/Y:%f", lines.left.first, lines.left.second);
					//LOG("Right X:%f/Y:%f", lines.right.first, lines.right.second);
				}
			}
		}
		collisionTree->DebugDrawBounds();
	}
}

void CollisionSystem::SetDebug()
{
	//LOG("Toggle coll debug");
	debug = !debug;
}