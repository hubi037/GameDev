#ifndef __Navigation_Node_h_
#define __Navigation_Node_h_

using namespace Ogre;

class Connection;
class NavigationGraphDebugDisplay;

class NavigationNode
{
	friend class NavigationGraph;
	
public:	
	static const float NODE_SIZE;
	static const float NODE_SIZE_HALF;

	NavigationNode(const Vector3& _gridIdx, const Vector3& _center);
	
	void debugDraw(NavigationGraphDebugDisplay* debugDisplay) const;

	const Vector3& getCenter() const
	{
		return center + Vector3(NODE_SIZE_HALF, 0, NODE_SIZE_HALF);
	}

	std::vector<Connection*> getConnections()
	{
		return connections;
	}
	
private:
	std::vector<Connection*> connections;
	
	Vector3 gridIdx;
	Vector3 center;
};

#endif