
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HELPER CLASSES
//
//	these are helper classes for the rtsp client from live555 examples
//	date:		April/2013
//	author:		Henry Portilla
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// If you're streaming just a single stream (i.e., just from a single URL, once), then you can define and use just a single
// "StreamClientState" structure, as a global variable in your application.  However, because - in this demo application - we're
// showing how to play multiple streams, concurrently, we can't do that.  Instead, we have to have a separate "StreamClientState"
// structure for each "RTSPClient".  To do this, we subclass "RTSPClient", and add a "StreamClientState" field to the subclass:
#pragma once

//      live555 libraries
#include "BasicUsageEnvironment.hh"
#include "liveMedia.hh"
#include "RTSPClient.hh"				//      RTSP client class
#include "MediaSession.hh"
#include "DelayQueue.hh"                //      time management
#include "RTPSink.hh"                   //      to convert to timestamp

#include "StreamClientState.h"



class ourRTSPClient: public RTSPClient {
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
				  int verbosityLevel = 0,
				  char const* applicationName = NULL,
				  portNumBits tunnelOverHTTPPortNum = 0);

protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  StreamClientState scs;
};