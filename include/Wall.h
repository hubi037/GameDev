 #ifndef __Wall_h_
 #define __Wall_h_ 

#include "ICollider.h"

/// Wall obstacle in the world
class Wall: public ICollider
{
public:
	/// creates a wall from the startPos to the endPos
	Wall(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, const Ogre::Vector3& startPos, const Ogre::Vector3& endPos);

	virtual ~Wall() {};

	virtual void onCollision(ICollider* collider) {};
private:
	/// creates a box with the given bounds
	Ogre::ManualObject* createCubeMesh(const Ogre::String& name, const Ogre::String& matName, const Ogre::Vector3& bounds) const;

	static int msInstanceCounter;
	static const float WALL_STRENGTH;

	OgreBulletDynamics::RigidBody* mBody;
	OgreBulletCollisions::BoxCollisionShape *mShape;
	Ogre::SceneNode* mNode;
};


#endif