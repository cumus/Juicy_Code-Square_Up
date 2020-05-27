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
	for (int i=0;i < MAX_COLLISION_LAYERS;i++)
	{
		//layerColliders[i].clear();
		for (int a=0;a < layerColliders->next;a++)
		{
			layerColliders[i].colliders[a] = nullptr;
		}
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
			LayerColliders* coll = &layerColliders[int(layer)];
			//layerColliders[int(layer)].push_back(col->AsCollider());
			coll->colliders[coll->next] = col->AsCollider();
			coll->next++;
		}
	}
}

void CollisionSystem::Add(Gameobject* obj)
{
	const Collider* col = obj->GetCollider();
	if (col != nullptr)
	{
		CollisionLayer layer = col->AsCollider()->GetCollLayer();
		LayerColliders* coll = &layerColliders[int(layer)];
		//layerColliders[int(layer)].push_back(col->AsCollider());
		coll->colliders[coll->next] = col->AsCollider();
		coll->next++;
	}	
}

void CollisionSystem::Add(Collider* coll)
{
	if (coll != nullptr)
	{
		int a = coll->GetCollLayer();
		LayerColliders* collLay = &layerColliders[a];
		//layerColliders[a].push_back(coll);
		collLay->colliders[collLay->next] = coll;
		collLay->next++;
	}
}

void CollisionSystem::ProcessRemovals()
{
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (layerColliders[i].next != 0)//not empty
		{
			LayerColliders* lay = &layerColliders[i];
			for (int a=0;a < lay->next;a++)
			{
				if (lay->colliders[a]->parentGo->GetBehaviour()->IsDestroyed())
				{
					lay->colliders[a] = lay->colliders[lay->next - 1];
					lay->next--;
					a -= 1;
				}
			}
		}
		/*if (!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if ((*it)->GetGameobject()->GetBehaviour()->GetState() == DESTROYED)
				{
					layerColliders[i].erase(it);
				}							
			}			
		}*/
	}
}

void CollisionSystem::ProcessRemovals(double id)
{
	//std::vector<Collider*> cache;
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (layerColliders[i].next != 0)//not empty
		{
			LayerColliders* lay = &layerColliders[i];
			for (int a = 0; a < lay->next; a++)
			{
				if (lay->colliders[a]->GetGoID() == id)
				{
					lay->colliders[a] = lay->colliders[lay->next - 1];
					lay->next--;
					a -= 1;
				}
			}
		}
		/*if (!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if ((*it)->GetGoID() == id)
				{
					layerColliders[i].erase(it);
				}
			}
		}*/
	}
}

void CollisionSystem::DeleteCollider(Collider coll)
{
	//std::vector<Collider*> cache;
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (layerColliders[i].next != 0)//not empty
		{
			LayerColliders* lay = &layerColliders[i];
			for (int a = 0; a < lay->next; a++)
			{
				if (lay->colliders[a]->GetID() == coll.GetID())
				{
					lay->colliders[a] = lay->colliders[lay->next - 1];
					lay->next--;
					a -= 1;
				}
			}
		}
		/*if (!layerColliders[i].empty())
		{
			for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if ((*it)->GetID() == coll.GetID())
				{
					layerColliders[i].erase(it);
				}
			}
		}*/
	}
}

void CollisionSystem::Resolve()
{
	//for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)//for each layer
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		if (i == UNIT_SELECTION_LAYER) continue;
		//if (layerColliders[i].empty()) continue;
		if (layerColliders[i].next == 0) continue;
		
		LayerColliders* lay = &layerColliders[i];
		//for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)//for each collider in layer
		for (int a = 0; a < lay->next;a++)//for each collider in layer
		{		
			//if (!(*it)->GetGameobject()->GetStatic())//static object not collision resolve
			if(!lay->colliders[a]->GetGameobject()->GetStatic())
			{
				//std::vector<Collider*> collisions = collisionTree->Search(*(*it));
				std::vector<Collider*> collisions = collisionTree->Search(*lay->colliders[a]);
				if (!collisions.empty())
				{
					//LOG("Got collisions: %d",collisions.size());
					for (std::vector<Collider*>::iterator itColls = collisions.begin(); itColls != collisions.end(); ++itColls)//For each posible detection in quad tree
					{					
						if (/*(*it)->GetID()*/lay->colliders[a]->GetID() != (*itColls)->GetID() && /*(*it)->GetGoID()*/  lay->colliders[a]->GetGoID() != (*itColls)->GetGoID() && !(*itColls)->parentGo->GetBehaviour()->IsDestroyed())
						{
							//LOG("Check if collides");
							if (collisionLayers[/*(*it)->GetCollLayer()*/lay->colliders[a]->GetCollLayer()][(*itColls)->GetCollLayer()])
							{
								Manifold m = lay->colliders[a]->Intersects(*itColls);//(*it)->Intersects(*itColls);
								if (m.colliding)
								{
									Event::Push(ON_COLLISION, lay->colliders[a]->parentGo, lay->colliders[a]->GetID(), (*itColls)->GetID());
									//Event::Push(ON_COLLISION, (*it)->parentGo, (*it)->GetID(), (*itColls)->GetID());
									//Event::Push(ON_COLLISION, (*itColls)->parentGo, (*itColls)->GetID(), (*it)->GetID());

									//if ((*it)->GetCollType() != TRIGGER && (*itColls)->GetCollType() != TRIGGER)
									if (lay->colliders[a]->GetCollType() != TRIGGER && (*itColls)->GetCollType() != TRIGGER)
									{
										if(!lay->colliders[a]->parentGo->GetStatic())
										//if (!(*it)->ParentGo->GetStatic())
										{
											lay->colliders[a]->ResolveOverlap(m);
											//(*it)->ResolveOverlap(m);
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
	//ProcessRemovals();
	//for (std::map<CollisionLayer, std::vector<Collider*>>::iterator itL = layerColliders.begin(); itL != layerColliders.end(); ++itL)
	for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
	{
		//if (!itL->second.empty())
		//if(!layerColliders[i].empty())
		if(layerColliders[i].next != 0)
		{
			/*for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
			{
				if (!(*it)->GetGameobject()->GetBehaviour()->GetState() != DESTROYED)
				{
					(*it)->SetPosition();
					if ((*it)->GetCollLayer() != UNIT_SELECTION_LAYER) collisionTree->Insert(*it);					
				}
			}*/
			LayerColliders* lay = &layerColliders[i];
			for (int a=0;a < lay->next;a++)
			{
				if (!lay->colliders[a]->parentGo->GetBehaviour()->IsDestroyed())
				{
					lay->colliders[a]->SetPosition();
					if (lay->colliders[a]->GetCollLayer() != UNIT_SELECTION_LAYER) collisionTree->Insert(lay->colliders[a]);
				}
			}
		}
	}
	Resolve();

	if (debug)
	{
		for (int i = 0; i < MAX_COLLISION_LAYERS; i++)
		{
			//if (!layerColliders[i].empty())
			if(layerColliders[i].next != 0)
			{
				//LOG("colliders length: %d", itL->second.size());
				/*for (std::vector<Collider*>::iterator it = layerColliders[i].begin(); it != layerColliders[i].end(); ++it)
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
				}*/
				LayerColliders* lay = &layerColliders[i];
				for (int a = 0; a < lay->next; a++)
				{
					//Coll iso
					IsoLinesCollider lines = lay->colliders[a]->GetIsoPoints();
					if(!lay->colliders[a]->selectionColl)
					{
						SDL_Rect quadTreeRect = lay->colliders[a]->GetColliderBounds();
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