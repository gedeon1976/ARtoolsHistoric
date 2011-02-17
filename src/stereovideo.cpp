/*

    StereoVideo class
    
    Description:  this class allows to connect to 
		  a video server and show the video
		  in a local machine through a coin node
		  
    author:	  Henry Portilla (c) 2010
		  Some code is based on code by Adolfo Rodriguez
		  and Leopoldo Palomo
    
    This code is freely available and is delivered as is
    without any warranty
    
    Copyright: See COPYING file that comes with this distribution

*/

#include "stereovideo.h"

StereoVideo::StereoVideo()
{
  
}

StereoVideo::StereoVideo(const char* URLcamL,const char* URLcamR, int width, int height, SoStereoTexture *node)
{
  // initializes variables
  StereoNode = new SoStereoTexture;  
  StereoNode->width = width;
  StereoNode->heigh = height;
  realNode = node;
  w = width;			// video size
  h = height;
  nc = 3;			// component number, here we use 3(RGB)
  set_format = LEFT_IMAGE;
  CamL.Init_Session(URLcamL,set_format,width,height);
  set_format = RIGHT_IMAGE;
  CamR.Init_Session(URLcamR,set_format,width,height);
  // timer for update the image every 40 ms = 25fps
  timer = new QTimer(this);
  timer->setInterval(40);
  timer->start();  
  connect(timer,SIGNAL(timeout()),this,SLOT(update())); 
    
}

StereoVideo::~StereoVideo()
{
  CamL.close_Session();
  CamR.close_Session();
}

/// this method grab an image from the remote site
Export_Frame StereoVideo::getImageL()
{
  Export_Frame Image;
  Image = CamL.getImage();
  return Image;
}

/// this method grab an image from the remote site
Export_Frame StereoVideo::getImageR()
{
  Export_Frame Image;
  Image = CamR.getImage();
  return Image;
}



/// this method allow the updating of the StereoNode node images
void StereoVideo::update()
{
   
      
    // this function updates the textures based on the frames got from the video streams
    Export_Frame FrL;		// struct for save frame
    Export_Frame FrR;
    
    
    FrL = getImageL();		// return an structure that contains image, frame size, width and height
    if (FrL.pData->data[0]!= NULL)	// check if the input buffer is empty
    {
      //StereoNode->imageL.setValue(SbVec2s(w,h),nc,FrL.pData->data[0]);
      realNode->imageL.setValue(SbVec2s(w,h),nc,FrL.pData->data[0]); 
     }
    
    FrR = getImageR();		// return an structure that contains image, frame size, width and height
    if (FrR.pData->data[0]!= NULL)	// check if the input buffer is empty
    {
      
      //StereoNode->imageR.setValue(SbVec2s(w,h),nc,FrR.pData->data[0]);
      realNode->imageR.setValue(SbVec2s(w,h),nc,FrR.pData->data[0]);      
    }
    // emit signal to indicate stereo updating is ready
     emit updatedone(); 
}

void StereoVideo::set_haptic_data(ioc_comm::vecData hapticData)
{
    // set the haptic values to the StereoNode
    if(hapticData.size() > 0)
      {
	ioc_comm::baseData& temp = hapticData[0];
	realNode->X_haptic = temp._data[0];
	realNode->Y_haptic = temp._data[1];
	realNode->Z_haptic = temp._data[2];
      }
}


#include "stereovideo.moc"

