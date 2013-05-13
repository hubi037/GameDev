#include "StdAfx.h"

#include "Rocket.h"
#include "Spacecraft.h"
#include "GameApplication.h"

using namespace Ogre;

Rocket::Rocket(const Ogre::String& name, SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, const Ogre::Vector3& position, const Ogre::Vector3& direction):
	mBody(NULL),
	mNode(NULL),
	mParticleSystem(NULL),
	mLinearSteering(Vector3::ZERO),
	mRadius(0.75f)
{
	Vector3 size(mRadius*2.0f, mRadius*2.0f, mRadius*2.0f);
 
 	// mesh creation
 	Entity *entity = sceneMgr->createEntity(name, "sphere.mesh");			    
 	// we need the bounding box of the box to be able to set the size of the Bullet-box
 		
 	AxisAlignedBox boundingB = entity->getBoundingBox();
	Vector3 meshSize = boundingB.getSize();

	// scale mesh to match the our radius
	Vector3 scale(size.x / meshSize.x, size.y / meshSize.y, size.z / meshSize.z);

 	entity->setMaterialName("spacecraft/rocket");
 	mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
 	mNode->attachObject(entity);
 	mNode->scale(scale);
		
 	// use a sphere as collision shape
	mShape = new OgreBulletCollisions::SphereCollisionShape(mRadius);

 	mBody = new OgreBulletDynamics::RigidBody(name + "_body", world);
 
	Quaternion orientaion(1, 0, 0, 0);
	mBody->setShape(mNode,
 				mShape,
 				0.6f,			// dynamic body restitution
 				0.6f,			// dynamic body friction
 				1.0f, 			// dynamic bodymass
 				position,		// starting position of the box
				orientaion);	// orientation of the box

	btRigidBody* rigidBody = mBody->getBulletRigidBody();
	rigidBody->setUserPointer(this);

	// limit body rotation to y axis.
	rigidBody->setLinearFactor(btVector3(1.0f, 0.0f, 1.0f));
	rigidBody->setAngularFactor(btVector3(0.0f, 0.1f, 0.0f));

	mParticleSystem = sceneMgr->createParticleSystem(name + "_particles", "Space/Rocket");
	mNode->attachObject(mParticleSystem);

	mBody->setLinearVelocity(direction * 100.0f);

	// no collision response
	rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

Rocket::~Rocket()
{
	SAFE_DELETE(mBody);
	SAFE_DELETE(mShape);
	SAFE_DELETE(mNode);
	SAFE_DELETE(mParticleSystem);
}

void Rocket::onCollision(ICollider* collider)
{
	Spacecraft* spacecraft = dynamic_cast<Spacecraft*> (collider);

	if (spacecraft != NULL)
	{
		spacecraft->hit();
		GameApplication::getSingleton().releaseRocket(this);
	}
}