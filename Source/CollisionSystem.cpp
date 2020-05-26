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
	/*for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	{
		itL->second.clear();		
	}*/
	for (int i=0;i<MAX_COLLISION_LAYERS;i++)
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
		//LOG("Layer %d",a);
		layerColliders[a].push_back(coll);
	}
}

void CollisionSystem::ProcessRemovals()
{
	/*for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	{
		for (std::vector<Collider*>::iterator itV = itL->second.begin(); itV != itL->second.end(); ++itV)
		{
			if ((*itV)->GetGameobject()->GetBehaviour()->GetState() == DESTROYED)
			{
				itL->second.erase(itV);
			}
		}
	}*/

	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if ((*it)->GetGameobject()->GetBehaviour()->GetState() == DESTROYED)
				{
					layerColliders[i].erase(it);
				}
			}
		}
	}
}

void CollisionSystem::ProcessRemovals(double id)
{
	/*for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	{
		for (std::vector<Collider*>::iterator itV = itL->second.begin(); itV != itL->second.end(); ++itV)
		{
			//LOG("Check 1");
			if ((*itV)->GetGoID() == obj->GetID())
			{
				itL->second.erase(itV);
				LOG("Delete go collider");
			}
		}
	}*/

	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if ((*it)->GetGoID() == id)
				{
					layerColliders[i].erase(it);
				}
			}
		}
	}
}

void CollisionSystem::DeleteCollider(Collider coll)
{
	/*for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	{
		for (std::vector<Collider*>::iterator itV = itL->second.begin(); itV != itL->second.end(); ++itV)
		{			
			if ((*itV)->GetID() == coll.GetID())
			{
				itL->second.erase(itV);
				LOG("Deleted collider");
				break;
			}
		}
	}*/
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if ((*it)->GetID() == coll.GetID())
				{
					layerColliders[i].erase(it);
				}
			}
		}
	}
}

void CollisionSystem::Resolve()
{
	//for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)//for each layer
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
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

		if (i == UNIT_SELECTION_LAYER) continue;
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
						if ((*it)->GetID() != (*itColls)->GetID() && (*it)->GetGoID() != (*itColls)->GetGoID() && !(*itColls)->parentGo->GetBehaviour()->GetState() != DESTROYED)
						{
							//LOG("Check if collides");
							if (collisionLayers[(*it)->GetCollLayer()][(*itColls)->GetCollLayer()])
							{
								Manifold m = (*it)->Intersects(*itColls);
								if (m.colliding)
								{
									//LOG("Save collision");
									/*if (!(*itV)->GetCollisionState((*it)->GetID()))//First collision
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
									}*/

									Event::Push(ON_COLLISION, (*it)->GetGameobject(), (*it)->GetID(), (*itColls)->GetID());
									Event::Push(ON_COLLISION, (*itColls)->GetGameobject(), (*itColls)->GetID(), (*it)->GetID());

									if ((*it)->GetCollType() != TRIGGER && (*itColls)->GetCollType() != TRIGGER)
									{
										if (!(*it)->GetGameobject()->GetStatic())
										{
											(*it)->ResolveOverlap(m);
											//(*itV)->ResolveOverlap(m);
											//LOG("Reolve overlap");
										}
										/*else
										{
											//Non static collision
											(*itV)->ResolveOverlap(m);
										}*/
									}
								}
								/*else
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
								}*/
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
	Behaviour::selectableUnits.clear();
//	LOG("Tree clear");
	//for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		//if (!itL->second.empty())
		if(!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if (!(*it)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED)
				{
					(*it)->SetPosition();
					if ((*it)->GetCollLayer() != UNIT_SELECTION_LAYER) collisionTree->Insert(*it);
					else Behaviour::selectableUnits.push_back((*it)->GetGameobject()->GetBehaviour()->GetID());
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
				//LOG("colliders length: %d", itL->second.size());
				for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
				{
					//Coll iso
					IsoLinesCollider lines = (*it)->GetIsoPoints();
					if(!(*it)->selectionColl)
					{
						SDL_Rect quadTreeRect = (*it)->GetColliderBounds();
						App->render->DrawLine({ int(lines.top.first), int(lines.top.second) }, { int(lines.left.first), int(lines.left.second) }, { 0,255,0,255 }, DEBUG_SCENE);
						App->render->DrawLine({ int(lines.top.first), int(lines.top.second) }, { int(lines.right.first), int(lines.right.second) }, { 0,255,0,255 }, DEBUG_SCENE);
						App->render->DrawLine({ int(lines.bot.first), int(lines.bot.second) }, { int(lines.left.first), int(lines.left.second) }, { 0,255,0,255 }, DEBUG_SCENE);
						App->render->DrawLine({ int(lines.bot.first), int(lines.bot.second) }, { int(lines.right.first), int(lines.right.second) }, { 0,255,0,255 }, DEBUG_SCENE);
						App->render->DrawQuad(quadTreeRect, { 234,254,30,255 }, false, DEBUG_SCENE);
					}
				}
			}
		}
		collisionTree->DebugDrawBounds();
	}
}

void CollisionSystem::SetDebug() { debug = !debug; }