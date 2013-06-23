 #ifndef __Game_Application_h_
 #define __Game_Application_h_ 

#include "BaseApplication.h"

using namespace Ogre;
using namespace OgreBulletDynamics;
using namespace OgreBulletCollisions;

namespace scripting
{
	class Manager;
}

class Spacecraft;
class SpacecraftController;
class HumanController;
class Rocket;
class NavigationGraph;

class DebugOverlay;

class GameApplication : public BaseApplication, Ogre::Singleton<GameApplication>
{
public:
	enum Mode
	{
		MODE_STANDALONE,
		MODE_SERVER,
		MODE_CLIENT,

		MODE_COUNT
	};

 	GameApplication(Mode mode, String address);
 
 	~GameApplication();

	static GameApplication& getSingleton(void);

    static GameApplication* getSingletonPtr(void);

	/// creates a new rocket at the given position.
	void createRocket(const Vector3& position, const Vector3& direction);

	/// releases a rocket (-> delete the rocket in the next frame)
	void releaseRocket(Rocket* rocket);

	Spacecraft* getSpacecraft(int idx);
	SpacecraftController* getController(int idx);

	int getSpacecraftCount();

	Ogre::Camera* mMapCamera;
 
 protected:
    virtual bool configure(void);

	virtual void createScene(void);

	virtual void createSpacecrafts(void);

	virtual void createCamera(void);

	virtual void createWalls(void);

	virtual void setupNetwork(void);

	void createDynamicWorld(Vector3 &gravityVector, AxisAlignedBox &bounds);

	void udpateNetwork(float delta);

	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool frameEnded(const Ogre::FrameEvent& evt);

	virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);

	void		toggleMap();

private:
	/// If number of Rockets reaches the limit, the oldest Rocket is deleted.
	static const float MAX_ROCKETS;

	/// OgreBullet World
 	OgreBulletDynamics::DynamicsWorld* mWorld;	

	// DebugDrawer for the physics world
 	OgreBulletCollisions::DebugDrawer* mDebugDrawer;	

	// Util for text debug rendering.
	DebugOverlay* mDebugOverlay;

	std::vector<Spacecraft*> mSpacecrafts;
	std::vector<SpacecraftController*> mControllers;
	std::list<Rocket*> mRockets;
	std::list<Rocket*> mReleasedRockets;

	HumanController* mHumanController;
	int mRocketCounter;

	scripting::Manager* mScriptingManager;

	bool mShowDebugDraw;
	bool mShowNavigationGraph;
	float mSynchTimer;

	Mode mMode;
	String mAddress;

	Ogre::AnimationState* mMapAnimation_open;
	Ogre::AnimationState* mMapAnimation_close;
	Ogre::Light* mViewportAnimationValues;
	bool mapIsOpen;
	float mapOpenDuration;

	void update(float delta);
 };

#endif
