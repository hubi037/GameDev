#ifndef __Navigation_Graph_h_
#define __Navigation_Graph_h_

#include <vector>

using namespace Ogre;

class NavigationNode;

/// Stores a navigation graph of the static environment used for pathfinding.
class NavigationGraph: public Ogre::Singleton<NavigationGraph>
{
public:	
	static NavigationGraph& getSingleton(void);
    static NavigationGraph* getSingletonPtr(void);

	NavigationGraph(int _x, int _z, int _width, int _depth);
	virtual ~NavigationGraph();


	/// calculates the graph based on the static environment
	void calcGraph(OgreBulletDynamics::DynamicsWorld* world);
	
	void debugDraw() const;
	
	/// returns the node at the given position
	/// returns NULL if there is no node at the given position
	NavigationNode* getNodeAt(const Vector3& position);
	
	/// calculates a path from currentPosition to tragetPosition.
	std::vector<Vector3> calcPath(const Vector3& currentPosition, const Vector3& targetPosition);
	
private:
	/// returns the node with the given index
	NavigationNode* getNode(int idxX, int idxZ);

	/// test if there is enough space for a node at the given position.
	bool checkSpaceForNode(OgreBulletDynamics::DynamicsWorld* world, const Vector3& position) const;
	
	Vector3 origin;
	int width;
	int height;
	int gridWidth;
	int gridDepth;
	std::vector<NavigationNode*> grid;
	std::vector<NavigationNode*> lookUp;
};

#endif