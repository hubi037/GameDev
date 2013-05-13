#include "StdAfx.h"

#include "Wall.h"

using namespace Ogre;

const float Wall::WALL_STRENGTH = 10.0f;
int Wall::msInstanceCounter = 0;

// Helper function used in constructor
template <class T> const T& max (const T& a, const T& b) 
{
  return (a<b)?b:a;
}

Wall::Wall(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld* world, const Ogre::Vector3& startPos, const Ogre::Vector3& endPos)
{
	String name = "wall_" + StringConverter::toString(msInstanceCounter++);

	Vector3 size = endPos - startPos;
	Vector3 position = (endPos + startPos) / 2.0f;

	size.x = max(Math::Abs(size.x), WALL_STRENGTH);
	size.y = max(Math::Abs(size.y), WALL_STRENGTH);
	size.z = max(Math::Abs(size.z), WALL_STRENGTH);

	ManualObject* entity = createCubeMesh(name, "wall", size / 2);
	
	mNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
 	mNode->attachObject(entity);
	//mNode->scale(scale);
	mNode->setPosition(position);

	// after that create the Bullet shape with the calculated size
    mShape = new OgreBulletCollisions::BoxCollisionShape(size / 2);
    // and the Bullet rigid body
    mBody = new OgreBulletDynamics::RigidBody(name, world);
    mBody->setStaticShape(mNode, mShape, 0.1f, 0.0f, position);
	btRigidBody* rigidBody = mBody->getBulletRigidBody();
	rigidBody->setUserPointer(this);
	
}

ManualObject* Wall::createCubeMesh(const Ogre::String& name, const Ogre::String& matName, const Vector3& bounds) const
{
	ManualObject* cube = new ManualObject(name);
	cube->begin(matName);

	const float TEXTURE_SCALE = 10.0f;
	float u, v;
   
	u = bounds.x / TEXTURE_SCALE;
	v = bounds.z / TEXTURE_SCALE;

	// down
	cube->position( bounds.x, -bounds.y,  bounds.z); cube->normal(0.0f, -1.0f, 0.0f); cube->textureCoord(u,0);	//0
	cube->position(-bounds.x, -bounds.y, -bounds.z); cube->normal(0.0f, -1.0f, 0.0f); cube->textureCoord(0,v);	//1
	cube->position( bounds.x, -bounds.y, -bounds.z); cube->normal(0.0f, -1.0f, 0.0f); cube->textureCoord(u,v);	//2
	cube->position(-bounds.x, -bounds.y,  bounds.z); cube->normal(0.0f, -1.0f, 0.0f); cube->textureCoord(0,0);	//3

	// up
	cube->position(-bounds.x, bounds.y,  bounds.z); cube->normal(0.0f, 1.0f, 0.0f); cube->textureCoord(u,0);	//4
	cube->position( bounds.x, bounds.y, -bounds.z); cube->normal(0.0f, 1.0f, 0.0f); cube->textureCoord(0,v);	//5
	cube->position(-bounds.x, bounds.y, -bounds.z); cube->normal(0.0f, 1.0f, 0.0f); cube->textureCoord(u,v);	//6
	cube->position( bounds.x, bounds.y,  bounds.z); cube->normal(0.0f, 1.0f, 0.0f); cube->textureCoord(0,0);	//7

	u = bounds.z / TEXTURE_SCALE;
	v = bounds.y / TEXTURE_SCALE;

	// east
	cube->position( bounds.x,  bounds.y,  bounds.z); cube->normal(1.0f, 0.0f, 0.0f); cube->textureCoord(u,v);	//8
	cube->position( bounds.x, -bounds.y, -bounds.z); cube->normal(1.0f, 0.0f, 0.0f); cube->textureCoord(0,0);	//9
	cube->position( bounds.x,  bounds.y, -bounds.z); cube->normal(1.0f, 0.0f, 0.0f); cube->textureCoord(0,v);	//10
	cube->position( bounds.x, -bounds.y,  bounds.z); cube->normal(1.0f, 0.0f, 0.0f); cube->textureCoord(u,0);	//11


	// west
	cube->position(-bounds.x, -bounds.y,  bounds.z); cube->normal(-1.0f, 0.0f, 0.0f); cube->textureCoord(0,0);	//12
	cube->position(-bounds.x,  bounds.y, -bounds.z); cube->normal(-1.0f, 0.0f, 0.0f); cube->textureCoord(u,v);	//13
	cube->position(-bounds.x, -bounds.y, -bounds.z); cube->normal(-1.0f, 0.0f, 0.0f); cube->textureCoord(u,0);	//14
	cube->position(-bounds.x,  bounds.y,  bounds.z); cube->normal(-1.0f, 0.0f, 0.0f); cube->textureCoord(0,v);	//15

	u = bounds.x / TEXTURE_SCALE;
	v = bounds.y / TEXTURE_SCALE;

	// south
	cube->position( bounds.x, -bounds.y,  bounds.z); cube->normal(0.0f, 0.0f, 1.0f); cube->textureCoord(u,0);	//16
	cube->position(-bounds.x,  bounds.y,  bounds.z); cube->normal(0.0f, 0.0f, 1.0f); cube->textureCoord(0,v);	//17
	cube->position(-bounds.x, -bounds.y,  bounds.z); cube->normal(0.0f, 0.0f, 1.0f); cube->textureCoord(0,0);	//18
	cube->position( bounds.x,  bounds.y,  bounds.z); cube->normal(0.0f, 0.0f, 1.0f); cube->textureCoord(u,v);	//19

	// north
	cube->position( bounds.x,  bounds.y, -bounds.z); cube->normal(0.0f, 0.0f, 1.0f); cube->textureCoord(u,v);	//20
	cube->position(-bounds.x, -bounds.y, -bounds.z); cube->normal(0.0f, 0.0f, 1.0f); cube->textureCoord(0,0);	//21
	cube->position(-bounds.x,  bounds.y, -bounds.z); cube->normal(0.0f, 0.0f, 1.0f); cube->textureCoord(0,v);	//22
	cube->position( bounds.x, -bounds.y, -bounds.z); cube->normal(0.0f, 0.0f, 1.0f); cube->textureCoord(u,0);	//23

	cube->triangle(0,1,2);   cube->triangle(3,1,0);
	cube->triangle(4,5,6);   cube->triangle(4,7,5);

	cube->triangle(8,9,10);   cube->triangle(8,11,9);
	cube->triangle(12,13,14);   cube->triangle(12,15,13);

	cube->triangle(16,17,18);   cube->triangle(16,19,17);
	cube->triangle(20,21,22);   cube->triangle(20,23,21);

	cube->end(); 

	return cube;
}