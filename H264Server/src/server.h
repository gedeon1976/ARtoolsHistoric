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

#ifndef SERVER_H
#define SERVER_H

// include signals and slot capacity
#include <QObject>

// LIVE555 LIBRARIES
#include "blueCherryCard.h"

class Server:public QObject{
  Q_OBJECT
  public:
	  Server(void);
	  virtual ~Server();
  public Q_SLOTS:
	  void setupServer(int rtspPort);
	  void AddRTPSession(const char *name);
	  ServerMediaSession* getRTPSession(int i);
	  bool play(void);
	  bool stop(void);
    
  private:
      // live555 variables
      UsageEnvironment *env;					// main environment
      H264VideoStreamDiscreteFramer *videoSource;		// video source 
      TaskScheduler *scheduler;					// RTSP, RTP scheduler
      struct in_addr destinationAddress;
      unsigned short rtpPortNumBase;
      unsigned char ttl;
      RTSPServer *rtspServer;					// RTSP server
};

#endif // SERVER_H
