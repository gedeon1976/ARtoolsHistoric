#include "ARtools.h"

ARtools::ARtools()
{
  // start the GUI and start all the GUI controls
  setupUi(this);
  setDockOptions(QMainWindow::VerticalTabs |
		 QMainWindow::AnimatedDocks |
		 QMainWindow::AllowTabbedDocks
		 );
  gettimeofday(&elapsedTimeFirst,&timeZone) ;
  
}

ARtools::~ARtools()
{}

void ARtools::show_fps()
{
  // show fps in a label  
  QString Value;
  float frames_per_second;
  // timing measurements
  gettimeofday(&elapsedTimeLast,&timeZone);     
  timeDiff = (elapsedTimeLast.tv_usec - elapsedTimeFirst.tv_usec)/1000;
  elapsedTimeFirst.tv_sec = elapsedTimeLast.tv_sec;
  elapsedTimeFirst.tv_usec = elapsedTimeLast.tv_usec;  
  
  frames_per_second = 1/(timeDiff*0.001);  
  Value.setNum(frames_per_second); 
  frame_rate_label->setText(Value);
}
// show data from haptic device
void ARtools::show_haptic_data(ioc_comm::vecData hapticData)
{
  // show haptic values on corresponding labels
  QString X;QString Y;QString Z;
  QString alpha;QString betha;QString gamma;
  QString Stylusbutton;
  
  if(hapticData.size() > 0)
    {
      ioc_comm::baseData& temp = hapticData[0];
      X.setNum(temp._data[0]);
      Y.setNum(temp._data[1]);
      Z.setNum(temp._data[2]);
      alpha.setNum(temp._data[3]);
      betha.setNum(temp._data[4]);
      gamma.setNum(temp._data[5]);
      Stylusbutton.setNum(temp._data[6]);        
      
    }
}


float ARtools::getTimeDiff()
{
  float timeinMsec;
  timeinMsec = timeDiff;
  return timeinMsec;
}


#include "ARtools.moc"
