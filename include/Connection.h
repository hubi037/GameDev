#ifndef __Connection_h_
#define __Connection_h_

class NavigationNode;

class Connection
{
public:
	Connection(NavigationNode* _fromNode, NavigationNode* _toNode, float _cost):
		fromNode(_fromNode),
		toNode(_toNode),
		cost(_cost)
	{}

	NavigationNode* getFromNode()
	{
		return fromNode;
	}

	NavigationNode* getToNode()
	{
		return toNode;
	}

	float getCost()
	{
		return cost;
	}

private:
	NavigationNode* fromNode;
	NavigationNode* toNode;
	float cost;
};

#endif