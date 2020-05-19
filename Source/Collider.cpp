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
    App->collSystem.Add(this);
    collType = t;
    layer = lay;
    tag = tg;  
    offset = off;
    GoID = go->GetID();
    parentGo = go;
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
    m.colliding = false;
    m.overX = 0;
    m.overY = 0;
    //LOG("This coll W:%f/H:%f",thisColl.w,thisColl.h);
    //LOG("Other coll W:%f/H:%f", otherColl.w, otherColl.h);

    SDL_Rect thisRect = GetColliderBounds();
    SDL_Rect rect = other->GetColliderBounds();
    m.otherColl = rect;

    if (thisRect.x > rect.x + rect.w || thisRect.x + thisRect.w < rect.x || thisRect.y > rect.y + rect.h || thisRect.y + thisRect.h < rect.y) m.colliding = false;
    else
    {
        m.colliding = true;
        m.overX = (rect.x+rect.w/2)-(thisRect.x + thisRect.w / 2);
        m.overY = (rect.y + rect.h / 2) - (thisRect.y + thisRect.h / 2);
    }

    //LOG("This coll top X:%f/Y:%f     bot X:%f/Y:%f", isoDraw.top.first, isoDraw.bot.second, isoDraw.bot.first, isoDraw.bot.second);
    //LOG("Other  coll top X:%f/Y:%f    bot X:%f/Y:%f", otherColl.top.first, otherColl.top.second, otherColl.bot.first, otherColl.bot.second);
    /*if (JMath::PointInsideTriangle(isoDraw.top,otherColl.top,otherColl.left,otherColl.right))
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
    }*/
    return m;
}

void Collider::ResolveOverlap(Manifold& m)
{
    if (collType != TRIGGER)
    {
        Transform* t = game_object->GetTransform();
        //LOG("Xdif: %f/Ydif:%f",xDif,yDif);
        //LOG("Move res %f",res); 

        //LOG("Move res %f", res);
        std::pair<float, float> mov = Map::F_WorldToMap(m.overX,m.overY);
        t->MoveX(-mov.first * App->time.GetGameDeltaTime());//Move x      

        //LOG("Move res %f", res);
        t->MoveY(-mov.second * App->time.GetGameDeltaTime());//Move y
        
        //LOG("New pos X:%f/Y:%f",pos.x,pos.y);
    }  
}

double Collider::GetGoID() { return GoID; }

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

SDL_Rect Collider::GetColliderBounds()
{
    SDL_Rect Rect = { int(isoDraw.left.first),int(isoDraw.bot.second),int(boundary.w),int(boundary.h) };
    return Rect;
}

void Collider::SetOffset(RectF off) { offset = off;}

void Collider::SetCollType(ColliderType t) { collType = t; }

ColliderType Collider::GetCollType() { return collType; }

void Collider::SetColliderTag(ColliderTag tg) { tag = tg; }

ColliderTag Collider::GetColliderTag() { return tag; }