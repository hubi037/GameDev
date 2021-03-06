#include "stdafx.h"


#include "OgreBulletDynamicsRigidBody.h"				 // for OgreBullet
#include "Shapes/OgreBulletCollisionsStaticPlaneShape.h" // for static planes
#include "Shapes/OgreBulletCollisionsBoxShape.h"		 // for Boxes
#include "Shapes/OgreBulletCollisionsSphereShape.h"
#include "Shapes/OgreBulletCollisionsCylinderShape.h"
#include "DebugOverlay.h"
#include "DebugDisplay.h"
#include "GameApplication.h"

#include "OgreConsole.h"
#include "Spacecraft.h"
#include "SpacecraftController.h"
#include "HumanController.h"
#include "AIController.h"
#include "Rocket.h"
#include "Wall.h"
#include "ICollider.h"

#include "LuaScriptManager.h"
#include "NavigationGraph.h"

#include "ClientSocketManager.h"
#include "GameServerListenSocket.h"
#include "BaseSocketManager.h"
#include "ServerSocketManager.h"
#include "BinaryPacket.h"
#include "RemoteSocket.h"
#include "RemoteHumanController.h"


#define SHOW_CONFIG_DIALOG

const float GameApplication::MAX_ROCKETS = 256;


template<> GameApplication* Ogre::Singleton<GameApplication>::msSingleton = 0;
GameApplication* GameApplication::getSingletonPtr(void)
{
    return msSingleton;
}
GameApplication& GameApplication::getSingleton(void)
{  
   assert(msSingleton);  
   return *msSingleton;  
}

GameApplication::GameApplication(Mode mode, String address):
	mHumanController(NULL),
	mRocketCounter(0),
	mWorld(NULL),
	mDebugDrawer(NULL),
	mDebugOverlay(NULL),
	mScriptingManager(NULL),
	mShowDebugDraw(false),
	mShowNavigationGraph(false),
	mSynchTimer(0.0f),
	mMode(mode),
	mAddress(address),
	mMapAnimation_open(NULL),
	mMapAnimation_close(NULL),
	mViewportAnimationValues(NULL),
	mapIsOpen(false),
	mapOpenDuration(0.5)
{}

GameApplication::~GameApplication()
{
	for (size_t i = 0; i < mControllers.size(); i++)
	{
		delete mControllers[i];
	}

	for (size_t i = 0; i < mSpacecrafts.size(); i++)
	{
		delete mSpacecrafts[i];
	}

	std::list<Rocket*>::iterator it;

	for (it = mRockets.begin(); it != mRockets.end(); ++it)
	{
		delete *it;
	}

	for (it = mReleasedRockets.begin(); it != mReleasedRockets.end(); ++it)
	{
		delete *it;
	}

	mControllers.clear();
	mSpacecrafts.clear();
	mRockets.clear();
	mReleasedRockets.clear();

	SAFE_DELETE(mDebugDrawer);

	SAFE_DELETE(mWorld);

}

void GameApplication::createScene(void)
{
	mMapCamera = mSceneMgr->createCamera("MapCamera");

// set up map animations
	mViewportAnimationValues = mSceneMgr->createLight();
	mViewportAnimationValues->setDiffuseColour(Ogre::ColourValue(0.7, 0.3, 0.0, 0.0));

	// 'open' animation
	Ogre::Animation* namedAnim1 = mSceneMgr->createAnimation("MapAnimation_open", mapOpenDuration);
	namedAnim1->setInterpolationMode(Ogre::Animation::IM_SPLINE);

	const Ogre::StringVector& valueNames = mViewportAnimationValues->getAnimableValueNames();

	Ogre::AnimableValuePtr AVPtr = mViewportAnimationValues->createAnimableValue(valueNames[0]);
	Ogre::NumericAnimationTrack* track_open = namedAnim1->createNumericTrack(0, AVPtr);
	AVPtr->setValue(Ogre::ColourValue::Black);
	AVPtr->setCurrentStateAsBaseValue();
	track_open->createNumericKeyFrame(0)->setValue(Ogre::AnyNumeric(Ogre::ColourValue(0.7, 0.3, 0.0, 0.0)));
	track_open->createNumericKeyFrame(mapOpenDuration)->setValue(Ogre::AnyNumeric(Ogre::ColourValue(0.0, 1.0, 0.0, 0.0)));

	mMapAnimation_open = mSceneMgr->createAnimationState("MapAnimation_open");
	mMapAnimation_open->setEnabled(false);
	mMapAnimation_open->setLoop(false);
	mMapAnimation_open->setTimePosition(0);

	// 'close' animation
	Ogre::Animation* namedAnim2 = mSceneMgr->createAnimation("MapAnimation_close", mapOpenDuration);
	namedAnim2->setInterpolationMode(Ogre::Animation::IM_SPLINE);

	AVPtr = mViewportAnimationValues->createAnimableValue(valueNames[0]);
	Ogre::NumericAnimationTrack* track_close = namedAnim2->createNumericTrack(0, AVPtr);
	AVPtr->setValue(Ogre::ColourValue::Black);
	AVPtr->setCurrentStateAsBaseValue();
	track_close->createNumericKeyFrame(0)->setValue(Ogre::AnyNumeric(Ogre::ColourValue(0.0, 1.0, 0.0, 0.0)));
	track_close->createNumericKeyFrame(mapOpenDuration)->setValue(Ogre::AnyNumeric(Ogre::ColourValue(0.7, 0.3, 0.0, 0.0)));

	mMapAnimation_close = mSceneMgr->createAnimationState("MapAnimation_close");
	mMapAnimation_close->setEnabled(false);
	mMapAnimation_close->setLoop(false);
	mMapAnimation_close->setTimePosition(0);

	mViewportAnimationValues->setDiffuseColour(Ogre::ColourValue(0.7, 0.3, 0.0, 0.0));

	// Set ambient light
 	mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
 
 	// Create a light
 	Light* l = mSceneMgr->createLight("MainLight");
 	l->setPosition(20,80,50);

	new DebugDisplay(mSceneMgr, 0.5f);
	DebugDisplay::getSingleton().setEnabled(false);

	mDebugOverlay = new DebugOverlay();

	new OgreConsole;
	OgreConsole::getSingleton().init(mRoot);
	OgreConsole::getSingleton().setVisible(false);

	createDynamicWorld(Vector3(0.0f, -9.81f, 0.0f), AxisAlignedBox (Ogre::Vector3 (-10000, -10000, -10000), Ogre::Vector3 (10000,  10000,  10000)));
	createWalls();

	// now create navigation graph
	new NavigationGraph(mSceneMgr, -500, -500, 1000, 1000);
	NavigationGraph::getSingleton().calcGraph(mWorld);
	NavigationGraph::getSingleton().setDebugDisplayEnabled(mShowNavigationGraph);

	createSpacecrafts();

	Ogre::LogManager::getSingletonPtr()->logMessage("running script file now");

	mScriptingManager = new scripting::Manager();
	mScriptingManager->runScriptFile("../../media/AIController.lua");

	Ogre::Viewport* vpMiniMap = mWindow->addViewport(mMapCamera, 2, 0.7, 0.7, 0.3, 0.3);
	vpMiniMap->setBackgroundColour(Ogre::ColourValue(0,0,0));

	mMapCamera->setNearClipDistance(1.0f);
	mMapCamera->setFarClipDistance(100000.0f);
	mMapCamera->setPosition(0,200,0);
	mMapCamera->lookAt(0,0,30);
	mMapCamera->rotate(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(180)), Ogre::Vector3::UNIT_Y));
	mMapCamera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
	mMapCamera->setOrthoWindow(vpMiniMap->getActualWidth(), vpMiniMap->getActualHeight());

	try
	{
		Ogre::CompositorManager::getSingleton().addCompositor(vpMiniMap, "Sepia");
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(vpMiniMap, "Sepia", true);
	}
	catch(...)
	{
		Ogre::LogManager::getSingleton().logMessage("Could not load compositor");
	}

	
}

void GameApplication::createDynamicWorld(Vector3 &gravityVector,AxisAlignedBox &bounds)
{
 	// Start Bullet
 	mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, bounds, gravityVector);
 
 	 // add Debug info display tool
 	mDebugDrawer = new OgreBulletCollisions::DebugDrawer();
 	mDebugDrawer->setDrawWireframe(true);	// we want to see the Bullet containers
	
 	mWorld->setDebugDrawer(mDebugDrawer);
 	mWorld->setShowDebugShapes(false);		// enable it if you want to see the Bullet containers
 	SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer", Ogre::Vector3::ZERO);
 	node->attachObject(static_cast <SimpleRenderable*> (mDebugDrawer));
 
    // Define a floor plane mesh
 	Entity *ent;
    Plane p;
    p.normal = Vector3(0,1,0); p.d = 0;
    MeshManager::getSingleton().createPlane("FloorPlane", 
                                            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
                                            p, 200000, 200000, 20, 20, true, 1, 9000, 9000, 
                                            Vector3::UNIT_Z);
    // Create an entity (the floor)
    ent = mSceneMgr->createEntity("floor", "FloorPlane");
 	ent->setMaterialName("Examples/BumpyMetal");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
 
 	// add collision detection to it
 	OgreBulletCollisions::CollisionShape *Shape;
 	Shape = new OgreBulletCollisions::StaticPlaneCollisionShape(Ogre::Vector3(0,1,0), 0);
 	// a body is needed for the shape
 	OgreBulletDynamics::RigidBody *defaultPlaneBody = new OgreBulletDynamics::RigidBody("BasePlane", mWorld);
 	defaultPlaneBody->setStaticShape(Shape, 0.0f, 0.8f); // (shape, restitution, friction)
}

void GameApplication::createWalls()
{
	Wall* wall1 = new Wall(mSceneMgr, mWorld, Vector3(50.0f, 5.0f, 50.0f), Vector3(50.0f, 5.0f, 150.0f));
	Wall* wall2 = new Wall(mSceneMgr, mWorld, Vector3(50.0f, 5.0f, -100.0f), Vector3(-50.0f, 5.0f, -100.0f));
	Wall* wall3 = new Wall(mSceneMgr, mWorld, Vector3(-50.0f, 5.0f, 50.0f), Vector3(-50.0f, 5.0f, 150.0f));
}


void GameApplication::createSpacecrafts()
{
	Spacecraft* craft1 = new Spacecraft(0, "craft1", mSceneMgr, mWorld, Vector3(0.0f, 20.0f, 0.0f), "spacecraft/blue", Ogre::ColourValue(0.0, 0.0, 1.0, 1.0));
	Spacecraft* craft2 = new Spacecraft(1, "craft2", mSceneMgr, mWorld, Vector3(20.0f, 20.0f, 0.0f), "spacecraft/red", Ogre::ColourValue(1.0, 0.0, 0.0, 1.0));

	mSpacecrafts.push_back(craft1);
	mSpacecrafts.push_back(craft2);
}


void GameApplication::createCamera(void)
{
    mCamera = mSceneMgr->createCamera("PlayerCam");

    mCamera->setPosition(Vector3(0,50,100));
    mCamera->lookAt(Vector3(0,0,-200));
    mCamera->setNearClipDistance(5);

    mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}

void GameApplication::setupNetwork(void)
{
	Spacecraft* craft0 = getSpacecraft(0);
	Spacecraft* craft1 = getSpacecraft(1);

	switch (mMode)
	{
		case MODE_STANDALONE:
		{
			mHumanController = new HumanController(craft0);
			mControllers.push_back(mHumanController);

			AIController* aiController = new AIController(craft1, craft0, mScriptingManager);
			mControllers.push_back(aiController);
			break;
		}
		case MODE_SERVER:
		{
			mHumanController = new HumanController(craft0);
			mControllers.push_back(mHumanController);

			RemoteHumanController* controller = new RemoteHumanController(craft1);
			mControllers.push_back(controller);
			
			Ogre::LogManager::getSingletonPtr()->logMessage("start server");

			ServerSocketManager* server = new ServerSocketManager();
			
			Ogre::LogManager::getSingletonPtr()->logMessage("starting as server");

			if (!server->start())
			{
				Ogre::LogManager::getSingletonPtr()->logMessage("server: couldn't listen to port");
			}
			else
			{
				Ogre::LogManager::getSingletonPtr()->logMessage("server: start listening");
			}
			break;
		}
		case MODE_CLIENT:
		{
			RemoteHumanController* controller = new RemoteHumanController(craft0);
			mControllers.push_back(controller);

			mHumanController = new HumanController(craft1);
			mControllers.push_back(mHumanController);

			ClientSocketManager* client = new ClientSocketManager(mAddress, 3709);

			Ogre::LogManager::getSingletonPtr()->logMessage("client address: " + mAddress);

			Ogre::LogManager::getSingletonPtr()->logMessage("starting as client");

			if (!client->connect())
			{
				Ogre::LogManager::getSingletonPtr()->logMessage("client: couldn't connect to game server");
			}
			else
			{
				Ogre::LogManager::getSingletonPtr()->logMessage("client: connected");
			}
			break;
		}
	}
}


bool GameApplication::frameStarted(const Ogre::FrameEvent& evt)
{
	if(mMapAnimation_open->getEnabled())
		mMapAnimation_open->addTime(evt.timeSinceLastFrame);
	if(mMapAnimation_close->getEnabled())
		mMapAnimation_close->addTime(evt.timeSinceLastFrame);

   bool ret = BaseApplication::frameStarted(evt);

   float delta = evt.timeSinceLastFrame;
   update(delta);

   // Right before the frame is rendered, call DebugDrawer::build().
   DebugDisplay::getSingleton().build();
   return ret;
}

void GameApplication::udpateNetwork(float delta)
{
	switch (mMode)
	{
		case MODE_STANDALONE:
		{
			break;
		}
		case MODE_CLIENT:
		{
			mSynchTimer += delta;

			if (mSynchTimer > 1.0f/20.0f)
			{
				mSynchTimer = 0.0f;

				std::ostrstream out;
				out << RemoteSocket::MESSAGE_INPUT << " ";

				out << mHumanController->getSpacecraft()->getId() << " ";
				mHumanController->serialize(out);

				shared_ptr<BinaryPacket> pkt(new BinaryPacket(out.rdbuf()->str(), out.pcount()));
				((ClientSocketManager*) BaseSocketManager::getSingletonPtr())->sendToServer(pkt);
			}

			BaseSocketManager::getSingleton().doSelect(10, true);
			break;
		}
		case MODE_SERVER:
		{
			BaseSocketManager::getSingleton().doSelect(10, true);
		}
	}
}

 

void GameApplication::update(float delta)
{

	if(mMapAnimation_open->getTimePosition() >= mapOpenDuration)
		mMapAnimation_open->setEnabled(false);
	if(mMapAnimation_close->getTimePosition() >= mapOpenDuration)
		mMapAnimation_close->setEnabled(false);

	Ogre::Real val = mViewportAnimationValues->getDiffuseColour().r;
	mMapCamera->getViewport()->setDimensions(val, val, 1-val, 1-val);


	udpateNetwork(delta);

	// update controllers
	for (size_t i = 0; i < mControllers.size(); i++)
	{
		mControllers[i]->update(delta);
	}

	// update spacecrafts
	for (size_t i = 0; i < mSpacecrafts.size(); i++)
	{
		mSpacecrafts[i]->update(delta);
	}

	// update Bullet Physics animation
	mWorld->stepSimulation(delta, 10);	

	// call onCollision of colliders
	btDynamicsWorld* dynamicWorld = mWorld->getBulletDynamicsWorld();

	int numManifolds = dynamicWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  dynamicWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
		const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
	
		int numContacts = contactManifold->getNumContacts();
		for (int j=0;j<numContacts;j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance()<0.f)
			{
				const btVector3& ptA = pt.getPositionWorldOnA();
				const btVector3& ptB = pt.getPositionWorldOnB();
				const btVector3& normalOnB = pt.m_normalWorldOnB;

				void* pointer0 = contactManifold->getBody0()->getUserPointer();
				void* pointer1 = contactManifold->getBody1()->getUserPointer();
				ICollider* collider0 = (pointer0 != NULL) ? static_cast<ICollider*>(pointer0) : NULL;
				ICollider* collider1 = (pointer1 != NULL) ? static_cast<ICollider*>(pointer1) : NULL;

				if (collider0 != NULL)
				{
					collider0->onCollision(collider1);
				}

				if (collider1 != NULL)
				{
					collider1->onCollision(collider0);
				}
			}
		}
	}

	// delete rockets if neccessary
	if (mReleasedRockets.size() > 0)
	{
		std::list<Rocket*>::iterator it;

		for (it = mReleasedRockets.begin(); it != mReleasedRockets.end(); ++it)
		{
			delete *it;
		}

		mReleasedRockets.clear();
	}

	if (mHumanController != NULL)
	{
		Spacecraft* craft = mHumanController->getSpacecraft();

		// look at human craft.
		mCamera->lookAt(mHumanController->getSpacecraft()->getPosition());
	}

	if(mMode == MODE_SERVER || mMode == MODE_STANDALONE)
	{
		mMapCamera->setPosition(mSpacecrafts[0]->getPosition().x, 200, mSpacecrafts[0]->getPosition().z);
	}
	
	else if( mMode == MODE_CLIENT)
	{
		mMapCamera->setPosition(mSpacecrafts[1]->getPosition().x, 200, mSpacecrafts[1]->getPosition().z);
	}

}


bool GameApplication::frameRenderingQueued(const FrameEvent& evt)
{
 	bool ret = BaseApplication::frameRenderingQueued(evt);
 	return ret;
}

bool GameApplication::frameEnded(const Ogre::FrameEvent& evt)
{
	bool ret = BaseApplication::frameEnded(evt);
   // After the frame is rendered, call DebugDrawer::clear()
   DebugDisplay::getSingleton().clear();

   return ret;
}

bool GameApplication::keyPressed(const OIS::KeyEvent &arg)
{
	if ((mHumanController != NULL) && mHumanController->keyPressed(arg))
	{
		return true;
	}

	return BaseApplication::keyPressed(arg);
}

bool GameApplication::keyReleased(const OIS::KeyEvent &arg)
{
	if ((mHumanController != NULL) && mHumanController->keyReleased(arg))
	{
		return true;
	}

	if (arg.key == OIS::KC_1)
	{
		mShowDebugDraw = !mShowDebugDraw;
		mWorld->setShowDebugShapes(mShowDebugDraw);
		DebugDisplay::getSingleton().setEnabled(mShowDebugDraw);
	}

	if (arg.key == OIS::KC_2)
	{
		mShowNavigationGraph = !mShowNavigationGraph;
		NavigationGraph::getSingleton().setDebugDisplayEnabled(mShowNavigationGraph);
	}

	if (arg.key == OIS::KC_3)
	{
		        
	}

	if (arg.key == OIS::KC_M)
	{
		toggleMap();
	}


	return BaseApplication::keyReleased(arg);
}

void GameApplication::toggleMap()
{
	if(mMapAnimation_open->getEnabled() || mMapAnimation_close->getEnabled())
		return;
	
	if(!mapIsOpen)
	{
		mMapAnimation_open->setEnabled(true);
		mMapAnimation_close->setEnabled(false);
		mMapAnimation_close->setTimePosition(0);
	}
	else
	{
		mMapAnimation_close->setEnabled(true);
		mMapAnimation_open->setEnabled(false);
		mMapAnimation_open->setTimePosition(0);
	}

	mapIsOpen = !mapIsOpen;
}



void GameApplication::createRocket(const Vector3& position, const Vector3& direction)
{
	if (mRockets.size() >= MAX_ROCKETS)
	{
		Rocket* oldestRocket = *mRockets.begin();
		releaseRocket(oldestRocket);
	}

	Ogre::String name = "Rocket_";
	name += (mRocketCounter++);
	Rocket* rocket = new Rocket(name, mSceneMgr, mWorld, position, direction);
	mRockets.push_back(rocket);
}

void GameApplication::releaseRocket(Rocket* rocket)
{
	mRockets.remove(rocket);
	mReleasedRockets.push_back(rocket);
}


Spacecraft* GameApplication::getSpacecraft(int idx)
{
	_ASSERT(idx >= 0 && idx < (int) mSpacecrafts.size());
	Spacecraft* craft = mSpacecrafts[idx];
	return craft;
}
 

SpacecraftController* GameApplication::getController(int idx)
{
	_ASSERT(idx >= 0 && idx < (int) mControllers.size());
	SpacecraftController* controller = mControllers[idx];
	return controller;
}

int GameApplication::getSpacecraftCount()
{
	return mControllers.size();
}
 
bool GameApplication::configure(void)
{
#ifdef SHOW_CONFIG_DIALOG
	if(mRoot->showConfigDialog())
#else
	if(mRoot->restoreConfig())
#endif
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'

		const String titles[] = { "Spacecrafts", "SC-Server", "SC-Client" };
        mWindow = mRoot->initialise(true, titles[mMode]);
		
		mWindow->setDeactivateOnFocusChange(false);
 		// Let's add a nice window icon
        return true;
    }
    else
    {
        return false;
    }
}
 
 
 #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
 #define WIN32_LEAN_AND_MEAN
 #include "windows.h"
 #include "shellapi.h"
 #endif
 
 #ifdef __cplusplus
 	extern "C" {
 #endif
 
 #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
 		//INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
		int main(int argc, char *argv[])
		{
			LPWSTR* argvw;
			//int argc;
			argvw = CommandLineToArgvW(GetCommandLineW(), &argc);

			GameApplication::Mode mode = GameApplication::MODE_STANDALONE;
			std::string address = "127.0.0.1";

			if (argc > 1)
			{
				if (wcscmp(argvw[1], L"server") == 0)
				{
					mode = GameApplication::MODE_SERVER;
				}
				else if (wcscmp(argvw[1], L"client") == 0)
				{
					mode = GameApplication::MODE_CLIENT;

					if (argc > 2)
					{
						std::wstring ws = std::wstring(argvw[2]);
						address = std::string(ws.begin(), ws.end());
					}
				}
			}

 #else
 		int main(int argc, char *argv[])
		{
 #endif
 			// Create application object
 			GameApplication app(mode, address);
 
 			try {
 				app.go();
 			} catch( Ogre::Exception& e ) {
 #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
 				MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
 #else
 				std::cerr << "An exception has occured: " <<
 					e.getFullDescription().c_str() << std::endl;
 #endif
 			}
 
 			return 0;
 		}
 
 #ifdef __cplusplus
 	}
 #endif