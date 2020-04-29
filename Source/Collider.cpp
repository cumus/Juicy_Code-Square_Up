#include "SDL/include/SDL.h"
#include "Collider.h"
#include "Gameobject.h"
#include "Transform.h"
#include "Vector3.h"
#include "Application.h"
#include "CollisionSystem.h"

Collider::Collider(Gameobject* go, RectF coll, ColliderType t, ComponentType ty) : Component(ty,go)
{
	boundary = coll;
    collType = t;
    //layer = DEFAULT_LAYER;
    App->collSystem.Add(this->game_object);
}

Collider::~Collider()
{}

void Collider::SetPosition()
{
    vec pos = game_object->GetTransform()->GetGlobalPosition();

    boundary.x = pos.x - (boundary.w / 2) + offset.x;
    boundary.y = pos.y - (boundary.h / 2) + offset.y;
}

Manifold Collider::Intersects(Collider* other)
{
    Manifold m;
    m.colliding = false;
    const RectF& thisColl = GetColliderBounds();
    const RectF& otherColl = other->GetColliderBounds();

    if (otherColl.x - otherColl.w > thisColl.x + thisColl.w ||
        otherColl.x + otherColl.w < thisColl.x - thisColl.w ||
        otherColl.y - otherColl.h > thisColl.y + thisColl.h ||
        otherColl.y + otherColl.h < thisColl.y - thisColl.h) //Intersects
    {
        m.colliding = true;
        m.other = &otherColl;
    }

    return m;
}

void Collider::ResolveOverlap(Manifold& m)
{
    if (collType != TRIGGER)
    {
        Transform* t = game_object->GetTransform();

        const RectF& rect1 = GetColliderBounds();
        const RectF* rect2 = m.other;
        float res = 0;
        float xDif = (rect1.x + (rect1.w * 0.5f)) - (rect2->x + (rect2->w * 0.5f));
        float yDif = (rect1.y + (rect1.h * 0.5f)) - (rect2->y + (rect2->h * 0.5f));

        if (fabs(xDif) > fabs(yDif))
        {
            if (xDif > 0) // Colliding on the left.
            {
                res = (rect2->x + rect2->w) - rect1.x;
            }
            else // Colliding on the right.
            {
                res = -((rect1.x + rect1.w) - rect2->x);
            }
            t->MoveX(res * App->time.GetGameDeltaTime());//Move x      
        }
        else
        {
            if (yDif > 0) // Colliding above.
            {
                res = (rect2->y + rect2->h) - rect1.y;
            }
            else // Colliding below
            {
                res = -((rect1.y + rect1.h) - rect2->y);
            }

            t->MoveY(res * App->time.GetGameDeltaTime());//Move y
        }
    }
}