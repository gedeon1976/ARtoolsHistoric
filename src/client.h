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
#include <Inventor/nodes/SoGroup.h>
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
#include "time.h"
//*********************************************************************************
enum views
{
	L=0,R=1						//	define views L = left, R = right
};
//*********************************************************************************		
#define  RTPDataSize 	70000				//	size of RTP data read
using namespace std;



/**	These class create a interface to get data from a camera through RTSP + RTP protocols
	based on Live555 libraries
*/
//class cliente
//{

////////////////////////////////////////////////////////////////////////////////////////////////////
//		 			**VARIABLES**
////////////////////////////////////////////////////////////////////////////////////////////////////
//	live variables
//public:

int verbosityLevel=0;
//const char *name;
portNumBits tunnelOverHTTPPortNum=0;

UsageEnvironment *env; 						//	environment to manage the RTSP session
RTSPClient *client;
MediaSession *Session;						//	session parent
MediaSubsession *Subsession;
MediaSubsessionIterator *iter;

//int   RTPport = 0;						//	RTP port
//char *RTCPport= "54001";					//	RTCP port
char *getOptions;						//	OPTIONS from server
char *getDescription;						//	SDP description of rtsp presentation

Boolean SetupResponse;
Boolean SetupSend;
Boolean conf;

char const *URL0 ="rtsp://sonar:7070/cam0";			//	resources to access
char const *URL1 ="rtsp://sonar:7070/cam1";
char const *URL2 ="rtsp://sonar:7070/cam2";
char const *URL3 ="rtsp://sonar:7070/cam3";

//views LEFT=L;							//	define left and righ views
//views RIGHT=R;

unsigned char *MP4H;
char const *MP4Header;						//	mp4 VOP header?
static int MP4Hsize;						//	size of mp4 header
unsigned char *dataRTP;						//	allocate buffer memory
//static int MP4FrameSize;					//	size of frame
unsigned int maxRTPDataSize = RTPDataSize;			//	length of RTP data to be read
unsigned timestampFreq;						//	timestamp clock
unsigned int SSRC;						//	SSRC identifier
unsigned maxFrameSize;						//	RTP frame size
unsigned actualCSeq;						//	actual sequence number
unsigned long actualRTPtimestamp;				//	current actual timestamp
unsigned fps;							//	frame rate
static char readOKFlag;					//	flag to indicate RTP reading data

//unsigned *DataReception  = new unsigned[2000000];		//	allocate bufer for frames from server
static int frameCounter;					//	frame counter
static int cam;							//	cameras to read
int ID;								//	camera ID

//static int FirstTime = 0;
//static int FirstExec = 0;
static int delay;
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	libavcodec libraries
AVFormatContext	*pFormatCtx;		//	to save the file and its data
int		decodeOK;	//	to access video stream within the file
AVCodecContext	*pCodecCtx;		//	to get the info of codec used in the file
AVCodec		*pCodec;		//	to get the codec to decode the file
AVFrame		*pFrame;		//	to get raw frame of video in format YUV?
AVFrame		*pFrameRGBA;		//	to convert the frame got to RGB format, native format for opengl
AVPacket	packet;			//	to get packets within the file
int		frameFinished;		//	flag to indicate that the fame has been read
int		numBytes;		//	to get the number of bytes of each frame
uint8_t		*buffer;		//	to save the data of each frame
unsigned char	*image;			//	to save the RGB data to load in the texture
unsigned char   *image2;		//	auxiliary image
////////////////////////////////////////////////////////////////////////////////////////////////////
//	pthread library
/* mutex to syncronize the access to the data buffer */
pthread_mutex_t mutexBuffer;
pthread_t camera[3];			//	threads ID
pthread_cond_t cond[3];			//	threads conditional variables
sem_t sem;				//	semaphores
////////////////////////////////////////////////////////////////////////////////////////////////////
//	time structures with microseconds resolution
struct timeval t,t1,t2;
struct ntptimeval ntpTime;
struct timezone tz;
//	structure for save the frames compresed and uncompressed
struct dataFrame{
	
	unsigned long timestamp;
	unsigned char *data;		//	compressed data
	int size;			//	size of compressed frame
	unsigned char *image;		//	data decoded
	struct timeval time;		//	save the time with good resolution (microseconds)
	long timeArrival;		//	time of arrival of frame
	long playoutTime;		//	time of render
	int width;			
	int height;
	int index;			//	save number of the acquired frame
};
////////////////////////////////////////////////////////////////////////////////////////////////////
//	buffers for saving frames
dataFrame dataBuffer;
dataFrame data_RTP;
dataFrame ReceivedFrame;
dataFrame Temp;
////////////////////////////////////////////////////////////////////////////////////////////////////
//	STL FIFOs
deque<dataFrame>InputBuffer;		//	input queue
queue<dataFrame>ShowBuffer;		//	showing queue

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 					METHODS
/*
public:
//	constructor
cliente()
{
}
//	destructor
~cliente()
{
}

int initCodecs();			/// 	Init libavcodec library used to decode the stream of data
int rtsp_Init(int CamNumber);		///	Init the connection with a RTSP server, besides these sends the different RTSP commands
						///	to configure the transmision
int rtsp_Close();			///	Close the current RTSP connection 
int rtsp_getFrame();			///	Get a frame from the RTSP video server (here Spook is used)
int rtsp_decode(dataFrame dataCompress);
						///	Decodes a compressed frame and convert to RGB format
int rtsp_Buffering(int n);		///	Buffer for get n frames at Start of reception, used to smooth the appareance of frames
void* rtsp_getData(void *arg);			///	Create a thread to get data from the RTSP Server

timeval timeNow();			///	To save time of arrival of the frames
int timeNow2();				///	Show the actual time using getoftimeday
double skew(dataFrame N, dataFrame N_1);///	Calculates skew between sender and receiver

static void afterReading(void *clientData,unsigned framesize,unsigned numTruncatedBytes,
				struct timeval presentationTime,unsigned durationInMicroseconds);		
					///	to save data of the last frame obtained
static void onClose(void *clientData);

};
*/



#endif
