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
#include "JuicyMath.h"


Collider::Collider(Gameobject* go, RectF coll, ColliderType t, ColliderTag tg, RectF off ,CollisionLayer lay, ComponentType ty) : Component(ty, go)
{
    tileSize = Map::GetTileSize_F();
    boundary = coll;
    boundary.w *= tileSize.first;
    boundary.h *= tileSize.second;

    collType = t;
    layer = lay;
    tag = tg;
    App->collSystem.Add(this);
    offset = off;
    GoID = go->GetID();
    if (lay == UNIT_SELECTION_LAYER) selectionColl = true;
    else selectionColl = false;
}

Collider::~Collider()
{}

void Collider::SetPosition()
{
    vec localpos = game_object->GetTransform()->GetGlobalPosition();
    std::pair<float, float> pos = Map::F_MapToWorld(localpos.x, localpos.y, localpos.z);
    //LOG("Map pos X:%f/Y:%f",localpos.x,localpos.y);
    //LOG("Screen pos X:%f/Y:%f", pos.first, pos.second);
    //LOG("Coll pos X:%f/Y:%f", pos.x, pos.y);
    boundary.x = pos.first;
    boundary.y = pos.second;
    isoDraw.left = { boundary.x - boundary.w * 0.5f + tileSize.first * 0.5f, boundary.y };
    isoDraw.bot = { boundary.x + tileSize.first * 0.5f, boundary.y - boundary.h * 0.5f };
    isoDraw.right = { boundary.x + boundary.w * 0.5f + tileSize.first * 0.5f, boundary.y };
    isoDraw.top = { boundary.x + tileSize.first * 0.5f, boundary.y + boundary.h * 0.5f };
    isoDraw.right.first += offset.x;
    isoDraw.left.first += offset.x;
    isoDraw.top.first += offset.x;
    isoDraw.bot.first += offset.x;
    isoDraw.right.second += offset.y;
    isoDraw.left.second += offset.y;
    isoDraw.top.second += offset.y;
    isoDraw.bot.second += offset.y;
    if (selectionColl)
    {
        isoDraw.right.first += rightOffset.first;
        isoDraw.left.first += leftOffset.first;
        isoDraw.top.first += topOffset.first;
        isoDraw.bot.first += botOffset.first;
        isoDraw.right.second += rightOffset.second;
        isoDraw.left.second += leftOffset.second;
        isoDraw.top.second += topOffset.second;
        isoDraw.bot.second += botOffset.second;
    }
    //LOG("Bound pos X:%f/Y:%f",boundary.x,boundary.y);
}

void Collider::ConvertToIsoPoints()
{  
    isoDraw.left = { boundary.x - boundary.w * 0.5f + tileSize.first * 0.5f, boundary.y };
    isoDraw.bot = { boundary.x + tileSize.first * 0.5f, boundary.y - boundary.h * 0.5f };
    isoDraw.right = { boundary.x + boundary.w * 0.5f + tileSize.first * 0.5f, boundary.y };
    isoDraw.top = { boundary.x + tileSize.first * 0.5f, boundary.y + boundary.h * 0.5f };
    isoDraw.right.first += offset.x;
    isoDraw.left.first += offset.x;
    isoDraw.top.first += offset.x;
    isoDraw.bot.first += offset.x;
    isoDraw.right.second += offset.y;
    isoDraw.left.second += offset.y;
    isoDraw.top.second += offset.y;
    isoDraw.bot.second += offset.y;
}

void Collider::SetPointsOffset(std::pair<float, float> top, std::pair<float, float> bot, std::pair<float, float> right, std::pair<float, float> left)
{
    topOffset = top;
    botOffset = bot;
    rightOffset = right;
    leftOffset = left;
}

IsoLinesCollider Collider::GetIsoPoints() { return isoDraw; }

Manifold Collider::Intersects(Collider* other)
{
    Manifold m;
    //const RectF thisColl = GetWorldColliderBounds();
    //const RectF otherColl = other->GetWorldColliderBounds();
    m.colliding = false;
    m.other = other->GetIsoPoints();
    m.overX = 0;
    m.overY = 0;
    //LOG("This coll W:%f/H:%f",thisColl.w,thisColl.h);
    //LOG("Other coll W:%f/H:%f", otherColl.w, otherColl.h);

    IsoLinesCollider otherColl = other->GetIsoPoints();

    /*fPoint topRight1(thisColl.x+thisColl.w, thisColl.y);
    fPoint topRight2(otherColl.x+otherColl.w, otherColl.y);
    fPoint bottomLeft1(thisColl.x, thisColl.y + thisColl.h);
    fPoint bottomLeft2(otherColl.x, otherColl.y + otherColl.h);
    //LOG("This coll tr X:%f/Y:%f    bl X:%f/Y:%f", topRight1.x, topRight1.y, bottomLeft1.x, bottomLeft1.y);
    //LOG("Other  coll tr X:%f/Y:%f    bl X:%f/Y:%f", topRight2.x, topRight2.y, bottomLeft2.x, bottomLeft2.y);
    if (topRight1.y > bottomLeft2.y || bottomLeft1.y < topRight2.y ||
        topRight1.x < bottomLeft2.x || bottomLeft1.x > topRight2.x) //Non colliding
    {
        //LOG("Non colliding");
        m.colliding = false;
        m.other = nullptr;
    }*/

    //LOG("This coll top X:%f/Y:%f     bot X:%f/Y:%f", isoDraw.top.first, isoDraw.bot.second, isoDraw.bot.first, isoDraw.bot.second);
    //LOG("Other  coll top X:%f/Y:%f    bot X:%f/Y:%f", otherColl.top.first, otherColl.top.second, otherColl.bot.first, otherColl.bot.second);
    if (JMath::PointInsideTriangle(isoDraw.top,otherColl.top,otherColl.left,otherColl.right))
    {
        m.colliding = true;
        m.other = other->GetIsoPoints();
        m.overY = otherColl.bot.second - isoDraw.top.second;
    }
    else if (JMath::PointInsideTriangle(isoDraw.top, otherColl.bot, otherColl.left, otherColl.right))
    {
        m.colliding = true;
        m.other = other->GetIsoPoints();
        m.overY = otherColl.bot.second - isoDraw.top.second;
    }

    if (JMath::PointInsideTriangle(isoDraw.bot, otherColl.top, otherColl.left, otherColl.right))
    {
        m.colliding = true;
        m.other = other->GetIsoPoints();
        m.overY = otherColl.top.second - isoDraw.bot.second;
    }
    else if (JMath::PointInsideTriangle(isoDraw.bot, otherColl.bot, otherColl.left, otherColl.right))
    {
        m.colliding = true;
        m.other = other->GetIsoPoints();
        m.overY = otherColl.top.second - isoDraw.bot.second;
    }

    if (JMath::PointInsideTriangle(isoDraw.right, otherColl.top, otherColl.left, otherColl.right))
    {
        m.colliding = true;
        m.other = other->GetIsoPoints();
        m.overX = otherColl.left.first - isoDraw.right.first;
    }
    else if (JMath::PointInsideTriangle(isoDraw.right, otherColl.bot, otherColl.left, otherColl.right))
    {
        m.colliding = true;
        m.other = other->GetIsoPoints();
        m.overX = otherColl.left.first - isoDraw.right.first;
    }

    if (JMath::PointInsideTriangle(isoDraw.left, otherColl.top, otherColl.left, otherColl.right))
    {
        m.colliding = true;
        m.other = other->GetIsoPoints();
        m.overX = otherColl.right.first - isoDraw.left.first;
    }
    else if (JMath::PointInsideTriangle(isoDraw.left, otherColl.bot, otherColl.left, otherColl.right))
    {
        m.colliding = true;
        m.other = other->GetIsoPoints();
        m.overX = otherColl.right.first - isoDraw.left.first;
    }
    return m;
}

void Collider::ResolveOverlap(Manifold& m)
{
    if (collType != TRIGGER)
    {
        Transform* t = game_object->GetTransform();
        //vec pos = t->GetGlobalPosition();
        //const RectF rect1 = GetColliderBounds();
        //IsoLinesCollider otherColl = m.other;
        //float xDif = isoDraw.top.first - otherColl.top.first;
        //float yDif = isoDraw.left.second - otherColl.left.second;


        //LOG("Xdif: %f/Ydif:%f",xDif,yDif);
        //LOG("Move res %f",res);
        //t->MoveX(xDif/6 * App->time.GetGameDeltaTime());//Move x      

        //LOG("Move res %f", res);
        //t->MoveY(yDif/6 * App->time.GetGameDeltaTime());//Move y

        t->MoveX(m.overX/4 * App->time.GetGameDeltaTime());//Move x      

        //LOG("Move res %f", res);
        t->MoveY(m.overY/4 * App->time.GetGameDeltaTime());//Move y
        
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

double Collider::GetGoID() { return GoID; }

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