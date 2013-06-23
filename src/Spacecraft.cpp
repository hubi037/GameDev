#include "StdAfx.h"


#include "Spacecraft.h"
#include "GameApplication.h"
#include "DebugOverlay.h"

using namespace Ogre;

const float Spacecraft::MAX_SPEED = 60.0f;
const float Spacecraft::MAX_ANGULAR_SPEED = Math::PI * 0.5f;

const float Spacecraft::MAX_LINEAR_ACCELERATION = 5.0f * 60.0f;
const float Spacecraft::MAX_ANGULAR_ACCELERATION = 20.0f * 60.0f;
const float Spacecraft::SHOOT_COOLDOWN_DURATION = 0.25f;
const float Spacecraft::HEALTH_RECOVERY_RATE = 0.05f;

static int offset = 0;

Spacecraft::Spacecraft(int id, const Ogre::String& name, SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, const Ogre::Vector3& position, const Ogre::String& texture, Ogre::ColourValue lightColor):
	mBody(NULL),
	mNode(NULL),
	mEngineParticleSystem(NULL),
	mDamageParticleSystem(NULL),
	mLinearSteering(Vector3::ZERO),
	mAngularSteering(0.0f),
	mRadius(5.0f),
	mHealth(1.0f),
	mShieldTimer(0.0f),
	mShootTimer(0.0f),
	mId(id),
	mName(name)
{
	Vector3 size(mRadius*2.0f, mRadius*2.0f, mRadius*2.0f);
 
 	// mesh creation
 	Entity *entity = sceneMgr->createEntity(name, "spacecraft.mesh");			    
 	entity->setCastShadows(true);
 	// we need the bounding box of the box to be able to set the size of the Bullet-box
 		
 	AxisAlignedBox boundingB = entity->getBoundingBox();
	Vector3 meshSize = boundingB.getSize();


	// scale mesh to math the our radius
	Vector3 scale(1.0f / size.x, 1.0f / size.x, 1.0f / size.x);

 	entity->setMaterialName(texture);
	mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
	mNode->setScale(mRadius * 2.0f, mRadius * 2.0f, mRadius* 2.0f);

	SceneNode* ship = mNode->createChildSceneNode(Vector3::UNIT_Z * 0.1f);
 	ship->attachObject(entity);
 	ship->scale(0.75f, 0.75f, 0.75f);
		
 	// use a sphere as collision shape
	OgreBulletCollisions::SphereCollisionShape *sceneBoxShape = new OgreBulletCollisions::SphereCollisionShape(mRadius);

 	mBody = new OgreBulletDynamics::RigidBody(name + "_body", world);
 		
	Quaternion orientaion(1, 0, 0, 0);
	mBody->setShape(mNode,
 				sceneBoxShape,
 				0.6f,			// dynamic body restitution
 				0.6f,			// dynamic body friction
 				1.0f, 			// dynamic bodymass
 				position,		// starting position of the box
				orientaion);	// orientation of the box

	// limit body rotation to y axis.
	mBody->getBulletRigidBody()->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
	mBody->getBulletRigidBody()->setUserPointer(this);
	mBody->getBulletRigidBody()->setDamping(0.0f, 0.4f);

	// never disable the physics of spacecraft.
	mBody->disableDeactivation();

	mEngineParticleSystem = sceneMgr->createParticleSystem(name + "_engine_particles", "Space/Engine");
	mNode->attachObject(mEngineParticleSystem);

	mShield = mNode->createChildSceneNode(name + "_shield");
	mShieldEntity = sceneMgr->createEntity(name + "_shield", "sphere.mesh");
	mShieldEntity->setMaterialName("ShieldTexture");
	mShield->attachObject(mShieldEntity);
	Vector3 sphereSize = mShieldEntity->getBoundingBox().getSize();
	mShield->setScale(1.0f / sphereSize.x, 1.0f / sphereSize.y, 1.0f / sphereSize.z);

	BillboardSet* billboardSet = sceneMgr->createBillboardSet(name + "_Health", 10);
	billboardSet->setMaterialName("spacecraft/health");
	mHealthBillboard = billboardSet->createBillboard(Vector3(0.0f, 0.4f, 0.0f), ColourValue::Red );//It should at least show red 
	mHealthBillboard->setDimensions(0.5f, 0.1f);
	mNode->attachObject(billboardSet);

	DebugOverlay::getSingleton().addTextBox(mName, "", 0, offset, 200, 20, ColourValue(0.0f, 1.0f, 0.0f));
	offset += 20;

	Ogre::SceneNode* mPositionLightNode = mNode->createChildSceneNode(name + "_light");
	mPositionLight = sceneMgr->createLight();
	mPositionLight->setType(Light::LightTypes::LT_POINT);
	mPositionLight->setDiffuseColour(lightColor);
	mPositionLight->setPosition(0, 20, 0);
	//mPositionLight->setPosition(getPosition());
	mPositionLightNode->attachObject(mPositionLight);
}

Spacecraft::~Spacecraft()
{}

void Spacecraft::serialize(std::ostrstream& out)
{
	Vector3 position = mBody->getWorldPosition();
	Quaternion orientation = mBody->getWorldOrientation();
	Vector3 linear = mBody->getLinearVelocity();
	btVector3 angular = mBody->getBulletRigidBody()->getAngularVelocity();
			
	out << position.x << " ";
	out << position.y << " ";
	out << position.z << " ";

	out << orientation.x << " ";
	out << orientation.y << " ";
	out << orientation.z << " ";
	out << orientation.w << " ";

	out << linear.x << " ";
	out << linear.y << " ";
	out << linear.z << " ";

	out << angular.x() << " ";
	out << angular.y() << " ";
	out << angular.z() << " ";
}


void Spacecraft::unserialize(std::istrstream& in)
{
	Vector3 position;
	Vector3 linear;
	Vector3 angular;
	Quaternion orientation;

	in >> position.x;
	in >> position.y;
	in >> position.z;

	in >> orientation.x;
	in >> orientation.y;
	in >> orientation.z;
	in >> orientation.w;

	in >> linear.x;
	in >> linear.y;
	in >> linear.z;

	in >> angular.x;
	in >> angular.y;
	in >> angular.z;

	float lerpPercentage = 0.0f;

	// lerp between old and new values
	position = Math::lerp(position, getPosition(), lerpPercentage);
	linear = Math::lerp(linear, getLinearVelocity(), lerpPercentage);
	btVector3 a = mBody->getBulletRigidBody()->getAngularVelocity();
	angular = Math::lerp(angular, Vector3(a.x(), a.y(), a.z()),lerpPercentage);
	orientation = Math::lerp(orientation, getOrientation(), lerpPercentage);

	mBody->setTransform(OgreBulletCollisions::OgreBtConverter::to(position), OgreBulletCollisions::OgreBtConverter::to(orientation));
	btTransform transform = mBody->getBulletRigidBody()->getCenterOfMassTransform();
	transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(position));
	transform.setRotation(OgreBulletCollisions::OgreBtConverter::to(orientation));

	// apply new values
	mBody->getBulletRigidBody()->setCenterOfMassTransform(transform);
	mBody->getBulletRigidBody()->setLinearVelocity(OgreBulletCollisions::OgreBtConverter::to(linear));
	mBody->getBulletRigidBody()->setAngularVelocity(OgreBulletCollisions::OgreBtConverter::to(angular));
}

Ogre::Quaternion Spacecraft::getOrientation() const
{
	return mBody->getWorldOrientation();
}

Vector3 Spacecraft::getDirection() const
{
	Quaternion orientation = mNode->getOrientation();
	return orientation.zAxis();
}

float Spacecraft::getYaw() const
{
	Quaternion orientation = mNode->getOrientation();
	return orientation.getYaw().valueRadians();
}

Vector3 Spacecraft::getLinearVelocity() const
{
	Vector3 vel = mBody->getLinearVelocity();
	vel.y = 0.0f;
	return vel;
}

float Spacecraft::getAngularVelocity() const
{
	btRigidBody* rb = mBody->getBulletRigidBody();
	return rb->getAngularVelocity().y();
}

void Spacecraft::setSteeringCommand(const Ogre::Vector3& linear, float angular)
{
	mLinearSteering = linear;
	mLinearSteering.y = 0.0f;	// disable y
	mAngularSteering = angular;

	float currentYaw = getYaw();

	Vector3 desiredDir = getLinearVelocity() + linear;
	desiredDir.normalise();
	float desiredYaw = Math::ATan2(-desiredDir.x, desiredDir.z).valueRadians();
	//DebugOverlay::getSingleton().setTextf(mName, "%2.2f %2.2f", currentYaw, desiredYaw);
}

void Spacecraft::setSteeringCommand(const Ogre::Vector3& linear)
{
	mLinearSteering = linear;
	mLinearSteering.y = 0.0f;	// disable y
	
	Vector3 dir = getDirection();
	Vector3 desiredDir = getLinearVelocity() + linear;

	desiredDir.normalise();
	mAngularSteering = Math::Clamp(-(dir.x * desiredDir.z - dir.z * desiredDir.x), -1.0f, 1.0f) - getAngularVelocity();
	//mAngularSteering = Math::Clamp((desiredAngularVel - getAngularVelocity()) * 100.0f, -50.0f, 50.0f);
}

void Spacecraft::shoot()
{
	if (mShootTimer <= 0.0f)
	{
		mShootTimer = SHOOT_COOLDOWN_DURATION;
		GameApplication::getSingleton().createRocket(getPosition() + getDirection() * 10.0f, getDirection());
	}
}

void Spacecraft::hit()
{
	mShieldTimer = 1.0f;
	mHealth -= 0.1f;
}

void Spacecraft::update(float delta)
{
	updatePhysics(delta);
	updateLogic(delta);
	updateEffects(delta);

	// Debug info
	//Vector3 vel = this->getLinearVelocity();
	//DebugOverlay::getSingleton().setTextf(mName, "%s: vel: %2.2f, angularvel: %2.2f", mName.c_str(), vel.length(), getAngularVelocity());
}

void Spacecraft::onCollision(ICollider* collider)
{
}

void Spacecraft::updatePhysics(float delta)
{
	btRigidBody* rb = mBody->getBulletRigidBody();

	Vector3 direction = getDirection();
	Vector3 linearVelocity = getLinearVelocity();
	float angularVelocity = getAngularVelocity();

	// Applies traction impulse so that the spacecraft only moves straight.

	float len = direction.dotProduct(linearVelocity);
	Vector3 tractionImpulse = ((direction * len)  - linearVelocity);
	rb->applyCentralImpulse(btVector3(tractionImpulse.x, tractionImpulse.y, tractionImpulse.z));


	// apply linear steering
	rb->applyCentralForce(btVector3(mLinearSteering.x, mLinearSteering.y, mLinearSteering.z));

	// apply angular steering

	const float ANGULAR_TORQUE_MULTIPLIER = 100.0f;
	const float ANGULAR_TORQUE_MAX = 100.0f;

	float torque = Math::Clamp((mAngularSteering) * ANGULAR_TORQUE_MULTIPLIER, -ANGULAR_TORQUE_MAX, ANGULAR_TORQUE_MAX);
	rb->applyTorque(btVector3(0.0f, torque, 0.0f)); 
}

void Spacecraft::updateLogic(float delta)
{
	mHealth = Math::Clamp(mHealth + delta * HEALTH_RECOVERY_RATE, 0.0f, 1.0f);
	mShootTimer = Math::Clamp(mShootTimer - delta, 0.0f, 1.0f);
}

void Spacecraft::updateEffects(float delta)
{
	Vector3 dir = this->getDirection();
	Vector3 vel = this->getLinearVelocity();

	if (vel.squaredLength() > 1.0f && dir.dotProduct(vel) > 0.0f)
	{
		mEngineParticleSystem->setEmitting(true);
	}
	else
	{
		mEngineParticleSystem->setEmitting(false);
	}


	if (mShieldTimer > 0.0f)
	{
		mShieldTimer -= delta;
	}
	else
	{
		mShieldTimer = 0.0f;
	}

	float length = 0.5f * mHealth;
	mShieldEntity->getSubEntity(0)->setCustomParameter(1, Vector4(0.f, 0.f, 0.f, mShieldTimer * 0.5f));
	mHealthBillboard->setDimensions(length, 0.1f);
}