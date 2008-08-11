//
// C++ Implementation: augmentedreality
//
// Description: 
//
//
// Author:  Henry Portilla (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "augmentedreality.h"

void AugmentedReality::init()
{
		

}

void AugmentedReality::update(void *data,SoSensor*)
{
	//	this function updates the texture based on the 				//	frame got from the video stream

	
	Export_Frame FrL;		//	struct for save the frame

	SoStereoTexture *Stereo = (SoStereoTexture*)data;

		
	// return a structure that contain the image, size, width and height
	//FrL = C1->Execute();

	if (FrL.pData->data[0]!=NULL)
	{
		//Stereo->imageL.setValue(SbVec2s(720,576),3,FrL.pData->data[0]);
	}else
	{
		printf("%s\n","There aren't image from the buffer");
	}

}

AugmentedReality::AugmentedReality(SoSeparator* videoNode, const char *URLcam)
{
	myfunctor<STREAM> D1(&camara,&STREAM::getImage);	
				//	this is a functor, some as a pointer to 
				//	a member function, here points to the
				//	getImage function of camara object
	C1 = &D1;		//	C1 is a abstract class			
	set_format = LEFT_IMAGE;
	camara.Init_Session(URLcam, set_format);	
				//	start connection with url address
				//	set the format of the image
				//	for the camera 
	
				//	Inventor code
				//	initialize the new node, required
	SoStereoTexture::initClass();

	SoStereoTexture *videoCell = new SoStereoTexture;
	videoCell->width.setValue(720);
	videoCell->heigh.setValue(576);
				//	add StereoNode to main node
	videoNode->addChild(videoCell);
				//	add timer to get each image from buffer
	timer = new SoTimerSensor(update,videoCell);
	timer->setBaseTime(SbTime::getTimeOfDay()); 	//	useconds resolution
	timer->setInterval(1.0/25.0); 			//	interval 40 ms = 25fps
	timer->schedule();				//	enable timer

}

void AugmentedReality::playVideo(bool Estado)
{
	switch(Estado)
	{
		case true:
			timer->schedule();
		break;

		case false:
			timer->unschedule();
		break;
	}


}


	

AugmentedReality::~AugmentedReality()
{
}


