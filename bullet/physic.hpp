#ifndef OENGINE_BULLET_PHYSIC_H
#define OENGINE_BULLET_PHYSIC_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include <string>
#include <list>
#include <map>
#include "BulletShapeLoader.h"

class btRigidBody;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btSequentialImpulseConstraintSolver;
class btCollisionDispatcher;
class btDiscreteDynamicsWorld;
class btKinematicCharacterController;

namespace BtOgre
{
    class DebugDrawer;
}

namespace MWWorld
{
    class World;
}

namespace OEngine {
namespace Physic
{
    class CMotionState;
    struct PhysicEvent;

    /**
    *This is just used to be able to name objects.
    */
    class PairCachingGhostObject : public btPairCachingGhostObject
    {
    public:
        PairCachingGhostObject(std::string name)
            :btPairCachingGhostObject(),mName(name)
        {
        }
        std::string mName;
    };

    /**
     * A physic Actor use a modifed KinematicCharacterController taken in the bullet forum.
     */
    class PhysicActor
    {
    public:
        PhysicActor(std::string name);

        ~PhysicActor();

        /**
         * This function set the walkDirection. This is not relative to the actor orientation.
         * I think it's also needed to take time into account. A typical call should look like this:
         * setWalkDirection( mvt * orientation * dt)
         */
        void setWalkDirection(const btVector3& mvt);

        void Rotate(const btQuaternion& quat);

        void setRotation(const btQuaternion& quat);

        void setGravity(float gravity);

        void setVerticalVelocity(float z);

        void enableCollisions(bool collision);

        bool getCollisionMode();

        btVector3 getPosition(void);

        btQuaternion getRotation(void);

        void setPosition(const btVector3& pos);

        btKinematicCharacterController* mCharacter;

        PairCachingGhostObject* internalGhostObject;
        btCollisionShape* internalCollisionShape;

        PairCachingGhostObject* externalGhostObject;
        btCollisionShape* externalCollisionShape;

        std::string mName;
    };

    /**
     *This class is just an extension of normal btRigidBody in order to add extra info.
     *When bullet give back a btRigidBody, you can just do a static_cast to RigidBody,
     *so one never should use btRigidBody directly!
     */
    class RigidBody: public btRigidBody
    {
    public:
        RigidBody(btRigidBody::btRigidBodyConstructionInfo& CI,std::string name);
        std::string mName;

        //is this body used for raycasting only?
        bool collide;
    };

    /**
     * The PhysicEngine class contain everything which is needed for Physic.
     * It's needed that Ogre Resources are set up before the PhysicEngine is created.
     * Note:deleting it WILL NOT delete the RigidBody!
     * TODO:unload unused resources?
     */
    class PhysicEngine
    {
    public:
        /**
         * Note that the shapeLoader IS destroyed by the phyic Engine!!
         */
        PhysicEngine(BulletShapeLoader* shapeLoader);

        /**
         * It DOES destroy the shape loader!
         */
        ~PhysicEngine();

        /**
         * Create a RigidBody.It does not add it to the simulation, but it does add it to the rigidBody Map,
         * so you can get it with the getRigidBody function.
         */
        RigidBody* createRigidBody(std::string mesh,std::string name);

        /**
         * Add a RigidBody to the simulation
         */
        void addRigidBody(RigidBody* body);

        /**
         * Remove a RigidBody from the simulation. It does not delete it, and does not remove it from the RigidBodyMap.
         */
        void removeRigidBody(std::string name);

        /**
         * Delete a RigidBody, and remove it from RigidBodyMap.
         */
        void deleteRigidBody(std::string name);

        /**
         * Return a pointer to a given rigid body.
         * TODO:check if exist
         */
        RigidBody* getRigidBody(std::string name);

        /**
         * Create and add a character to the scene, and add it to the ActorMap.
         */
        void addCharacter(std::string name);

        /**
         * Remove a character from the scene. TODO:delete it! for now, a small memory leak^^ done?
         */
        void removeCharacter(std::string name);

        /**
         * Return a pointer to a character
         * TODO:check if the actor exist...
         */
        PhysicActor* getCharacter(std::string name);

        /**
         * This step the simulation of a given time.
         */
        void stepSimulation(double deltaT);

        /**
         * Empty events lists
         */
        void emptyEventLists(void);

        /**
         * Create a debug rendering. It is called by setDebgRenderingMode if it's not created yet.
         * Important Note: this will crash if the Render is not yet initialise!
         */
        void createDebugRendering();

        /**
         * Set the debug rendering mode. 0 to turn it off.
         * Important Note: this will crash if the Render is not yet initialise!
         */
        void setDebugRenderingMode(int mode);

        /**
         * Return the closest object hit by a ray. If there are no objects, it will return ("",-1).
         */
        std::pair<std::string,float> rayTest(btVector3& from,btVector3& to);

        //event list of non player object
        std::list<PhysicEvent> NPEventList;

        //event list affecting the player
        std::list<PhysicEvent> PEventList;

        //Bullet Stuff
        btBroadphaseInterface* broadphase;
        btDefaultCollisionConfiguration* collisionConfiguration;
        btSequentialImpulseConstraintSolver* solver;
        btCollisionDispatcher* dispatcher;
        btDiscreteDynamicsWorld* dynamicsWorld;

        //the NIF file loader.
        BulletShapeLoader* mShapeLoader;

        std::map<std::string,RigidBody*> RigidBodyMap;
        std::map<std::string,PhysicActor*> PhysicActorMap;

        //debug rendering
        BtOgre::DebugDrawer* mDebugDrawer;
        bool isDebugCreated;
    };

}}

#endif
