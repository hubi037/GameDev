////////////////////////////////////////////////////////////////////////////////////////////////////
// Ogre Sample
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include "Ogre.h"
#include "OIS.h"
#include "OgreFrameListener.h"


namespace scripting { class Manager; }

class Sample : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener
{
public:
	Sample();
	virtual ~Sample();

	void setupEngine();
	void setupScene();
	void startMainLoop();

private:
	Ogre::Root* mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Camera* mCamera;
	bool mExitMainLoop;

	OIS::InputManager* mInputSystem;
	OIS::Mouse* mMouse;
	OIS::Keyboard* mKeyboard;

	scripting::Manager* mScriptMgr;
	Ogre::Entity* mEntity;

	virtual bool frameStarted(const Ogre::FrameEvent& evt);

	void parseResources();
	void setupInput();

	bool keyPressed(const OIS::KeyEvent &e);
	bool keyReleased(const OIS::KeyEvent &e);
	bool mouseMoved(const OIS::MouseEvent &e);
	bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
};
#endif