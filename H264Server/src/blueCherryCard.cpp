/*
 * 
 * 	H264Server bluecherry interface class
 * 
 * 	brief@	this class serves to capture a video and audio flow
		from a Bluecherry H.264/AVC 16 ports card 
 * 
 * 	date		November/23/2012
 * 	author		Henry Portilla
 *
 * 	
 *	Notes:	The code is based on the file bc-record.c
 *		from bluecherry and the code will be
 *		under the same General Public License
 */

#include "blueCherryCard.h"

blueCherryCard::blueCherryCard(){
  try{
      // initialize variables
      got_vop = 0;			// key frame
      BufferMaxSize = 5;		// buffer size default
      semaphores_global_flag = -1;      // flag to start semaphore inizialization
    
  }catch(...){
  }

}


blueCherryCard::blueCherryCard(QString name, int width, int height){
  try{
      // initialize variables
      videoName = name;
      videoWidth = width;
      videoHeight = height;
      got_vop = 0;			// key frame
      BufferMaxSize = 5;		// buffer size default
      semaphores_global_flag = -1;	// flag to start semaphore inizialization
    
    
  }catch(...){
  }
  
}

blueCherryCard::~blueCherryCard(){
  
}
// set video input ID
void blueCherryCard::setInputID(int cameraID)
{
    try{
	ID = cameraID;
	
    }catch(...){
    }

}


// set video size
void blueCherryCard::setVideoSize(int width, int height)
{
  try{
      videoWidth = width;
      videoHeight = height;
    
  }catch(...){
  }

}

// set video source
void blueCherryCard::setVideoSource(QString name)
{
  try{
     videoName = name;
     
  }catch(...){
  }
}

// set the buffer size
void blueCherryCard::setBufferSize(int bufferSize)
{
    try{
	BufferMaxSize = bufferSize;
	
    }catch(...){
    }

}


// open the corresponding device
void blueCherryCard::open_video_dev(QString name, int width, int height){
  try{
      // get the current input to open
      const char* dev = name.toStdString().c_str();
      
      if ((vfd = open(dev, O_RDWR)) < 0)// | O_NONBLOCK)
	err_out("Opening video device");

      /* Verify this is the correct type */
      if (ioctl(vfd, VIDIOC_QUERYCAP, &vcap) < 0)
	err_out("Querying video capabilities");

      if (!(vcap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
	!(vcap.capabilities & V4L2_CAP_STREAMING))
	err_out("Invalid video device type");

      /* Get the parameters */
      vparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if (ioctl(vfd, VIDIOC_G_PARM, &vparm) < 0)
	err_out("Getting parameters for video device");

      /* Get the format */
      vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      if (ioctl(vfd, VIDIOC_G_FMT, &vfmt) < 0)
      err_out("Getting video device format");

      /* Set FPS/GOP */
      vparm.parm.capture.timeperframe.denominator = 25;
      vparm.parm.capture.timeperframe.numerator = 1;
      ioctl(vfd, VIDIOC_S_PARM, &vparm);

      /* Set format */
      vfmt.fmt.pix.width = width;
      vfmt.fmt.pix.height = height;
      if (ioctl(vfd, VIDIOC_S_FMT, &vfmt) < 0)
      err_out("Setting video format");
    
  }
  catch(...){
  }
}
// set a title with date and time on the image
void blueCherryCard::set_osd(char* text){
  try{
    
    
  }catch(...){
  }
}
// prepare video buffers for linux video interface
void blueCherryCard::v4l_prepare(void){
  try{
      v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      v4l2_requestbuffers req;
      int i;

      reset_vbuf(&req);
      req.count = V_BUFFERS;

      if (ioctl(vfd, VIDIOC_REQBUFS, &req) < 0)
      err_out("Requesting buffers");

      if (req.count != V_BUFFERS)
      err_out("Requested buffer count mismatch");

      for (i = 0; i < V_BUFFERS; i++) {
	v4l2_buffer vb;
	reset_vbuf(&vb);
	vb.index = i;

	if (ioctl(vfd, VIDIOC_QUERYBUF, &vb) < 0)
	err_out("Querying buffer");

	p_buf[i].size = vb.length;
	p_buf[i].data = mmap(NULL, vb.length,
	PROT_WRITE | PROT_READ, MAP_SHARED,
	  vfd, vb.m.offset);
	  if (p_buf[i].data == MAP_FAILED)
	  err_out("Mmap of buffer");
      }

      if (ioctl(vfd, VIDIOC_STREAMON, &type) < 0)
      err_out("Starting video stream");

      /* Queue all buffers */
      for (i = 0; i < V_BUFFERS; i++) {
	v4l2_buffer vb;

	reset_vbuf(&vb);
	vb.index = i;

	if (ioctl(vfd, VIDIOC_QBUF, &vb) < 0)
	err_out("Queuing buffer");
      }
      
	
    
  }catch(...){
  }
}
// prepare ffmpeg structures
void blueCherryCard::av_prepare(void){
  try{
    
      AVCodec *codec;
      char outfile[256] = "test.mkv";
      
      /* Get the output format */
       fmt_out = av_guess_format(NULL, "test.mkv", NULL);
       if (!fmt_out)
 	err_out("Error guessing format for %s", outfile);

      // set the format directly instead of a file

      if ((oc = avformat_alloc_context()) == NULL)
	err_out("Error allocating av context");

      oc->oformat = fmt_out;
   //   snprintf(oc->filename, sizeof(oc->filename), "%s", outfile);

      /* Setup new video stream */
      if ((video_st = av_new_stream(oc, 0)) == NULL)
	err_out("Error creating new av stream");

      video_st->time_base.den = vparm.parm.capture.timeperframe.denominator;
      video_st->time_base.num = vparm.parm.capture.timeperframe.numerator;

      if (strstr((char *)vcap.card, "Softlogic 6010")) {
	video_st->codec->codec_id = CODEC_ID_MPEG4;
      } else if (strstr((char *)vcap.card, "Softlogic 6110")) {
	video_st->codec->codec_id = CODEC_ID_H264 ;
	video_st->codec->crf = 20;
	video_st->codec->me_range = 16;
	video_st->codec->me_subpel_quality = 7;
	video_st->codec->qmin = 10;
	video_st->codec->qmax = 51;
	video_st->codec->max_qdiff = 4;
	video_st->codec->qcompress = 0.6;
	video_st->codec->i_quant_factor = 0.71;
	video_st->codec->b_frame_strategy = 1;
      } else {
	err_out("Unknown card: %s\n", vcap.card);
      }

      video_st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
      video_st->codec->pix_fmt = PIX_FMT_YUV420P;
      video_st->codec->width = vfmt.fmt.pix.width;
      video_st->codec->height = vfmt.fmt.pix.height;
      video_st->codec->time_base = video_st->time_base;

      if (oc->oformat->flags & AVFMT_GLOBALHEADER)
      video_st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

      /* Open Video output */
      codec = avcodec_find_encoder(video_st->codec->codec_id);
      if (codec == NULL)
	err_out("Error finding video encoder");
      
      if (avcodec_open2(video_st->codec, codec, NULL) < 0)
	err_out("Error opening video encoder");

      /* Open output file */  // here we change to save in a buffer or send a signal better
   //   if (avio_open(&oc->pb, outfile, URL_RDWR) < 0)
   //	err_out("Error opening out file");

   //   avformat_write_header(oc, NULL);
      

    
  }catch(...){
  }
}

// prepare some structures for decode the frames
void blueCherryCard::decode_prepare(void )
{
    try{
	AVCodec *codec;
	decodeCtx = NULL;
	
	// find the video decoder
	codec = avcodec_find_decoder(CODEC_ID_H264);
	if (!codec) {
	    printf("Codec not found\n");
	    exit(1);
	}	
	// allocate a codec context
	decodeCtx = avcodec_alloc_context3(codec);
	if (!decodeCtx) {
	    printf("Could not allocate video codec context\n");
	    exit(1);
	}
	
	// start some values of codec context
	avcodec_get_context_defaults(decodeCtx);
	decodeCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	decodeCtx->pix_fmt = PIX_FMT_YUV420P;
	decodeCtx->width = videoWidth;
	decodeCtx->height = videoHeight;
	decodeCtx->time_base.den = video_st->time_base.den;
	decodeCtx->time_base.num = video_st->time_base.num;
	// add SPS, PPS NAL units
	decodeCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
	decodeCtx->extradata = video_st->codec->extradata;
	decodeCtx->extradata_size = video_st->codec->extradata_size;
	 
	// open the codec
	if (avcodec_open2(decodeCtx,codec,NULL) < 0){
	    printf("Could not open codec\n");
	}
	
	
    }catch(...){
    }
    
}



// get the current compressed frame from the card
AVPacket blueCherryCard::get_CompressedFrame(v4l2_buffer *vb){
  try{
    
      AVCodecContext *c = video_st->codec;
      AVPacket pkt;

      av_init_packet(&pkt);

      if (vb->flags & V4L2_BUF_FLAG_KEYFRAME)	
	pkt.flags |= AV_PKT_FLAG_KEY;

      if (vb->bytesused < 100 || vb->bytesused > (128 * 1024))
	err_out("Invalid size: %d\n", vb->bytesused);

      pkt.data = (uint8_t*)p_buf[vb->index].data;
      pkt.size = vb->bytesused;

      if (c->coded_frame->pts != AV_NOPTS_VALUE)
	pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base,
	video_st->time_base);
	pkt.stream_index = video_st->index;
      
      // return current compressed frames
      return pkt;

    /*  if (av_write_frame(oc, &pkt))
	err_out("Error writing frame to file");  */ 
	
  }catch(...){
  }
}

// get the compressed data and save in the buffer
void blueCherryCard::getData(void)
{
  try{
      v4l2_buffer videoFrame;
      AVPacket pkt;
      // keep the thread alive
      while(1){
	  
	  // check semaphores flags
	    if(semaphores_global_flag == 0){
	      wait_semaphore(1);			
	    }
	  
	    videoFrame = getNextFrame();
	    pkt = get_CompressedFrame(&videoFrame);
	    frameCounter = frameCounter + 1;
	    printf("received frames: %d\n",frameCounter);
	    
	    // save to camera buffer
	    InputBuffer.push_back(pkt);
	    
	    // keep the buffer to a limit size
	    if (InputBuffer.size() < BufferMaxSize){
		// delete the head frame from FIFO buffer
		InputBuffer.pop_front();
	    }
	
	    if(semaphores_global_flag == 0){
	      set_semaphore(1);	     // set the semaphore 	      
	    }    
      }      
  }
  catch(...){
    }

}


// get the SPS Nal units
void blueCherryCard::getSPS_NAL(AVPacket pkt)
{
 try{
     int nal_start, nal_end;
     uint8_t* buf = pkt.data;
     int len;
     
     // read some H264 data into buf
     h264_stream_t* h = h264_new();
     find_nal_unit(buf, len, &nal_start, &nal_end);
     read_nal_unit(h, &buf[nal_start], nal_end - nal_start);
     debug_nal(h,h->nal);
	
    }
    catch(...){
    }
    

}

// get the video frames
v4l2_buffer blueCherryCard::getNextFrame(void)
{
  try{
      v4l2_buffer vb;
      int ret;
      time_t tm = time(NULL);
      char *tm_buf = ctime(&tm);

      tm_buf[strlen(tm_buf) - 1] = '\0';
     // set_osd("%s", tm_buf);

      reset_vbuf(&vb);
      ret = ioctl(vfd, VIDIOC_DQBUF, &vb);
      if (ret < 0) {
	fprintf(stderr, "Failure in dqbuf\n");
	ioctl(vfd, VIDIOC_QBUF, &vb);
      }

      /* Wait for key frame */
      if (!got_vop) {
	if (!(vb.flags & V4L2_BUF_FLAG_KEYFRAME)) {
	  ioctl(vfd, VIDIOC_QBUF, &vb);
	}
	got_vop = 1;
      }
      //send frame
      return vb;
      
    
    
  }catch(...){
  }

}

// decode the current packet
int blueCherryCard::get_decodedFrame(AVPacket *pkt, AVFrame *frame)
{
 try{
    int got_frame = 0;
    int bytesUsed = 0;
    frameCounter = frameCounter + 1;
    
    // check data size
    if (pkt->size > 0){
	bytesUsed = avcodec_decode_video2(decodeCtx,frame,&got_frame,pkt);
	if (bytesUsed < 0){
	    printf("Error while decoding frame %d\n", frameCounter);
	    return bytesUsed;
	 }
    
	if (got_frame!= 0){
	    printf("Decodind frame %d\n",frameCounter);
	}  
    
	return bytesUsed;
    }
	return -1;
    }
    catch(...){
    }
}


// start the capturing of video frames
void blueCherryCard::start(void )
{
  try{
      // start ffmpeg libraries
      avcodec_init();
      av_register_all();
      
      // open video input
      open_video_dev(videoName,videoWidth,videoHeight);
      
      // prepare structures and codecs
      v4l_prepare();
      av_prepare();
      decode_prepare();
      
      // start the capturing
      init_semaphore(1,1);
      create_Thread();
      
   
  }catch(...){
  }
  

}

// stop the capturing of video frames
void blueCherryCard::stop(void )
{
  try{

    
  }catch(...){
  }

}

// create a thread to get data from the bluecherry card
int blueCherryCard::create_Thread(void)
{
  try{
    int cod, error;
    int ID = 1;

    pthread_attr_t attr;            //      attribute object
    struct sched_param param;       //      struct for set the priority
    

    if (pthread_attr_init(&attr)==0){	
	
	//pthread_attr_getschedparam(&attr,&param);
	pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);

	   // param.sched_priority = 10;
	    // printf("setup thread scope \n");
	    // set priority to 10; max priority is = ?
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

    cod = pthread_create(&videoInput,&attr,blueCherryCard::Entry_Point,this);
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
  catch(...){
      
  }
}

//      Entry point function for the thread in C++
//      static function
void *blueCherryCard::Entry_Point(void *pthis)
{
        try{
                blueCherryCard *pS = (blueCherryCard*)pthis; 
		//  convert to class bluecherryCard to allow correct thread work
                pS->getData();
                return 0; // TODO: Revisar el puntero de retorno
        }
        catch(...)
        {
                printf("%s"," Something wrong happened with the thread ");
        }
}

// get the thread running priority
int blueCherryCard::get_ThreadPriority(void)
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
void blueCherryCard::init_semaphore(int sem, int value)
{
    try{	
        switch(sem)
        {
        case 1:
	    if (sem_init(&Sem1,0,value)==-1){   
            // start semaphore to value
            // 2d parameter = 0; only shared by threads in this process(class)
               
		printf("Failed to initialize the semaphore %d in camera %d",Sem1,ID);
            }     
            break;
        case 2:
	    if (sem_init(&Sem2,0,value)==-1){   
            // start semaphore to value
            // 2d parameter = 0; only shared by threads in this process(class)
               
		printf("Failed to initialize the semaphore %d in camera %d",Sem2,ID);
            }
            break;
        }
	
    }catch(...){
    }

}

// set the semaphore signal
void blueCherryCard::set_semaphore(int sem)
{
    try{
	switch(sem)
        {
        case 1:
	    if (sem_post(&Sem1)==-1){ 
            //	increase semaphore
		printf("Failed to unlock or increase the semaphore %d in camera %d",Sem1,ID);
            }else{
		printf("camera: %d  Semaphore: %d\n",ID,Sem1);
            }
            break;
        case 2:
            if (sem_post(&Sem2)==-1){
            //	increase semaphore
		printf("Failed to unlock or increase the semaphore %d in camera %d",Sem2,ID);
            }else{
                printf("camera: %d  Semaphore: %d\n",ID,Sem2);
                printf("No deberia entrar aqui\n");
            }
            break;
        }
	
    }catch(...){
    }

}

// wait and lock the semaphore
void blueCherryCard::wait_semaphore(int sem)
{
    try{
        switch(sem)
        {
        case 1:
	    if (sem_wait(&Sem1)==-1){        
                // decrease semaphore value
                printf("Failed to lock or decrease the semaphore %d in camera %d",Sem1,ID);
            }else{
                printf("camera: %d  Semaphore: %d\n",ID,Sem1);
            }
            break;
        case 2:
            if (sem_wait(&Sem2)==-1){        
		// decrease semaphore value
                printf("Failed to lock or decrease the semaphore %d in camera %d",Sem2,ID);
            }else{
                printf("camera: %d  Semaphore: %d\n",ID,Sem2);
                printf("No deberia entrar aqui\n");
            }
            break;
        }      
        
        	
	
    }catch(...){
    }

}

 // include extra qt moc files
 #include "blueCherryCard.moc"


