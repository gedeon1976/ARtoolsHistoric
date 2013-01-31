/*
 * 
 * 	H264_rtspServer class
 * 
 * 	brief@	this class serves to stream a video and audio flow
			from a Bluecherry H.264/AVC 16 ports card to the Internet
 * 
 * 	date		January/2013
 * 	author		Henry Portilla
 * 
 */
#ifndef H264_RTSPSERVER_H
#define H264_RTSPSERVER_H

// bluecherry Class
#include "blueCherryCard.h"

// class declaration
class H264_rtspServer:public QObject{
      Q_OBJECT
      public:
	    H264_rtspServer();
	    virtual ~H264_rtspServer();
      public Q_SLOTS:
	    void setPort(int Port);
	    void getEncodedFrames(H264Frame encodedFrame);
	    void AddRTSPSession(const char *videoName, int i);
	    void play(int i);
	    void afterPlaying(void*);
      private:
	    void init_semaphore(int sem, int value);	
	    void set_semaphore(int sem);			
	    void wait_semaphore(int sem);	
	    
      private:
	// flow control
	int videoGeneralIndex;
	sem_t Sem1,Sem2;						// flow semaphore control
	typedef std::deque<AVPacket> codedFrameBuffer;
	std::deque<codedFrameBuffer> cameraCodedBufferList;
	// live555 variables      
	TaskScheduler *scheduler;					// RTSP, RTP scheduler
	UsageEnvironment *env;						// main environment
	H264VideoStreamDiscreteFramer *videoSource;			// video source      
	struct in_addr destinationAddress;
	unsigned short rtpPortNumBase;
	unsigned char ttl;
	bool isRTSPServerStarted;
	RTSPServer *rtspServer;						// RTSP server
	int rtspPort;
	RTPSink* videoSink;
	const char* newStreamName;
	const char* inputFileName;
	char const* inputFileNameTest;
};
#endif