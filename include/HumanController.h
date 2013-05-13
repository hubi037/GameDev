 #ifndef __HumanController_h_
 #define __HumanController_h_ 

#include <OISEvents.h>
#include <OISKeyboard.h>

#include "SpacecraftController.h"

/// Maps user input to steering command.
class HumanController: public SpacecraftController
{
public:
	HumanController(Spacecraft* spacecraft);

	virtual void update(float delta);

	bool keyPressed(const OIS::KeyEvent &arg);
    bool keyReleased(const OIS::KeyEvent &arg);

private:
	float mSteer;
	float mMove;
};

#endif