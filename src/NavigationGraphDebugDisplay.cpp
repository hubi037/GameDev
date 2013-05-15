#include "stdafx.h"
#include "NavigationGraphDebugDisplay.h"
 
#include <OgreSceneManager.h>
#include <OgreRenderQueue.h>
#include <OgreManualObject.h>
#include <OgreAxisAlignedBox.h>
 

 
// ===============================================================================================

NavigationGraphDebugDisplay::NavigationGraphDebugDisplay(Ogre::SceneManager *_sceneManager, float _fillAlpha)
   : sceneManager(_sceneManager), fillAlpha(_fillAlpha), manualObject(0), linesIndex(0)
{
        initialise();
}
 
NavigationGraphDebugDisplay::~NavigationGraphDebugDisplay()
{
        shutdown();
}
 
void NavigationGraphDebugDisplay::initialise()
{
        manualObject = sceneManager->createManualObject("debug_object_navigation_graph");
        sceneManager->getRootSceneNode()->createChildSceneNode("debug_object_navigation_graph")->attachObject(manualObject);
        manualObject->setDynamic(true);

        manualObject->begin("debug_draw", Ogre::RenderOperation::OT_LINE_LIST);
        manualObject->position(Ogre::Vector3::ZERO);
        manualObject->colour(Ogre::ColourValue::ZERO);
        manualObject->index(0);
        manualObject->end();
        manualObject->begin("debug_draw", Ogre::RenderOperation::OT_TRIANGLE_LIST);
        manualObject->position(Ogre::Vector3::ZERO);
        manualObject->colour(Ogre::ColourValue::ZERO);
        manualObject->index(0);
        manualObject->end();
 
                linesIndex = trianglesIndex = 0;
}
 
void NavigationGraphDebugDisplay::shutdown()
{
    sceneManager->destroySceneNode("debug_object_nav");
    sceneManager->destroyManualObject(manualObject);
}
 
void NavigationGraphDebugDisplay::buildLine(const Ogre::Vector3& start,
                     const Ogre::Vector3& end,
                     const Ogre::ColourValue& colour,
                     float alpha)
{
        int i = addLineVertex(start, Ogre::ColourValue(colour.r, colour.g, colour.b, alpha));
        addLineVertex(end, Ogre::ColourValue(colour.r, colour.g, colour.b, alpha));
 
        addLineIndices(i, i + 1);
}

 
void NavigationGraphDebugDisplay::drawLine(const Ogre::Vector3& start,
                     const Ogre::Vector3& end,
                     const Ogre::ColourValue& colour)
{
        buildLine(start, end, colour);
}
 
void NavigationGraphDebugDisplay::build()
{
        manualObject->beginUpdate(0);
        if (lineVertices.size() > 0 && isEnabled)
        {
                manualObject->estimateVertexCount(lineVertices.size());
                manualObject->estimateIndexCount(lineIndices.size());
                for (std::list<VertexPair>::iterator i = lineVertices.begin(); i != lineVertices.end(); i++)
                {
                                manualObject->position(i->first);
                                manualObject->colour(i->second);
                }
                for (std::list<int>::iterator i = lineIndices.begin(); i != lineIndices.end(); i++)
                        manualObject->index(*i);
        }
        manualObject->end();

}
 
void NavigationGraphDebugDisplay::clear()
{
    lineVertices.clear();
    lineIndices.clear();
    linesIndex = 0;
}

void NavigationGraphDebugDisplay::setEnabled(bool _isEnabled) 
{ 
	isEnabled = _isEnabled;
	manualObject->setVisible(isEnabled);
}
 
int NavigationGraphDebugDisplay::addLineVertex(const Ogre::Vector3 &vertex, const Ogre::ColourValue &colour)
{
    lineVertices.push_back(VertexPair(vertex, colour));
    return linesIndex++;
}
 
void NavigationGraphDebugDisplay::addLineIndices(int index1, int index2)
{
    lineIndices.push_back(index1);
    lineIndices.push_back(index2);
}
 
