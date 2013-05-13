 #ifndef __AIController_h_
 #define __AIController_h_ 

#include "SpacecraftController.h"

class AIController: public SpacecraftController
{
public:
	AIController(Spacecraft* spacecraft, Spacecraft* mHumanSpacecraft);

	virtual void update(float delta);


private:
	Spacecraft* mHumanSpacecraft;
};


#endif