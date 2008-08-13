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
#include "SoStereoTexture.h"	//	Custom Inventor node, this node has several 						// 	modifications to support until 720*576 video size, 					//	besides it allows a faster drawing of the data
#include <QObject>
/**
	@author 
*/
class QTimer;

class AugmentedReality:public QObject
{
Q_OBJECT
public:
	AugmentedReality();
    	AugmentedReality(SoSeparator* videoNode, const char *URLcam);
    	~AugmentedReality();

	void playVideo(bool Estado);
	Export_Frame getImage();
		
private slots:
	void update();

private:
	IMAGE set_format;	//	set the format to use in the image	
	STREAM camara;		//  	create an stream object
	QTimer *timer;		//	Qt timer
	SoStereoTexture *videoCell;	//	Video Node 
	

};

#endif
