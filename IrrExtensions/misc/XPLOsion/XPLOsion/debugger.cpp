/*
Debugger for the XPLOsion project
*/

#include <irrlicht.h>
//#include <irrWinWriter.h>

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#include "xplo.h"

#include <irrMultiEventReciever.h>

#include <iostream>
using std::cout;

// c++ files
/*
#include "GUIfunctionFire.cpp" // removed
#include "CSSTagDataNess.cpp"
#include "CSSTagDataImpl.cpp"
#include "CSSLibraryNess.cpp"
#include "CSSLibraryImpl.cpp"
#include "sdiv.cpp"
#include "xplo.cpp"
*/

using namespace irr;


class SER : public IEventReceiver
{
public:
	SER() : IEventReceiver()
	{
	}

	bool OnEvent( const irr::SEvent& event )
	{
		if ( event.EventType == EET_GUI_EVENT )
		{
			gui::IGUIElement* elem = event.GUIEvent.Caller;

			if ( elem->getID() == 10 || elem->getID() == 15 || elem->getID() == 20 )
			{
				cout << "\nMenu(" << elem->getID() << ")";
				s32 i = ((gui::IGUIContextMenu*)elem)->getSelectedItem();
				cout << "\nButton(" << i << ")";
			}
		}

		return false;
	}
};


void main()
{
	IrrlichtDevice * irrdev = createDevice(
			video::EDT_BURNINGSVIDEO//,
			//core::dimension2du( 1024, 768 )
		);

	video::IVideoDriver * vid = irrdev->getVideoDriver();

	XPLOsion * xplo = new XPLOsion( irrdev, false );
	//GUIFunctionFire * gff = new GUIFunctionFire( irrdev->getGUIEnvironment() );
	CSSLibrary * css = new CSSLibrary( irrdev );

	SER ser;
	irrMultiEventReceiver mer;
	mer.registerReceiver( &ser );
	//mer.registerReceiver( gff );

	irrdev->setEventReceiver( &mer );

	xplo->setCSSDataPointer( css );
	//xplo->setFunctionFireList( gff );

	css->ParseCSSFile( "../test files/testcss.css" );


	// change the stupid font
	//irr::gui::IGUIFont * bif = irrdev->getGUIEnvironment()
	//	->getFont( "../test files/biflarge.bmp" );
	irr::gui::IGUIFont * bif = irrdev->getGUIEnvironment()
		->getFont( "../test files/batang/jp_batang.xml" );

	irrdev->getGUIEnvironment()->getSkin()->setFont( bif );


	if ( xplo->LoadFile( "../test files/testgui5.xml" ) )
	{
		xplo->ParseFile();


		while ( irrdev->run() )
		{
			vid->beginScene();
			
			irrdev->getGUIEnvironment()->drawAll();

			vid->endScene();
		}
	}

}