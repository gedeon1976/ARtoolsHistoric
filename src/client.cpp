/*  ********************************************************************************************************
//			RTSP CLIENT PROGRAM

//	Author:		Henry Portilla
//	Date:		june/2006
//	modified:	july/2006
//	Thanks

//	Description:	This program use the live555 libraries for make a RTSP
			client
										*/
// RFC 2326 RTSP protocol

/** M�odos soportados: OPTIONS
// Options permite obtener los m�odos soportados por el servidor de video
// DESCRIBE: Describe el tipo de videostream que se est�transmitiendo( tipo codec, protocolo de transmisi�(RTP),...)
//SETUP: Configura los puertos tanto en el cliente como en el servidor para efectuar la comunicaci� para efectuar la transmisi�
//TEARDOWN: Termina la sesi�
//PLAY:Reproducci� del video
//PAUSE:Pause del video
*/
//************************************************************************************************************
//	includes header files

#include "client.h"	
#include "time.h"					
//*********************************************************************************
#define  RTPDataSize 	60000					//	size of RTP data read
//************************************************************************************************************
//	variables
//********************************************************************
//	live variables
int verbosityLevel=0;
const char *name ="RTSP";
portNumBits tunnelOverHTTPPortNum=0;
UsageEnvironment *env; 						//	environment to manage the RTSP session
RTSPClient *client;
MediaSession *Session;						//	session parent
MediaSubsession *Subsession;
MediaSubsessionIterator *iter;					//	subsession iterator
Timeval *timeArrival;	
RTPSink *converter=0;						//	time control

char const *URL0 ="rtsp://sonar:7070/cam0";			//	resources to access
char const *URL1 ="rtsp://sonar:7070/cam1";
char const *URL2 ="rtsp://sonar:7070/cam2";
char const *URL3 ="rtsp://sonar:7070/cam3";
int   RTPport = 0;						//	RTP port
char *RTCPport= "54001";					//	RTCP port
char *getOptions;						//	OPTIONS from server
char *getDescription;						//	sdp description of rtsp presentation

Boolean SetupResponse;
Boolean SetupSend;
Boolean conf;

unsigned char *MP4H;
char const *MP4Header;						//	mp4 VOP header?
static int MP4Hsize;						//	size of mp4 header
//static int MP4FrameSize;					//	size of frame
unsigned int maxRTPDataSize = RTPDataSize;			//	length of RTP data to be read
int   CSeq;							//	CSeq number
int ReceptionBufferSize;					//	size of buffer reception
//unsigned *DataReception;
unsigned timestampFreq;						//	timestamp clock
unsigned int SSRC;							//	SSRC identifier
unsigned maxFrameSize;						//	RTP frame size
unsigned actualCSeq;						//	actual sequence number
unsigned long actualRTPtimestamp;				//	current actual timestamp
unsigned fps;							//	frame rate
static char	readOKFlag;					//	flag to indicate RTP reading data
unsigned char *dataRTP;
//	allocate buffer memory
unsigned *DataReception  = new unsigned[2000000];		//	allocate bufer for frames from server
static int frameCounter=0;					//	frame counter

static int counter=10;
static int updates=0;
static int FirstTime = 0;
static int FirstExec = 0;
static int delay;
//*********************************************************************
//	libavcodec libraries
AVFormatContext	*pFormatCtx;		//	to save the file and its data
int		k,videoStream,decodeOK;	//	to access video stream within the file
AVCodecContext	*pCodecCtx=NULL;	//	to get the info of codec used in the file
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

pthread_t camera[3];		//	threads ID
pthread_cond_t cond[3];		//	threads conditional variables
sem_t sem;

static int flagShow = -1;	//	flag to indicate shown data
static int flagData =  1;	//	flag to indicate data has been got
////////////////////////////////////////////////////////////////////////////////////////////////////
//	structure for save the frames compresed and uncompressed
typedef struct dataFrame{
	
	unsigned char *data;		
	unsigned char *image;
	int size;			
	unsigned long timestamp;
	struct timeval time;
	long timeArrival;
	int width;
	int height;	
};
////////////////////////////////////////////////////////////////////////////////////////////////////
//	time structures with microseconds resolution
struct timeval t,t1,t2;
struct ntptimeval ntpTime;
struct timezone tz;
////////////////////////////////////////////////////////////////////////////////////////////////////
//	buffers for saving frames
dataFrame dataBuffer;
dataFrame data_RTP;
dataFrame ReceivedFrame;
unsigned char *DecodedFrame[100];
//dataFrame InputBuffer[8];	//	to save input compressed frames 
////////////////////////////////////////////////////////////////////////////////////////////////////
//	STL FIFOs
//deque<dataFrame>IB;
//queue<dataFrame>InputBuffer;		
deque<dataFrame>InputBuffer;		//	input queue
queue<dataFrame>ShowBuffer;		//	showing queue
queue<unsigned char*>SB;
queue<int>T;
////////////////////////////////////////////////////////////////////////////////////////////////////
//	init libavcodec
int init_codecs(void)
{
//	initialize the codecs
	av_register_all();

//	Find codec for decode the video, here the MP4 codec
	pCodec =avcodec_find_decoder(CODEC_ID_MPEG4);
	if(pCodec==NULL)
		{
		printf("%s\n","codec not found");
		return -1;	//	codec not found
		}
//	allocate memory for AVodecCtx

	pCodecCtx = avcodec_alloc_context();
//	initialize width and height by now
	pCodecCtx->width=768;
	pCodecCtx->height=576;
	pCodecCtx->bit_rate = 1000000;

//	look for truncated bitstreams

//	if(pCodec->capabilities&&CODEC_CAP_TRUNCATED)
//	{
//		pCodecCtx->flags|= CODEC_FLAG_TRUNCATED;  /* we dont send complete frames */
//	}
	
//	open the codec
	if(avcodec_open(pCodecCtx,pCodec)<0)
		{
		printf("%s\n","cannot open the codec");
		return -1;	//	cannot open the codec
		}
//	allocate memory to the save a raw video frame
	pFrame = avcodec_alloc_frame();

//	allocate memory to save a RGB frame
	pFrameRGBA= avcodec_alloc_frame();
	if(pFrameRGBA==NULL)
	return -1;	//	cannot allocate memory

//	determine required buffer size for allocate buffer
	numBytes = avpicture_get_size(PIX_FMT_RGB24,768,576);
	buffer=new uint8_t[numBytes];	
	printf(" size of frame in bytes : %d\n",numBytes);

//	get data from video
	
//	fps = (pFormatCtx->bit_rate/1000)*(pFormatCtx->duration) / numBytes;
	//printf(" fps  : %f\n",fps);
//	assign appropiate parts of buffer to image planes in pFrameRGB
	avpicture_fill((AVPicture*)pFrameRGBA,buffer,PIX_FMT_RGB24,768,576);
	return 0;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	this function calculates the time arrival of the frames

timeval timeNow() // 
{
	//unsigned long RTPtime;
	timeval t;
	int time,time2;
	//time = ntp_gettime(&ntpTime);
	time = gettimeofday(&t,&tz);
	if (time==0)
	{
		//printf("time of arrival was:%li.%06li\n",ntpTime.time.tv_sec,ntpTime.time.tv_usec);
		//printf("time arrival of frame %d was:%li.%06li\n",frameCounter,t.tv_sec,t.tv_usec);
		//printf("time was:%li.%06li\n",t.tv_sec,t.tv_usec);
		//return t;
	}else{
		printf("error was:%i\n",errno);
		
	}
		
	//t = time(NULL);
	//RTPtime = converter->convertToRTPTimestamp(Timevint size;al::Timeval(50,20));
	//printf("seconds was %li\n",t);
	//return RTPtime;
	return t;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int timeNow2()
{
	timeval t;
	int time,time2;
	//time = ntp_gettime(&ntpTime);
	time = gettimeofday(&t,&tz);
	if (time==0)
	{
		//printf("time of arrival was:%li.%06li\n",ntpTime.time.tv_sec,ntpTime.time.tv_usec);
		//printf("time of timer was:%li.%06li\n",t.tv_sec,t.tv_usec);
		printf("time was:%li.%06li\n",t.tv_sec,t.tv_usec);
		//return t;
	}else{
		printf("error was:%i\n",errno);
		
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	function to execute after Reading data from RTP source
//void *clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds
static void afterReading(void *clientData,unsigned framesize,unsigned /*numTruncatedBytes*/,
				struct timeval presentationTime,unsigned /*durationInMicroseconds*/)
{
	//	test size of frame
if(framesize>=maxRTPDataSize)
{
	printf("framesize: %d is greater that maxRTPDATASize: %i\n",framesize,maxRTPDataSize);
	}else{
	       //printf("Data read is: %d\n",framesize);
	       }
	
	//unsigned char *Data = new unsigned char[maxRTPDataSize];
	//unsigned char *Data = new unsigned char[framesize];
	dataBuffer.data = (unsigned char*)clientData;
	dataBuffer.size = framesize;
	
	//	we get the data // 
	data_RTP.size =  MP4Hsize + dataBuffer.size;		//	save MP4Header
	//	resize  buffer to save data
   	memcpy(data_RTP.data,MP4H,MP4Hsize);			//	save frame in memory
	memmove(data_RTP.data + MP4Hsize, dataRTP, dataBuffer.size);
	//memcpy(data_RTP.data, dataRTP, dataBuffer.size);
										//	save timestamp
	data_RTP.timestamp = Subsession->rtpSource()->curPacketRTPTimestamp();
 	data_RTP.time = timeNow();						//	time arrival
	//actualRTPtimestamp = 
	//printf("current RTP timestamp %lu\n",data_RTP.timestamp);
	//printf("Subsession ID: %s\n",Subsession->sessionId);
	if (dataBuffer.data == NULL)
	{ 
		printf("%s \n","data was not read");
	}else{
		
	//	printf("Data size: %i\n",dataBuffer.size);
		//printf("%i\n",strlen(dataBuffer.data));
	}
		
	
	       readOKFlag = ~0;	//	set flag to new data
	      // delete Data;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	function to execute when the RTP is closed
static void onClose(void *clientData)
{

	readOKFlag = ~0;	//	set flag to new data
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/*	this function calculates the skew  between sender/receiver
//	to know if the sender or  the receiver is fastest that other
//	see chapter 6 C. perkins book page 176
			
		s = (timestampN) - timestampN-1 / (timearrivalN - timearrivalN-1)

	here, we use double instead of unsigned long that is used to timestamps
*/
double skew(dataFrame N, dataFrame N_1)
{
	double d,d1,d2,s,tstampN,tstampN_1;
	//	differences between 2 frames
	d1 = (N.time.tv_sec + (N.time.tv_usec/1000000.0)); 
	d2 = (N_1.time.tv_sec + (N_1.time.tv_usec/1000000.0));
	//printf("time arrival 1: %06f\n",d);	
	//printf("time arrival 2: %06f\n",d2);
	d = d1-d2;
	//printf("time arrival difference: %06f\n",d);
	d = 90000.0*d;	//	multiplies by timestamp frequency, here 90000 for video
	//printf("time arrival difference: %06f\n",d);
	tstampN = N.timestamp;
	tstampN_1 = N_1.timestamp;
	s = (tstampN - tstampN_1);
	//printf("skew: %06f\n",s);
	//	calculates skew
	s = s/d;
	//printf("skew: %06f\n",s);
	return s;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//		this function start a connection with a RTSP source data
//		description:
//			input: 	URL as "rtsp://sonar:7070/cam1"
//			output: A subsession associated to the URL
int rtsp_Init(char const *URL)
 {

/////////////////	RTSP START	/////////////////

	//	assign an environment to work
	TaskScheduler *scheduler = BasicTaskScheduler::createNew();				
	if (scheduler==NULL)
		{printf("error %s\n"," scheduler was not created");
		return -1;}
	env = BasicUsageEnvironment::createNew(*scheduler);	
	if (env==NULL)
		{printf("error %s\n"," environment was not created");
		return -1;}
	//	create a RTSP client
	client =  RTSPClient::createNew(*env,verbosityLevel,name,tunnelOverHTTPPortNum); 	
	;
	if (client==NULL)
		{printf("error %s\n"," rtsp client was not created");
		return -1;}

////////////////////////////////////////////////////////
	//	RTSP PROTOCOL
	
	//	Send OPTIONS method
	
	getOptions = client->sendOptionsCmd(URL,NULL,NULL,NULL);	//	connect to server
	printf("OPTIONS are: %s\n",getOptions);				//	print the response
		
	//	send DESCRIBE method
	
	getDescription = client->describeURL(URL);			//	get the Session description
	printf("DESCRIBE is: \n%s\n",getDescription);			//	print this description

	//	setup live libraries for send SETUP method
	
	Session = MediaSession::createNew(*env,getDescription);		//	create session
	if (Session==NULL)
		{printf("error %s\n","Session was not created");
		return -1;}
	
	//	create a subsession for the RTP receiver (only video for now)
	
	
	MediaSubsessionIterator iter(*Session);// iter;
	//iter(*Session);

	if((Subsession= iter.next())!=NULL)		//	 is there a valid subsession?
	{
	
	//	look for video subsession

		if(strcmp(Subsession->mediumName(),"video")==0)		//	if video subsession
		{
			//ReceptionBufferSize =2000000;			//	set size of buffer reception
		}//else{
		//continue;}
		
		//	assign RTP port,  it must be even number    RFC3550?
	
		//Subsession->setClientPortNum(RTPport);
	
		//	initiate  subsession
		if(!Subsession->initiate())
		{
		//	 print error when subsession was not initiated
		printf("Failed to initiate RTP subsession %s %s %s \n",Subsession->mediumName(),Subsession->codecName(),env->getResultMsg());
		}else{
		//	print info about RTP session type, codec,port number
		printf("RTP %s/%s subsession initiated on port %d\n",Subsession->mediumName(),Subsession->codecName(),Subsession->clientPortNum());
		}
		//	timestamp clock
		timestampFreq = Subsession->rtpSource()->timestampFrequency();
		printf("timestamp frequency %u\n",timestampFreq);

		//	config line, VOL header or  VOP header?
		MP4Header = Subsession->fmtp_config();
		MP4Hsize = strlen(MP4Header);
		//	convert to unsigned char
		MP4H = reinterpret_cast<unsigned char*>(const_cast<char*>(MP4Header));
		printf("Mp4 header: %s\n",MP4H);			//	print MPEG4 header
		
		//	send SETUP command

		if(client!=NULL)
		{
		SetupResponse = client->setupMediaSubsession(*Subsession,False,False,False);
		if(SetupResponse==False)
			{
				printf("%s \n","SETUP was not sent");
				return -1;
			}
		//	review status code
		printf("Subsession ID: %s\n",Subsession->sessionId);
		//printf("URL %u\n",Subsession->rtpInfo.trackId);
		printf("URL %u\n",Subsession->rtpChannelId);
		//printf("SSRC ID: %u\n",Subsession->rtpSource()->lastReceivedSSRC());
		}	

		//	now send PLAY method
		if(client!=NULL)
		{
	
			if(client->playMediaSession(*Session)==0)
			{
				printf("%s\n","PLAY command was not sent");
				return -1;
			}
		}
	}
	//iter.reset();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//int rtsp_getFrame(unsigned char *image)
int rtsp_getFrame()
{
	
	//printf("Subsession ID: %s\n",Subsession->sessionId);

//	START TO READ THE DATA
	/*
	if(client==NULL)
	{
		printf("%s\n","client is not available");
		return -1;

	}
	*/
	//MediaSubsessionIterator iter(*Session);
	//iter.reset();		//	reset subsessions
	
	//	start decoding frames
	
	//	update frame	
	//frameCounter=0;
	//if((Subsession=iter.next())!=NULL)		// while	check for correct subsession
	//{
	
		//if (Subsession->readSource()==NULL) continue;	//	check source of data
		/*
		if (Subsession->readSource()==NULL)
		{
			printf("%s\n","Reading from RTP source was not possible"); 
			return -1;
		}
		*/
	//int fcounter =0;
	//if(Subsession->readSource()!=NULL)	//	valid data source
	//{
	//	while(strcmp(Subsession->mediumName(),"video")==0)// before while  // if
	//	{
			//	get the data from rtp //readSource
			Subsession->readSource()->getNextFrame(dataRTP,maxRTPDataSize,
								afterReading,DataReception,
								onClose,DataReception);
			//	wait until data is available, it allows for other task to be performed
			//	while we wait data from rtp source					                                                        	
			readOKFlag = 0;								                               //	schedule read                         	
			TaskScheduler& scheduler = Subsession->readSource()->envir().taskScheduler();
			scheduler.doEventLoop(&readOKFlag);
			//printf("readOKFlag is: %i\n",readOKFlag );
			//timeNow();
			frameCounter++;
			//usleep(delay);
	/*
			decodeOK = avcodec_decode_video(pCodecCtx,pFrame,&frameFinished,data_RTP.data,data_RTP.size);
		
			//if(decodeOK!=0)
			//{printf("%s\n","error in decoding");}
		//else{
		//	if we get a frame then convert to RGB
			if(frameFinished)
			{
			//	convert the image from his format to RGB
				img_convert((AVPicture*)pFrameRGBA,PIX_FMT_RGB24,(AVPicture*)pFrame,pCodecCtx->pix_fmt,768,576);
					
				//pCodecCtx->width,pCodecCtx->height);
				//static unsigned char image[]={pFrameRGB->data[0]};
				//	save to a buffer
				//image = pFrameRGBA->data[0];	//	save RGB frame
				//data_RTP.image = pFrameRGBA->data[0];
				//frameCounter++;
				DecodedFrame[frameCounter] = pFrameRGBA->data[0];
				//ShowBuffer.push(data_RTP);
				printf("frame %d was decoded\n",frameCounter);
				//	get the timestamp of the frame
				//break;
			}
			else{
				//frameCounter++;	
				printf("there was an error while decoding the frame %d\n",frameCounter);
				//image = pFrame->data[0];
			}
			
			//fcounter++;
			if(frameCounter == 25)
			{break;}
			 
		
		}
	
	}
	*/

	//}
	//	Now send PAUSE method
	/*
	if(client!=NULL)
	{
		if(client->pauseMediaSession(*Session)==0)
		{
			printf("%s\n","PAUSE command was not sent");
			
		}
	}
	*/
	//	stop getting the frames
	//Subsession->readSource()->stopGettingFrames();
	//printf("SSRC ID: %u\n",Subsession->rtpSource()->lastReceivedSSRC());
	
	/*
	

	*/
	return 0;	//	exit with sucess
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//int rtsp_getFrame(unsigned char *image)
int rtsp_loop()
{
	
	//printf("Subsession ID: %s\n",Subsession->sessionId);

//	START TO READ THE DATA

	if(client==NULL)
	{
		printf("%s\n","client is not available");
		return -1;

	}
	
	//int fcounter =0;
	if(Subsession->readSource()!=NULL)	//	valid data source
	{
		while(strcmp(Subsession->mediumName(),"video")==0)// before while  // if
		{
	

			 
		}
	}
	
	//}
	//	Now send PAUSE method
	/*
	if(client!=NULL)
	{
		if(client->pauseMediaSession(*Session)==0)
		{
			printf("%s\n","PAUSE command was not sent");
			
		}
	}
	*/
	//	stop getting the frames
	//Subsession->readSource()->stopGettingFrames();
	//printf("SSRC ID: %u\n",Subsession->rtpSource()->lastReceivedSSRC());
	
	/*
	

	*/
	return 0;	//	exit with sucess
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//		this function start to get n frames: the compressed frames
//		this acts like a buffer to save tha last frame get it and to read and decode 
//		one appropiate frame

//		At start it can save n frames to be used like a buffer to let smooth the rate
//		of showinf frames  and too to decrease the network jitter of packets
//		here, this decouples the reception part of the showing part
int rtsp_buffering(int n)
{
	double s;
	for (int i=0; i<n;i++)	//	save n compressed frames
	{
		rtsp_getFrame();
		InputBuffer.push_back(data_RTP);		//	save to the queue
		
		if (i>1)
			{
			dataFrame N = InputBuffer.at(i);
			dataFrame N_1 = InputBuffer.at(i-1);
			s = skew(N,N_1);
			printf("skew : %06f\n",s);	
			}
		
		//T.push(i);
	}
return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int rtsp_decode(dataFrame data_RTP)
{
	//	decode the video frame using libavcodec

	decodeOK = avcodec_decode_video(pCodecCtx,pFrame,&frameFinished,data_RTP.data,data_RTP.size);
		
	//if(decodeOK!=0)
		//{printf("%s\n","error in decoding");}
	//else{
	//	if we get a frame then convert to RGB
		if(frameFinished)
		{
		//	convert the image from his format to RGB
			img_convert((AVPicture*)pFrameRGBA,PIX_FMT_RGB24,(AVPicture*)pFrame,pCodecCtx->pix_fmt,768,576);
					
			//pCodecCtx->width,pCodecCtx->height);
			//static unsigned char image[]={pFrameRGB->data[0]};
			//	save to a buffer
			image = pFrameRGBA->data[0];	//	save RGB frame
			//data_RTP.image = pFrameRGBA->data[0];
			//frameCounter++;
			//printf("frame %d was decoded\n",frameCounter);
			//	get the timestamp of the frame
			//break;
		}
		else{
			//frameCounter++;	
			printf("there was an error while decoding the frame %d\n",frameCounter);
			//image = pFrame->data[0];
		}
		
	//}
	return 0;

}
// /////////////////////////////////////////////////////////////////////////////////////////////////
//	Getting data thread
void* getData(void*)
{
	double s;
	int cod;
	while(1)	//	threads is always in execution
	{
		//usleep(delay);	//	delay
	
		//	LOCK THE RESOURCE
		
		cod = pthread_mutex_trylock(&mutexBuffer);
		if (cod!=0)	
			{
				//printf("%s\n","Error locking get RTP data");
			}
		else{
		//condition
			//while (flagShow<0)
			//cod = pthread_cond_wait(&cond[0],&mutexBuffer);		//	goto sleep until condition
			//assert(flagShow>0);
			//if (cod!=0)	
			//{printf("%s\n","Error condition in get RTP data");}
			//
			if (FirstTime==0)
			{
				FirstTime = -1;
			}else{
				sem_trywait(&sem);		//	wait semaphore
			}
		
			rtsp_getFrame();
			rtsp_decode(data_RTP);
			sem_post(&sem);
			//s = skew(data_RTP,InputBuffer.back());
			//InputBuffer.push_back(data_RTP);
			//printf("skew: %06f\n",s);
			//counter++;
			//T.push(counter);
			//printf("writing %d\n",counter);
			//printf("FIFO size: %d\n",InputBuffer.size());
		//
		
		//	UNLOCK THE RESOURCE
	
			//flagData = 1;
			//flagShow = -1;

		//	WAKE UP THE OTHER THREAD: SHOW DATA THREAD
			cod = pthread_cond_signal(&cond[1]);
			
			
		}

		cod = pthread_mutex_unlock(&mutexBuffer);
		if (cod!=0)	
			{printf("%s\n","Error unlocking get RTP data");}

		

		
		//if (cod!=0)	
		//	{printf("%s\n","Error signal condition in decode RTP data");}
		
	}
	return 0;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	this works but not update the image continuosly
void* ShowData(void* data)
{
	int cod;
	SoTexture2 *robot = (SoTexture2*)data;
	while(1)
	{
	
		//	LOCK THE RESOURCE

		cod = pthread_mutex_trylock(&mutexBuffer);
		if (cod!=0)	
			{printf("%s\n","Error locking RTP decode data");}
			//condition
			//while (flagData<0)
		else{
			cod = pthread_cond_wait(&cond[1],&mutexBuffer);		//	go to sleep until we get some data
										//	from the data thread
			//if(!T.empty())
			//if(!IB.empty())

			//if(!InputBuffer.empty())
			//{
				//int t = T.front();
				//T.pop();
				//printf("decoding: %d\n",t);
			
				//ReceivedFrame = InputBuffer.front();	//	get the frame from the FIFO buffer
				robot->image.setValue(SbVec2s(768,576),3,image);
				//ReceivedFrame = IB.front();
				//rtsp_decode(ReceivedFrame);
				//	INCREASE SEMAPHORE: DATA IS DECODED AND READY TO SHOW
				//ShowBuffer.push(data_RTP);		//	save decoded frame
				printf("update image No: %ld: \n",frameCounter);
				sem_post(&sem);				
				//InputBuffer.pop_front();		//	delete compressed frame from FIFO buffer
				//IB.pop_front();
				//printf("FIFO size: %d\n",InputBuffer.size());
				//robot->image.setValue(SbVec2s(720,576),3,image);
		
		//	}
	
			//flagShow = 1;
			//flagData =-1;

		//cod = pthread_cond_signal(&cond[2]);
		}

		//	UNLOCK THE RESOURCE
		cod = pthread_mutex_unlock(&mutexBuffer);
	
		if (cod!=0)	
			{printf("%s\n","Error unlocking RTP decode data");}
			
			//if (cod!=0)	
			//{printf("%s\n","Error signal condition in decode RTP data");}
		//sem_post(&sem);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void* updateImage(void* data)
{

int cod;
while(1)
{
	sem_trywait(&sem);
	
	//	LOCK THE RESOURCE

	cod = pthread_mutex_trylock(&mutexBuffer);
	if (cod!=0)	
			{printf("%s\n","Error locking RTP decode data");}
	else{
	
		//cod = pthread_cond_wait(&cond[2],&mutexBuffer);	

		SoTexture2 *robot = (SoTexture2*)data;
		robot->image.setValue(SbVec2s(768,576),3,image);
		//printf("update image No %i\n",frameCounter);
		sem_post(&sem);
	
	}
	//	UNLOCK THE RESOURCE
	cod = pthread_mutex_unlock(&mutexBuffer);
	
}
	return 0;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
//	these thread open a rtp session for read data using 3 threads
/*
void* rtp_dataSession(void* data)
{
	SoTexture2 *robot = (SoTexture2*)data;
	//while(1)
	//{
	if (FirstExec==0)robot->image.setValue(SbVec2s(768,576),3,image);	//	update image
	{
		//	START TO READ THE DATA

		if(client==NULL)
		{
			printf("%s\n","client is not available");
			//return -1;

		}
	
		if(Subsession->readSource()!=NULL)	//	valid data source
		{
			if(strcmp(Subsession->mediumName(),"video")==0)// before while  // if
			{
				pthread_create(&camera[0],0,ShowData,0);
				pthread_create(&camera[2],0,updateImage,(SoTexture2*)robot);			
				pthread_create(&camera[1],0,getData,0);
			
			}
		}

	FirstExec=-1;		//	only create the threads the first time after ony read the data
	
	}else{

	
	
	}


	//}
return 0;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
//	update function show the frame from cameras
/*
void update(void *data,SoSensor*)	//	this function updates the texture based on the frame got
					//	from the video stream
{
	//	get the video frames
	
	SoTexture2 *robot = (SoTexture2*)data;
	
	//	WAIT FOR SEMAPHORE
	sem_trywait(&sem);	//	update image
	
	//pthread_mutex_trylock(&mutexBuffer);
	
	robot->image.setValue(SbVec2s(768,576),3,image);	//	update image
	printf("update image No: %ld: \n",frameCounter);
	//if(!ShowBuffer.empty())
	//{
	//DecodedFrame = ShowBuffer.front();			//	get image
	//rtsp_decode(DecodedFrame);
	

	if (frameCounter>=0)
	{
	image = DecodedFrame[frameCounter];
	
	//ShowBuffer.pop();					// 	delete readed element
	
	//printf("FIFO Size %i\n",ShowBuffer.size());
	printf("frames reamining %d\n",frameCounter);
	//}
	frameCounter--;
	}
*/	
	//pthread_mutex_unlock(&mutexBuffer);
	//updates++;
	//printf("timesensor calls: %d\n",updates);
	//timeNow2();
	//usleep(20);
	//sem_post(&sem);	



////////////////////////////////////////////////////////////////////////////////////////////////////
//		MAIN PROGRAM
int main(int argc,char **argv)
{
	
	char const *URL;
	//delay = atoi(argv[1]);
	int cam = atoi(argv[1]);		// choose the camera to see
	//******************************************************************************************
	//	list of camera to access

	switch(cam)	//	update image
	{
		case 0:
			URL = URL0;
			break;
		case 1:
			URL = URL1;
			break;
		case 2:
			URL = URL2;
			break;
		case 3:
			URL = URL3;
			break;
		default:
			URL = URL0;
			break;
	}
	//**************************0*****************************************************************
	//	Allocation of  memory to save the compressed and uncompressed frames

	dataRTP = new unsigned char[70000];			//      buffer for RTP data available
	data_RTP.data = new unsigned char[70000];		//	uncompressed frames with MPEG4 Headers

	//******************************************************************************************
	//	INIT THE VIDEO CODECS	
	
	if (init_codecs()==-1)					//	init libavcodec
	{
		printf("%s\n","codecs was not initialized");	//	codec not initialized
		return -1;
	}						
	
	//******************************************************************************************
	//	Start threads and semaphores

	sem_init(&sem,0,0);					//	start semaphore with a  value of 0 = blocked
	
	int cod = pthread_mutex_init(&mutexBuffer,0);		//	start mutex exclusion of buffers
	if (cod!=0)	
		{printf("%s\n","Error initilization on mutex");}
	
	cod = pthread_cond_init(&cond[0],0);			//	start conditional variable 
	if (cod!=0)
		{printf("%s\n","error starting condition");}

	cod = pthread_cond_init(&cond[1],0);			//	start conditional variable 2
	if (cod!=0)						//	used with the thread getdata
		{printf("%s\n","error starting condition");}

	cod = pthread_cond_init(&cond[2],0);			//	start conditional variable 3
	if (cod!=0)						//	used with the thread getdata
		{printf("%s\n","error starting condition");}

	//*******************************************************************************************
	//	connect to RTSP server

	int status;
	status= rtsp_Init(URL);//atoi(argv[2])					//	Init RTSP Client
	//rtsp_buffering(1);				//	start buffering n frames

	//printf("FIFO size: %d\n", InputBuffer.size());
	/*
	if (status!=-1)
	{
		rtsp_getFrame();	//	get the frame
	}else{
		printf("%s\n","Is not possible to initializate the client ");
	}

	*/
	// ******************************************************************************************** 
	//		Graphics Scene
	
	// Initializes SoQt library (and implicitly also the Coin and Qt
    	// libraries). Returns a top-level / shell Qt window to use.
    	 QWidget * mainwin = SoQt::init(argc, argv, argv[0]);

	//SoDB::init();
	//SoDB::setRealTimeInterval(1/120.0);
	SoSeparator * root = new SoSeparator;
    	root->ref();
	/*
	SoPerspectiveCamera *mycamera = new SoPerspectiveCamera;
	SoDirectionalLight *light = new SoDirectionalLight;
	root->addChild(mycamera);
	root->addChild(light);
	*/	

	SoTexture2  *robot = new SoTexture2;
    	root->addChild(robot);		// add to shape
    	robot->filename.setValue("");	// this set is for use an image from memory in place of a file */
	//robot->image.setValue(SbVec2s(720,576),3,image);//pCodecCtx->height,pCodecCtx->width
	//	make a plane background and associate a texture
	//printf("imnage was %u\n",image[0]);
	SoCoordinate3 *Origin = new SoCoordinate3;
	root->addChild(Origin);			//	set plane origin at (0,0,0)
	
	Origin->point.set1Value(3,SbVec3f( pCodecCtx->width/2, pCodecCtx->height/2, 0));
	Origin->point.set1Value(2,SbVec3f( pCodecCtx->width/2, -pCodecCtx->height/2, 0));
	Origin->point.set1Value(1,SbVec3f(-pCodecCtx->width/2, -pCodecCtx->height/2, 0));
	Origin->point.set1Value(0,SbVec3f(-pCodecCtx->width/2,  pCodecCtx->height/2, 0));
	//	define the square's normal
	SoNormal *normal = new SoNormal;
	root->addChild(normal);
	normal->vector.set1Value(0,SbVec3f(0,0,1));	//	normal to z	
	
	//	define the texture coordinates

	SoTextureCoordinate2 *TextCoord = new SoTextureCoordinate2;
	root->addChild(TextCoord);
	TextCoord->point.set1Value(0,SbVec2f(0,0));
	TextCoord->point.set1Value(1,SbVec2f(0,1));
	TextCoord->point.set1Value(2,SbVec2f(1,1));
	TextCoord->point.set1Value(3,SbVec2f(1,0));

	// define normal and texture coordinate binding
	
	SoNormalBinding *nBind = new SoNormalBinding;
	SoTextureCoordinateBinding *TextBind = new SoTextureCoordinateBinding;
	root->addChild(nBind);	
	root->addChild(TextBind);
	nBind->value.setValue(SoNormalBinding::OVERALL);
	TextBind->value.setValue(SoTextureCoordinateBinding::PER_VERTEX);

	//	define a faceset

	SoFaceSet *Face = new SoFaceSet;
	root->addChild(Face);
	Face->numVertices.set1Value(0,4);

	//	timer sensor for show the data

	//	START TO READ THE DATA

	if(client==NULL)
	{
		printf("%s\n","client is not available");
		return -1;

	}
	
	//pthread_create(&camera[3],0,rtp_dataSession,0);
	
	int fcounter =0;
	if(Subsession->readSource()!=NULL)	//	valid data source
	{
		if(strcmp(Subsession->mediumName(),"video")==0)// before while  // if
			{
			
			pthread_create(&camera[0],0,ShowData,(SoTexture2*)robot);
			pthread_create(&camera[1],0,getData,0);
			//pthread_create(&camera[2],0,updateImage,(SoTexture2*)robot);
			
			
		}
	}
	
	
	//****************************************************************************
	//	start threads to get and show the video stream	
	
	
	
	//****************************************************************************
	//	setup timer sensor for recursive image updating 

	/*	
	SoTimerSensor *timer = new SoTimerSensor(update,robot);
	//atoi(argv[3]
	timer->setBaseTime(SbTime::getTimeOfDay());
	timer->setInterval(1.0/25.0);//fps	//	set interval 40 ms
	timer->schedule();				//	enable
        //root->addChild(new SoCube);
	*/
	
     	SoTransform *myTrans = new SoTransform;
	root->addChild(myTrans);
   	myTrans->translation.setValue(0.0,0.0,100.0);
	
	
	
	//****************************************************************************
	//	test for show the scene

	/*
	SoSceneManager  *sceneManager = new SoSceneManager;
	sceneManager->activate();
	//	camerasem_post(&sem);
	mycamera->position.setValue(0,0,5);
	SbVec3f lookAt(0,0,0);	
	mycamera->pointAt(lookAt);
	
	sceneManager->setSceneGraph(root);
	sceneManager->setWindowSize(SbVec2s(800,600));
	sceneManager->setSize(SbVec2s(800,600));
	sceneManager->setViewportRegion(SbViewportRegion(800,600));

	//sceneManager->render();
	SbViewportRegion region(800,600); 
	SoGLRenderAction myRender(region);
	myRender.apply(root);
	*/
	//robot->image.setValue(SbVec2s(768,576),3,image);
	// Use one of the convenient SoQt viewer classes.
	SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(mainwin);
    	eviewer->setSceneGraph(root);
    	eviewer->show();
	// Pop up the main window.
    	SoQt::show(mainwin);
    	// Loop until exit.
    	SoQt::mainLoop();
	
		
	
	//***************************************************************************
	//			wait until the threads finish
	
	
	pthread_join(camera[0],0);
	pthread_join(camera[1],0);
	pthread_join(camera[2],0);
	//pthread_join(camera[3],0);
	//****************************************************************************
	// 			Clean up resources.				  
    	delete eviewer;
    	root->unref();
	delete DataReception;
	delete pFrame;
	delete pFrameRGBA;
	delete data_RTP.data;
	delete dataRTP;
		
return 0;
}	



