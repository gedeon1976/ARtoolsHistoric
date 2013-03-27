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

// include rtsp class header
#include "H264_rtspServer.h"

// auxiliar functions

// this function is called as a callback in the startPlaying
// function from a RTP sink
void afterFunction(void* clientData)
{
  try{
    H264_rtspServer *rtspLocal = (H264_rtspServer*)clientData;
    printf("calling after function\n");
    //rtspLocal->play(0);
    
  }catch(...){
  }  
}

// after play function
void afterPlaying(void* dataClient)
{
 try{
	H264_rtspServer *rtsp = (H264_rtspServer*)dataClient;
	printf("calling after playing \n");
	rtsp->stopPlay();
	int i = 0;
	rtsp->play(i);	
		
    }catch(...){
    }  
}

// play forward definition
void play(void* clientData);
// constructor
H264_rtspServer::H264_rtspServer()
{
    // set ports and maximum jumps for RTP protocols
    isRTSPServerStarted = false;
    rtpPortNumBase = 18888;
    ttl = 255;
    rtspPort = 8554;   
    functionCallcounter = 0;
    frameCounter = 0;
    nextStreamNAL = false;
    isEventLoop = false;
    // init semaphore
    init_semaphore(1,1);
    semaphores_global_flag = -1;
    // register the own structure types to be used in signal/slot mechanism
    qRegisterMetaType<pictureFrame>();
    qRegisterMetaType<H264Frame>();
    
}

// destructor
H264_rtspServer::~H264_rtspServer()
{

}

// set Name for this stream
void H264_rtspServer::setName(std::string name)
{
    // set the name for this stream
    StreamName = name;
}

// set ID for this stream
void H264_rtspServer::setID(int StreamID)
{
    // set the stream ID
    ID = StreamID;
}

// get the ID for this stream
int H264_rtspServer::getID(void )
{
    int currentID = ID;
    return currentID;
}

// set port for rtsp server
void H264_rtspServer::setPort(int Port)
{
    // set the corresponding port
    rtspPort = Port;
}

// get the frames from camera inputs
void H264_rtspServer::getFrames()
{
  try{
      AVPacket currentFrame;
      // keeps the thread alive
      while(1){
	// check global class semaphore
	if(semaphores_global_flag==0){
	  // set semaphore
	 // wait_semaphore(1);
	}  
	  // get the data	  
	  
	  printf("start thread RTSP server\n");
	  AddRTSPSession();
	  printf("end thread RTSP server\n");	  
	  
	if(semaphores_global_flag==0){
	  // free the semaphore
	  //set_semaphore(1);
	}	  
      }
     
  }
  // catch to get the thread cancel exception
  catch (abi::__forced_unwind&) {
	throw;
  } catch (...) {
    // do something
    printf("RTSP thread error\n");
    }
    
}

// get the encoded frames from external threads or sources
void H264_rtspServer::getEncodedFrames(H264Frame encodedFrame)
{
  try{
      int maxSize = 10;
      AVPacket pktFrame = encodedFrame.frame;
      int camID = encodedFrame.camera_ID;
      videoGeneralIndex = camID - 1;
      
      if (semaphores_global_flag==-1){      
	  semaphores_global_flag=0;
	  printf("setting semaphores global flag to 0\n");
      }
      if (semaphores_global_flag==0){
	
	wait_semaphore(1);
	// save the frame to the corresponding camera buffer
	H264CamerasBufferList.at(videoGeneralIndex).push_back(pktFrame);
	if (H264CamerasBufferList.at(videoGeneralIndex).size()> maxSize){
	    H264CamerasBufferList.at(videoGeneralIndex).pop_front();	// delete the first received frame
	    printf("camera %d H264 coded Buffer size is %d\n",camID,H264CamerasBufferList.at(videoGeneralIndex).size());
	}
	// set signal for new data from camera
	if ((NAL_Source!= NULL)&(isEventLoop==true)){
	    
	    int nal_start,nal_end;
	    int  NAL_length;
	    uint8_t* NAL_buffer;
	    h264_stream_t* h = h264_new(); 
	    int bufSize = encodedFrame.frame.size;
	      find_nal_unit(encodedFrame.frame.data,bufSize,&nal_start,&nal_end);
	    NAL_length= read_nal_unit(h,&encodedFrame.frame.data[nal_start],nal_end-nal_start);
	    printf("NAL unit size = %d  end-start: %d\n",NAL_length,nal_end-nal_start );
	    
	    // read dataClient
	    //debug_bytes(encodedFrame.frame.data,50);
	    
	    // send data to the RTP videoSource
	    BlueCherrySource::signalNewDataSource(NAL_Source,encodedFrame);
	    nextStreamNAL = true;	
	    readOKFlag = 0;
	}
	set_semaphore(1);
      }
      
    
  }catch(...){
    
  }

}

// get the current data from the Bluecherry cards
cameraCodedBufferList H264_rtspServer::getH264camerasBuffer(void)
{
    try{
	cameraCodedBufferList currentH264data;
	if (semaphores_global_flag==0){
	    wait_semaphore(1);
	    currentH264data = H264CamerasBufferList;	
	    set_semaphore(1);
	}
	return currentH264data;
	
    }
    catch(...){
    }
}
                                                                      
// add a RTP session to manage each camera input
void H264_rtspServer::AddRTSPSession(void)
{
    try{     
     
      if (!isRTSPServerStarted){
	// Begin by setting up our usage environment:
	isRTSPServerStarted = true;
	
	scheduler = BasicTaskScheduler::createNew();
	env= BasicUsageEnvironment::createNew(*scheduler);
      
	// create groupsocks for RTP and RTCP
	destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
	// Note:: this is a multicast address
       
	// create a RTSP server     
	Port outPort(rtspPort);
	rtspServer = RTSPServer::createNew(*env,outPort);
	if (rtspServer == NULL) {
	  *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
	  exit(1);
	}
	            
	// add camera to the buffer
	codedFrameBuffer tmpFrame;
	H264CamerasBufferList.push_back(tmpFrame);
      
	unsigned short rtpPortNum;
	unsigned short rtcpPortNum;
	RTPSink *videoSink;			// RTP sink         
      
	rtpPortNum = rtpPortNumBase + 2*ID;
	rtcpPortNum = rtpPortNum + 1;
      
	Port rtpPort(rtpPortNum);
	Port rtcpPort(rtcpPortNum);
      
	// sockets groupsock 
	Groupsock rtpGroupsock(*env,destinationAddress,rtpPort,ttl);
	rtpGroupsock.multicastSendOnly(); // we're a SSM source
	Groupsock rtcpGroupsock(*env,destinationAddress,rtcpPort,ttl);
	rtcpGroupsock.multicastSendOnly();// we're a SSM source
      
	// create a video RTP sink from the rtpGroupsock
      
	// Add SPS and PPS NAl units information to proper RTSP stream setup
      
	// define SPS NAL obtained through debug_bytes(h264bitstream library) of codec->extradata
	const u_int8_t sps[] = {0x67,0x4D,0x40,0x1E,0xDA,0x02,0xC0,0x49,0xA1,0x00,0x00,0x03,0x00,0x01,0x00,0x00,0x03,0x00,0x32,0x0F,0x16,0x2E,0xA0};
	unsigned spsSize = sizeof(sps);
      
	// define PPS NAL
	const u_int8_t pps[] ={0x68,0xCF,0x06,0xCB,0x20};
	unsigned ppsSize = sizeof(pps);
      
	OutPacketBuffer::maxSize = 100000;      
	videoSink = H264VideoRTPSink::createNew(*env,&rtpGroupsock,96,sps,spsSize,
					      pps,ppsSize);
      
	// create and start a RTCP instance  for this RTP sink
	unsigned estimatedSessionBandwidth = 500;// in kbps; for RTCP b/w share
	unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen+1];
	gethostname((char*)CNAME,maxCNAMElen);
	CNAME[maxCNAMElen]='\0';			// just in case
      
	RTCPInstance* rtcp = RTCPInstance::createNew(*env,&rtcpGroupsock,
						   estimatedSessionBandwidth,
						   CNAME,videoSink,NULL/*we're a server*/,
						   True /* we're a SSM source */);
	// Note: This starts RTCP running automatically
	
	std::string inputSource("Bluecherry BC-H16480A 16 port H.264 video and audio encoder / decoder");
	ServerMediaSession *sms 
	= ServerMediaSession::createNew(*env,StreamName.c_str(),
					inputSource.c_str(),
					"Session streamed by \"H264VideoStreamer\"",
					True/*SSM*/);
	sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink,rtcp));
      
	// add camera input to the rtsp Server
	rtspServer->addServerMediaSession(sms);
	
      
	// show access address
	*env<<"play this stream using the URl: "<<rtspServer->rtspURL(sms)<<"\"\n";
      
	// Start the streaming:
	*env << "Beginning streaming...\n";     
	
	// send frame to RTSP server
	  
	// create data source
	NAL_Source = BlueCherrySource::createNew(*env,NALparams);
	
	// Open the device source in this case are encoded frames 
	if (NAL_Source == NULL) {
	      *env << "Unable to open input camera \"" << "\" as a device source\n";
	      exit(1);
	}  
	  		  
	// send frame to RTSP server
	// Create a framer for the Video Elementary Stream:
	videoSource = H264VideoStreamDiscreteFramer::createNew(*env, NAL_Source);

	// Finally, start playing:
	playFunction = afterPlaying;
	bool testStartplaying;
	testStartplaying = videoSink->startPlaying(*videoSource,playFunction,videoSink);
	*env<<testStartplaying<<"\n";
		  
	// create periodic task to check data from input device
	// each 40 ms
	//int usecToDelay = 40000;
	//env->taskScheduler().scheduleDelayedTask(usecToDelay,(TaskFunc*)play,this); 
	isEventLoop = true;
	env->taskScheduler().doEventLoop();	
      }
    
    
  }catch(...){
    
  }
}

// play the stream
void H264_rtspServer::play(int ID)
{
  try{
      // Open the device source in this case is encoded frames 
      if (NAL_Source == NULL) {
	  *env << "Unable to open input camera \"" << "\" as a device source\n";
	  exit(1);
      }  
      // Finally, start playing next NAL unit:
      videoSink->startPlaying(*videoSource,playFunction,videoSink);
    
  }catch(...){
    
  }
}

// stop the stream playing
void H264_rtspServer::stopPlay()
{
  try{
	//*env << "...done reading from file\n";
	//videoSink->stopPlaying();
	//Medium::close(videoSource);
      
  }
  catch(...){
 
  }
}

// stop the RTSP server
void H264_rtspServer::closeServer()
{
  try{
      // close the server
      const char* name = (const char*)rtspServer->name();
      rtspServer->close(*env,name);     
    
  }
  catch(...){
  
  }

}


// wrapper to call the play function
void H264_rtspServer::wrapperToCallPlay(void* pt2object, int i)
{
    try{
	
	H264_rtspServer *myself = (H264_rtspServer*)pt2object;
	myself->play(i);
	printf("entering wrappertoCallPlay\n");

    }catch(...){
    }

}

// create a thread to manage a RTSP session
// that controls a stream flow
int H264_rtspServer::create_Thread(void )
{
  try{
    
      int cod, error;
      int ID = 1;
      
      pthread_attr_t attr;            //      attribute object
      struct sched_param param;       //      struct for set the priority
    

      if (pthread_attr_init(&attr)==0){	
	
	pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);

	// in linux the threads are created with the maximum priority, equal to the kernel
      }else{
	    printf("error: %d \n", errno);
      }      
    
      error=pthread_attr_setschedparam(&attr,&param);

      if (error!=0)
      {
	    printf("error: %d \n", errno);
      }
		    
      //      create thread
      cod = pthread_create(&StreamOut,&attr,H264_rtspServer::Entry_Point,this);
      if (cod!=0)
      {
	    printf("error creating thread \n");
	    return cod;
      }else{
	    printf("creating Stream thread %d with a priority of %d \n",ID,get_ThreadPriority());
	    //printf("creating thread\n");
	    return cod;
      }
    
  }
    // catch to get the thread cancel exception
    catch (abi::__forced_unwind&) {
	throw;
    } catch (...) {
    // do something
	printf("thread creation error\n");
    }

}

// cancel the thread
int H264_rtspServer::cancel_Thread(void )
{
    try{
        //      cancel the camera associated thread
        int cod;
	
        cod = pthread_cancel(StreamOut);
        if (cod!=0)
        {
                printf("thread cannot be canceled\n");
                return cod;
        }else{
                printf("canceling thread %d with a priority of %d \n",ID,get_ThreadPriority());
                //printf("creating thread\n");
                return cod;
        }
    }
    // catch to get the thread cancel exception
    catch (abi::__forced_unwind&) {
	throw;
    } catch (...) {
	// do something
	printf("thread has been canceled\n");
    }

}

//      Entry point function for the thread in C++
//      static function
void* H264_rtspServer::Entry_Point(void* pthis)
{
  try{
	H264_rtspServer *pS = (H264_rtspServer*)pthis; 
	//  convert to class H264_rtspServer to allow correct thread work
        pS->getFrames();
			
         return 0; // TODO: Revisar el puntero de retorno
       }
       // catch to get the thread cancel exception
       catch (abi::__forced_unwind&) {
	    throw;
       } catch (...) {
       // do something
	  printf("stream thread has been canceled\n");
	}
}

// get thread priority
int H264_rtspServer::get_ThreadPriority(void )
{
      try{
        pthread_attr_t attr;            //      attributes
        int cod, priority;
        struct sched_param param;       //      contain the priority of the thread

        if (!(cod=pthread_attr_init(&attr)) &&
        !(cod=pthread_attr_getschedparam(&attr,&param)))

                priority = param.sched_priority;
       
        return priority;
    }
    catch(...){
    }

}

// init the semaphore
void H264_rtspServer::init_semaphore(int sem, int value)
{
    try{	
        switch(sem)
        {
        case 1:
	    if (sem_init(&Sem1,0,value)==-1){   
            // start semaphore to value
            // 2d parameter = 0; only shared by threads in this process(class)
               
		printf("Failed to initialize the semaphore %d in camera %d",Sem1,videoGeneralIndex+1);
            }     
            break;
        case 2:
	    if (sem_init(&Sem2,0,value)==-1){   
            // start semaphore to value
            // 2d parameter = 0; only shared by threads in this process(class)
               
		printf("Failed to initialize the semaphore %d in camera %d",Sem2,videoGeneralIndex+1);
            }
            break;
        }
	
    }catch(...){
    }

}

// set the semaphore signal
void H264_rtspServer::set_semaphore(int sem)
{
    try{
	switch(sem)
        {
        case 1:
	    if (sem_post(&Sem1)==-1){ 
            //	increase semaphore
		printf("Failed to unlock or increase the semaphore %d in camera %d",Sem1,videoGeneralIndex+1);
            }else{
		//printf("camera: %d  Semaphore: %d\n",ID,Sem1);
            }
            break;
        case 2:
            if (sem_post(&Sem2)==-1){
            //	increase semaphore
		printf("Failed to unlock or increase the semaphore %d in camera %d",Sem2,videoGeneralIndex+1);
            }else{
                printf("camera: %d  Semaphore: %d\n",videoGeneralIndex+1,Sem2);
                printf("No deberia entrar aqui\n");
            }
            break;
        }
	
    }catch(...){
    }

}

// wait and lock the semaphore
void H264_rtspServer::wait_semaphore(int sem)
{
    try{
        switch(sem)
        {
        case 1:
	    if (sem_wait(&Sem1)==-1){        
                // decrease semaphore value
                printf("Failed to lock or decrease the semaphore %d in camera %d",Sem1,videoGeneralIndex+1);
            }else{
               // printf("camera: %d  Semaphore: %d\n",ID,Sem1);
            }
            break;
        case 2:
            if (sem_wait(&Sem2)==-1){        
		// decrease semaphore value
                printf("Failed to lock or decrease the semaphore %d in camera %d",Sem2,videoGeneralIndex+1);
            }else{
                printf("camera: %d  Semaphore: %d\n",videoGeneralIndex+1,Sem2);
                printf("No deberia entrar aqui\n");
            }
            break;
        }      
        
        	
	
    }catch(...){
    }

}

 
 // include extra qt moc files
 #include "H264_rtspServer.moc"
