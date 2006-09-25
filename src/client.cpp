/*  ********************************************************************************************************
//			CLASS STREAM

//	Author:		Henry Portilla
//	Date:		june/2006
//	modified:	september/2006
//	Thanks

//	Description:	This program use the live555 libraries for make a RTSP
			client
										*/
// 	RFC 2326 RTSP protocol

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
////////////////////////////////////////////////////////////////////////////////////////////////////
//	initialization of static data members

char STREAM::readOKFlag=0;
void *STREAM::temp=0;
int members=0;
int ID1=0;
//int  STREAM::MP4Hsize=0;
////////////////////////////////////////////////////////////////////////////////////////////////////
//	Constructor 
STREAM::STREAM()
{
//	initializaton of variables
verbosityLevel = 0;
name="RTSP";
tunnelOverHTTPPortNum = 0;
maxRTPDataSize = RTPDataSize;				//	max size of frames
MP4Hsize=0;
frameCounter=0;						//	start frame counter
ID=0;

//	Allocation of  memory to save the compressed and uncompressed frames
	
dataRTP = new unsigned char[70000];			// 	compressed frame
data_RTP.data = new unsigned char[70000];		//	uncompressed frames with MPEG4 Headers

}
///////////////////////////////////////////////////////////////////////////////////////////////////
//	Destructor class
STREAM::~ STREAM()
{
	//	free resources
	delete dataRTP;
	delete data_RTP.data;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::set_ID()		//	increase ID at each created object
{

	ID++;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::get_ID()		//	get ID of object
{

	return ID;

}


////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::bind_object()
{
	int ID;
	ID = get_ID();
	temp=(void*)this;		//	asssign correct object

}	

////////////////////////////////////////////////////////////////////////////////////////////////////
//	init libavcodec
int STREAM::initCodecs()
{
	pCodecCtx=NULL;
	
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
	pCodecCtx->width=720;//768
	pCodecCtx->height=576;//576
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
	pFrameCrop = avcodec_alloc_frame();
//	allocate memory to save a RGB frame
	pFrameRGBA= avcodec_alloc_frame();
	if(pFrameRGBA==NULL)
	return -1;	//	cannot allocate memory

//	determine required buffer size for allocate buffer
	numBytes = avpicture_get_size(PIX_FMT_RGB24,512,512);
	buffer=new uint8_t[numBytes];	
	printf(" size of frame in bytes : %d\n",numBytes);

//	get data from video
	
//	fps = (pFormatCtx->bit_rate/1000)*(pFormatCtx->duration) / numBytes;
	//printf(" fps  : %f\n",fps);
//	assign appropiate parts of buffer to image planes in pFrameRGB
	avpicture_fill((AVPicture*)pFrameRGBA,buffer,PIX_FMT_RGB24,512,512);
	return 0;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	this function calculates the time arrival of the frames

timeval STREAM::timeNow() // 
{
	//unsigned long RTPtime;
	struct timezone tz;
	timeval t;
	int time;
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
	
	return t;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::timeNow2()
{
	timeval t;
	int time;
	//time = ntp_gettime(&ntpTime);
	time = gettimeofday(&t,&tz);
	if (time==0)
	{
		//printf("time of arrival was:%li.%06li\n",ntpTime.time.tv_sec,ntpTime.time.tv_usec);
		//printf("time of timer was:%li.%06li\n",t.tv_sec,t.tv_usec);
		//printf("time was:%li.%06li\n",t.tv_sec,t.tv_usec);
		//return t;
	}else{
		printf("error was:%i\n",errno);
		
	}
	return time;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::init_mutex()
{
	int cod;
	cod = pthread_mutex_init(&mutexBuffer,0);	//	start mutex exclusion of buffers
	if (cod!=0)	
		{
		printf(" Error initilization on mutex \n");
		return 0;
		}

}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	lock a mutex
int STREAM::lock_mutex()
{
	int cod;
	cod = pthread_mutex_lock(&mutexBuffer);
	return cod;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::unlock_mutex()
{
	int cod;
	cod = pthread_mutex_unlock(&mutexBuffer);
	return cod;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::create_Thread()
{
	int cod;
	cod = pthread_create(&camera,0,STREAM::Entry_Point,this);
	if (cod!=0)
	{
		printf("error creating thread \n");
		return cod;
	}else{
		printf("creating thread \n");
		return cod;
	}
		
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::set_Semaphore()
{
	sem_post(&sem);

}
////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::init_Semaphore(int i)
{
	sem_init(&sem,0,i);		//	start semaphore to i value

}
////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::wait_Semaphore()
{
	sem_wait(&sem);

}
////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::ProcessFrame(void *clientData, unsigned framesize)
{
	

	dataBuffer.data = (unsigned char*)clientData;
	dataBuffer.size = framesize;
	
	//	we get the data // 
	data_RTP.size = MP4Hsize + dataBuffer.size;				//	save MP4Header
	//	resize  buffer to save data
	
   	memcpy(data_RTP.data,MP4H,MP4Hsize);				   	//	save frame in memory
	memmove(data_RTP.data + MP4Hsize, dataRTP, dataBuffer.size);
	//memcpy(data_RTP.data, dataRTP, dataBuffer.size);
										//	save timestamp
	//data_RTP.timestamp = Subsession->rtpSource()->curPacketRTPTimestamp();
 	data_RTP.time = timeNow();
	data_RTP.index= frameCounter;						//	time arrival
	//actualRTPtimestamp = 
	//printf("current RTP timestamp %lu\n",data_RTP.timestamp);
	//printf("Subsession ID: %s\n",Subsession->sessionId);
	if (dataBuffer.data == NULL)
	{ 
		//printf("%s \n","data was not read");
	}else{
		
	//	printf("Data size: %i\n",dataBuffer.size);
		//printf("%i\n",strlen(dataBuffer.data));
	}

		
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//	This method process the compressed frame, first it add an MPEG4 Header from RTP data obtained
//	from config line in Description response, then save the time arrival timestamp, increase a 
//	reference counter
void STREAM::SaveFrame(void *clientData, unsigned framesize)
{
	
	//temp = this;
	//temp->ProcessFrame(clientData,framesize);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//	FUNCTION AFTER READING DATA FROM REMOTE SERVER THROUGH RTP protocol
//void *clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds
void STREAM::afterReading(void *clientData,unsigned framesize,unsigned /*numTruncatedBytes*/,
				struct timeval presentationTime,unsigned /*durationInMicroseconds*/)
{
	
	unsigned int maxSize = 70000;			//	max size of the got frame
	//test size of frame
	if(framesize>=maxSize)				//	maxRTPDataSize
	{
	printf("framesize: %d is greater that maxRTPDATASize: %i\n",framesize,maxSize);
	}else{
	       //printf("Data read is: %d\n",framesize);
		
		STREAM *ps= (STREAM*)temp;		// 	Auxiliar object to make reference to the actual 
							//	being used
		ps->ProcessFrame(clientData,framesize);
		//printf("%s\n","OK");
		
	       }
		
	readOKFlag = ~0;				//	set flag to new data   before ~0
	      // delete Data;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	function to execute when the RTP is closed
void STREAM::onClose(void *clientData)
{

	readOKFlag = ~0;	//	set flag to new data
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/*	this function calculates the skew  between sender/receiver
//	to know if the sender or  the receiver is fastest that other
//	see chapter 6 C. perkins book page 176
			
		s = (timestampN - timestampN-1) / (timearrivalN - timearrivalN-1)

	here, we use double instead of unsigned long that is used to timestamps
*/
double STREAM::skew(dataFrame N, dataFrame N_1)
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
int STREAM::rtsp_Init()// int camNumber
 {
	//const char *name = "RTSP";					//	name of client in live555 libraries
	
	
	bind_object();
	
	//	check for correct value of URL address 

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
	
	getDescription = client->describeURL(URL);		//	get the Session description
	printf("DESCRIBE is: \n%s\n",getDescription);			//	print this description

	//	setup live libraries for send SETUP metunsigned long timestamp;hod
	
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
		printf("RTP %s/%s subsession  initiated on port %d\n",Subsession->mediumName(),Subsession->codecName(),Subsession->clientPortNum());
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
		printf("Subsession  ID: %s\n",Subsession->sessionId);
		//printf("URL %u\n",Subsession->rtpInfo.trackId);
		//printf("URL %u\n",Subsession->rtpChannelId);
		//printf("SSRC ID: %u\n",Subsession->rtpSource()->lastReceivedSSRC());
		}	

		//	now send PLAY method
		if(client!=NULL)
		{
	
			if(client->playMediaSession(*Session)==0)
			{
				printf("%s\n","PLAY command was not sent");
				return -1;
			}else{
				printf("%s", "PLAY command sent");
			}
		}else{
			printf("%s", "CLIENT PROBLEMS");
			}
	}
	//iter.reset();
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	this function close a rtsp connection
int STREAM::rtsp_Close()
{
	//	CLOSE RTSP CONNECTION
	if(client !=NULL)
	{
	//	send TEARDOWN command
		client->teardownMediaSession(*Session);
		
	}
return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//int rtsp_getFrame(unsigned char *image)
int STREAM::rtsp_getFrame()
{
	

	unsigned Var;
	//	get the data from rtp //readSource
	Subsession->readSource()->getNextFrame(dataRTP,maxRTPDataSize,
								afterReading,(void*)Var,
								onClose,(void*)Var);
	//	wait until data is available, it allows for other task to be performed
	//	while we wait data from rtp source					                                                        	
	//	HERE WE GET THE FRAME FROM REMOTE SOURCE

	readOKFlag = 0;						 //	schedule read                         	
	TaskScheduler& scheduler = Subsession->readSource()->envir().taskScheduler();//&
	scheduler.doEventLoop(&readOKFlag);
	//printf("readOKFlag is: %i\n",readOKFlag );
	//timeNow();
	frameCounter++;					//	increase frame counter
	//usleep(delay);
	
	
	
	return 0;	//	exit with sucess
}
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::rtsp_decode(Frame dataCompress)
{
	//	decode the video frame using libavcodec

	decodeOK = avcodec_decode_video(pCodecCtx,pFrame,&frameFinished,dataCompress.data,dataCompress.size);
		
	//if(decodeOK!=0)
		//{printf("%s\n","error in decoding");}
	//else{
	//	if we get a frame then convert to RGB
		if(frameFinished)
		{
		//	cut the left side of the image, so 720-512 = 208 
		//	cut the top side of the image,	nothing by that 0
			img_crop((AVPicture*)pFrameCrop,(AVPicture*)pFrame,pCodecCtx->pix_fmt,0,208);
		//	convert the image from his format to RGB
			img_convert((AVPicture*)pFrameRGBA,PIX_FMT_RGB24,(AVPicture*)pFrameCrop,pCodecCtx->pix_fmt,512,512);
					
			//pCodecCtx->width,pCodecCtx->height);
			//	save to a buffer
			data_RTP.image = pFrameRGBA->data[0];	//	save RGB frame
			data_RTP.pFrame = pFrameRGBA;		//	save frame to export
			//printf("frame %d was decoded\n",frameCounter);
			//	get the timestamp of the frame
			//break;
		}
		else{
			//frameCounter++;	
			printf("there was an error while decoding the frame %d in the camera %d\n",frameCounter,ID);
			data_RTP.pFrame = pFrameRGBA;
			//image = pFrame->data[0];
		}
		
	//}
	return 0;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
//		this function start to get n frames: the compressed frames
//		this acts like a buffer to save tha last frame get it and to read and decode 
//		one appropiate frame

//		At start it can save n frames to be used like a buffer to let smooth the rate
//		of showinf frames  and too to decrease the network jitter of packets
//		here, this decouples the reception part of the showing part
int STREAM::rtsp_Buffering(int n)
{
	double s;
	for (int i=0; i<n;i++)	//	save n compressed frames
	{
		rtsp_getFrame();
		rtsp_decode(data_RTP);
		InputBuffer.push_back(data_RTP);		//	save to the queue
		
		if (i>1)
			{
			dataFrame N = InputBuffer.at(i);
			dataFrame N_1 = InputBuffer.at(i-1);
			s = skew(N,N_1);
			//printf("skew : %06f\n",s);	
			}

	}
return 0;
}
// ///////////////////////////////////////////////////////
//	Entry point function for the thread in C++
//	static function
void *STREAM::Entry_Point(void *pthis)
{
	STREAM *pS = (STREAM*)pthis;	//	convert to class Stream to allow correct thread work
	pS->rtsp_getData();
	return 0;
}


//////////////////////////////////////////
//	Getting data thread function
void STREAM::rtsp_getData()
{
	int cod;
	double s;

			
	while(1)	//	threads is always in execution
	{
		//usleep(delay);	//	delay
	
		//	LOCK THE RESOURCE
		cod = lock_mutex();
		                           
		if (cod!=0)	
			{
				//printf("%s\n","Error locking get RTP data");
			}
		else{
		//conditaugustion
			
			rtsp_getFrame();
			rtsp_decode(data_RTP);
			//sem_post(&sem);
			
			if(!InputBuffer.empty())
				{
				
				dataFrame N = data_RTP;
				dataFrame N_1 = InputBuffer.back();
				s = skew(N,N_1);
				//printf("skew : %06f\n",s);
				//s= skew(Temp,InputBuffer.back());
				
				}
			InputBuffer.push_back(data_RTP);
			//counter++;
			//T.push(counter);
			//printf("writing frame %d from the camera %d \n",frameCounter,ID);
			//printf("FIFO size: %d\n",InputBuffer.size());
		

		//	WAKE UP THE OTHER THREAD: SHOW DATA THREAD
			//cod = pthread_cond_signal(&cond[1]);
			//sem_post(&sem);
			set_Semaphore();
			
		}
		cod = unlock_mutex();
		if (cod!=0)	
			{printf("%s\n","Error unlocking get RTP data");}

		
	}
	//return 0;
	
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//	his function get an image from remote server
int STREAM::Init_Session(char const *URLcam, int id)
{
	int status;
	URL = URLcam;
	ID=id;
	////////////////////////////////////////////////////////////////////////
	//	start the codecs	
	initCodecs();					//	init codecs
	////////////////////////////////////////////////////////////////////////
	//	Start threads and semaphores
	init_Semaphore(0);				//	start semaphores with a  value of 0 = blocked
	init_mutex();					//	start mutex for exclusion of buffers
	
	//int cod;					//	not used			
	//cod = pthread_mutex_init(&mutexFrame,0);
		
	//	connect to RTSP server

	status = rtsp_Init();			//	Init RTSP Clients for the camera
	/////////////////////////////////////////////////////////////////////////
	//	START TO READ THE DATA FROM CAMERA

	if(client==NULL)				//	check for client
		{
		printf(" client is not available\n");
		return -1;
		}
	if(Subsession->readSource()!=NULL)		//	check for valid data source
		{
		if(strcmp(Subsession->mediumName(),"video")==0)// before while  // if
			{
			rtsp_Buffering(4);		//	start buffering n frames

			//	create thread for data reading using one static function

			create_Thread();		//	start to get data
			
			}
	}
	//
return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
Export_Frame STREAM::getImage()//dataFrame
{
	//sem_wait(&sem);					
	wait_Semaphore();				//	update image
	unsigned char* ActualFrame;
	Export_Frame I_Frame;
	if(!InputBuffer.empty())			//	check if buffer is not empty
	{
					
		ReceivedFrame = InputBuffer.front();	//	get the frame from the FIFO buffer
		//ActualFrame = ReceivedFrame.image;
		//assert(ReceivedFrame.pFrame);
		//if (ReceivedFrame.pFrame)
		I_Frame.pData = ReceivedFrame.pFrame;	//	save image
		I_Frame.h=512;
		I_Frame.w=512;
 		
		InputBuffer.pop_front();		//	delete frame from the FIFO buffer
		
		//printf("FIFO size: %d\n",InputBuffer.size());
		
		
	}else
	{
		printf("empty buffer %d\n", InputBuffer.size());
	}	

	//return ReceivedFrame.image;				//	return the last frame
	return I_Frame; 	
	
}
///////////////////////////////////////////////////////////////////////////////////////////////////
Export_Frame STREAM::callImage()
{
	//unsigned char *t5;
	Export_Frame t5;
	STREAM *ps = (STREAM*)temp;
	t5 = ps->getImage();
	return t5;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	update function show the frame from cameras

void update(void *data,SoSensor*)	//	this function updates the texture based on the frame got
					//	from the video stream
{
	//	get the video frames
	
	//int CAM = 1;
	//unsigned char *Fr;
	Export_Frame Fr;
	SoTexture2 *leftImage = (SoTexture2*)data;
	//temp1 =(void*)&camara1;
	
	//	WAIT FOR SEMAPHORE
		Fr = STREAM::callImage();		//	better return a structure
		leftImage->image.setValue(SbVec2s(512,512),3,Fr.pData->data[0]);
		
		//printf("update image No: %d: from camera \n",ReceivedFrame.index);
		//timeNow2();
		
	

	
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//		MAIN PROGRAM
//			use: .\client rtsp://sonar:7070/cam1

int main(int argc,char **argv)
{
	unsigned char *t,*t2;//"rtsp://sonar:7070/cam3";
	const char *cam =argv[1];		//	
							
	
	STREAM camara1;				//  	create an stream object
	camara1.Init_Session(cam,0);		//	start connection with cam
						//	the zero is used as an internal Identifier
						//	for the camera 
		
	//t = camara1.getImage();
	
	
	//STREAM camara2;
	//camara2.Init_Session(cam,1);

	//t2 = camara2.getImage();
	
	// ******************************************************************************************** 
	//		Graphics Scene
	
	// Initializes SoQt library (and implicitly also the Coin and Qt
    	// libraries). Returns a top-level / shell Qt window to use.
    	 QWidget * mainwin = SoQt::init(argc, argv, argv[0]);

	//SoDB::init();
	//SoDB::setRealTimeInterval(1/120.0);
	SoSeparator *root = new SoSeparator;
    	root->ref();
	
	//	MAKE A PLANE FOR IMAGE 

	SoGroup *plane = new SoGroup;
	
	SoCoordinate3 *Origin = new SoCoordinate3;
	plane->addChild(Origin);			//	set plane origin at (0,0,0)
	
	Origin->point.set1Value(3,SbVec3f( 512/2,  512/2, 0));
	Origin->point.set1Value(2,SbVec3f( 512/2, -512/2, 0));
	Origin->point.set1Value(1,SbVec3f(-512/2, -512/2, 0));
	Origin->point.set1Value(0,SbVec3f(-512/2,  512/2, 0));
	
	//	define the square's normal
	SoNormal *normal = new SoNormal;				//	start buffering n frames;
	plane->addChild(normal);
	normal->vector.set1Value(0,SbVec3f(0,0,1));	//	normal to z	
	
	//	define the texture coordinates

	SoTextureCoordinate2 *TextCoord = new SoTextureCoordinate2;
	plane->addChild(TextCoord);
	TextCoord->point.set1Value(0,SbVec2f(0,0));
	TextCoord->point.set1Value(1,SbVec2f(0,1));
	TextCoord->point.set1Value(2,SbVec2f(1,1));
	TextCoord->point.set1Value(3,SbVec2f(1,0));
	
	// define normal and texture coordinate binding
	
	SoNormalBinding *nBind = new SoNormalBinding;
	SoTextureCoordinateBinding *TextBind = new SoTextureCoordinateBinding;
	plane->addChild(nBind);	
	plane->addChild(TextBind);

                           
	nBind->value.setValue(SoNormalBinding::OVERALL);
	TextBind->value.setValue(SoTextureCoordinateBinding::PER_VERTEX);

	//	define a faceset

	SoFaceSet *Face = new SoFaceSet;
	plane->addChild(Face);
	Face->numVertices.set1Value(0,4);

	// MAKE LEFT AND RIGHT PLANES	

	//	LEFT PLANE
	
	SoSeparator *leftPlane = new SoSeparator;
	
	//	ADD THE IMAGE FROM THE FIRST CAMERA
	SoTexture2  *leftImage = new SoTexture2;
	leftImage->filename.setValue("");	// this set is for use an image from memory in place of a file */
	//leftImage->image.setValue(SbVec2s(512,512),3,t);

	SoTransform *leftTransform = new SoTransform;
	leftTransform->translation.setValue(-512/2,0.0,0.0);
	leftPlane->addChild(leftTransform);
	leftPlane->addChild(leftImage);
	leftPlane->addChild(plane);
	
	//	RIGHT PLANE

	SoSeparator *rightPlane = new SoSeparator;

	//	ADD THE IMAGE FROM THE SECOND CAMERA
	SoTexture2 *rightImage = new SoTexture2;
	rightImage->filename.setValue("");
	//rightImage->image.setValue(SbVec2s(512,512),3,t2);

	SoTransform *rightTransform = new SoTransform;
	rightTransform->translation.setValue(512/2,0.0,0.0);
		
	rightPlane->addChild(rightTransform);
	rightPlane->addChild(rightImage);
	rightPlane->addChild(plane);
	
	//	ADD THE TWO PLANES

	root->addChild(leftPlane);		// 
	root->addChild(rightPlane);

	//****************************************************************************
	//	setup timer sensor for recursive image updating 

			
	SoTimerSensor *timer = new SoTimerSensor(update,leftImage);
	//
	timer->setBaseTime(SbTime::getTimeOfDay());//atoi(argv[3])
	timer->setInterval(1.0/25.0);//fps	//	set interval 40 ms
	timer->schedule();				//	enable

     	SoTransform *myTrans = new SoTransform;
	root->addChild(myTrans);
   	myTrans->translation.setValue(0.0,0.0,200.0);
	
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
	
	//pthread_join(camera[0],0);
	//pthread_join(camera[0],0);
	//pthread_join(camera[2],0);
	//pthread_join(camera[3],0);
	//****************************************************************************
	// 			Clean up resources.				  
    	delete eviewer;
    	root->unref();
	

		
return 0;
}	



