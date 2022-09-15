#pragma once
#include <reactphysics3d/reactphysics3d.h>

class CCollisionListener : public rp3d::EventListener
{
public:
    virtual void onContact(const CollisionCallback::CallbackData& callbackData) override;
};