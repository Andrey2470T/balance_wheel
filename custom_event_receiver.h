#include <irrlicht.h>

using namespace std;
using namespace irr;

class AppEventReceiver : public IEventReceiver
{
public:
	AppEventReceiver() : isEscPressed(false), camera_input(nullptr)
	{
	}
	
	virtual bool OnEvent(const SEvent& event)
	{
		
		if (event.EventType == EET_KEY_INPUT_EVENT &&
			event.KeyInput.Key == KEY_ESCAPE &&
			event.KeyInput.PressedDown
		)
		{
			isEscPressed = true;
		}
		
		return false;
	}
	
	bool isEscPressed;
	
	scene::ICameraSceneNode* camera_input;
};
