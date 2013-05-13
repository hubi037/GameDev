 #ifndef __Rocket_h_
 #define __Rocket_h_ 

#include "OgreBulletDynamicsRigidBody.h"				 // for OgreBullet
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h" // for static planes
#include "Shapes/OgreBulletCollisionsBoxShape.h"		 // for Boxes
#include "Shapes/OgreBulletCollisionsSphereShape.h"
#include "Shapes/OgreBulletCollisionsCylinderShape.h"
#include "ICollider.h"

class Rocket:
	public ICollider
{
public:
	/** maximum speed */
	static const float MAX_SPEED;

	/** maximum angular speed */
	static const float MAX_ANGULAR_SPEED;

	/// constructs a new spacecraft.
	/// name: The name of the spacecraft is used as id for the SceneNode and BulletObject.
	Rocket(const Ogre::String& name, Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, const Ogre::Vector3& position, const Ogre::Vector3& direction);

	virtual ~Rocket();

	/// returns the position of the spacecraft.
	Ogre::Vector3 getPosition()
	{
		return mNode->getPosition();
	}

	/// returns the radius of the spacecraft.
	float getRadius()
	{
		return mRadius;
	}

	/// returns the Ogre SceneNode of the spacecraft.
	Ogre::SceneNode* getSceneNode()
	{
		return mNode;
	}

	/// returns the RigidBody of the spacecraft.
	OgreBulletDynamics::RigidBody* getRigidBody()
	{
		return mBody;
	}

	/// updates the spacecraft
	void update(float delta);

	/// called after a collision
	void onCollision(ICollider* collider);

private:
	static const float MAX_LINEAR_ACCELERATION;
	static const float MAX_ANGULAR_ACCELERATION;

	OgreBulletDynamics::RigidBody* mBody;
	OgreBulletCollisions::SphereCollisionShape *mShape;
	Ogre::SceneNode* mNode;
	Ogre::ParticleSystem* mParticleSystem;

	Ogre::Vector3 mLinearSteering;

	float mRadius;
};

#endif