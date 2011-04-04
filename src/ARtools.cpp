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
void ARtools::show_haptic_data(ioc_comm::vecData hapticData)
{
  // show haptic values on corresponding labels
  QString X;QString Y;QString Z;
  QString alpha;QString beta;QString gamma;
  QString Stylusbutton;
  
  if(hapticData.size() > 0)
    {
      ioc_comm::baseData& temp = hapticData[0];
      X.setNum(temp._data[0]);Xhaptic->setText(X);X_Haptic = temp._data[0];
      Y.setNum(temp._data[1]);Yhaptic->setText(Y);Y_Haptic = temp._data[1];
      Z.setNum(temp._data[2]);Zhaptic->setText(Z);Z_Haptic = temp._data[2];
      alpha.setNum(temp._data[3]);alphaHaptic->setText(alpha);
      beta.setNum(temp._data[4]);betaHaptic->setText(beta);
      gamma.setNum(temp._data[5]);gammaHaptic->setText(gamma);
      Stylusbutton.setNum(temp._data[6]);        
      
    }else{
      // default values
      X.setNum(1);Xhaptic->setText(X);X_Haptic = 1;
      Y.setNum(1);Yhaptic->setText(Y);Y_Haptic = 1;
      Z.setNum(1);Zhaptic->setText(Z);Z_Haptic = 1;
      alpha.setNum(0);alphaHaptic->setText(alpha);
      beta.setNum(0);betaHaptic->setText(beta);
      gamma.setNum(0);gammaHaptic->setText(gamma);    
      
    }
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
  
  xil.setNum(actualPoints.xiL);xiL->setText(xil);
  yil.setNum(actualPoints.yiL);xiL->setText(yil);
  xir.setNum(actualPoints.xiR);xiL->setText(xir);
  yir.setNum(actualPoints.yiR);xiL->setText(yir);
  
}


#include "ARtools.moc"
