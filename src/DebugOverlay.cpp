#include "StdAfx.h"

#include "DebugOverlay.h"
#include "OgreFontManager.h"
 
template<> DebugOverlay* Ogre::Singleton<DebugOverlay>::msSingleton = 0;
 
DebugOverlay::DebugOverlay()
{
	// get the resource manager
	ResourceGroupManager &resGroupMgr = ResourceGroupManager::getSingleton();
	resGroupMgr.addResourceLocation("../media/fonts", "FileSystem");

	FontManager &fontMgr = FontManager::getSingleton();
	ResourcePtr font = fontMgr.create("MyFont","General");
	// set as truetype

	font->setParameter("type","truetype");
	font->setParameter("source","consolab.ttf");
	font->setParameter("size","26");
	font->setParameter("resolution","96");
	font->load();

    mOverlayMgr = Ogre::OverlayManager::getSingletonPtr();
 
    mOverlay = mOverlayMgr->create("overlay1");
    mPanel = static_cast<Ogre::OverlayContainer*>(mOverlayMgr->createOverlayElement("Panel", "container1"));
    mPanel->setDimensions(1, 1);
    mPanel->setPosition(0, 0);
 
    mOverlay->add2D(mPanel);
 
    mOverlay->show();
}
 
void DebugOverlay::addTextBox(const std::string& ID,
                const std::string& text,
                Ogre::Real x, Ogre::Real y,
                Ogre::Real width, Ogre::Real height,
                const Ogre::ColourValue& color)
{
    Ogre::OverlayElement* textBox = mOverlayMgr->createOverlayElement("TextArea", ID);
    textBox->setDimensions(width, height);
    textBox->setMetricsMode(Ogre::GMM_PIXELS);
    textBox->setPosition(x, y);
    textBox->setWidth(width);
    textBox->setHeight(height);
	textBox->setParameter("font_name", "MyFont");
    textBox->setParameter("char_height", "16");
    textBox->setColour(color);
 
    textBox->setCaption(text);
 
    mPanel->addChild(textBox);
}
 
void DebugOverlay::removeTextBox(const std::string& ID)
{
    mPanel->removeChild(ID);
    mOverlayMgr->destroyOverlayElement(ID);
}
 
void DebugOverlay::setText(const std::string& ID, const std::string& Text)
{
    Ogre::OverlayElement* textBox = mOverlayMgr->getOverlayElement(ID);
    textBox->setCaption(Text);
}

void DebugOverlay::setTextf(const std::string& ID,  const char *fmt, /* args*/ ...)
{    
    char        text[256];
    va_list        ap;
 
    if (fmt == NULL)
        *text=0;
 
    else {
    va_start(ap, fmt);
		vsprintf_s(text, fmt, ap);
    va_end(ap);
    }
 
    Ogre::OverlayElement* textBox = mOverlayMgr->getOverlayElement(ID);
    textBox->setCaption(text);
}
 
std::string DebugOverlay::getText(const std::string& ID)
{
    Ogre::OverlayElement* textBox = mOverlayMgr->getOverlayElement(ID);
    return textBox->getCaption();
}