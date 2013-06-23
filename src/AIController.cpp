#include "StdAfx.h"

#include "AIController.h"
#include "DebugOverlay.h"

#include "DebugDisplay.h"

#include "NavigationGraph.h"
#include "NavigationNode.h"

using namespace Ogre;

AIController::AIController(Spacecraft* spacecraft, Spacecraft* humanSpacecraft, scripting::Manager* mgr):
	SpacecraftController(spacecraft),
	mHumanSpacecraft(humanSpacecraft),
	mScriptManager(mgr),
	fleeDistance(NavigationNode::NODE_SIZE * 5),
	state(behaviourState::PURSUE)
{
}

void AIController::update(float delta)
{
	state = (behaviourState) mScriptManager->callFunction<int, float, int>("updateState", mSpacecraft->getHealth(), state);

	if(state == FLEE)
	{
		flee(delta);
	}
	else if(state == PURSUE)
	{
		pursue(delta);
	}
	else if (state == WANDER)
	{
		// wander();
	}

}


void AIController::pursue(float delta)
{
	Vector3 thisPos = getSpacecraft()->getPosition();
	std::vector<Vector3> path = NavigationGraph::getSingleton().calcPath(thisPos, mHumanSpacecraft->getPosition());

	Vector3 p1 = thisPos;
	if(path.size() > 1)
	{
		p1 = path[0];

		for(int i=1; i<(int)path.size(); i++)
		{
			Vector3 p2 = path[i];
			DebugDisplay::getSingleton().drawLine(p1, p2, ColourValue(1,0,0));
			p1 = p2;
		}
	}

	p1 = path[1];

	Vector3 direction = mScriptManager->callFunction<Vector3, Vector3, Vector3>("update", thisPos, p1);
	direction.normalise();
	Vector3 desiredVelocity = direction * Spacecraft::MAX_SPEED;
	Vector3 currentVelocity = mSpacecraft->getLinearVelocity();	

	if(checkFacingPlayer(mSpacecraft->getDirection(), thisPos, mHumanSpacecraft->getPosition()))
	{
		mSpacecraft->shoot();
	}

	DebugDisplay::getSingleton().drawLine(mSpacecraft->getPosition(), mSpacecraft->getPosition() + desiredVelocity * 10.0f, ColourValue(0.0f, 1.0f, 1.0f));
	mSpacecraft->setSteeringCommand(desiredVelocity - currentVelocity);

}


void AIController::flee(float delta)
{

	Vector3 thisPos = getSpacecraft()->getPosition();
	Vector3 fleeDirection = thisPos - mHumanSpacecraft->getPosition();
	fleeDirection.normalise();

	Vector3 targetPoint = thisPos + fleeDirection * fleeDistance;

	NavigationNode* targetNode = NavigationGraph::getSingleton().getNodeAt(targetPoint);
	int i = 0;
	while(targetNode == NULL)
	{
		targetPoint = thisPos + fleeDirection * (fleeDistance + i);
		targetNode = NavigationGraph::getSingleton().getNodeAt(targetPoint);
		i += NavigationNode::NODE_SIZE;
	}
	
	std::vector<Vector3> path = NavigationGraph::getSingleton().calcPath(thisPos, targetNode->getCenter());

	Vector3 p1 = path[1];

	Vector3 direction = mScriptManager->callFunction<Vector3, Vector3, Vector3>("update", thisPos, p1);
	direction.normalise();
	Vector3 desiredVelocity = direction * Spacecraft::MAX_SPEED;
	Vector3 currentVelocity = mSpacecraft->getLinearVelocity();	

	DebugDisplay::getSingleton().drawLine(mSpacecraft->getPosition(), mSpacecraft->getPosition() + desiredVelocity * 10.0f, ColourValue(0.0f, 1.0f, 1.0f));
	mSpacecraft->setSteeringCommand(desiredVelocity - currentVelocity);

}

bool AIController::checkFacingPlayer(Vector3 direction, Vector3 ownPos, Vector3 playerPos)
{
	Vector3 dirToPlayer = playerPos - ownPos;

	float angulus = dirToPlayer.angleBetween(direction).valueDegrees();

	if((angulus <= 30.0) || (angulus >= 330.0))
	{
		return true;
	}

	return false;
}
