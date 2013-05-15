#ifndef __NavigationDebugDisplayNav_h_
#define __NavigationDebugDisplayNav_h_
 
#include <OgreSingleton.h>
#include <map>
 
typedef std::pair<Ogre::Vector3, Ogre::ColourValue> VertexPair;


/// Utility class for debug rendering
class NavigationGraphDebugDisplay
{
public:
        NavigationGraphDebugDisplay(Ogre::SceneManager *_sceneManager, float _fillAlpha);
        ~NavigationGraphDebugDisplay();
 
        void build();
 
        void drawLine(const Ogre::Vector3 &start, const Ogre::Vector3 &end, const Ogre::ColourValue &colour);

        bool getEnabled() { return isEnabled; }
        void setEnabled(bool _isEnabled);
        void switchEnabled() { isEnabled = !isEnabled; }
        
        void clear();
 
private:
        Ogre::SceneManager *sceneManager;
        Ogre::ManualObject *manualObject;
        float fillAlpha;
 
        bool isEnabled;
 
        std::list<VertexPair> lineVertices;
        std::list<int> lineIndices;
 
        int linesIndex, trianglesIndex;

        void initialise();
        void shutdown();
 
        void buildLine(const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::ColourValue& colour, float alpha = 1.0f);

        int addLineVertex(const Ogre::Vector3 &vertex, const Ogre::ColourValue &colour);
        void addLineIndices(int index1, int index2);
};
 
#endif