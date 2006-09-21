/* *******************************************************************************
//		RTSP CLIENT

//	Author:		Henry Portilla
//	Date:		june/2006
//	Modified:	sept/06

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
//#include "DelayQueue.hh"				//	time management
//#include "RTPSink.hh"					//	to convert to timestanp
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
#define  RTPDataSize 	70000				//	size of RTP data read
using namespace std;

//	MAIN DATA STRUCTURES
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
typedef dataFrame Frame;		//	define main frames

/**	These class create a interface to get data from a camera through RTSP + RTP protocols
	based on Live555 libraries
*/

class STREAM
{

////////////////////////////////////////////////////////////////////////////////////////////////////
//		 			**VARIABLES**
////////////////////////////////////////////////////////////////////////////////////////////////////
private:
////////////////////////////////////////////////////////////////////////////////////////////////////
//	LIVE555 libraries

int verbosityLevel;						//	these variables are use for create client
const char *name;
portNumBits tunnelOverHTTPPortNum;				

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

char const *URL;
/*
char const *URL0;						//	resources to access
char const *URL1;
char const *URL2;
char const *URL3;
*/
unsigned char *MP4H;
char const *MP4Header;						//	mp4 VOP header?
int MP4Hsize;							//	size of mp4 header
unsigned char *dataRTP;						//	allocate buffer memory
//static int MP4FrameSize;					//	size of frame
unsigned int maxRTPDataSize;					//	length of RTP data to be read
unsigned timestampFreq;						//	timestamp clock
unsigned int SSRC;						//	SSRC identifier
unsigned maxFrameSize;						//	RTP frame size
unsigned actualCSeq;					 	//	actual sequence number
unsigned long actualRTPtimestamp;				//	current actual timestamp
unsigned fps;							//	frame rate
static char readOKFlag;//static					//	flag to indicate RTP reading data
int frameCounter;						//	frame counter
int cam;							//	cameras to read
int ID;								//	camera ID
int delay;
static int ID1;
/////////////////////////////////////////////////////////////////////////////////////////////////////
//	LIBAVCODEC libraries

AVFormatContext	*pFormatCtx;		//	to save the file and its data
int		decodeOK;		//	to access video stream within the file
AVCodecContext	*pCodecCtx;		//	to get the info of codec used in the file
AVCodec		*pCodec;		//	to get thfriend e codec to decode the file
AVFrame		*pFrame;		//	to get raw frame of video in format YUV?
AVFrame		*pFrameCrop;		//	to cut an image
AVFrame		*pFrameRGBA;		//	to convert the frame got to RGB format, native format for opengl
AVPacket	packet;			//	to get packets within the file
int		frameFinished;		//	flag to indicate that the fame has been read
int		numBytes;		//	to get the number of bytes of each frame
uint8_t		*buffer;		//	to save the data of each frame
unsigned char	*image;			//	to save the RGB data to load in the texture
////////////////////////////////////////////////////////////////////////////////////////////////////
//	PTHREAD library

pthread_mutex_t mutexBuffer,mutexFrame;	//	mutex to syncronize the access to the data buffer 
sem_t sem;				//	semaphores
pthread_t camera;			//	threads ID
//pthread_cond_t cond;			//	threads conditional variables

/////////////////////////////////////////////////////////friend ///////////////////////////////////////////
//	TIME STRUCTURES with microseconds resolution
struct timeval t,t1,t2;
struct ntptimeval ntpTime;
struct timezone tz;

//	STRUCTURES FOR FRAMES  : save the compresed and uncompressed frames 

////////////////////////////////////////////////////////////////////////////////////////////////////
//	buffers for saving frames
Frame dataBuffer;
Frame data_RTP;
Frame ReceivedFrame;
Frame Temp;
////////////////////////////////////////////////////////////////////////////////////////////////////
//	STL FIFOs
deque<Frame>InputBuffer;		//	input FIFO queue, here used as the main buffer
//queue<dataFrame>ShowBuffer;	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 					METHODS

int initCodecs();			// 	Init libavcodec library used to decode the stream of data
int rtsp_Init();			//	Init the connection with a RTSP server, besides these sends the different RTSP commands
					//	to configure the transmision
int rtsp_Close();			//	Close the current RTSP connection 
int rtsp_getFrame();			//	Get a frame from the RTSP video server (here Spook www.litech.org program is used)
int rtsp_decode(Frame dataCompress);
					//	Decodes a compressed frame and convert to RGB format
int rtsp_Buffering(int n);		//	Buffer for get n frames at Start of reception, used to smooth the appareance of frames


static timeval timeNow();		//	To save time of arrival of the frames
int timeNow2();				//	Show the actual time using getoftimeday with microseconds resolution
double skew(Frame N, Frame N_1);	//	Calculates skew between sender and receiver, See Colin Perkins Book on RTP for meaning

					//	these two function are used to save the frame in a structure
					//	these are use so because the use afterReading is an static function
					//	and the way to access the non static data is instancing an object
					//	assigning their this pointer to the actual object, by that the use of
					//	temp object
void SaveFrame(void* clienData, unsigned framesize);
void ProcessFrame(void *clientData, unsigned framesize);
//THESE function are used with live555 GetNextFrame function
//************************************************************************************************
static void afterReading(void *clientData,unsigned framesize,unsigned numTruncatedBytes,
				struct timeval presentationTime,unsigned durationInMicroseconds);	
					//	to save data of the last frame obtained
static void onClose(void *clientData);

//************************************************************************************************

int create_Thread();			//	create a thread
void rtsp_getData();			//	thread function
void init_Semaphore(int i);		//	init the semaphore
int init_mutex();			//	init the mutex
int lock_mutex();			//	lock the mutex
int unlock_mutex();			//	unlock the mutex

static int members;			//	number of cameras
void bind_object();		//	assign correct object
//void set_Frameflag();
//void clear_Frameflag();
int get_ID();
void set_ID();

public:
static void *temp;			//	Aux object to allow calls from static methods here
//	constructor
STREAM();

//	destructor
~STREAM();

//unsigned char*
static unsigned char* callImage();
unsigned char* getImage();		//	get the last frame available from the FIFO Buffer

int Init_Session(char const *URL,int ID);//	Setup the connection 
static void *Entry_Point(void*);	//	to make thread function, Create a thread to get data from the RTSP Server

void set_Semaphore();			//	set the class semaphore
//int down_Semaphore();			//	decrease the semaphore
void wait_Semaphore();			//	wait for semaphore 
};




#endif
