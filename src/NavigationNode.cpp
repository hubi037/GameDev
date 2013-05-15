#include "stdafx.h"

#include "NavigationNode.h"
#include "Connection.h"
#include "NavigationGraphDebugDisplay.h"

const float NavigationNode::NODE_SIZE = 20.0f;
const float NavigationNode::NODE_SIZE_HALF = NODE_SIZE / 2.0f;

NavigationNode::NavigationNode(const Vector3& _gridIdx, const Vector3& _center):
	gridIdx(_gridIdx),
	center(_center)
{}

void NavigationNode::debugDraw(NavigationGraphDebugDisplay* debugDisplay) const
{
	debugDisplay->drawLine(center - Vector3(-NODE_SIZE_HALF, 0.0f, -NODE_SIZE_HALF), center - Vector3( NODE_SIZE_HALF, 0.0f, -NODE_SIZE_HALF), ColourValue::White);
	debugDisplay->drawLine(center - Vector3( NODE_SIZE_HALF, 0.0f, -NODE_SIZE_HALF), center - Vector3( NODE_SIZE_HALF, 0.0f,  NODE_SIZE_HALF), ColourValue::White);
	debugDisplay->drawLine(center - Vector3( NODE_SIZE_HALF, 0.0f,  NODE_SIZE_HALF), center - Vector3(-NODE_SIZE_HALF, 0.0f,  NODE_SIZE_HALF), ColourValue::White);
	debugDisplay->drawLine(center - Vector3(-NODE_SIZE_HALF, 0.0f,  NODE_SIZE_HALF), center - Vector3(-NODE_SIZE_HALF, 0.0f, -NODE_SIZE_HALF), ColourValue::White);

	for (int i = 0; i < (int) connections.size(); i++)
	{
		Connection* con = connections[i];

		Vector3 dir = con->getToNode()->center - con->getFromNode()->center;
		dir.normalise();
		Vector3 a = con->getFromNode()->center + dir * 1.0f;
		Vector3 b = con->getFromNode()->center + dir * (NODE_SIZE-1);
			
		if (a.x > b.x || a.z > b.z)	// avoid drawing the same connection 2 times. 
		{
			debugDisplay->drawLine(a, b, ColourValue::Green);
		}	
	}
}