//
// C++ Interface: augmentedreality
//
// Description: this class allows to  connect
//		to a video server  and show the video
//		in a local machine through a coin node
//
//
// Author:  Henry Portilla (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AUGMENTEDREALITY_H
#define AUGMENTEDREALITY_H

#include "client.h"		//	this class contains the definition of 
				//	the STREAM class used to conect to a remote 
				//	server using RTSP, RTP,RTCP, SDP and other protocols
				//	through the lib555 libraries
#include "SoStereoTexture.h"	//	Custom Inventor node, this node has several 						// 	modifications to support 720*576 video, besides 
				//	it allows a faster drawing of the data

/**
	@author 
*/

class AugmentedReality{
public:
	
    	AugmentedReality(SoSeparator* videoNode, const char *URLcam);
    	~AugmentedReality();

	void playVideo(bool Estado);
	static void update(void *data, SoSensor*);
	static void init();

private:
	IMAGE set_format;	//	set the format to use in the image	
	STREAM camara;		//  	create an stream object
	TFunctor *C1;		//	pointers to abstract class
public:
	SoTimerSensor *timer;	//	timer for get frames under Coin3D 

};

#endif
