 #ifndef __Debug_Text_h_
 #define __Debug_Text_h_


using namespace Ogre;
 
/// Utility class to render debug text on the screen.
class DebugOverlay : public Ogre::Singleton<DebugOverlay>
{
private:
	OverlayManager*    mOverlayMgr;
	Overlay*           mOverlay;
	OverlayContainer*  mPanel;
 
public:
    DebugOverlay();
    ~DebugOverlay();
 
	/// adds a new text box
    void addTextBox(
        const std::string& ID,
        const std::string& text,
        Real x, Real y,
        Real width, Real height,
        const ColourValue& color = ColourValue(1.0, 1.0, 1.0, 1.0));
 
	/// removes a text box with the given id from the view.
    void removeTextBox(const std::string& ID);
 
	/// updates the content of the text box with the given id.
    void setText(const std::string& ID, const std::string& Text);

	/// updates the content of the text box with the given id using printf style
	void setTextf(const std::string& ID, const char *fmt, ...);

	/// returns the content of the text box with the given id.
    std::string getText(const std::string& ID);
};

#endif