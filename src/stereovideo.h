/*

    StereoVideo class
    
    Description:  this class allows to connect to 
		  a video server and show the video
		  in a local machine through a coin node
		  
    author:	  Henry Portilla (c) 2010
		  Some code is based on code by Adolfo Rodriguez
		  and Leopoldo Palomo
    
    This code is freely available and is delivered as is
    without warranty
    
    Copyright: See COPYING file that comes with this distribution

*/

#ifndef STEREOVIDEO_H
#define STEREOVIDEO_H

#include <QObject>
// Stereo camera coin node
#include "SoStereoTexture.h"
// Stream class: to connect to the remote video server
// using RTSP, RTP, RTCP, SDP and other protocols
#include "client.h"
// include IOCCOMM communications library
#include "../libcomm/client.h"
// include common types
#include "common.h"
//mt library
#include <mt/mt.h>

/**
    @author
*/
// qt nokia
#include <QTimer>




class StereoVideo:public QObject
{
  Q_OBJECT
  public:
    StereoVideo();
	StereoVideo(int width, int height, SoStereoTexture* node);
    StereoVideo(const char* URLcamL,const char* URLcamR, int width, int height, SoStereoTexture* node);
    ~StereoVideo();
    Export_Frame getImageL();
    Export_Frame getImageR();
    
    void updateDone();
    QTimer *timer;		// timer to properly recovering of the remote images 
    
  private slots:
    void update();
    void set_haptic_data(mt::Vector3);
  signals:
    void updatedone();
    void sendimagepoints(imagePoints actualPoints);
   
  private:
    IMAGE set_format;		// set the left and right cameras
    STREAM CamL;		// create the stream objects
    STREAM CamR;
    
    SoStereoTexture *StereoNode;// a coin node for stereo visualization management
    SoStereoTexture *realNode;	// real stereo node from the main GUI window
    int w,h,nc;			// width, height, number of components of image(RGB color)
     
  
};

#endif // STEREOVIDEO_H
