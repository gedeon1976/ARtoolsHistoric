
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

// constructor
mainWidget::mainWidget(void)
{
	// built the main user GUI
	userGUI = new ARtools();	

	// add the SoQt viewer and set as central widget
	ARviewer = new ARScene(userGUI);	

	// connect signal and slots
	connect(userGUI,SIGNAL(SetCameraRTSPaddress(rtspAddress)),
	ARviewer,SLOT(GetCameraRTSPAddresses(rtspAddress)));

	// connect the PTZ cameras
	connect(userGUI,SIGNAL(StartPTZCameras(bool)),
		this,SLOT(createCoinScene(bool)));

	userGUI->show();
}	

mainWidget::~mainWidget(void)
{
}

// initialize the coinScene with video at background
void mainWidget::createCoinScene(bool status){

	// draw Coin scene		
	int width = 704;
	int height = 576;
	if (status == true){
		ARviewer->createVideoScene(width,height);
		// set central widget
		userGUI->setCentralWidget(ARviewer);
	}else{
	}

	
	
}

#include "mainWidget.moc"