#include "Sample.h"
#include "OgreSceneManager.h"
#include "OgreManualObject.h"
#include "OgrePrefabFactory.h"
#include "LuaScriptManager.h"

#ifdef _DEBUG
#define PLUGIN_CFG "plugins_d.cfg"
#define RESOURCE_CFG "resources_d.cfg"
#else
#define PLUGIN_CFG "plugins.cfg"
#define RESOURCE_CFG "resources.cfg"
#endif

#define SAFE_DELETE(x) if(x){delete x;  x= NULL;}

Sample::Sample() : mRoot(NULL), mWindow(NULL), mSceneMgr(NULL), mCamera(NULL), mExitMainLoop(false), mEntity(NULL)
{
	mScriptMgr = new scripting::Manager();
}

Sample::~Sample()
{
	SAFE_DELETE(mScriptMgr)
	SAFE_DELETE(mRoot)
}

void Sample::setupEngine()
{
	// Create root node and load plugins which are defined inside our plugin.cfg
	mRoot = new Ogre::Root(PLUGIN_CFG);
	if (!mRoot->restoreConfig()) { 
		mRoot->showConfigDialog();
	}

	// Create our main window
	mWindow = mRoot->initialise(true, "RenderWindow Title");

	// Parse resources
	parseResources();

	// start script parsing
	Ogre::ResourceGroupManager::getSingletonPtr()->initialiseResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	// Create the scenemanger
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
	mCamera = mSceneMgr->createCamera("MyCam");

	// Setup input
	setupInput();

	mRoot->addFrameListener(this);
}

void Sample::parseResources()
{
	// load resource paths from config file
	Ogre::ConfigFile resCfg;
	resCfg.load(RESOURCE_CFG);
	Ogre::ConfigFile::SectionIterator seci = resCfg.getSectionIterator();
	// go through all specified resource groups
	while (seci.hasMoreElements()) {
		Ogre::String sec = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
		// go through all resource paths
		for (Ogre::ConfigFile::SettingsMultiMap::iterator i = settings->begin(); i != settings->end(); i++) {
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(i->second, i->first, sec);
		}
	}
}

void Sample::setupInput()
{
	size_t winHWND = 0;
	mWindow->getCustomAttribute( "WINDOW", &winHWND );

	OIS::ParamList paramList; 
	std::ostringstream windowHndStr;
	windowHndStr << winHWND;

	paramList.insert(std::make_pair( std::string("WINDOW"), windowHndStr.str()));
	paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	paramList.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));

	// Create input system
	mInputSystem = OIS::InputManager::createInputSystem( paramList );

	// If possible create a buffered mouse
	if( mInputSystem->getNumberOfDevices(OIS::OISMouse) > 0 ) {
		mMouse = static_cast<OIS::Mouse*>( mInputSystem->createInputObject( OIS::OISMouse, true ) );
		mMouse->setEventCallback( this );
	}

	// If possible create a buffered keyboard
	if( mInputSystem->getNumberOfDevices(OIS::OISKeyboard) > 0 ) {
		mKeyboard = static_cast<OIS::Keyboard*>( mInputSystem->createInputObject( OIS::OISKeyboard, true ) );
		mKeyboard->setEventCallback( this );
	}
}


void Sample::setupScene()
{
	mSceneMgr->setSkyBox(true, "spaceSkyBox");
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));

	Ogre::SceneNode* pNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Light* pDirLight = mSceneMgr->createLight();
	pDirLight->setType(Ogre::Light::LT_DIRECTIONAL);
	pDirLight->setDirection(Ogre::Vector3(0,0.5,-1));
	pNode->attachObject(pDirLight);

	mEntity = mSceneMgr->createEntity("SinbadInstance", "Sinbad.mesh");
	pNode->attachObject(mEntity);

	mCamera->setNearClipDistance(1.0f);
	mCamera->setFarClipDistance(100000.0f);
	mCamera->setPosition(0,0,15.0f);
	mCamera->lookAt(0,0,0);
	mCamera->setAutoAspectRatio(true);

	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(1,0,0));

	mScriptMgr->runScriptFile("../../Media/entitytest.lua");
}

bool Sample::frameStarted(const Ogre::FrameEvent& evt)
{
	if(mScriptMgr->hasGlobalFunction("update"))
	{
		mScriptMgr->GetGlobalTable()["update"](evt.timeSinceLastFrame, mEntity);
	}
	return true;
}

bool Sample::keyPressed(const OIS::KeyEvent &e)
{
	if (e.key == OIS::KC_ESCAPE)
	{
		mExitMainLoop = true;
		return true; 
	}
	return false;
}

bool Sample::keyReleased(const OIS::KeyEvent &e)
{
	return false;
}

bool Sample::mouseMoved(const OIS::MouseEvent &e)
{
	return false;
}

bool Sample::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return false;
}

bool Sample::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return false;
}

void Sample::startMainLoop()
{
	while(!mExitMainLoop) {
		Ogre::WindowEventUtilities::messagePump(); 

		if(mWindow->isClosed()) 
			return; 

		if(mMouse)
			mMouse->capture();

		if (mKeyboard)
			mKeyboard->capture();
		
		if(!mRoot->renderOneFrame()) 
			return; 
	}
}