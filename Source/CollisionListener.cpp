#include "CollisionListener.h"
#include "Entity.h"

// Dispatch actions on collisions of 2 objects.
void CCollisionListener::onContact(const CollisionCallback::CallbackData& callbackData)
{
    using namespace rp3d;

    std::cout << "Collision!\n";

    for (uint16_t p = 0; p < callbackData.getNbContactPairs(); p++)
    {
        CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);

        CEntity& entity1 = *(CEntity*)contactPair.getBody1()->getUserData();
        CEntity& entity2 = *(CEntity*)contactPair.getBody2()->getUserData();

        entity1.OnCollision(entity2);
        entity2.OnCollision(entity1);
    }
}