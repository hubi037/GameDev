#include "stdafx.h"

#include <stack>
#include <queue>
#include <map>
#include "NavigationGraph.h"
#include "NavigationNode.h"
#include "Connection.h"
#include "NavigationGraphDebugDisplay.h"

#include "AStarNode.h"

template<> NavigationGraph* Ogre::Singleton<NavigationGraph>::msSingleton = 0;


NavigationGraph::NavigationGraph(SceneManager* sceneMgr, int _x, int _z, int _width, int _height):
	origin((float) _x, 0.0f, (float) _z),
	width(_width),
	height(_height)
{
	mDebugDisplay = new NavigationGraphDebugDisplay(sceneMgr, 0.5f);
	gridWidth = (int) floor(width / NavigationNode::NODE_SIZE);
	gridDepth = (int) floor(height / NavigationNode::NODE_SIZE);
	
	int gridSize = gridWidth * gridDepth;
	grid.reserve(gridSize);
	
	for (int i = 0; i < gridSize; i++)
	{
		grid.push_back(NULL);
	}
}


NavigationGraph::~NavigationGraph()
{
	for (size_t i = 0; i < grid.size(); i++)
	{
		delete grid[i];
	}

	grid.clear();
}

NavigationGraph* NavigationGraph::getSingletonPtr(void)
{
    return msSingleton;
}
NavigationGraph& NavigationGraph::getSingleton(void)
{  
    assert( msSingleton );  return ( *msSingleton );  
}

void NavigationGraph::setDebugDisplayEnabled(bool enable)
{
	mDebugDisplay->setEnabled(enable);
}

bool NavigationGraph::checkSpaceForNode(OgreBulletDynamics::DynamicsWorld* world, const Vector3& position) const
{
	const Vector3 offset[] = { 
		Vector3(0.0f, 0.0f, 0.0f), 
		Vector3(-NavigationNode::NODE_SIZE_HALF, 0.0f, 0.0f), 
		Vector3(NavigationNode::NODE_SIZE_HALF, 0.0f, 0.0f), 
		Vector3(0.0f, 0.0f, -NavigationNode::NODE_SIZE_HALF), 
		Vector3(0.0f, 0.0f, NavigationNode::NODE_SIZE_HALF) 
	};

	// send multiple rays to check if spot is free.
	for (int i = 0; i < 5; i++)
	{
		btVector3 start(position.x + offset[i].x, 100.0f, position.z + offset[i].z);
		btVector3 end(position.x + offset[i].x, 1.0f, position.z + offset[i].z);
			
		btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
 
		// Perform raycast
		world->getBulletCollisionWorld()->rayTest(start, end, rayCallback);
 
		if(rayCallback.hasHit()) 
		{
			return false;
		}
	}

	return true;
}


void NavigationGraph::calcGraph(OgreBulletDynamics::DynamicsWorld* world)
{
	for (int z = 0; z < gridDepth; z++)
	{
		for (int x = 0; x < gridWidth; x++)
		{
			Vector3 pos((float) (x * NavigationNode::NODE_SIZE), 5.0f,
					 (float) (z * NavigationNode::NODE_SIZE));

			pos += origin;

			if(!checkSpaceForNode(world, pos))
			{
				continue;
			}
			
			NavigationNode* node = new NavigationNode(Vector3((float) x, 0.0f, (float) z), pos);
			NavigationNode* leftNode = getNode(x-1, z);
			NavigationNode* topNode = getNode(x, z-1);
			
			if (leftNode != NULL)
			{
				Vector3 distance = node->center - leftNode->center;
				float cost = distance.length();
				node->connections.push_back(new Connection(node, leftNode, cost));
				leftNode->connections.push_back(new Connection(leftNode, node, cost));
			}
			if (topNode != NULL)
			{
				Vector3 distance = node->center - topNode->center;
				float cost = distance.length();
				node->connections.push_back(new Connection(node, topNode, cost));
				topNode->connections.push_back(new Connection(topNode, node, cost));
			}

			grid[x + z * gridWidth] = node;
		}
	}

	debugDraw();
}

template<class T>
static T round(T r) 
{
	return (r > (T) 0.0) ? floor(r + (T) 0.5) : ceil(r - (T) 0.5);
}

NavigationNode* NavigationGraph::getNodeAt(const Vector3& position)
{
	int idxX = (int) round((position.x - origin.x - NavigationNode::NODE_SIZE_HALF) / NavigationNode::NODE_SIZE);
	int idxZ = (int) round((position.z - origin.z - NavigationNode::NODE_SIZE_HALF) / NavigationNode::NODE_SIZE);
	
	return getNode(idxX, idxZ);
}

NavigationNode* NavigationGraph::getNode(int idxX, int idxZ)
{
	if ((idxX >= 0) && (idxX <= gridWidth) && (idxZ >= 0) && (idxZ <= gridDepth))
	{
		return grid[idxX + idxZ * gridWidth];
	}
	else
	{
		return NULL;
	}
}

void NavigationGraph::debugDraw() const
{
	for (size_t i = 0; i < grid.size(); i++)
	{
		if (grid[i] != NULL)
		{
			grid[i]->debugDraw(mDebugDisplay);
		}
	}

	mDebugDisplay->build();
}

std::vector<Vector3> NavigationGraph::calcPath(const Vector3& currentPosition, const Vector3& targetPosition)
{
	NavigationNode* startNode = getNodeAt(currentPosition);
	NavigationNode* endNode = getNodeAt(targetPosition);

	//std::cout << "start: " << startNode->getCenter() << std::endl;
	//std::cout << "end: " << endNode->getCenter() << std::endl;
	std::vector<Vector3> path;
	if(startNode && endNode && (startNode != endNode))
	{
		// A* path finding
		//std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> open_list;
		std::list<AStarNode> open_list;
		std::list<AStarNode> closed_list;

		AStarNode end = AStarNode(endNode, NULL, startNode, 0); 

		open_list.push_back(end);

		AStarNode curNode = end;
		
		while(!open_list.empty() && (curNode.node != startNode))
		{
			
			curNode = open_list.front();
			closed_list.push_back(curNode);
			open_list.pop_front();

			bool toSort = false;
		
			std::vector<Connection*> connections = curNode.node->getConnections();
			for(int i=0; i<connections.size(); i++)
			{
				AStarNode n(connections[i]->getToNode(), &closed_list.back(), startNode, curNode.distToStart+1);
				if(findInList(n, closed_list) == NULL)
				{
					AStarNode* nInOpenList = findInList(n, open_list);
					if(nInOpenList != NULL)
					{
						if(n.distToStart < nInOpenList->distToStart )
						{
							nInOpenList->distToStart = n.distToStart;
						}
					}
					else
					{
						open_list.push_back(n);
						toSort = true;
					}
				}
			}

			if(toSort)
			{
				open_list.sort(AStarNode::compare);
			}
		}

		//std::cout << "FLUUUHTSCH!!!" << std::endl;

		AStarNode* curPathNode = &closed_list.back();

		path.push_back(curPathNode->node->getCenter());
		
		while(curPathNode != NULL)
		{
			curPathNode = curPathNode->previousNode;

			if(curPathNode != NULL)
			{
				//curPathNode->printNode();
				path.push_back(curPathNode->node->getCenter());
			}

		}

		if(path.size() < 2)
		{
			path.push_back(endNode->getCenter());
		}
		/*
		NavigationNode* bestNeighbour = connections[0]->getToNode();
		Vector3 toTarget = bestNeighbour->getCenter() - endNode->getCenter();
		float minDist = toTarget.x + toTarget.z;
		for(int i=1; i<connections.size(); i++)
		{
			toTarget = connections[i]->getToNode()->getCenter() - endNode->getCenter();
			float dist = toTarget.x + toTarget.z;
			if(dist <= minDist)
			{
				bestNeighbour = connections[i]->getToNode();
				minDist = dist;
			}
		}

	
		path.push_back(startNode->getCenter());
		path.push_back(endNode->getCenter());
		*/
	}
	else
	{
		path.push_back(currentPosition);
		path.push_back(targetPosition);
	}
	return path;
}


bool NavigationGraph::isInList(AStarNode& n, std::list<AStarNode>& _list )
{
	std::list<AStarNode>::iterator it = _list.begin();
	for( ; it != _list.end(); it++)
	{
		if( (*it) == n)
			return true;
	}
	return false;
}

AStarNode* NavigationGraph::findInList(AStarNode& n, std::list<AStarNode>& _list)
{
	std::list<AStarNode>::iterator it = _list.begin();
	for( ; it != _list.end(); it++)
	{
		if( (*it) == n)
			return &(*it);
	}
	return NULL;
}
