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


float ARtools::getTimeDiff()
{
  float timeinMsec;
  timeinMsec = timeDiff;
  return timeinMsec;
}


#include "ARtools.moc"
