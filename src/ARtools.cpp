#include "ARtools.h"
//#if _WIN32
//	// Visual Studio Debug
//	#include "reportingHook.h"
//#endif

ARtools::ARtools()
{
  // start the GUI and start all the GUI controls
  setupUi(this);
  setDockOptions(QMainWindow::VerticalTabs |
		 QMainWindow::AnimatedDocks |
		 QMainWindow::AllowTabbedDocks
		 );
  createActions();
  createMenus();
	#if (UNIX)
		gettimeofday(&elapsedTimeFirst,&timeZone) ;
	#endif
	// initializes variables
	// haptic 
	X_Haptic = 0;
	Y_Haptic = 0;
	Z_Haptic = 0;
	// openCV
	leftImage = NULL;
	rightImage = NULL;
  
}

ARtools::~ARtools()
{}

void ARtools::show_fps()
{
  // show fps in a label  
  QString Value;
  float frames_per_second;
	#if (UNIX)
	// timing measurements
	  gettimeofday(&elapsedTimeLast,&timeZone);     
	  timeDiff = (elapsedTimeLast.tv_usec - elapsedTimeFirst.tv_usec)/1000;
	  elapsedTimeFirst.tv_sec = elapsedTimeLast.tv_sec;
	  elapsedTimeFirst.tv_usec = elapsedTimeLast.tv_usec;  
	  
	  frames_per_second = 1/(timeDiff*0.001);  
	  Value.setNum(frames_per_second); 
	  frame_rate_label->setText(Value);
	#endif
}
// show data from haptic device
void ARtools::show_haptic_data(mt::Vector3 position)
{
  // show haptic values on corresponding labels
  QString X;QString Y;QString Z;
  QString alpha;QString beta;QString gamma;
  QString StylusButton;
             
    // default values
  X.setNum(position[0]);Xhaptic->setText(X);X_Haptic = position[0];
  Y.setNum(position[1]);Yhaptic->setText(Y);Y_Haptic = position[1];
  Z.setNum(position[2]);Zhaptic->setText(Z);Z_Haptic = position[2];
  alpha.setNum(0);alphaHaptic->setText(alpha);
  beta.setNum(0);betaHaptic->setText(beta);
  gamma.setNum(0);gammaHaptic->setText(gamma);    
      
   
}


float ARtools::getTimeDiff()
{
  float timeinMsec;
  timeinMsec = timeDiff;
  return timeinMsec;
}

float ARtools::get_X_value()
{
  float Value;
  Value = X_Haptic;
  return Value;
}
float ARtools::get_Y_value()
{
  float Value;
  Value = Y_Haptic;
  return Value;
}
float ARtools::get_Z_value()
{
  float Value;
  Value = Z_Haptic;
  return Value;
}

void ARtools::get_image_points(imagePoints actualPoints)
{
   // show image projected values on corresponding labels
  QString xil;QString yil;
  QString xir;QString yir;
  QString currentPositionDisparity;
  QString currentPositionDepth;
  QString currentPositionX_World;
  QString currentPositionY_World;
  float currentPixelDisparity;
  float currentPixelDepth;
  float currentPixelX_World;
  float currentPixelY_World;

  // fix to set automatic the w,h,B and stereo condition
  float w = 640;
  float f = 780;
  float B = 72;

  xil.setNum(actualPoints.xiL);xiL->setText(xil);
  yil.setNum(actualPoints.yiL);yiL->setText(yil);
  xir.setNum(actualPoints.xiR);xiR->setText(xir);
  yir.setNum(actualPoints.yiR);yiR->setText(yir); 
  // show current disparity
  currentPixelDisparity = fabs(fabs(actualPoints.xiL) - fabs(actualPoints.xiR));
  currentPositionDisparity.setNum(currentPixelDisparity);
  lbl_disparityValue->setText(currentPositionDisparity);
  // show current depth of pointer
  currentPixelDepth = f*B/currentPixelDisparity;
  currentPositionDepth.setNum(currentPixelDepth);
  lbl_depthValue->setText(currentPositionDepth);
  // show X, Y world coordinates from left camera
  currentPixelX_World = actualPoints.xiL*B/currentPixelDisparity;
  currentPixelY_World = actualPoints.yiL*B/currentPixelDisparity;
  currentPositionX_World.setNum(currentPixelX_World);
  currentPositionY_World.setNum(currentPixelY_World);
  lbl_XWorldValue->setText(currentPositionX_World);
  lbl_YWorldValue->setText(currentPositionY_World);

}

// Get the images from the remote cameras
void ARtools::get_IplImageStereo(IplImage *actualImageL,IplImage *actualImageR){
	// copy the images
	try{
		leftImage = cvCloneImage(actualImageL);
		rightImage = cvCloneImage(actualImageR);
		ShowStereoVideo(); 
		cvReleaseImage(&actualImageL);
		cvReleaseImage(&actualImageR);
	}
	catch(...){

	}
}

void ARtools::get_IplImageL(IplImage *actualImageL){

	// copy IplImage from remote cameras
	try{
		leftImage = cvCloneImage(actualImageL);
		ShowLeftVideo();
		cvReleaseImage(&actualImageL);
	}
	catch(...){
		int err = cvGetErrStatus();
		const char* description = cvErrorStr(err);
		int i=143;
		cvGuiBoxReport(err,"get_IplImage",description,"ARtools.cpp",i,"2");

	}
}
// copy Rigth remote camera images
void ARtools::get_IplImageR(IplImage *actualImageR){
	try{
		rightImage = cvCloneImage(actualImageR);
		ShowRightVideo();
		cvReleaseImage(&actualImageR);
	}
	catch(...){
	}

}
// show the left and right cameras on OpenCV window
void ARtools::ShowStereoVideo(){
	try{
		CvSize imgSize;
		imgSize = cvSize(640,480);
		IplImage *testImageL = cvCloneImage(leftImage);
		IplImage *testImageR = cvCloneImage(rightImage);
		// copy image from camera and convert to OpenCV format: BGR
		cvCvtColor(leftImage,testImageL,CV_RGB2BGR);
		cvCvtColor(rightImage,testImageR,CV_RGB2BGR);
		// image processing

		// show images
		cvNamedWindow("Left Video");
		cvNamedWindow("Right Video");
		cvShowImage("Left Video",testImageL);
		cvShowImage("Right Video",testImageR);
		// free the resources
		cvReleaseImage(&testImageL);
		cvReleaseImage(&testImageR);
		cvReleaseImage(&leftImage);		
		cvReleaseImage(&rightImage);
	}
	catch(...){
	}
}
// Show the left camera video on a OpenCV window
void ARtools::ShowLeftVideo(){
	// show a openCV window
	try{
		CvSize imgSize;
		imgSize = cvSize(640,480);
		IplImage *testImage = cvCloneImage(leftImage);
		// copy image from camera and convert to OpenCV format: BGR
		cvCvtColor(leftImage,testImage,CV_RGB2BGR);
		cvNamedWindow("Left Video");
		cvShowImage("Left Video",testImage);
		cvReleaseImage(&testImage);
		cvReleaseImage(&leftImage);
	}
	catch(...){

		int err = cvGetErrStatus();
		const char* description = cvErrorStr(err);
		int i=143;
		cvGuiBoxReport(err,"ShowLeftVideo",description,"ARtools.cpp",i,"2");

	}
}
// Show the right camera video on a OpenCV window
void ARtools::ShowRightVideo(){
	// show a openCV window
	try{
		CvSize imgSize;
		imgSize = cvSize(640,480);
		IplImage *testImage = cvCloneImage(rightImage);
		// copy image from camera and convert to OpenCV format: BGR
		cvCvtColor(rightImage,testImage,CV_RGB2BGR);
		cvNamedWindow("Right Video");
		cvShowImage("Right Video",testImage);
		cvReleaseImage(&testImage);
		cvReleaseImage(&rightImage);
	}
	catch(...){
	}
}

void ARtools::AboutAct(){
	// Show a message about the application 
	
		QMessageBox::about(this, tr("About ARtools"),
		tr("This is a prototype of teleoperation tools based on augmented reality technology"));
	
}
void ARtools::createActions(){
	// create the action for the menus
	bool status = true;
	// Help actions
	aboutApp = new QAction(tr("About..."),this);
	connect(aboutApp,SIGNAL(triggered()),
			this,SLOT(AboutAct()));
	// Video Processing actions
	showleftCamera = new QAction(tr("Show Left Camera"),this);
	connect(showleftCamera,SIGNAL(triggered()),
			this,SLOT(ShowLeftVideo()));
	

	
}
void ARtools::createMenus(){
	// create the menus
	// Video Processing
	VideoProcessing_Menu = this->menuBar()->addMenu("&Video Processing");
	VideoProcessing_Menu->addAction(showleftCamera);
	// Help
	help_Menu = this->menuBar()->addMenu("&Help");
	help_Menu->addAction(aboutApp);
	


	
}

#include "ARtools.moc"
