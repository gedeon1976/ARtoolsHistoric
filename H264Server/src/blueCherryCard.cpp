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
    
  }catch(...){
  }
  
}

blueCherryCard::~blueCherryCard(){
  
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


// open the corresponding device
void blueCherryCard::open_video_dev(QString name, int width, int height){
  try{
      // get the current input to open
      const char* dev = name.toStdString().c_str();
      
      if ((vfd = open(dev, O_RDWR | O_NONBLOCK)) < 0)
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

      /* Open output file */  // here we cahnge to save in a buffer or send a signal better
   //   if (avio_open(&oc->pb, outfile, URL_RDWR) < 0)
   //	err_out("Error opening out file");

   //   avformat_write_header(oc, NULL);
      

    
  }catch(...){
  }
}
// save the current frame
void blueCherryCard::video_out(v4l2_buffer *vb){
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

      if (av_write_frame(oc, &pkt))
	err_out("Error writing frame to file");   
	
  }catch(...){
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

 // include extra qt moc files
 #include "blueCherryCard.moc"


