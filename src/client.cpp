/*  ********************************************************************************************************
//			CLASS STREAM

//	Author:		Henry Portilla
//	Date:		June/2006
//	modified:	december/2006
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

//	Custom Inventor nodes
#include "SoStereoTexture.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
//	initialization of static data members

//char STREAM::readOKFlag=0;
//void *STREAM::temp=0;
int members=0;
int ID1=0;
TFunctor *C1,*C2;					//	pointers to abstract class

TIME t1L,t2L,t1R,t2R;
//long int fpsL=0;

float fpsL,fpsR;
int FirstL =0;						//	flags for fps calculus
int FirstR =0;

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
m_global_flag = -1;					//	flag to start semaphore synchronization


//	Allocation of memory to save the compressed and uncompressed frames

//unsigned char dataRTP[70000];	
//dataRTP = new unsigned char[RTPDataSize];		// 	compressed frame
//data_RTP.data = new unsigned char[RTPDataSize];		//	uncompressed frames with MPEG4 Headers

}
///////////////////////////////////////////////////////////////////////////////////////////////////
//	Destructor class
STREAM::~ STREAM()
{
	//	free resources
	//delete dataRTP;		// to avoid double free or corruption
					// don't use this form to free the memory	
	//delete data_RTP.data;
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
STREAM* STREAM::bind_object()
{
	int ID;
	ID = get_ID();
 	temp=(void*)this;		//	asssign correct object
	return this;
}	

////////////////////////////////////////////////////////////////////////////////////////////////////
//	init libavcodec
int STREAM::initCodecs()
{
	pCodecCtx = NULL;
	
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
	pCodecCtx->bit_rate = 1000000;	//	bit rate?

	//MPEG4 global header used for decode the frames
	//pCodecCtx->extradata = (void*)"000001b0f5000001b509000001000000012000c888b0e0e0fa62d089028307";	//	info given by Leopold Avellaneda
	//pCodecCtx->extradata_size= 62;

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
	pFrameRGBA = avcodec_alloc_frame();
	if(pFrameRGBA==NULL)
	return -1;	//	cannot allocate memory

//	determine required buffer size for allocate buffer
	numBytes = avpicture_get_size(PIX_FMT_RGB24,720,576);//RGB24
	buffer=new uint8_t[numBytes];	
	printf(" size of frame in bytes : %d\n",numBytes);

//	get data from video
	
//	fps = (pFormatCtx->bit_rate/1000)*(pFormatCtx->duration) / numBytes;
	//printf(" fps  : %f\n",fps);
//	assign appropiate parts of buffer to image planes in pFrameRGB
	avpicture_fill((AVPicture*)pFrameRGBA,buffer,PIX_FMT_RGB24,720,576);//RGB32
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
	printf("time of capture was:%u.%06li from camera %d\n",t.tv_sec + 0x83AA7E80,t.tv_usec,ID);
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
		return -1;
		}
	else{
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
	int cod, error;


	pthread_attr_t attr;		//	attribute object
	struct sched_param param;	//	struct for set the priority
	

	if (!(cod=pthread_attr_init(&attr)) &&
	!(cod=pthread_attr_getschedparam(&attr,&param)))
//	!(cod=pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM)))

		param.sched_priority = 10;
		//printf("setup thread scope \n");
					//	set priority to 10; max priority is = ?
					// in linux the threads are created with the maximum priority, equal to the kernel
	else{
		printf("error: %d \n", errno);
	}	
	
	error=pthread_attr_setschedparam(&attr,&param);

	if (error!=0)
	{
		printf("error: %d \n", errno);
	}
			
	//	create thread

	cod = pthread_create(&camera,&attr,STREAM::Entry_Point,this);
	if (cod!=0)
	{
		printf("error creating thread \n");
		return cod;
	}else{
		printf("creating thread %d with a priority of %d \n",ID,get_ThreadPriority());
		//printf("creating thread\n");
		return cod;
	}
		
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::get_ThreadPriority()
{
	pthread_attr_t attr;		//	attributes
	int cod, priority;
	struct sched_param param;	//	contain the priority of the thread

	if (!(cod=pthread_attr_init(&attr)) && 
	!(cod=pthread_attr_getschedparam(&attr,&param)))

		priority = param.sched_priority;
	
	return priority;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::join_Thread()
{
	int cod; 
	int *exitcode;
	if (cod = pthread_join(camera,(void**)&exitcode))
		printf("thread of data doesn't join to main flow\n");
	else
		printf("the exit code was %d \n",*exitcode);

	return cod;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::cancel_Thread()
{
	int cod;
	if(cod = pthread_cancel(camera))
		printf("thread cannot be canceled\n");	
	else{
		printf("thread has been canceled\n");
	}

	

}
////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::set_Semaphore(int sem)
{
	switch(sem)
	{
	case 1:
		if (sem_post(&Sem1)==-1)	
					//	increase semaphore 
		{
			//printf("Failed to unlock or increase the semaphore %d in camera %d",Sem1,ID);
		}else
		{
			//printf("camera: %d  Semaphore: %d\n",ID,Sem1);
		}
		
	
		break;
	case 2:
		if (sem_post(&Sem2)==-1)
					//	increase semaphore 
							{
			//printf("Failed to unlock or increase the semaphore %d in camera %d",Sem2,ID);
		}else{
			printf("camera: %d  Semaphore: %d\n",ID,Sem2);
			printf("No deberia entrar aqui\n");
		}

		break;
	}	
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::init_Semaphore(int sem,int i)
{
	switch(sem)
	{
	case 1:
		if (sem_init(&Sem1,0,i)==-1)	
					//	start semaphore to i value
					//	2d parameter = 0; only shared by threads in this process(class)
		{
			printf("Failed to initialize the semaphore %d in camera %d",Sem1,ID);
		}
		
	
		break;
	case 2:
		if (sem_init(&Sem2,0,i)==-1)	
					//	start semaphore to i value
					//	2d parameter = 0; only shared by threads in this process(class)
		{
			printf("Failed to initialize the semaphore %d in camera %d",Sem2,ID);
		}
		break;
	}
	
	
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::wait_Semaphore(int sem)
{
	
	switch(sem)
	{
	case 1:
		if (sem_wait(&Sem1)==-1)	
					//	decrease semaphore value
					
		{
			printf("Failed to lock or decrease the semaphore %d in camera %d",Sem1,ID);
		}else
		{
			//printf("camera: %d  Semaphore: %d\n",ID,Sem1);
		}
		
	
		break;
	case 2:
		if (sem_wait(&Sem2)==-1)	
					//	decrease semaphore value
		{
			printf("Failed to lock or decrease the semaphore %d in camera %d",Sem2,ID);
		}else{
			printf("camera: %d  Semaphore: %d\n",ID,Sem2);
			printf("No deberia entrar aqui\n");
		}

		break;
	}	
	
	
	//throw sem.count;
/*	
	catch(sem)
	{
		printf("the thread %id is blocked by a deadlock\n",ID);
	}*/
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void STREAM::ProcessFrame(unsigned framesize, TIME presentationTime)
{
unsigned long SSRC;	// Synchronization source = source of the images data	
			// variables to save NTP time in timestamps format
			// NTPmswrtp = NTPmsw*90000;
double NTPmswrtp,NTPlswrtp;
unsigned long R = 90000; // Clock for MPEG4 video, here it is 90000 units per second
struct timeval lastSR;
try{
	//dataBuffer.data = (unsigned char*)clientData;
	dataBuffer.size = framesize;
	
	//	we get the data // 
	//data_RTP.size = MP4Hsize + dataBuffer.size;				//	save MP4Header
	data_RTP.size = dataBuffer.size;					//	use an extradata global header previously got from 
										//	read the fmtp field in the SDP protocol.
	//	resize  buffer to save data
	//printf("%s\n","saved frame1");
   	memcpy(data_RTP.data,MP4H,MP4Hsize);				   	//	save frame in memory
	memmove(data_RTP.data + MP4Hsize, dataRTP, dataBuffer.size);
	memcpy(data_RTP.data, dataRTP, dataBuffer.size);
	//data_RTP.data=dataRTP;
	//printf("%s\n","saved frameFin");

										
	SSRC = Subsession->rtpSource()->lastReceivedSSRC();			// 	last SSRC received
	
	RTPSource* src = Subsession->rtpSource();				//	read NTP times
	RTPReceptionStats *Statistics = src->receptionStatsDB().lookup(SSRC);

	data_RTP.NTPmsw = Statistics->lastReceivedSR_NTPmsw();
	data_RTP.NTPlsw = Statistics->lastReceivedSR_NTPlsw();
	
	// convert NTP to timestamp units
	
	// convert NTP timestamp to usec
	unsigned int NTPusec = (double)(((double)data_RTP.NTPlsw*15625.0)/0x4000000);
	NTPmswrtp = (double)data_RTP.NTPmsw*90000;
	NTPlswrtp = (double)NTPusec*0.09;	// 90000/1000000 usec
	
	//printf("NTPmswrtp: %u\n",NTPmswrtp);
	//printf("NTPlswrtp: %u\n",NTPlswrtp);
	
	lastSR = Statistics->lastReceivedSR_time();


	if (data_RTP.NTPmsw != data_RTP.lastNTPmsw)
	{
				// convert NTPlsw timestamp to usec according to Spook formula
		//  its meaning is not clear yet, I think that it is to convert 
		//  to 32 bits format
		unsigned int usec2 = (double)(((double)data_RTP.NTPlsw*15625.0)/0x4000000);

		printf("SSRC: %u\n",SSRC);
		printf("NTP sec: %u.%06li\n",data_RTP.NTPmsw,usec2);
		printf("current timestamp: %u\n",data_RTP.timestamp);
		printf("Msr timestamp: %u\n",data_RTP.Msr);
		//printf("NTP usec: %06u\n",data_RTP.NTPlsw);	// print 32bits NTPlsw timestamp
		//printf("NTP usec: %06u\n",usec2);		// from spook code

		// calculate the Ts to syncronize the streams
		//printf("NTPmswrtp: %u\n",NTPmswrtp);
		//printf("NTPlswrtp: %u\n",NTPlswrtp);//NTPlswrtp +
		data_RTP.Ts = (double)((NTPmswrtp + (double)(data_RTP.timestamp - data_RTP.Msr))/R);
		printf("Ts mapped to remote site is: %u.%06u\n",data_RTP.Ts,NTPusec);
		
		
		//unsigned int usec= (double)( (double)lastSR.tv_usec * (double)0x4000000 ) / 15625.0;
		//unsigned int usec3 = (double)(((double)usec*15625.0)/0x4000000);
		
		//printf("last SR original time: %u.%06li\n",(lastSR.tv_sec + 0x83AA7E80),lastSR.tv_usec);
		//printf("last SR check time: %u.%06li\n",(lastSR.tv_sec + 0x83AA7E80),usec3);
		//printf("last SR 32bits time: %u.%06li\n",(lastSR.tv_sec + 0x83AA7E80),usec);
		
		//	0x83AA7E80 seconds is added to get the time since 1/01/1900 UNIX times
		//	see page 108 colin perkins's book and spook rtp.c code
 		data_RTP.time = timeNow();   //	capture time used as arrival time
		//	this timestamp is used as SR timestamp
		data_RTP.Msr = 	Subsession->rtpSource()->curPacketRTPTimestamp();				
	}
	//	save NTP times to see interval times of SR sended by the Spook Server

	data_RTP.lastNTPmsw = data_RTP.NTPmsw;
	data_RTP.lastNTPlsw = data_RTP.NTPmsw;
	data_RTP.timestamp = Subsession->rtpSource()->curPacketRTPTimestamp();	//	save timestamp

	//	print presentation Time
//	cout<<"presentation time:"<<presentationTime.tv_sec<<"."<<presentationTime.tv_usec<<" camera "<<ID<<endl;

	data_RTP.index= frameCounter;						//	time arrival
	//actualRTPtimestamp = 
	//printf("current RTP timestamp %lu\n",data_RTP.timestamp);
	//printf("Subsession ID: %s\n",Subsession->sessionId);
	/*if (dataBuffer.data == NULL)
	{ 
		printf("%s \n","data was not read");
	}else{
	*/	
		//printf("Data size: %i\n",dataBuffer.size);
		   //printf("%i\n",strlen(dataBuffer.data));
	//}
}
catch(...)
{
	printf("%s","there was an error accessing memory\n");	
}
		
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//	This method process the compressed frame, first it add an MPEG4 Header from RTP data obtained
//	from config line in Description response, then save the time arrival timestamp, increase a 
//	reference counter
void STREAM::SaveFrame(void *clientData, unsigned framesize)
{
	
	//temp = this;
	//->ProcessFrame(clientData,framesize);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//	FUNCTION AFTER READING DATA FROM REMOTE SERVER THROUGH RTP protocol
//void *clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds
 void STREAM::afterR(void *clientData,unsigned framesize,unsigned /*numTruncatedBytes*/,
				struct timeval presentationTime,unsigned /*durationInMicroseconds*/)
{
	
	//STREAM *ps = (STREAM*)clientData;
	//ps = ps->
	//bind_object();	// not required now, early way of assign a correct object to call
				//  their methods
	
	unsigned int maxSize = RTPDataSize;		//	max size of the got frame
	//test size of frame
	if(framesize >= maxSize)				//	maxf
	{
		printf("framesize: %d is greater that maxRTPDATASize: %i\n",framesize,maxSize);
	}else{
	       //printf("Data read is: %d\n",framesize);
		
		
		//ps->ProcessFrame(framesize);
		ProcessFrame(framesize,presentationTime);
		//readOKFlag = ~0;//~0
		//ps->readOKFlag = ~0;			//	set flag to new data   before ~0
		//STREAM *ps= (STREAM*)temp;		// 	Auxiliar object to make reference to the actual 
							//	being used
		//ps->ProcessFrame(clientData,framesize);
		//printf("%s\n","OK");
		
	       }
	readOKFlag = ~0;//~0	
	//ps->readOKFlag=~0;
	      // delete Data;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	function to execute when the RTP is closed

void STREAM::onClose(void* clientData)
{
	//STREAM *ps =(STREAM*)clientData;
	//ps->readOKFlag = ~0;	//	set flag to new data
	readOKFlag=~0;//~0


}
////////////////////////////////////////////////////////////////////////////////////////////////////
//	function to check readOKflag when receiving frames, it allows to run other task
//	while execute other tasks

void  STREAM::checkFunc(void* clientData)
{
	//STREAM *ps =(STREAM*)clientData;
	//ps->readOKFlag = ~0;	//	set flag to new data
	readOKFlag= 0;//~0


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
				printf("%s", "PLAY command sent\n");
				
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
// This function will be used as callback function in rtsp_getNextFrame
// because their signature is like the signature required by the live555 library
void Zclose(void* clientData)
{
	STREAM *ps = (STREAM*)clientData;
	
	closeFunctor<STREAM> *close = new closeFunctor<STREAM>;
	close->setClass(ps);
	close->setMethod(&STREAM::onClose);
	TFunctorClose *A = close;
	A->method(clientData);

}
////////////////////////////////////////////////////////////////////////////////////////////////////
// This function will be used as callback function in rtsp_getNextFrame
// because their signature is like the signature required by the live555 library
void ZcheckFlag(void* clientData)
{
	STREAM *ps = (STREAM*)clientData;
	
	closeFunctor<STREAM> *close = new closeFunctor<STREAM>;
	close->setClass(ps);
	close->setMethod(&STREAM::checkFunc);
	TFunctorClose *A = close;
	A->method(clientData);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
// This function will be used as callback function in rtsp_getNextFrame
// because their signature is like the signature required by the live555 library
void Zread(void *clientData,unsigned framesize,unsigned /*numTruncatedBytes*/,
				struct timeval presentationTime,unsigned /*durationInMicroseconds*/)
{
try{
	unsigned a,b;
	STREAM *ps = (STREAM*)clientData;
	//	pointer to member function
	void (STREAM::*pmf)(void *clientData,unsigned framesize,unsigned /*numTruncatedBytes*/,
				struct timeval presentationTime,unsigned /*durationInMicroseconds*/);
	//	assign the member function to call
	pmf = &STREAM::afterR;
	//	call the member function with parameters
	(ps->*pmf)(clientData,framesize, a, presentationTime, b);
}
catch(...)
{
	cout<<"error on reading frame"<<endl;

}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//int rtsp_getFrame(unsigned char *image)
int STREAM::rtsp_getFrame()
{
	
try{
	//unsigned Var,Var2;
	
	onRead = Zread;			//	assign functions to the pointers
	onClosing = Zclose;		//	to be used as callbacks functions
	onCheckFunc = ZcheckFlag;		//	
	//	get the data from rtp source

	//	IMPORTANT: the this pointer is very important because is passed as the afterGettingData parameter
	//	if not is used in this form the program will not work!

	//	the data is saved in the dataRTP buffer
	Subsession->readSource()->getNextFrame(dataRTP,maxRTPDataSize,
								onRead,(void*)this,	
								onClosing ,(void*)this);//this		

	//	wait until the data is available, it allows for other task to be performed
	//	while we wait data from rtp source					                                                        	
	//	HERE WE GET THE FRAME FROM REMOTE SOURCE
	//readOKFlag = 0;	
						 //	schedule read                         	
	TaskScheduler& scheduler = Subsession->readSource()->envir().taskScheduler();//&
 	/*
	unsigned usecsToDelay = 10; // 1 ms
     	scheduler.scheduleDelayedTask(usecsToDelay,
                                             onCheckFunc,(void*)this);
	*/
	readOKFlag = 0;	
	scheduler.doEventLoop(&readOKFlag);		// waits for the frame
	//printf("readOKFlag is: %i\n",readOKFlag );
	//timeNow();
	frameCounter++;					//	increase frame counter
	//usleep(delay);
}
catch(...)
{
	cout<<"error reading frame from camera"<<ID<<endl;
}	
	
	
	return 0;	//	exit with sucess
}
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
int STREAM::rtsp_decode(Frame dataCompress)
{
	//	decode the video frame using libavcodec
try{
	//printf("%s\n","decode frame");
	decodeOK = avcodec_decode_video(pCodecCtx,pFrame,&frameFinished,dataCompress.data,dataCompress.size);
		
	//if(decodeOK!=0)
		//{printf("%s\n","error in decoding");}
	//else{
	//	if we get a frame then convert to RGB
		if(frameFinished)
		{
		//	cut the left side of the image, so 720-512 = 208 
		//	cut the top side of the image,	nothing by that 0
		//	if (ID==0)		//	flag for cut image
		//	{
			//	img_crop((AVPicture*)pFrameCrop,(AVPicture*)pFrame,pCodecCtx->pix_fmt,0,208);//208
	//			img_convert((AVPicture*)pFrameRGBA,PIX_FMT_RGB24,(AVPicture*)pFrameCrop,pCodecCtx->pix_fmt,720,576);//RGB24
	//		}else{

		//	convert the image from its format to RGB
			img_convert((AVPicture*)pFrameRGBA,PIX_FMT_RGB24,(AVPicture*)pFrame,pCodecCtx->pix_fmt,720,576);//RGB24
	//		}	
			//pCodecCtx->width,pCodecCtx->height);
			//	save to a buffer
			data_RTP.image = pFrameRGBA->data[0];	//	save RGB frame
			data_RTP.pFrame = pFrameRGBA;		//	save frame to export

			//printf("frame %d from camera %d was decoded\n",frameCounter,ID);
			//	get the timestamp of the frame
			//break;
		}
		else{
			//frameCounter++;	
			printf("there was an error while decoding the frame %d in the camera %d\n",frameCounter,ID);
			data_RTP.pFrame = pFrameRGBA;
			//throw pFrameRGBA;
		//}
		}
}
catch(...)
{	
	cout<<"frame are not available\n";
}		
	//}
	return 0;

}
////////////////////////////////////////////////////////////////////////////////////////////////////
//		this function start to get n frames: the compressed frames
//		this acts like a buffer to save tha last frame get it and to read and decode 
//		one appropiate frame

//		At start it can save n frames to be used like a buffer to let smooth the rate
//		of showing frames  and too to decrease the network jitter of packets
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
	try{
		STREAM *pS = (STREAM*)pthis;	//	convert to class Stream to allow correct thread work
		pS->rtsp_getData();
		//return 0;
	}
	catch(...)
	{
		printf("%s"," Something wrong happened with the thread ");
	}
}

//////////////////////////////////////////
//	Getting data thread function

void STREAM::rtsp_getData()
{
	int cod;
	double s;

try{			
	while(1)			//	threads are always in execution
	{
		//usleep(delay);	//	delay
 	if (m_global_flag==0)
	{
		wait_Semaphore(1);
	}
	
	//	LOCK THE RESOURCE
		cod = lock_mutex();
		                           
		if (cod!=0)	
			{
				printf("%s\n","Error locking get RTP data");
			}
		else{
	
		//condition
			
			rtsp_getFrame();
			//printf("start decoding, camera %d\n",ID);
			//timeNow();		//	true capture time
			rtsp_decode(data_RTP);
			//timeNow();
			//printf("finish decoding, camera %d\n",ID);		
			//sem_post(&sem);
			
			


			if(InputBuffer.empty() | InputBuffer.size()>= 1 )
			{
				InputBuffer.push_back(data_RTP);	//	save data
				//printf("writing frame %d from the camera %d \n",frameCounter,ID);
				//set_Semaphore();			//	increase the semaphore
			}

			if(!InputBuffer.empty())
				{
				
				dataFrame N = data_RTP;
				dataFrame N_1 = InputBuffer.back();
				s = skew(N,N_1);
				//printf("skew : %06f camera %d\n",s,ID);
				//s= skew(Temp,InputBuffer.back());
				
				}
			
			//	limit the size of FIFO buffer
			
			
			if(InputBuffer.size() >= 1)
			{ 
				InputBuffer.pop_front();		//	delete head frame in th FIFO
				//wait_Semaphore();			//	decrease semaphore
			}
			
			//counter++;
			//T.push(counter);
			//printf("writing frame %d from the camera %d \n",frameCounter,ID);
		//	printf("FIFO size: %d from camera %d\n",InputBuffer.size(),ID);
		}
		cod = unlock_mutex();
		if (cod!=0)	
			{printf("%s\n","Error unlocking get RTP data");}
		//	WAKE UP THE OTHER THREAD: SHOW DATA THREAD
			
			//sem_post(&sem);
	
	//	 	set_Semaphore();		//	send signal
			
		//}
		
	
	 if(m_global_flag==0)
		{
			set_Semaphore(1);
		} 
	} 
	//return 0;
}
catch(...)
{
	cout<<"error obtaining frames from camera:"<<ID<<endl;
}	
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//	this function get an image from remote server
int STREAM::Init_Session(char const *URLcam, int id)
{
	int status;
	URL = URLcam;
	ID=id;
try{
	////////////////////////////////////////////////////////////////////////
	//	start the codecs	
	initCodecs();					//	init codecs
	////////////////////////////////////////////////////////////////////////
	//	Start threads and semaphores
	//	start semaphores with a  value of 1 
	init_Semaphore(1,1);				//	writing semaphore
	//init_Semaphore(2,1);				//	reading semaphore
	init_mutex();					//	start mutex for exclusion of buffers
	
	//int cod;					//	not used			
	//cod = pthread_mutex_init(&mutexFrame,0);
		
	//	connect to RTSP server

	status = rtsp_Init();			//	Init RTSP Clients for the camera
	//throw status;
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
catch(int status)
{
	cout<<"Video not available, review if the server is working\n"<<endl;

}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
Export_Frame STREAM::getImage()//dataFrame
{
Export_Frame I_Frame;
int cod;
	
if (m_global_flag == -1)
{
		m_global_flag = 0;
}

if (m_global_flag==0)
{	
  			
	wait_Semaphore(1);				//	look for semaphore, test if it is in green to get the image		
							//	from the network
	//Export_Frame I_Frame;
	char *ID_cam[2]={"LEFT","RIGTH"};
 	
	//	LOCK THE RESOURCE
/*	cod = lock_mutex();
		                           
if (cod!=0)	
{
	printf("%s\n","Error locking get RTP data");
}
else{	
*/

	if(!InputBuffer.empty())			//	check if buffer is not empty
	{
					
		ReceivedFrame = InputBuffer.front();	//	get the frame from the FIFO buffer 

		I_Frame.pData = ReceivedFrame.pFrame;	//	save image within a frame structure
		I_Frame.h=720;				//	width of image
		I_Frame.w=576;				//	height of image
 	
		InputBuffer.pop_front();		//	delete the head frame from the FIFO buffer
		
		//printf("FIFO size: %d\n",InputBuffer.size());
		//printf("FIFO size: %d from camera %d\n",InputBuffer.size(),ID);
		
		
	}
	else
	{
	//	printf("Empty buffer from  %s camera, Size = %d\n",ID_cam[ID],InputBuffer.size());
		I_Frame.pData = pFrameRGBA;		//	to avoid empty frame
	}	
/*
}
cod = unlock_mutex();
if (cod!=0)	
{printf("%s\n","Error unlocking get RTP data");}
 */	
	
	set_Semaphore(1);		//	send signal increase semaphore
}
		//return ReceivedFrame.image;				//	return the last frame

	 return I_Frame;
	
}
TIME getTime()
{
	//timeval t;
	TIME t;
	struct timezone tz;
	int time;
	//time = ntp_gettime(&ntpTime);
	time = gettimeofday(&t,&tz);
	if (time==0)
	{
		//printf("time of arrival was:%li.%06li\n",ntpTime.time.tv_sec,ntpTime.time.tv_usec);
		//printf("time of timer was:%li.%06li\n",t.tv_sec,t.tv_usec);
	//	 printf("time in coin was:%li.%06li\n",t.tv_sec,t.tv_usec);
		//time = t.tv_sec + (t.tv_usec/1000000);
		//printf("time was:%li.%06li\n",time);
		//return t;
	}else{
		printf("error was:%i\n",errno);
		
	}
	return t;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	update function show the frame from cameras

void updateR(void *data,SoSensor*)	//	this function updates the texture based on the frame got
					//	from the video stream
{
	long int AR,BR,diffR;//,fpsR;
	
	Export_Frame Fr;		//	struct for save the frame
try{
	
	SoTexture2 *rightImage = (SoTexture2*)data;
	//	calculus for get the video frames rate per second
	/////////////////////////////////////////////////////////////////////////////////////////
	if (FirstR==0)
	{
		t1R = getTime();
		FirstR=-1;
	}
	else{
	t2R = getTime();
	AR = (t2R.tv_sec - t1R.tv_sec);		//	seconds
	BR = (t2R.tv_usec - t1R.tv_usec);	//	microseconds

	diffR =(AR*1000000 + BR);			//	get the difference in microseconds
	//printf("the difference in time was sec:%li ,usec:%li, diff:%li\n",A,B,diffL);
	fpsR = 1000000/(diffR);			//	fps
	((SoSFFloat*)SoDB::getGlobalField("FpsR"))->setValue(fpsR);
	t1R = t2R;				//	update last time for next frame arrival
	}
	/////////////////////////////////////////////////////////////////////////////////////////

	// return a structure that contain the image, size, width and height
	Fr=C2->Execute();	//	call getImage() using a functor	
	rightImage->image.setValue(SbVec2s(512,512),3,Fr.pData->data[0]);// 3 components = RGB, 4 = RGBA
		
	//printf("fps %03f from right camera \n",fpsR);
		

	}
catch(...)
{
	cout<<"error not viewer";
}	

}
///////////////////////////////////////////////////////////////////////////////////////////////////
//	update function show the frame from cameras

void updateL(void *data,SoSensor*)	//	this function updates the texture based on the frame got
					//	from the video stream
{
	//	get the video frames
	long int AL,BL,diffL;//fpsL
	//float diffL;
	Export_Frame FrL,FrR;		//	struct for save the frame
try{
	//SoTexture2 *leftImage = (SoTexture2*)data;
	SoStereoTexture *Stereo = (SoStereoTexture*)data;
	//	calculus for get the video frames rate per second
	/////////////////////////////////////////////////////////////////////////////////////////
	/*
	if (FirstL==0)
	{
		t1L = getTime();
		FirstL=-1;
	}
	else{
	t2L = getTime();
	AL = (t2L.tv_sec - t1L.tv_sec);		//	seconds
	BL = (t2L.tv_usec - t1L.tv_usec);	//	microseconds

	diffL =(AL*1000000 + BL);		//	get the difference in microseconds

	//printf("the difference in time was sec:%li ,usec:%li, diff:%li\n",A,B,diffL);

	fpsL = 1000000/(diffL);			//	fps
	((SoSFFloat*)SoDB::getGlobalField("FpsL"))->setValue(fpsL);
	//FpsL->setValue(fpsL);
	t1L = t2L;				//	update last time for the next frame arrival
	}
	*/
	/////////////////////////////////////////////////////////////////////////////////////////
	// return a structure that contain the image, size, width and height
	FrL=C1->Execute();
	if (FrL.pData->data[0]!=NULL)
	{
		Stereo->imageL.setValue(SbVec2s(720,576),3,FrL.pData->data[0]);
	}else
	{
		printf("%s\n","There aren't image from left buffer");
	}
	FrR=C2->Execute();			//	call getImage() using a functor
	if (FrR.pData->data[0]!=NULL)
	{
		Stereo->imageR.setValue(SbVec2s(720,576),3,FrR.pData->data[0]);
	}else
	{
		printf("%s\n","There aren't image from right buffer");
	}
	//Stereo->imageL = Fr.pData->data[0];	
	//leftImage->image.setValue(SbVec2s(512,512),3,Fr.pData->data[0]);// 3 components = RGB 4 = RGBA
		
	//printf("fps %f from left camera \n",fpsL);
	//timeNow2();
	
	
}
catch(...)
{
	cout<<"error not viewer";
}	
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//		MAIN PROGRAM
//		user input: .\client rtsp://sonar:7070/cam1 rtsp://sonar:7070/cam2 IOD

/**
 * 
 * @param argc 
 * @param argv rtsp://sonar:7070
 * @return 
 */
int main(int argc,char **argv)
{
try{	
	IMAGE set_format;					/*	set the format to use in the image
									there are 2 formats:
									CUT_IMAGE 
									where the left part of the image
									is cutted, and
									NORMAL_IMAGE 
									where the image kept the original size
								*/	

	//;	channel 0 and 3
	//;
	const char *camL ="rtsp://sonar:7070/cam3";	//argv[1];//	
	const char *camR ="rtsp://sonar:7070/cam0";	//argv[2];	
	float IOD = 1.5;//atof(argv[3]);

	STREAM camara1;						//  	create an stream object
	
	myfunctor<STREAM> D1(&camara1,&STREAM::getImage);	//	this is a functor, some as a pointer to 
								//	a member function, here points to the
								//	getImage function of camara1 object
	C1 =&D1;						//	C1 is a abstract class base for myfunctor class
	//set_format=CUT_IMAGE;					//	with a virtual method to overload
	set_format=LEFT_IMAGE;
	camara1.Init_Session(camL,set_format);			//	start connection with url
								//	set the format of the image
								//	for the camera 
	STREAM camara2;
	myfunctor<STREAM> D2(&camara2,&STREAM::getImage);	//	do the same for the right camera
	C2 =&D2;
	set_format=RIGHT_IMAGE;
	camara2.Init_Session(camR,set_format);

	// ******************************************************************************************** 
	//		Graphics Scene
	// Initializes SoQt library (and implicitly also the Coin and Qt
    	// libraries). Returns a top-level / shell Qt window to use.
    	//QWidget * mainwin = SoQt::init(argc, argv, argv[0]);
	QWidget * mainwin = SoQt::init(argv[0]);
	if (mainwin==NULL) exit(1);
	
	//	initialize the new nodes, required
	SoStereoTexture::initClass();

	//	create global variable to show a text with the fps value for each stream
/*
	SoSFFloat *FpsL = (SoSFFloat*)SoDB::createGlobalField(SbName("FpsL"),SoSFFloat::getClassTypeId());
	SoSFFloat *FpsR = (SoSFFloat*)SoDB::createGlobalField(SbName("FpsR"),SoSFFloat::getClassTypeId());
*/	
	SoSeparator *root = new SoSeparator;
    	root->ref();

	//	add  camera and lights
/*
	SoPerspectiveCamera *mycam = new SoPerspectiveCamera;
	SoDirectionalLight *Light = new SoDirectionalLight;

	root->addChild(mycam);
	root->addChild(Light);
*/
	//	add Stereo node
	
	SoStereoTexture *Stereo = new SoStereoTexture;
	Stereo->width.setValue(720);
	Stereo->heigh.setValue(576);
	Stereo->IOD.setValue(IOD);

	root->addChild(Stereo);


	
//	Add the transparency node, used to create the augmented reality appareance
	
	// read the tx90 model

/*
	SoInput TX90;
	SoSeparator *tx = new SoSeparator;
		
	if (TX90.openFile("/home/portilla/Tesis/tx90.iv"))
	{
		tx = SoDB::readAll(&TX90);
	}
*/
	
    
	//	MAKE A PLANE FOR IMAGE 
/*
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

	SoFont *font = new SoFont;			//	create a font for the texts 
	font->name.setValue("Arial:Bold Italic");
	font->size.setValue(20.0);
	root->addChild(font);
*/
	//	LEFT PLANE
/*	
	SoSeparator *leftPlane = new SoSeparator;
	
	//	ADD THE IMAGE FROM THE FIRST CAMERA
	SoTexture2  *leftImage = new SoTexture2;
	leftImage->filename.setValue("");	// this set is for use an image from memory in place of a file 
	
	SoTransform *leftTransform = new SoTransform;
	leftTransform->translation.setValue(-256,0.0,0.0);
	leftPlane->addChild(leftTransform);
	leftPlane->addChild(leftImage);
	leftPlane->addChild(plane);
	
	SoBaseColor *F1 = new SoBaseColor;
	F1->rgb.setValue(1.0,0.0,0.0);	
	leftPlane->addChild(F1);
	/*
	SoAsciiText *FPSL = new SoAsciiText;
	FPSL->justification.setValue("LEFT");
	FPSL->spacing = 1;
	FPSL->width =80;
	
	SoText2 *FPSL = new SoText2;
	//FPSL->string = 'li%fpsL';
	//FPSL->string= "esto es una prueba";
	FPSL->string.connectFrom(FpsL);		//	connect from global frame Left variable
	leftPlane->addChild(FPSL);		//	add text on fps for this camera
	
	//	RIGHT PLANE
/*
	SoSeparator *rightPlane = new SoSeparator;

	//	ADD THE IMAGE FROM THE SECOND CAMERA
	SoTexture2 *rightImage = new SoTexture2;
	rightImage->filename.setValue("");

	SoTransform *rightTransform = new SoTransform;
	rightTransform->translation.setValue(256,0.0,0.0);
		
	rightPlane->addChild(rightTransform);
	rightPlane->addChild(rightImage);
	rightPlane->addChild(plane);

	//	set color of text
	SoBaseColor *F2 = new SoBaseColor;
	F2->rgb.setValue(1.0,0.0,0.0);	
	rightPlane->addChild(F2);

	SoText2 *FPSR = new SoText2;
	FPSR->string.connectFrom(FpsR);
	rightPlane->addChild(FPSR);		//	add text on fps for this camera
	
	//	ADD THE TWO PLANES

	root->addChild(leftPlane);		// 
	root->addChild(rightPlane);
*/
	//****************************************************************************
	//	setup timer sensor for recursive image updating 



	SoTimerSensor *timerL = new SoTimerSensor(updateL,Stereo);//leftImage
	timerL->setBaseTime(SbTime::getTimeOfDay()); 	//	useconds resolution
	timerL->setInterval(1.0/25.0);//	 	//	interval 40 ms = 25fps
	timerL->schedule();				//	enable timer		


/*
	SoTimerSensor *timerR = new SoTimerSensor(updateR,rightImage);//
	//
	timerR->setBaseTime(SbTime::getTimeOfDay());	//	useconds resolution
	timerR->setInterval(1.0/25.0);			//	set interval 40 ms = 25fps
	timerR->schedule();				//	enable timer
	
	//****************************************************************************

	/*
	SoTransform *t1 = new SoTransform;
	t1->translation.setValue(-128,0,0);
	root->addChild(t1);
	
	SoCube *cube = new SoCube;
	cube->width = 200;
	cube->height = 200;
	root->addChild(cube);
	*/

/*
	SoTransparencyType *Trans1 = new SoTransparencyType;
	Trans1->value.setValue(SoTransparencyType::ADD);
	SoMaterial *mat = new SoMaterial;
	mat->transparency.setValue(1.0);
	mat->diffuseColor.setValue(1.0,0.0,0.0);
	SoMaterial *mat2 = new SoMaterial;
	mat2->diffuseColor.setValue(1.0,0.0,0.0);

	SoCube *Cube = new SoCube;
	Cube->height.setValue(600.0);
	Cube->width.setValue(2.0);
	Cube->depth.setValue(0.5);

	SoCube *CubeR = new SoCube;
	CubeR->height.setValue(1000.0);
	CubeR->width.setValue(2.0);
	CubeR->depth.setValue(0.5);
	SoCylinder *cyl = new SoCylinder;
	cyl->radius.setValue(68.0f);
	cyl->height.setValue(200.0);
*/	
	//SoSeparator *OverlayL = new SoSeparator;
	//SoPerspectiveCamera *CamL = new SoPerspectiveCamera;

	//	set camera position to parallel configuration

	//CamL->position.setValue(-256.0,0.0,0.0);
	//CamL->nearDistance.setValue(50.0);
	//CamL->farDistance.setValue(300.0);
	//OverlayL->addChild(CamL);
	//SoBaseColor *Color1 = new SoBaseColor;
	//Color1->rgb.setValue(0.0,1.0,0.0);


/*

	SoSeparator *OverlayL = new SoSeparator;
	SoTransform *T1 = new SoTransform;
	SoTransform *T2 = new SoTransform;
	//T1->translation.setValue(300.0,-107.0,0.0);

	T1->translation.setValue(-190.0,-140.0,60.0);
	//T1->rotation.setValue(0.0,0.0,1.0,0.0);
	T2->translation.setValue(-150.0,-30.0,0.0);
	T2->rotation.setValue(SbVec3f(0.0,0.0,1.0),-75*(M_PI/180.0));	
	//	watch out with quaternion constructors, use this form
	SoRotation *rl = new SoRotation;
	SoRotation *rl2 = new SoRotation;
	//rl->rotation.setValue(SbVec3f(-30.0,4.0,25.0),-25*(M_PI/180));
	//rl2->rotation.setValue(1.0,1.0,0.0,-45*(M_PI/180));
	OverlayL->addChild(Trans1);
	OverlayL->addChild(mat);	//	transparency
	OverlayL->addChild(T1);
	OverlayL->addChild(cyl);
	
	OverlayL->addChild(mat2);	//	normal color
	OverlayL->addChild(T2);
	OverlayL->addChild(rl);
	//OverlayL->addChild(rl2);
	OverlayL->addChild(Cube);
	//	right side

	SoSeparator *OverlayR = new SoSeparator;
	SoTransform *T3 = new SoTransform;
	SoTransform *T4 = new SoTransform;
	//T1->translation.setValue(300.0,-107.0,0.0);
	T3->translation.setValue(400.0,-140.0,60.0);
	//T1->rotation.setValue(0.0,0.0,1.0,0.0);
	T4->translation.setValue(-248.0,-63.0,0.0);
	T4->rotation.setValue(0.0,0.0,1.0,-75*(M_PI/180));
	OverlayR->addChild(Trans1);
	OverlayR->addChild(mat);	//	transparency
	OverlayR->addChild(T3);
	OverlayR->addChild(cyl);
	
	OverlayR->addChild(mat2);	//	normal color
	OverlayR->addChild(T4);
	OverlayR->addChild(CubeR);

//	root->addChild(OverlayL);
//	root->addChild(OverlayR);

*/

//	SoTransform *tRobot = new SoTransform;
	//SoTransform *r1 = new SoTransform;
	//tRobot->translation.setValue(-100.0,-100.0,0.0);
//	tRobot->rotation.setValue(1.0,0.0,0.0,180*(M_PI/180));
//	r1->rotation.setValue(0.0,1.0,0.0,30*(M_PI/180));
//	root->addChild(tRobot);
	//root->addChild(r1);
//	root->addChild(tx);			// load robot model
/*
	//	CAMERA TEST
	//	set camera position to parallel configuration
	SoPerspectiveCamera *CamL = new SoPerspectiveCamera;
	CamL->position.setValue(-256.0,0.0,0.0);
	CamL->nearDistance.setValue(50.0);
	CamL->farDistance.setValue(300.0);

	root->addChild(CamL);

	SoCone *c1 = new SoCone;
	//c1->height.setValue(20.0);
	//c1->radius.setValue(25.0);

	SoSeparator *S1 = new SoSeparator;
	S1->addChild(c1);
	//root->addChild(c1);
*/	
/*
     	SoTransform *myTrans = new SoTransform;
	root->addChild(myTrans);
   	myTrans->translation.setValue(0.0,50.0,200.0);
*/
/*	
	SoQtRenderArea *render =new SoQtRenderArea(mainwin);
	mycam->viewAll(root,render->getViewportRegion());
	render->setSceneGraph(root);
	render->setQuadBufferStereo(TRUE);
	render->setBackgroundColor(SbColor(0.0f,0.5f,0.5f));
	render->show();
	SoQt::show(mainwin);
	SoQt::mainLoop();
	delete render;
*/
	
	SbColor color(10, 0.5,0.5);
	// Use one of the convenient SoQt viewer classes.

	SoQtExaminerViewer * eviewer = new SoQtExaminerViewer(mainwin);
   	eviewer->setSceneGraph(root);
//	eviewer->setStereoViewing(TRUE);
//	eviewer->setQuadBufferStereo(TRUE);	//	set stereo mode = QUAD BUFFER = page flipping

/*
	eviewer->setOverlaySceneGraph(S1);
	eviewer->setOverlayColorMap(1,1,&color);
	eviewer->setTitle("Overlay Plane");
*/
	/*
	//***********************************************
	//	test for rendering bottleneck
	//***********************************************
	SoSwitch *renderOff = new SoSwitch;
	renderOff->ref();
	renderOff->addChild(root);
	eviewer->setSceneGraph(renderOff);
	//***********************************************
	*/

    	eviewer->show();
	// Pop up the main window.
    	SoQt::show(mainwin);
    	// Loop until exit.
    	SoQt::mainLoop();
	delete eviewer;

	// 			Clean up resources.				  
    	
    	root->unref();
		
return 0;			// from main function
}
catch(exception& e)
{
 	cout<<e.what()<<endl;


}
catch(bad_exception& e)
{
	cout<<"error"<<e.what()<<endl;

}  
catch(...)
{
	cout<<"unknow error\n"<<endl;
}
}	



