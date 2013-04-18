/************************************************************************

    ARscene class
    
    Description:	this class allow to show AR over a stereo video
		  
    author:			Henry Portilla (c) 2013 IOC
					
    
    This code is freely available and is delivered as is
    without any warranty
    
    Copyright: See COPYING file that comes with this distribution

************************************************************************/
#include "ARScene.h"
// Stereo camera
#include "SoStereoTexture.h"
// Stereo video
#include "stereovideo.h"
// haptic connection
#include "hapticConnection.h"
#include <QTimer>
#include "common.h"

// Coin3D
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/SoRenderManager.h>
#include <Inventor/actions/SoGLRenderAction.h>

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

// Quarter viewer
//#include <Quarter/Quarter.h>
//#include <Quarter/QuarterWidget.h>



ARScene::ARScene(QWidget *parent):QWidget(parent)
{
	try{
		SoQt::init(this);
    	SoStereoTexture::initClass();   
        
		// make a simple scene to test the viewer
		SoSeparator *root = new SoSeparator;
		root->ref();
		SoSelection *selection= new SoSelection;
		selection->ref();
		SoOrthographicCamera *camera = new SoOrthographicCamera;
		selection->addChild(camera);
    
		SoBaseColor *color = new SoBaseColor;
		color->rgb = SbColor(1.0,1.0,0.0);
		root->addChild(color);
		////selection->addChild(color);
    
		//      add Stereo node    
		SoStereoTexture *Stereo = new SoStereoTexture;
		Stereo->width.setValue(videoWidth);
		Stereo->heigh.setValue(videoHeight);
		Stereo->IOD.setValue(7.0);//IOD
	  ////camera->viewAll(selection,viewer->getSoRenderManager()->getViewportRegion());
		//root->addChild(Stereo);
		root->addChild(new SoCone);    
    
		// get the rtsp addresses for left and rigth cameras
	
		
	//	StereoVideo video(leftRTSPCam,rightRTSPCam,videoWidth,videoHeight,Stereo);
		//QObject::connect(&video,SIGNAL(updatedone()),&mainGUI,SLOT(show_fps())); 
		// timer for update the image every 40 ms = 25fps	
		//QTimer* timer = new QTimer;
		//timer->setInterval(40);
		//timer->start();  
		//QObject::connect(timer,SIGNAL(timeout()),&mainGUI,SLOT(show_fps())); 
    
		// add haptic connection
		//const char* URLserver = "localhost";
		//const char* port = "5000";
		//hapticConnection hapticDevice(URLserver,port);    
		//hapticDevice.startConnection();
		//hapticConnection hapticDevice0;   
		//hapticDevice0.startConnection();
		//   
		//   QObject::connect(timer,SIGNAL(timeout()),&hapticDevice0,SLOT(enable_haptic_readings()));
		//QObject::connect(&hapticDevice0,SIGNAL(sendHapticData(mt::Transform)),
		//	     &mainGUI,SLOT(show_haptic_data(mt::Transform)));
		//   // send the haptic values to the virtual pointer
		//   QObject::connect(&hapticDevice0,SIGNAL(sendHapticData(mt::Transform)),
	 // 		     &video,SLOT(set_haptic_data(mt::Transform)));
	 //   //// send image projected points to main GUI
	 //   QObject::connect(&video,SIGNAL(sendimagepoints(imagePoints)),
		//	     &mainGUI,SLOT(get_image_points(imagePoints)));
		//// send left and right captured images to openCV processing
		//QObject::connect(&video,SIGNAL(sendIplImageStereo(IplImage*,IplImage*)),
		//		&mainGUI,SLOT(get_IplImageStereo(IplImage*,IplImage*)));
		//// send haptic limits to the haptic device
		//QObject::connect(&mainGUI,SIGNAL(SetWorkSpaceLimits(mt::Vector3, mt::Vector3)),
		//		&hapticDevice0,SLOT(getWorkSpaceLimits(mt::Vector3, mt::Vector3)));
		//// send 3D pointer visibility status
		//QObject::connect(&mainGUI,SIGNAL(SetVisibility3Dpointer(Visibility_Status)),
		//		&video,SLOT(set_3DpointerVisibility(Visibility_Status)));
	
		// set scene viewer

		viewer = new SoQtExaminerViewer(this);
		viewer->setSceneGraph(root);
		viewer->show();
    
		//mainGUI.setCentralWidget(viewer->getWidget()); 

		// add extra widgets

    
		 // make the viewer react to inputs events similar to the good old 
		 // examinerViewer
    
		//viewer->setInteractionModeEnabled(true);
		//viewer->setContextMenuEnabled(true);
	//    /* viewer->setNavigationModeFile(QUrl("coin:///scxml/navigation/examiner.xml"));
	//     viewer->move(10,40);
	//     viewer->resize(800,600);*/
		//viewer->show();
		//SoQt::show(mainwin);
		//SoQt::mainLoop();
	//     QDockWidget* stereo_dock = new QDockWidget(viewer->getWidget(),0);
	//     stereo_dock->setAttribute(Qt::WA_DeleteOnClose);// important
	//     stereo_dock->setAllowedAreas(Qt::LeftDockWidgetArea);
	//     stereo_dock->setWidget(viewer->getWidget());
	//     mainGUI.addDockWidget(Qt::LeftDockWidgetArea,stereo_dock);

      
      


    // get Coin's render manager (code from virtualtech at google code)
   /* 
    SoRenderManager *renderManager = new SoRenderManager;
    renderManager = viewer->getSoRenderManager();
    root->GLRender(renderManager->getGLRenderAction());
   */
    
    // Clean up resources.
    //root->unref();
    //delete viewer;
	}
	catch(...){
	}
}

ARScene::~ARScene(void)
{
}

// get the current rtsp addresses
void ARScene::GetCameraRTSPAddresses(rtspAddress address){

	// save the current rtsp addresses
	leftRTSPCam = address.leftCamRTSP;
	rightRTSPCam = address.rightCamRTSP;
}

#include "ARScene.moc"