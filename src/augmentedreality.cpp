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
#include <augmentedreality.moc>

#include <QTimer>

AugmentedReality::AugmentedReality()
{
}

void AugmentedReality::update()
{
	// this function updates the texture based on the 				//	frame got from the video stream

	
	Export_Frame FrL;		//	struct for save the frame
	
	// return a structure that contain the image, size, width and height
	FrL = getImage();

	if (FrL.pData->data[0]!=NULL)
	{
		videoCell->imageL.setValue(SbVec2s(videoCell->w,videoCell->h),3,FrL.pData->data[0]);
		cout<<"function update"<<endl;
	}else
	{
		printf("%s\n","There aren't image from the buffer");
	}

}

AugmentedReality::AugmentedReality(SoSeparator* videoNode, const char *URLcam)
{
	set_format = LEFT_IMAGE;
	camara.Init_Session(URLcam, set_format);	
				//	start connection with url address
				//	set the format of the image
				//	for the camera 
	
				//	Inventor code
				//	initialize the new node, required
	SoStereoTexture::initClass();

	videoCell = new SoStereoTexture;
	videoCell->width.setValue(720);
	videoCell->heigh.setValue(576);
	//videoCell->w = 720;
	//videoCell->h = 576;
				//	add StereoNode to main node
	videoNode->addChild(videoCell);
				//	add timer to get each image from buffer


	timer = new QTimer(this);
     	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
     	timer->start(40);


}

void AugmentedReality::playVideo(bool Estado)
{
	switch(Estado)
	{
		case true:
			timer->start();
		break;

		case false:
			timer->stop();
		break;
	}

}

Export_Frame AugmentedReality::getImage()
{
	Export_Frame Image;
	Image = camara.getImage();
	return Image;

}


AugmentedReality::~AugmentedReality()
{
}


