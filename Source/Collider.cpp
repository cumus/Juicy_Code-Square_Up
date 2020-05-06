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
#include "Point.h"


Collider::Collider(Gameobject* go, RectF coll, ColliderType t, ColliderTag tg, CollisionLayer lay, ComponentType ty) : Component(ty, go)
{
    /*std::pair<float, float> localPos = Map::F_MapToWorld(coll.x, coll.y, 1.0f);
	boundary.x = localPos.first;
    boundary.y = localPos.second;
    boundary.w = coll.w * 64;
    boundary.h = coll.h * 64;*/
    vec s = game_object->GetTransform()->GetGlobalScale();
    std::pair<float, float> tile_size = Map::GetTileSize_F();

    boundary = coll;
    boundary.w = tile_size.first *= s.x;
    boundary.h = tile_size.second *= s.y;
   
    collType = t;
    layer = lay;
    tag = tg;
    App->collSystem.Add(this);
    offset.x = 33.0f;
    offset.y = 16.5f;
    offset.w = 0;
    offset.h = 0;
}

Collider::~Collider()
{}

void Collider::SetPosition()
{
    vec pos = game_object->GetTransform()->GetGlobalPosition();
    //std::pair<float, float> localPos = Map::F_MapToWorld(pos.x, pos.y, pos.z);
    //LOG("Coll pos X:%f/Y:%f", pos.x, pos.y);
   // boundary.x = localPos.first - (boundary.w / 2) + offset.x;
   // boundary.y = localPos.second - (boundary.h / 2) + offset.y;
    boundary.x = pos.x - (boundary.w / 2) + offset.x;
    boundary.y = pos.y - (boundary.h / 2) + offset.y;
    
    //LOG("Bound pos X:%f/Y:%f",boundary.x,boundary.y);
}

Manifold Collider::Intersects(Collider* other)
{
    Manifold m;
    const RectF thisColl = GetWorldColliderBounds();
    const RectF otherColl = other->GetWorldColliderBounds();
    m.colliding = true;
    m.other = &otherColl;
    //LOG("This coll W:%f/H:%f",thisColl.w,thisColl.h);
    //LOG("Other coll W:%f/H:%f", otherColl.w, otherColl.h);

    /*if (otherColl.x - otherColl.w > thisColl.x + thisColl.w ||
        otherColl.x + otherColl.w < thisColl.x - thisColl.w ||
        otherColl.y - otherColl.h > thisColl.y + thisColl.h ||
        otherColl.y + otherColl.h < thisColl.y - thisColl.h) //Intersects
    {
        m.colliding = true;
        m.other = &otherColl;
        LOG("Intersects");
    }*/

    fPoint topRight1(thisColl.x+thisColl.w, thisColl.y);
    fPoint topRight2(otherColl.x+otherColl.w, otherColl.y);
    fPoint bottomLeft1(thisColl.x, thisColl.y + thisColl.h);
    fPoint bottomLeft2(otherColl.x, otherColl.y + otherColl.h);
   // LOG("This coll tr X:%f/Y:%f    bl X:%f/Y:%f", topRight1.x, topRight1.y, bottomLeft1.x, bottomLeft1.y);
    //LOG("Other  coll tr X:%f/Y:%f    bl X:%f/Y:%f", topRight2.x, topRight2.y, bottomLeft2.x, bottomLeft2.y);
    if (topRight1.y > bottomLeft2.y || bottomLeft1.y < topRight2.y ||
        topRight1.x < bottomLeft2.x || bottomLeft1.x > topRight2.x) //Non colliding
    {
        //LOG("Non colliding");
        m.colliding = false;
        m.other = nullptr;
    }
   /* if (topRight1.y > bottomLeft2.y)
    {
        LOG("Bottom");
    }
    if (bottomLeft1.y < topRight2.y)
    {
        LOG("TOP");
    }
    if (topRight1.x < bottomLeft2.x)
    {
        LOG("LEFT");
    }
    if (bottomLeft1.x > topRight2.x)
    {
        LOG("RIGHT");
    }*/

 
    return m;
}

void Collider::ResolveOverlap(Manifold& m)
{
    if (collType != TRIGGER)
    {
        Transform* t = game_object->GetTransform();

        //const RectF& rect1 = GetColliderBounds();
        const RectF rect1 = GetWorldColliderBounds();
        const RectF* rect2 = m.other;
        float res = 0;
        float xDif = (rect1.x + (rect1.w * 0.5f)) - (rect2->x + (rect2->w * 0.5f));
        float yDif = (rect1.y + (rect1.h * 0.5f)) - (rect2->y + (rect2->h * 0.5f));

        if (fabs(xDif) > fabs(yDif))
        {
            if (xDif > 0) // Colliding on the left.
            {
                res = 8;//(rect2->x + rect2->w) - rect1.x;
            }
            else // Colliding on the right.
            {
                res = -8;//((rect1.x + rect1.w) - rect2->x);
            }
            //LOG("Move res %f",res);
            t->MoveX(res * App->time.GetGameDeltaTime());//Move x      
        }
        else
        {
            if (yDif > 0) // Colliding above.
            {
                res = -8;//(rect2->y + rect2->h) - rect1.y;
            }
            else // Colliding below
            {
                res = 8;// ((rect1.y + rect1.h) - rect2->y);
            }
            //LOG("Move res %f", res);
            t->MoveY(res * App->time.GetGameDeltaTime());//Move y
        }
        //vec pos = t->GetGlobalPosition();
        //LOG("New pos X:%f/Y:%f",pos.x,pos.y);
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

RectF Collider::GetWorldColliderBounds()
{
    std::pair<float, float> pos = Map::F_MapToWorld(boundary.x, boundary.y, 1.0f);
    //std::pair<float, float> length = Map::F_MapToWorld(boundary.x + boundary.w, boundary.y + boundary.h, 1.0f);
    return  RectF({ pos.first,pos.second,boundary.w,boundary.h });
}

void Collider::SetLayer(CollisionLayer lay) { layer = lay; }

CollisionLayer Collider::GetCollLayer() { return layer; }

void Collider::SetColliderBounds(RectF& rect) 
{
    vec s = game_object->GetTransform()->GetGlobalScale();
    std::pair<float, float> tile_size = Map::GetTileSize_F();

    boundary = rect;
    boundary.w = tile_size.first *= s.x;
    boundary.h = tile_size.second *= s.y;
}

RectF Collider::GetColliderBounds() { return boundary; }

void Collider::SetOffset(RectF off) { offset = off;}

void Collider::SetCollType(ColliderType t) { collType = t; }

ColliderType Collider::GetCollType() { return collType; }

void Collider::SetColliderTag(ColliderTag tg) { tag = tg; }

ColliderTag Collider::GetColliderTag() { return tag; }