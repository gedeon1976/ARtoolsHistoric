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
//	verify the range of the different types of variables
#include <limits>


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
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/SoInput.h>				//	to open iv files
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/Qt/viewers/SoQtViewer.h>
#include <Inventor/SoDB.h>				//	classes to see the graphic scene
#include <Inventor/fields/SoSFFloat.h>			// 	contains a float point value
#include <Inventor/SoSceneManager.h>
#include <Inventor/nodes/SoSwitch.h>			//	switch to test if rendering is the bottleneck
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/actions/SoGLRenderAction.h>		//	GL render Action
#include <Inventor/SbViewportRegion.h>

#include <Inventor/SbTime.h>				//	timer
#include <Inventor/nodes/SoFont.h>			//	fonts
#include <Inventor/nodes/SoText2.h>			//	texts
#include <Inventor/nodes/SoAsciiText.h>			//	ascii text node used for fps texts
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTransformation.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransparencyType.h>		//  Only coin compliant, not in the original SGI version
							//  this allows several kinds of tranparencies in the same scene	
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



//#include <GL/gl.h>					
//#include <GL/glext.h>					//GL extensions

				
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
#include <iostream>
//*********************************************************************************
//	special linux libraries
#include <sys/timex.h>					//	ntp time
#include "time.h"
//*********************************************************************************		
#define  RTPDataSize 	70000				//	size of
// RTP data read
using namespace std;

enum IMAGE {LEFT_IMAGE=0,RIGHT_IMAGE=1};


//	MAIN DATA STRUCTURES
struct dataFrame{
	
	unsigned long timestamp;
	//unsigned char data[]="";	//	compressed data
	//unsigned char *data;
	unsigned char data[70000];

	int size;			//	size of compressed frame
	unsigned char *image;		//	data decoded
	struct timeval time;		//	save the time with good resolution (microseconds)
	//	spook uses unsigned int for these times
	//	SR times useful for synchronization
	unsigned long NTPmsw;		//	NTP time in seconds since 1/1/1900 UNIX time
	unsigned long NTPlsw;		//	NTP time in fraction of seconds
	//	synchronization variables
	unsigned long Msr;		//	last SR timestamp
	double Ts;			//	save the TS mapped time, see pag 217 perkins
	double Tm;			//	to map Trtp to local reference clock
	double Dstream;			//      save Tm -Ts

	unsigned long lastNTPmsw,lastNTPlsw; //      used to compare SR NTP times in development phase

	long timeArrival;		//	time of arrival of frame
	long playoutTime;		//	time of render
	int width;			
	int height;
	int index;			//	save number of the acquired frame
	AVFrame *pFrame;		//	to export a frame
};
struct ExportFrame{

	AVFrame *pData;
	int index;
	int h;
	int w;
};

typedef dataFrame Frame;			//	define main frames
typedef ExportFrame Export_Frame;		//	export to Ingrid Program

//	callback functions
typedef void (afterReading)(void *clientData,unsigned framesize,unsigned numTruncatedBytes,
				struct timeval presentationTime,unsigned durationInMicroseconds);
typedef void (Close)(void *clientData);	
typedef void Check(void *clientData);
	

// timeval structure : used for get the time of arrival of frames
typedef timeval TIME;
//	OPENGL variables
//static GLuint texName;			//	to assign textures names in openGL

//GLuint bufferID;				//	PBO (pixel_buffer_object) name	
//	OPENGL EXTENSIONS       
/*
//	define procedures for PBO according to glext.h
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;	// define a glGenBufferARB according to opengl Extensions procedures
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;	// define an association function for the PBO (pixel buffer object)
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;	// define load of data procedure
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;// delete the object
PFNGLMAPBUFFERARBPROC glMapBufferARB = NULL; 	// pointer to memory of the PBO
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB = NULL;// releases the mapping
*/
///////////////////////////////////////////////////////////////////////////
//	
class TFunctor				//	abstract class
{
public:

//TFunctor();
virtual ~TFunctor()
{
}
					//	virtual functions to call function member 
virtual Export_Frame Execute()=0;	// getImage
virtual double getDiffStream()=0;	// getDiffStream

};


template<class TStream> 
class myfunctor :public TFunctor
{
private:

Export_Frame (TStream::*method)();		//    method to call 
						//    this is a pointer to a member function
						//    pointer to object

double (TStream::*method2)();			//    to call the getDiffStream method							
TStream* clase;					
//method m;

public:
//	constructor
myfunctor(TStream* _p2object,Export_Frame (TStream::*m)())
{
	clase = _p2object;
	method = m;
}
// constructor no 2
myfunctor(TStream* _p2object, double (TStream::*m2)())
{
	clase = _p2object;
	method2 =m2;
}
~myfunctor()
{
}

//	override ProcessFrame function
virtual Export_Frame Execute()
{
	Export_Frame t;
	t = (*clase.*method)();			//secure way for calling the corresponding method according to the papers found
	return t;
}
//	override GetDiffStream function
virtual double getDiffStream()
{

	double tdiff;
	tdiff = (*clase.*method2)();
	return tdiff;
	
}


};
///////////////////////////////////////////////////////////////////////
//	callback functions in C++
class TFunctorClose			//	abstract class
{
public:
//typedef void (onclose)(void* clientData);
virtual void operator()(void* clientData)=0;	//	function to call using () operator
virtual void method(void* clientData)=0;	//	method to call
//virtual void* method2(void* clientData)=0;	//      method to call but with void* as return
//TFunctorClose();

virtual ~TFunctorClose()
{ }

};


template<class TStream> 
class closeFunctor:public TFunctorClose	//	derived class
{
public:
typedef void (TStream::*Constant_method)(void* clientData);
//typedef void* (TStream::*Constant_method2)(void* clientData);

private:
TStream* T;				//	save class
Constant_method  method_k;		//	save method to access from the early class
//Constant_method2 method_c;		//	save method to access from earlier class
//void (TStream::*fpt)(void*);		//	pointer to member function
//TStream *p2object;			//	pointer to object

public:

//	constructor
closeFunctor(){
	T = 0;				//	start pointers
	method_k=0;
	//method_c=0;
}
~closeFunctor(){}
//	methods
void setClass(TStream* IncomeClass)	//	assign kind of class to use
{
	T = IncomeClass;
}

void setMethod(Constant_method method_to_call)
{
	method_k = method_to_call;	//	set the methos to call
}
/*
void* setMethod(Constant_method2 method_to_call)
{
	method_c = method_to_call;	//	set the methos to call
}*/

//protected:

/*void* method2(void* clientData)
{
	(T->*method_c)(clientData);
}*/


void method(void* clientData)
{
	(T->*method_k)(clientData);
}

void operator()(void* clientData)
{
	(T->*method_k)(clientData);
}

};


////////////////////////////////////////////////////////////////////////////////////////////////////
//************** THIS IS THE MAIN CLASS FOR ACCESS THE STREAM FROM AN URL ADDRESS ******************
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
unsigned char dataRTP[70000];					//	TESTING
//unsigned char dataRTP[]="  ";
//unsigned char *dataRTP;						//	allocate buffer memory, this is the main reception buffer in the software
//static int MP4FrameSize;					//	size of frame
unsigned int maxRTPDataSize;					//	length of RTP data to be read
unsigned timestampFreq;						//	timestamp clock
unsigned int SSRC;						//	SSRC identifier
unsigned maxFrameSize;						//	RTP frame size
unsigned actualCSeq;					 	//	actual sequence number
unsigned long actualRTPtimestamp;				//	current actual timestamp
unsigned fps;							//	frame rate
char readOKFlag;//static					//	flag to indicate RTP reading data
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
AVCodec		*pCodec;		//	to get codec to decode the file
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

sem_t Sem1,Sem2;			//	semaphores
pthread_t camera;			//	threads IDs
pthread_t render;

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

//static
timeval timeNow();		//	To save time of arrival of the frames
int timeNow2();				//	Show the actual time using getoftimeday with microseconds resolution
  double skew(Frame N, Frame N_1);	//	Calculates skew between sender and receiver, See Colin Perkins Book on RTP for meaning

					//	these two function are used to save the frame in a structure
					//	these are use so because the use afterReading is an static function
					//	and the way to access the non static data is instancing an object
					//	assigning their this pointer to the actual object, by that the use of
					//	temp object
void SaveFrame(void* clienData, unsigned framesize);
void ProcessFrame(unsigned framesize, TIME presentationTime);//void *clientData
//THESE function are used with live555 GetNextFrame function
//************************************************************************************************
//	pointer to function
Check *onCheckFunc;			// it is used to wait in the sockets for data
Close* onClosing;			// on close function pointer	
afterReading* onRead;		 	// function pointer for process the frame after its capture 	
//************************************************************************************************

int create_Thread();			//	create a thread
int join_Thread();			//	wait until a thread finish
int cancel_Thread();			//	cancel a thread
int get_ThreadPriority();
void rtsp_getData();			//	thread function
void init_Semaphore(int sem, int i);	//	init the semaphore
int init_mutex();			//	init the mutex
int lock_mutex();			//	lock the mutex
int unlock_mutex();			//	unlock the mutex
int m_global_flag;			//	flag to comtrol semaphores start
int NTP_flag;				//	flag to control NTP time readings
//static int members;			//	number of cameras
STREAM *bind_object();			//	assign correct object

int get_ID();
void set_ID();

public:

void *temp;
//	constructor
STREAM();

//	destructor
~STREAM();

Export_Frame getImage();		//	get the last frame available from the FIFO Buffer
double getDiffStream();			//      get the relative difference between local and remote mapping
int Init_Session(char const *URL,int ID);//	Setup the connection 
static void *Entry_Point(void*);	//	to make a thread function, Create a thread to get data from the RTSP Server

void set_Semaphore(int sem);		//	set the semaphore signal
//int down_Semaphore();			//	decrease the semaphore
void wait_Semaphore(int sem);	//	wait and lock the semaphore 

//	
void checkFunc(void *clientData);	//	to check flag of received frames from live555
					//	taskScheduler.ScheduleDelayedTask()
void onClose(void *clientData);		//	on close after reading frame
void afterR(void *clientData,unsigned framesize,unsigned numTruncatedBytes,
				struct timeval presentationTime,unsigned durationInMicroseconds);	
};


#endif
