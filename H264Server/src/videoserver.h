/*
 * 
 * 	Server class
 * 
 * 	brief@	this class contains the basic functionality
 * 		to stream a H.264 encoded by hardware input
 * 		using RTSP and RTP/RTCP protocols
 * 
 * 	date		January/2013
 * 	author		Henry Portilla
 * 
 */

#ifndef VIDEOSERVER_H
#define VIDEOSERVER_H

// include signals and slot capacity
#include <QObject>

// LIVE555 LIBRARIES
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>

class videoServer:public QObject{
  Q_OBJECT
  public:
	  videoServer();
	  virtual ~videoServer();
	  void setupServer(int rtspPort);
	  void AddRTPSession(const char *name);
	  ServerMediaSession* getRTPSession(int i);

  public Q_SLOTS: 
    	  bool play(void);
	  bool stop(void);
  private:
      // live555 variables      
      TaskScheduler *scheduler;					// RTSP, RTP scheduler
      UsageEnvironment *env;					// main environment
      H264VideoStreamDiscreteFramer *videoSource;		// video source       
      struct in_addr destinationAddress;
      unsigned short rtpPortNumBase;
      unsigned char ttl;
      RTSPServer *rtspServer;					// RTSP server
};

#endif // SERVER_H
