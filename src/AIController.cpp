#include "StdAfx.h"

#include "AIController.h"
#include "DebugOverlay.h"

#include "DebugDisplay.h"

#include "NavigationGraph.h"

using namespace Ogre;

AIController::AIController(Spacecraft* spacecraft, Spacecraft* humanSpacecraft):
	SpacecraftController(spacecraft),
	mHumanSpacecraft(humanSpacecraft)
{}

void AIController::update(float delta)
{}

 
