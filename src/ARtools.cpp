#include "ARtools.h"

ARtools::ARtools()
{
  // start the GUI and start all the GUI controls
  setupUi(this);
  setDockOptions(QMainWindow::VerticalTabs |
		 QMainWindow::AnimatedDocks |
		 QMainWindow::AllowTabbedDocks
		 );
	#if (UNIX)
		gettimeofday(&elapsedTimeFirst,&timeZone) ;
	#endif
  // initializes variables
	X_Haptic = 0;
	Y_Haptic = 0;
	Z_Haptic = 0;
  
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


#include "ARtools.moc"
