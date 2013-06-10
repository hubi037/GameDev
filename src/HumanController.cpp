#include "StdAfx.h"

#include "HumanController.h"

#include "DebugDisplay.h"

using namespace Ogre;

HumanController::HumanController(Spacecraft* spacecraft):
	SpacecraftController(spacecraft),
	mSteer(0.0f),
	mMove(0.0f)
{}

void HumanController::serialize(std::ostrstream& out)
{
	out << mMove << " ";
	out << mSteer << " ";
}

void HumanController::unserialize(std::istrstream& in)
{
	in >> mMove;
	in >> mSteer;
}

void HumanController::update(float delta)
{
	Vector3 direction = mSpacecraft->getDirection();
	Vector3 desiredVelocity = direction * mMove * Spacecraft::MAX_SPEED;
	Vector3 currentVelocity = mSpacecraft->getLinearVelocity();

	DebugDisplay::getSingleton().drawLine(mSpacecraft->getPosition(), mSpacecraft->getPosition() + desiredVelocity * 10.0f, ColourValue(0.0f, 1.0f, 0.0f));
	mSpacecraft->setSteeringCommand(desiredVelocity - currentVelocity, mSteer * Spacecraft::MAX_ANGULAR_SPEED - mSpacecraft->getAngularVelocity());
}

bool HumanController::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_A)
	{
		mSteer = 1.0f;
		return true;
	}

	if (arg.key == OIS::KC_D)
	{
		mSteer = -1.0f;
		return true;
	}

	if (arg.key == OIS::KC_W)
	{
		mMove = 1.0f;
		return true;
	}

	if (arg.key == OIS::KC_S)
	{
		mMove = -1.0f;
		return true;
	}

	return false;
}

bool HumanController::keyReleased(const OIS::KeyEvent &arg)
{
	if ((arg.key == OIS::KC_A) && (mSteer > 0.0f))
	{
		mSteer = 0.0f;
		return true;
	}

	if ((arg.key == OIS::KC_D) && (mSteer < 0.0f))
	{
		mSteer = 0.0f;
		return true;
	}

	if ((arg.key == OIS::KC_W) && (mMove > 0.0f))
	{
		mMove = 0.0f;
		return true;
	}

	if ((arg.key == OIS::KC_S) && (mMove < 0.0f))
	{
		mMove = 0.0f;
		return true;
	}

	if (arg.key == OIS::KC_SPACE)
	{
		mSpacecraft->shoot();
		return true;
	}

	return false;
}