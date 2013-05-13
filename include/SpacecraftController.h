 #ifndef __SpacecraftController_h_
 #define __SpacecraftController_h_ 

#include "Spacecraft.h"

class SpacecraftController
{
public:
	SpacecraftController(Spacecraft* spacecraft);
	virtual ~SpacecraftController();

	virtual void update(float delta) = NULL;

	Spacecraft* getSpacecraft()
	{
		return mSpacecraft;
	}

protected:
	Spacecraft* mSpacecraft;
};

#endif