#pragma once

#include "HumanController.h"

class RemoteHumanController: public HumanController
{
public:
	RemoteHumanController(Spacecraft* craft);
};