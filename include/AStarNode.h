
#pragma once
#include "NavigationNode.h"

struct AStarNode
{

	NavigationNode* node;
	AStarNode* previousNode;
	float distToStart;
	float distToEnd;

	AStarNode(NavigationNode* n, AStarNode* prev, NavigationNode* end, float _distToStart) : node(n), previousNode(prev), distToStart(_distToStart)
	{
		Ogre::Vector3 toTarget = end->getCenter() - n->getCenter();
		distToEnd = toTarget.squaredLength();// abs(toTarget.x) + abs(toTarget.z);
	}

	bool operator== (const AStarNode& _n) const
	{
		return (node == _n.node);
	}

	static bool compare (const AStarNode& n1, const AStarNode& n2)
	{
		return (n1.distToEnd < n2.distToEnd);
	}

	void printNode()
	{
		std::cout << "Node: " << node << "    distToStart: " << distToStart << "    distToEnd: " << distToEnd << std::endl;
	}
};