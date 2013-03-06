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
#include "blueCherrySource.h"

// typedefs
//typedef void (afterPlay)(void* clientData);

// auxiliar classes for live555 access

///////////////////////////////////////////////////////////////////////
//      callback functions in C++
class TFunctorPlay   		//      abstract class
{
public:
    // method to call but with void* as return
    virtual void call(int i)=0;
};

//   derived class
template<class TStream>
class playFunctor:public TFunctorPlay 
{
private:
    TStream* pt2object; 	// pointer to object    
    void (TStream::*fpt)(int i);// pointer to member function
        
public:

    //	constructor
    playFunctor(TStream *_pt2object, void (TStream::*_fpt)(int i))
    {
	pt2object = _pt2object;
	fpt = _fpt;       
    }
       
    // 	methods
    // override function call
    void call(int i)
    {
	(*pt2object->*fpt)(i);	// execute member function
    }

};

// H264 class declaration
class H264_rtspServer:public QObject{
      Q_OBJECT
    
	    
      public:
	    H264_rtspServer();
	    virtual ~H264_rtspServer();
      public Q_SLOTS:
	    void setName(std::string name);
	    void setPort(int Port);
	    void setID(int StreamID);
	    void getFrames();
	    void getEncodedFrames(H264Frame encodedFrame);
	    void AddRTSPSession(void);
	    void play(int i);
	    void stopPlay();
	    static void afterPlaying(void* dataClient);
	    static void wrapperToCallPlay(void *pt2object, int i);
	    int create_Thread(void);
       Q_SIGNALS:
	    void sendToRTSP(H264Frame NAL_data);
      private:
	    // threads code	    
	    int cancel_Thread(void);
	    static void* Entry_Point(void *pthis);
	    int get_ThreadPriority(void);
	    void init_semaphore(int sem, int value);	
	    void set_semaphore(int sem);			
	    void wait_semaphore(int sem);	
	    
      private:
	// flow control
	bool sessionStarted;
	bool nextStreamNAL;
	bool isEventLoop;
	char playOKFlag;
	int videoGeneralIndex;
	int frameCounter;
	char readOKFlag;
	typedef std::deque<AVPacket> codedFrameBuffer;
	std::deque<codedFrameBuffer> cameraCodedBufferList;
	// live555 variables      
	TaskScheduler *scheduler;			/// RTSP, RTP scheduler
	UsageEnvironment *env;				/// main environment
	H264VideoStreamDiscreteFramer *videoSource;	/// video source  
	BlueCherrySource *cameraSource;			
	dataForRTSP *dataforStream;
	struct in_addr destinationAddress;
	unsigned short rtpPortNumBase;
	unsigned char ttl;
	bool isRTSPServerStarted;
	RTSPServer *rtspServer;				/// RTSP server
	int rtspPort;
	RTPSink* videoSink;
	CamParameters NALparams;
	BlueCherrySource *NAL_Source;
	const char* newStreamName;
	const char* inputFileName;
	char const* inputFileNameTest;
	int functionCallcounter;
	// threads code
	int ID;						/// video input ID
	std::string StreamName;
	pthread_t StreamOut;				/// Threads code
	sem_t Sem1,Sem2;				/// flow semaphore control
	int semaphores_global_flag;			/// flag to control semaphores start
	
};
#endif