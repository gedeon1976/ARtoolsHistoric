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
	matrixF_calculated = false;
	actualImages_Points.xiL = 1;
	actualImages_Points.xiR = 1;
	actualImages_Points.yiL = 1;
	actualImages_Points.yiR = 1;

	CannyWindowSize = 3;
	thresholdCannyLow = 1;
	thresholdCannyHigh = 10;
	HoughThreshold = 20;
    HoughMinLengthDetection = 10;
    HoughMaxGapBetweenLines = 5;
	LICF_MaxDistanceBetweenLines = 10;
	rendering_type = RENDERING_OPENCV;
	// connect Canny thresholds controls
	QObject::connect(this->hSldCannyLow,SIGNAL(sliderMoved(int)),
		this,SLOT(SetCannyLowThreshold(int)));
	QObject::connect(this->hSldCannyHigh,SIGNAL(sliderMoved(int)),
		this,SLOT(SetCannyHighThreshold(int)));
	QObject::connect(this->hSldCannyAperture,SIGNAL(sliderMoved(int)),
		this,SLOT(SetCannyAperture(int)));
	// connect hough threshold controls
	QObject::connect(this->spinBoxHoughThreshold, SIGNAL(valueChanged(int)),
		this,SLOT(SetHoughThreshold(int)));
	QObject::connect(this->spinBoxGapBetweenLines, SIGNAL(valueChanged(int)),
		this,SLOT(SetHoughMaxGapBetweenLines(int)));
	QObject::connect(this->spinBoxLineMinLength,SIGNAL(valueChanged(int)),
		this,SLOT(SetHoughMinLengthDetection(int)));
	// connect LICF controls
	QObject::connect(this->spinBoxLICFmaxLineDistance,SIGNAL(valueChanged(int)),
		this,SLOT(SetHoughLICF_MaxDistanceBetweenLines(int)));


  
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

  // fix: set automatic parameters w,h,B and stereo condition
  float w = 640; float h = 480;
  float f = 780;
  float B = 72;

  // save current points from haptic mapping to images
  switch(rendering_type){
  case RENDERING_3D:// 3D
		actualImages_Points.xiL = abs(-actualPoints.xiL + actualPoints.uo_L - w);
		actualImages_Points.xiR = abs(actualPoints.xiR - actualPoints.uo_R + w);
		actualImages_Points.yiL = abs(-actualPoints.yiL + actualPoints.vo_L - 0.5*h);
		actualImages_Points.yiR = abs(-actualPoints.yiR + actualPoints.vo_R - 0.5*h);
		break;
  case RENDERING_OPENCV:// OpenCV
		actualImages_Points.xiL = abs(w + actualPoints.xiL);
		actualImages_Points.xiR = abs(actualPoints.xiR);
		actualImages_Points.yiL = abs(actualPoints.yiL - 0.5*h);
		actualImages_Points.yiR = abs(actualPoints.yiR - 0.5*h);

  }
 // show data on GUI
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
		IMAGE_TYPE currentImage = LEFT;
		CvPoint2D32f leftSubImageCenter;
		CvPoint2D32f rightSubImageCenter;
		SubArea_Structure SubAreaLimitsL;
		SubArea_Structure SubAreaLimitsR;

		CvMemStorage* HoughStorageL = cvCreateMemStorage(0);
		CvMemStorage* HoughStorageR = cvCreateMemStorage(0);
		CvSeq* linesL = 0;
		CvSeq* linesR = 0;

		vector<LICFs_Structure> Actual_LICFs_L;
		vector<LICFs_Structure> Actual_LICFs_R;

		imgSize = cvGetSize(leftImage);
		IplImage *testImageL = cvCreateImage(imgSize,IPL_DEPTH_8U,1);
		IplImage *testImageR = cvCreateImage(imgSize,IPL_DEPTH_8U,1);
		IplImage *EdgeImageL = cvCreateImage(imgSize,IPL_DEPTH_8U,1);
		IplImage *EdgeImageR = cvCreateImage(imgSize,IPL_DEPTH_8U,1);

		IplImage *HoughLeftSubImage,*LICF_LeftSubImage;
		IplImage *HoughRightSubImage,*LICF_RightSubImage;
		IplImage *LeftSubImage,*LeftSubImageGray,*EdgeLeftSubImage;
		IplImage *RightSubImage,*RightSubImageGray,*EdgeRightSubImage;

		// copy image from camera and convert to OpenCV format: BGR
		IplImage *leftImageBGR = cvCloneImage(leftImage);
		IplImage *rightImageBGR = cvCloneImage(rightImage);
		cvCvtColor(leftImage,leftImageBGR,CV_RGB2BGR);
		cvCvtColor(rightImage,rightImageBGR,CV_RGB2BGR);

		// Convert to gray values to process the images
		cvCvtColor(leftImage,testImageL,CV_RGB2GRAY);
		cvCvtColor(rightImage,testImageR,CV_RGB2GRAY);

		// Epipolar geometry calculus
		if (matrixF_calculated == false){
			ImageProcessing.LoadImages(testImageL,testImageR);
			ImageProcessing.MatchPoints(0.995);
			ImageProcessing.FindFundamentalMatrix();
			//ImageProcessing.FindEpipoles();
			//ImageProcessing.FindEpipolarLines();
			// do this calculus only when the scene changes, no more
			matrixF_calculated = true;
		}else{
			matrixF_calculated = true;
		}

		// detect LICFs features for the left image
		LICFs LICFs_FeaturesL(leftImageBGR); 
		//// Get a gray image
		currentImage = LEFT;
		LICFs_FeaturesL.GetSubImage(actualImages_Points,0.1,currentImage);
		//// Get an Edge image
		LICFs_FeaturesL.ApplyCannyEdgeDetector(CannyWindowSize,thresholdCannyLow,
			thresholdCannyHigh);
		//// Get the lines of the image
		linesL = LICFs_FeaturesL.ApplyHoughLineDetection(HoughThreshold,
			HoughMinLengthDetection,HoughMaxGapBetweenLines);
		//// Get the LICFs features for the image
		Actual_LICFs_L = LICFs_FeaturesL.ApplyLICF_Detection(linesL,
			LICF_MaxDistanceBetweenLines);
		//// Get the limits for the area selected to analysis on the left image
		SubAreaLimitsL = LICFs_FeaturesL.GetSubAreaBoundaries();
		// draw a rectangle  and a circle to identify the area selected
		CvPoint upperLeft_L,lowerRight_L;
		upperLeft_L = cvPoint(SubAreaLimitsL.x_AreaCenter - abs(0.5*SubAreaLimitsL.width),
			SubAreaLimitsL.y_AreaCenter - abs(0.5*SubAreaLimitsL.heigh));
		lowerRight_L = cvPoint(SubAreaLimitsL.x_AreaCenter + abs(0.5*SubAreaLimitsL.width),
			SubAreaLimitsL.y_AreaCenter + abs(0.5*SubAreaLimitsL.heigh));

		cvRectangle(leftImageBGR,upperLeft_L,lowerRight_L,CV_RGB(0,255,0));
		cvCircle(leftImageBGR,cvPoint((int)SubAreaLimitsL.x_AreaCenter,(int)SubAreaLimitsL.y_AreaCenter)
			,2,CV_RGB(255,0,0));

		// detect LICFs features for the right image
		LICFs LICFs_FeaturesR(rightImageBGR); 
		//// Get a gray image
		currentImage = RIGHT;
		LICFs_FeaturesR.GetSubImage(actualImages_Points,0.1,currentImage);
		//// Get an Edge image
		LICFs_FeaturesR.ApplyCannyEdgeDetector(CannyWindowSize,thresholdCannyLow,
			thresholdCannyHigh);
		//// Get the lines of the image
		linesR = LICFs_FeaturesR.ApplyHoughLineDetection(HoughThreshold,
			HoughMinLengthDetection,HoughMaxGapBetweenLines);
		//// Get the LICFs features for the image
		Actual_LICFs_R = LICFs_FeaturesR.ApplyLICF_Detection(linesR,
			LICF_MaxDistanceBetweenLines);
		//// Get the limits for the area selected to analysis on the left image
		SubAreaLimitsR = LICFs_FeaturesR.GetSubAreaBoundaries();
		// draw a rectangle  and a circle to identify the area selected
		CvPoint upperLeft_R,lowerRight_R;
		upperLeft_R = cvPoint(SubAreaLimitsR.x_AreaCenter - abs(0.5*SubAreaLimitsR.width),
			SubAreaLimitsR.y_AreaCenter - abs(0.5*SubAreaLimitsR.heigh));
		lowerRight_R = cvPoint(SubAreaLimitsR.x_AreaCenter + abs(0.5*SubAreaLimitsR.width),
			SubAreaLimitsR.y_AreaCenter + abs(0.5*SubAreaLimitsR.heigh));

		cvRectangle(rightImageBGR,upperLeft_R,lowerRight_R,CV_RGB(0,255,0));
		cvCircle(rightImageBGR,cvPoint((int)SubAreaLimitsR.x_AreaCenter,(int)SubAreaLimitsR.y_AreaCenter)
			,2,CV_RGB(255,0,0));

		// draw results
		// LEFT IMAGE
		LICFs_Structure tmp_currentLICF;
		int xo_L = upperLeft_L.x;
		int yo_L = upperLeft_L.y;
		for (int i=0;i < Actual_LICFs_L.size();i++){
			if (Actual_LICFs_L.size() != 0){
			 tmp_currentLICF = Actual_LICFs_L.at(i);
			 cvLine(leftImageBGR, cvPoint(tmp_currentLICF.L_1.x1 + xo_L,tmp_currentLICF.L_1.y1 + yo_L),
				cvPoint(tmp_currentLICF.L_1.x2 + xo_L,tmp_currentLICF.L_1.y2 + yo_L), CV_RGB(0,128,255), 1, 8 );
			 cvLine(leftImageBGR, cvPoint(tmp_currentLICF.L_2.x1 + xo_L,tmp_currentLICF.L_2.y1 + yo_L),
				cvPoint(tmp_currentLICF.L_1.x2 + xo_L,tmp_currentLICF.L_1.y2 + yo_L), CV_RGB(0,255,128), 1, 8 );
			 cvCircle(leftImageBGR,cvPoint(tmp_currentLICF.x_xK + xo_L,tmp_currentLICF.y_xK + yo_L),2,CV_RGB(0,0,255),1,3,0);
			}
		}
		// RIGHT IMAGE
		LICFs_Structure tmp_currentLICF_R;
		int xo_R = upperLeft_R.x;
		int yo_R = upperLeft_R.y;
		for (int i=0;i < Actual_LICFs_R.size();i++){
			if (Actual_LICFs_R.size() != 0){
			 tmp_currentLICF_R = Actual_LICFs_R.at(i);
			 cvLine(rightImageBGR, cvPoint(tmp_currentLICF_R.L_1.x1 + xo_R,tmp_currentLICF_R.L_1.y1 + yo_R),
				cvPoint(tmp_currentLICF_R.L_1.x2 + xo_R,tmp_currentLICF_R.L_1.y2 + yo_R), CV_RGB(0,128,255), 1, 8 );
			 cvLine(rightImageBGR, cvPoint(tmp_currentLICF_R.L_2.x1 + xo_R,tmp_currentLICF_R.L_2.y1 + yo_R),
				cvPoint(tmp_currentLICF_R.L_1.x2 + xo_R,tmp_currentLICF_R.L_1.y2 + yo_R), CV_RGB(0,255,128), 1, 8 );
			 cvCircle(rightImageBGR,cvPoint(tmp_currentLICF_R.x_xK + xo_R,tmp_currentLICF_R.y_xK + yo_R),2,CV_RGB(0,0,255),1,3,0);
			}
		}




		
		// draw the corresponding haptic point

		// draw a rectangle for the influence area of the 3D pointer
		// the value chosen was 10 % of image size
			//CvPoint leftUpperL,rightLowerL;
			//CvPoint leftUpperR,rightLowerR;
			//// left image
			//leftUpperL = cvPoint((actualImages_Points.xiL - abs((0.1*leftImageBGR->width/2))),actualImages_Points.yiL - abs((0.1*leftImageBGR->height/2)));
			//rightLowerL = cvPoint((actualImages_Points.xiL + abs((0.1*leftImageBGR->width/2))),actualImages_Points.yiL + abs((0.1*leftImageBGR->height/2)));
			//// right image
			//leftUpperR = cvPoint((actualImages_Points.xiR - abs((0.1*rightImageBGR->width/2))),actualImages_Points.yiR - abs((0.1*rightImageBGR->height/2)));
			//rightLowerR = cvPoint((actualImages_Points.xiR + abs((0.1*rightImageBGR->width/2))),actualImages_Points.yiR + abs((0.1*rightImageBGR->height/2)));
			//
			//// sub image rectangle size making
			//CvSize LeftSubImageSize = cvSize((rightLowerL.x - leftUpperL.x),(rightLowerL.y - leftUpperL.y));
			//CvSize RightSubImageSize = cvSize((rightLowerR.x - leftUpperR.x),(rightLowerR.y - leftUpperR.y));
			//
			//LeftSubImage = cvCreateImage(LeftSubImageSize,IPL_DEPTH_8U,3);
			//RightSubImage = cvCreateImage(RightSubImageSize,IPL_DEPTH_8U,3);
			//
			//LeftSubImageGray = cvCreateImage(LeftSubImageSize,IPL_DEPTH_8U,1);
			//RightSubImageGray = cvCreateImage(RightSubImageSize,IPL_DEPTH_8U,1);

			//HoughLeftSubImage = cvCreateImage(LeftSubImageSize,IPL_DEPTH_8U,3);
			//HoughRightSubImage = cvCreateImage(RightSubImageSize,IPL_DEPTH_8U,3);

			//EdgeLeftSubImage = cvCreateImage(LeftSubImageSize,IPL_DEPTH_8U,1);
			//EdgeRightSubImage = cvCreateImage(RightSubImageSize,IPL_DEPTH_8U,1);

			////EdgeLeftSubImageSobel = cvCreateImage(LeftSubImageSize,IPL_DEPTH_16S,1);		

			//leftSubImageCenter.x = actualImages_Points.xiL;
			//leftSubImageCenter.y = actualImages_Points.yiL;
			//
			//rightSubImageCenter.x = actualImages_Points.xiR;
			//rightSubImageCenter.y = actualImages_Points.yiR;

			//cvGetRectSubPix(leftImageBGR,LeftSubImage,leftSubImageCenter);
			//cvCvtColor(LeftSubImage,LeftSubImageGray,CV_BGR2GRAY);
			//LICF_LeftSubImage = cvCloneImage(LeftSubImage); 

			//cvGetRectSubPix(rightImageBGR,RightSubImage,rightSubImageCenter);
			//cvCvtColor(RightSubImage,RightSubImageGray,CV_BGR2GRAY);
			//LICF_RightSubImage = cvCloneImage(RightSubImage);	

			//// put the matches and a rectangle influence area on a test image
			//cvRectangle(leftImageBGR,leftUpperL,rightLowerL,CV_RGB(0,255,0));
			//cvCircle(leftImageBGR,cvPoint(actualImages_Points.xiL,actualImages_Points.yiL)
			//	,3,CV_RGB(255,0,0),2,3,0);

			//cvRectangle(rightImageBGR,leftUpperR,rightLowerR,CV_RGB(0,255,0));
			//cvCircle(rightImageBGR,cvPoint(actualImages_Points.xiR,actualImages_Points.yiR)
			//	,3,CV_RGB(255,0,0),2,3,0);
		// show images
		cvNamedWindow("Left Video");
		cvNamedWindow("Right Video");

		//cvNamedWindow("Left SubImage");
		//cvNamedWindow("Right SubImage");

		cvNamedWindow("Edge Image L");
		//cvNamedWindow("Edge Image R");

	/*	cvNamedWindow("Hough Image L");
		cvNamedWindow("Hough Image R");

		cvNamedWindow("LICF Image L");
		cvNamedWindow("LICF Image R");*/

		cvShowImage("Left Video",leftImageBGR);
		cvShowImage("Right Video",rightImageBGR);

		//cvShowImage("Left SubImage",LeftSubImage);
		//cvShowImage("Left SubImage",RightSubImage);	

		//cvShowImage("Edge Image L",EdgeLeftSubImage);
		

		// Find Canny edges
		/*cvLaplace(LeftSubImageGray,EdgeLeftSubImageSobel,CannyWindowSize);
		cvConvertScaleAbs(EdgeLeftSubImageSobel,EdgeLeftSubImage);*/
			//cvCanny(LeftSubImageGray,EdgeLeftSubImage,thresholdCannyLow,thresholdCannyHigh,CannyWindowSize);
			//cvCanny(RightSubImageGray,EdgeRightSubImage,thresholdCannyLow,thresholdCannyHigh,CannyWindowSize);
			//
			//linesL = cvHoughLines2(EdgeLeftSubImage,HoughStorageL,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,
			//	HoughThreshold,HoughMinLengthDetection,HoughMaxGapBetweenLines);
			//linesR = cvHoughLines2(EdgeRightSubImage,HoughStorageR,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,
			//	HoughThreshold,HoughMinLengthDetection,HoughMaxGapBetweenLines);

			//cvCvtColor( EdgeLeftSubImage, HoughLeftSubImage, CV_GRAY2BGR );
			//cvCvtColor( EdgeRightSubImage, HoughRightSubImage, CV_GRAY2BGR );
			//// Detect LICFs features
			//
			//// minimum intersection distance in pixels
			//float dth_threshold = LICF_MaxDistanceBetweenLines;	
			//lineParameters currentLine;
			//LICFs_Structure currentLICF,tmp_currentLICF;
			//
			//vector<lineParameters> Actual_Lines;
			//vector<LICFs_Structure> Actual_LICFs;
			//	
			//for( int i = 0; i < linesL->total; i++ )
			//{
			//	CvPoint* line = (CvPoint*)cvGetSeqElem(linesL,i);
			//	cvLine( HoughLeftSubImage, line[0], line[1], CV_RGB(255,0,0), 1, 8 );
			//	// find line equations
			//	if ((line[0].x - line[1].x)== 0){
			//		// this case correspond to a vertical line with undefined slope
			//		// and the line equation is reduced to x = a
			//		currentLine.m = 1000;
			//		currentLine.a = line[1].x;
			//		currentLine.b = 1000;
			//		currentLine.c = 1000;		
			//	}
			//	else{
			//		currentLine.m = (line[0].y - line[1].y)/(line[0].x - line[1].x);
			//		currentLine.a = currentLine.m*(-line[1].x);
			//		currentLine.b = -1;
			//		currentLine.c = line[1].y;				

			//		}

			//	
			//	// save line parameters
			//	currentLine.x1 = line[1].x;
			//	currentLine.x2 = line[0].x;
			//	currentLine.y1 = line[1].y;
			//	currentLine.y2 = line[0].y;
			//	Actual_Lines.push_back(currentLine);
			//
			//}
			//cvShowImage("Hough Image L",HoughLeftSubImage);
			//cvShowImage("Edge Image L",EdgeLeftSubImage);
			//
			//
			//// Find LICFs intersections
			//float d_betweenLines1 = 10;
			//float d_betweenLines2 = 10;
			//float d_betweenLines3 = 10;
			//float d_betweenLines4 = 10;
			//float d_Min_betweenLines,d_Min1,d_Min2 = 10;
			//lineParameters currentLine1;
			//lineParameters currentLine2;
			//// using line intersection method by Paul Borke's web page
			//float U_a,U_b = 1;		
			//float numerator_U_a = 1, numerator_U_b = 1;
			//float denominator_U_a = 1, denominator_U_b = 1;
			//float xi,yi;

			//for(int i=0; i < Actual_Lines.size(); i++){
			//	for(int j=0; j < Actual_Lines.size(); j++){
			//		// check that the checking lines are not the same line
			//		// at the vector structure
			//		if(i != j){
			//			currentLine1 = Actual_Lines.at(i);
			//			currentLine2 = Actual_Lines.at(j);
			//			// check the distance among the 4 line end points of the 2 lines
			//			d_betweenLines1 = sqrt(pow((currentLine1.x1 - currentLine2.x1),2)+ pow((currentLine1.y1 - currentLine2.y1),2));
			//			d_betweenLines2 = sqrt(pow((currentLine1.x1 - currentLine2.x2),2)+ pow((currentLine1.y1 - currentLine2.y2),2));
			//			d_betweenLines3 = sqrt(pow((currentLine1.x2 - currentLine2.x1),2)+ pow((currentLine1.y2 - currentLine2.y1),2));
			//			d_betweenLines4 = sqrt(pow((currentLine1.x2 - currentLine2.x2),2)+ pow((currentLine1.y2 - currentLine2.y2),2));
			//			// check min value
			//			d_Min1 = min(d_betweenLines1, d_betweenLines1);
			//			d_Min2 = min(d_betweenLines3, d_betweenLines4);
			//			d_Min_betweenLines = min(d_Min1,d_Min2);

			//			if (d_Min_betweenLines < dth_threshold ){
			//			 // calculate intersection check 						 
			//			 numerator_U_a = (currentLine2.x2 - currentLine2.x1)*(currentLine2.y1 - currentLine1.y1) -
			//				 (currentLine2.x1 - currentLine1.x1)*(currentLine2.y2 - currentLine2.y1);
			//			 denominator_U_a = (currentLine2.x2 - currentLine2.x1)*(currentLine1.y2 - currentLine1.y1) -
			//				 (currentLine2.y2 - currentLine2.y1)*(currentLine1.x2 - currentLine1.x1);
			//			 numerator_U_b = (currentLine2.y1 - currentLine1.y1)*(currentLine1.x2 - currentLine1.x1) -
			//				 (currentLine2.x1 - currentLine1.x1)*(currentLine1.y2 - currentLine1.y1);
			//			 denominator_U_b = denominator_U_a;
			//			 U_a = numerator_U_a/denominator_U_a;
			//			 U_b = numerator_U_b/denominator_U_b;

			//			 // check line status: Parallel, Coincident, not intersecting, intersecting

			//			 // intersecting case
			//			 if ((denominator_U_a != 0.0f) & (numerator_U_a != 0.0f) & (numerator_U_b != 0.0f)){

			//				// intersecting point between line segments
			//				 if (((U_a >= 0.0f) & (U_a <= 1.0f) & (U_b >= 0.0f) & (U_b <= 1.0f))){
			//					
			//					yi = (currentLine1.y1 + U_a*(currentLine1.y2 - currentLine1.y1));
			//					xi = (currentLine1.x1 + U_a*(currentLine1.x2 - currentLine1.x1));
			//					// save LICF
			//					currentLICF.x_xK = xi;
			//					currentLICF.y_xK = yi;
			//					currentLICF.L_1 = currentLine1;
			//					currentLICF.L_2 = currentLine2;
			//					// save found LICF
			//					Actual_LICFs.push_back(currentLICF);
			//				
			//				 }
			//			 }						 
			//
			//			}
			//			// show last found LICF
			//			if (Actual_LICFs.size() != 0){
			//				tmp_currentLICF = Actual_LICFs.back();
			//				cvLine(LICF_LeftSubImage, cvPoint(tmp_currentLICF.L_1.x1,tmp_currentLICF.L_1.y1), cvPoint(tmp_currentLICF.L_1.x2,tmp_currentLICF.L_1.y2), CV_RGB(0,128,255), 1, 8 );
			//				cvLine(LICF_LeftSubImage, cvPoint(tmp_currentLICF.L_2.x1,tmp_currentLICF.L_2.y1), cvPoint(tmp_currentLICF.L_1.x2,tmp_currentLICF.L_1.y2), CV_RGB(0,255,128), 1, 8 );
			//				cvCircle(LICF_LeftSubImage,cvPoint(xi,yi),2,CV_RGB(0,0,255),1,3,0);
			//				
			//			}

			//		}
			//	}
			//	cvShowImage("LICF Image L",LICF_LeftSubImage);
			//}
		
		// free the resources
		cvReleaseImage(&testImageL);
		cvReleaseImage(&testImageR);

		cvReleaseImage(&leftImage);		
		cvReleaseImage(&rightImage);

		cvReleaseImage(&leftImageBGR);
		cvReleaseImage(&rightImageBGR);

		//cvReleaseImage(&EdgeImageL);
		//cvReleaseImage(&EdgeImageR);

		/*cvReleaseImage(&HoughLeftSubImage);
		cvReleaseImage(&HoughRightSubImage);

		cvReleaseImage(&LICF_LeftSubImage);
		cvReleaseImage(&LICF_RightSubImage);

		cvReleaseImage(&EdgeLeftSubImage);
		cvReleaseImage(&EdgeRightSubImage);

		cvReleaseImage(&LeftSubImage);
		cvReleaseImage(&RightSubImage);

		cvReleaseImage(&LeftSubImageGray);
		cvReleaseImage(&RightSubImageGray);*/

		

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

// set canny window size
void ARtools::SetCannyAperture(int hSldCannyAperture){
	try{
		CannyWindowSize = hSldCannyAperture;
	}
	catch(...){
	}
}
void ARtools::SetCannyLowThreshold(int hSldCannyLowValue){
	// set the canny thresholds
	try{
		thresholdCannyLow = hSldCannyLowValue;				
	}
	catch(...){
	}
}
void ARtools::SetCannyHighThreshold(int hSldCannyHighValue){
	// set the canny thresholds
	try{
		thresholdCannyHigh= hSldCannyHighValue;				
	}
	catch(...){
	}
}
// set Hough Parameters
void ARtools::SetHoughThreshold(int HoughThreshold){
	try{
		HoughThreshold = HoughThreshold;
        }
	catch(...){
	}
}
void ARtools::SetHoughMinLengthDetection(int minLengthDetection){
	try{
		HoughMinLengthDetection = minLengthDetection;
    }
	catch(...){
	}
}
void ARtools::SetHoughMaxGapBetweenLines(int maxGapBetweenLines){
	try{
		HoughMaxGapBetweenLines = maxGapBetweenLines;
	}
	catch(...){
	}
}

// set LICF parameters
void ARtools::SetLICF_MaxDistanceBetweenLines(int maxDistance){
	try{

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
