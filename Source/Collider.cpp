#include "SDL/include/SDL.h"
#include "Collider.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Vector3.h"
#include "Application.h"
#include "Render.h"
#include "Log.h"
#include "CollisionSystem.h"
#include "Map.h"


Collider::Collider(Gameobject* go, RectF coll, ColliderType t, ColliderTag tg, CollisionLayer lay, ComponentType ty) : Component(ty, go)
{
	boundary = coll;
    collType = t;
    layer = lay;
    tag = tg;
    App->collSystem.Add(this);
    offset.x = 0;
    offset.y = 0;
    offset.w = 0;
    offset.h = 0;
}

Collider::~Collider()
{}

void Collider::SetPosition()
{
    vec pos = game_object->GetTransform()->GetGlobalPosition();
    //LOG("Coll pos X:%f/Y:%f", pos.x, pos.y);
    boundary.x = pos.x - (boundary.w / 2) + offset.x;
    boundary.y = pos.y - (boundary.h / 2) + offset.y;
    
    //LOG("Bound pos X:%f/Y:%f",boundary.x,boundary.y);
}

Manifold Collider::Intersects(Collider* other)
{
    Manifold m;
    m.colliding = false;
    //const RectF& thisColl = GetColliderBounds();
    //const RectF& otherColl = other->GetColliderBounds();
    const RectF& thisColl = GetISOColliderBounds();
    const RectF& otherColl = other->GetISOColliderBounds();

    if (otherColl.x - otherColl.w > thisColl.x + thisColl.w ||
        otherColl.x + otherColl.w < thisColl.x - thisColl.w ||
        otherColl.y - otherColl.h > thisColl.y + thisColl.h ||
        otherColl.y + otherColl.h < thisColl.y - thisColl.h) //Intersects
    {
        m.colliding = true;
        m.other = &otherColl;
        LOG("Intersects");
    }

    return m;
}

void Collider::ResolveOverlap(Manifold& m)
{
    if (collType != TRIGGER)
    {
        Transform* t = game_object->GetTransform();

        //const RectF& rect1 = GetColliderBounds();
        const RectF rect1 = GetISOColliderBounds();
        const RectF* rect2 = m.other;
        float res = 0;
        float xDif = (rect1.x + (rect1.w * 0.5f)) - (rect2->x + (rect2->w * 0.5f));
        float yDif = (rect1.y + (rect1.h * 0.5f)) - (rect2->y + (rect2->h * 0.5f));

        if (fabs(xDif) > fabs(yDif))
        {
            if (xDif > 0) // Colliding on the left.
            {
                res = 5;//(rect2->x + rect2->w) - rect1.x;
            }
            else // Colliding on the right.
            {
                res = -5;//((rect1.x + rect1.w) - rect2->x);
            }
            LOG("Move res %f",res);
            t->MoveX(res * App->time.GetGameDeltaTime());//Move x      
        }
        else
        {
            if (yDif > 0) // Colliding above.
            {
                res = 5;//(rect2->y + rect2->h) - rect1.y;
            }
            else // Colliding below
            {
                res = -5;//((rect1.y + rect1.h) - rect2->y);
            }
            LOG("Move res %f", res);
            t->MoveY(res * App->time.GetGameDeltaTime());//Move y
        }
        vec pos = t->GetGlobalPosition();
        LOG("New pos X:%f/Y:%f",pos.x,pos.y);
    }  
}

void Collider::SaveCollision(double id)
{    
    if (!GetCollisionState(id))
    {
        collisions.push_back(id);
    }    
}

bool Collider::GetCollisionState(double ID)
{
    bool ret = false;
    for (std::vector<double>::iterator it = collisions.begin(); it != collisions.end(); ++it)
    {
        if (ID == *it) ret = true;
    }
    return ret;
}

void Collider::DeleteCollision(double ID)
{
    for (std::vector<double>::iterator it = collisions.begin(); it != collisions.end(); ++it)
    {
        if (ID == *it)
        {
            collisions.erase(it);
            break;
        }
    }
}

RectF Collider::GetISOColliderBounds()
{ 
    std::pair<float, float> pos = Map::F_WorldToMap(boundary.x, boundary.y);
    std::pair<float, float> length = Map::F_WorldToMap(boundary.x + boundary.w, boundary.y + boundary.h);
    return  RectF({ pos.first,pos.second,length.first,length.second});
}

void Collider::SetLayer(CollisionLayer lay) { layer = lay; }

CollisionLayer Collider::GetCollLayer() { return layer; }

void Collider::SetColliderBounds(RectF& rect) { boundary = rect; }

RectF Collider::GetColliderBounds() { return boundary; }

void Collider::SetOffset(RectF off) { offset = off;}

void Collider::SetCollType(ColliderType t) { collType = t; }

ColliderType Collider::GetCollType() { return collType; }

void Collider::SetColliderTag(ColliderTag tg) { tag = tg; }

ColliderTag Collider::GetColliderTag() { return tag; }