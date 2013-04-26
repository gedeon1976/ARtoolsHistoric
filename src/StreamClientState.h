
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HELPER CLASSES
//
//	these are helper classes for the rtsp client from live555 examples
//	date:		April/2013
//	author:		Henry Portilla
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

//      live555 libraries
#include "BasicUsageEnvironment.hh"
#include "liveMedia.hh"
#include "RTSPClient.hh"				//      RTSP client class
#include "MediaSession.hh"
#include "DelayQueue.hh"                //      time management
#include "RTPSink.hh"                   //      to convert to timestamp

class StreamClientState {
public:
  StreamClientState();
  virtual ~StreamClientState();

public:
  MediaSubsessionIterator* iter;
  MediaSession* session;
  MediaSubsession* subsession;
  TaskToken streamTimerTask;
  double duration;
};
