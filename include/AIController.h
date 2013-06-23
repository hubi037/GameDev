 #ifndef __AIController_h_
 #define __AIController_h_ 

#include "SpacecraftController.h"
#include "LuaScriptManager.h"

class AIController: public SpacecraftController
{
public:
	enum behaviourState {PURSUE, FLEE, WANDER};

	AIController(Spacecraft* spacecraft, Spacecraft* mHumanSpacecraft, scripting::Manager* mgr);

	virtual void update(float delta);


private:
	Spacecraft* mHumanSpacecraft;
	scripting::Manager* mScriptManager;
	void pursue(float delta);
	void flee(float delta);
	bool checkFacingPlayer(Ogre::Vector3 direction, Ogre::Vector3 ownPos, Ogre::Vector3 playerPos);
	float fleeDistance;
	behaviourState state;
};


#endif