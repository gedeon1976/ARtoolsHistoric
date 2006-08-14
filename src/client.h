/* *******************************************************************************
//		RTSP CLIENT

//	Author:		Henry Portilla
//	Date:		june/2006

//	Thanks:		

//	Description: 	this program uses the live555 libraries 
			to make a client to communicate with spook video server

										*/
#ifndef _CLIENT_H_
#define _CLIENT_H_
//********************************************************************************
//	include the libraries used
//********************************************************************************
//	libavcodec libraries
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>				// libavformat and libavcodec libraries
//********************************************************************************
//	live555 libraries
#include "BasicUsageEnvironment.hh" 			//	usage environment for client
#include "liveMedia.hh"			
#include "RTSPClient.hh"				//	RTSP client class
#include "Media.hh"
#include "MediaSession.hh"				//	RTP session management
#include "DelayQueue.hh"				//	time management
#include "RTPSink.hh"					//	to convert to timestanp
//********************************************************************************
//	include coin libraries
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/SoDB.h>				//	classes to see the graphic scene
#include <Inventor/SoSceneManager.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/actions/SoGLRenderAction.h>		//	GL render Action
#include <Inventor/SbViewportRegion.h>

#include <Inventor/SbTime.h>				//	timer
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>			// for texture association
#include <Inventor/nodes/SoComplexity.h>		// set texture quality

#include <Inventor/nodes/SoCoordinate3.h>		// for set the origin coordinates of the background plane
#include <Inventor/nodes/SoNormal.h>			// set texture quality
#include <Inventor/nodes/SoTextureCoordinate2.h>	// texture coordinates set by proggrammer  
#include <Inventor/nodes/SoNormalBinding.h>		// set normal association
#include <Inventor/nodes/SoTextureCoordinateBinding.h>	// set associate texture coordinates 
#include <Inventor/nodes/SoFaceSet.h>			// set of faces 
#include <Inventor/sensors/SoNodeSensor.h>		// detect changes of nodes
#include <Inventor/sensors/SoTimerSensor.h>		// repeat cycles every time

#include <Inventor/nodes/SoCallback.h>			// to use opengl
#include <GL/gl.h>
//*********************************************************************************
//	C/C++ libraries
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cstring>
#include <pthread.h>					//	threads
#include <semaphore.h>
#include <queue>					//	FIFO parametric structure
#include <deque>					//	double FIFO parametric structure
//*********************************************************************************
//	special linux libraries
#include <sys/timex.h>					//	ntp time
//*********************************************************************************				
using namespace std;

#endif
