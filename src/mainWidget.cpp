
/************************************************************************

    GUI class
    
    Description:	this class allow to show a main GUI
		  
    author:			Henry Portilla (c) 2013
					based on code by Carlos vazquez
    
    This code is freely available and is delivered as is
    without any warranty
    
    Copyright: See COPYING file that comes with this distribution

************************************************************************/
#include "mainWidget.h"
#include "ARScene.h"
#include "ARtools.h"

// constructor
mainWidget::mainWidget(void)
{
	// built the main user GUI
	ARtools *userGUI = new ARtools();
	userGUI->show();
	// add the SoQt viewer and set as central widget
	ARScene *ARviewer = new ARScene(userGUI);
	userGUI->setCentralWidget(ARviewer);
	
	// connect signal and slots
	connect(userGUI,SIGNAL(SetCameraRTSPaddress(rtspAddress)),
		ARviewer,SLOT(GetCameraRTSPAddresses(rtspAddress)));

}	

mainWidget::~mainWidget(void)
{
}

#include "mainWidget.moc"