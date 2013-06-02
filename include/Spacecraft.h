 #ifndef __Spacecraft_h_
 #define __Spacecraft_h_ 

#include "OgreBulletDynamicsRigidBody.h"				 // for OgreBullet
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h" // for static planes
#include "Shapes/OgreBulletCollisionsBoxShape.h"		 // for Boxes
#include "Shapes/OgreBulletCollisionsSphereShape.h"
#include "Shapes/OgreBulletCollisionsCylinderShape.h"
#include "ICollider.h"



class Spacecraft:
	public ICollider
{
public:
	/** maximum speed */
	static const float MAX_SPEED;

	/** maximum angular speed */
	static const float MAX_ANGULAR_SPEED;

	/// constructs a new spacecraft.
	/// name: The name of the spacecraft is used as id for the SceneNode and BulletObject.
	Spacecraft(int id, const Ogre::String& name, Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, const Ogre::Vector3& position, const Ogre::String& texture);

	virtual ~Spacecraft();

	/// Serializes the object into a stream
	void serialize(std::ostrstream& out);

	/// Unserializes the object from a stream
	void unserialize(std::istrstream& in);

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

	/// returns the health of the craft (0 - 1)
	float getHealth()
	{
		return mHealth;
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

	/// returns the direction where the spacecraft is heading to.
	Ogre::Vector3 getDirection() const;

	/// returns the orientation of the spacecraft as quaternion.
	Ogre::Quaternion getOrientation() const;

	/// returns the yaw of the spacecraft in radian
	float getYaw() const;

	// returns the id of the spacecraft
	int getId() const { return mId; }

	/// returns the linear velocity of the spacecraft.
	/// Note: y velocity is omitted since spacecraft mainly travel on the plane.
	Ogre::Vector3 getLinearVelocity() const;

	/// returns the angular velocity of the spacecraft.
	float getAngularVelocity() const;

	/// sets the steering command of this spacecraft.
	void setSteeringCommand(const Ogre::Vector3& linear, float angular);

	void setSteeringCommand(const Ogre::Vector3& linear);

	/// updates the spacecraft
	void update(float delta);

	/// called after a collision
	void onCollision(ICollider* collider);

	/// fires a rocket
	void shoot();

	/// called when spacecraft is hit.
	void hit();

private:
	static const float MAX_LINEAR_ACCELERATION;
	static const float MAX_ANGULAR_ACCELERATION;
	static const float SHOOT_COOLDOWN_DURATION;
	static const float HEALTH_RECOVERY_RATE;

	OgreBulletDynamics::RigidBody* mBody;
	Ogre::SceneNode* mNode;
	Ogre::ParticleSystem* mEngineParticleSystem;
	Ogre::ParticleSystem* mDamageParticleSystem;
	Ogre::SceneNode* mShield;
	Ogre::Billboard* mHealthBillboard;
	Ogre::Entity* mShieldEntity;

	Ogre::Vector3 mLinearSteering;
	float mAngularSteering;

	float mRadius;

	float mHealth;
	float mShieldTimer;
	float mShootTimer;

	int mId;

	Ogre::String mName;

	void updatePhysics(float delta);
	void updateLogic(float delta);
	void updateEffects(float delta);

};

#endif