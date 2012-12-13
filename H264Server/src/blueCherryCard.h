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

#ifndef BLUECHERRYCARDH
#define BLUECHERRYCARDH

#include <stdlib.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <linux/videodev2.h>

// include signals and slot capacity
#include <QObject>
#include <QTimer>

// FFMPEG LIBRARIES
extern "C"{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libavutil/avutil.h>
  #include <libavformat/avio.h>
  #include <libavutil/mathematics.h>
}

// LIVE555 LIBRARIES
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>

// special macros
#define reset_vbuf(__vb) do { \
memset((__vb), 0, sizeof(*(__vb))); \
(__vb)->type = V4L2_BUF_TYPE_VIDEO_CAPTURE; \
(__vb)->memory = V4L2_MEMORY_MMAP; \
} while(0)

#define err_out(__msg, args...) do { \
fprintf(stderr, __msg ": %m\n", ## args); \
exit(1); \
} while(0)

#define V_BUFFERS 8
struct video_buf{
  void *data;
  size_t size;
};

class blueCherryCard:public QObject{
  Q_OBJECT
  public:
    blueCherryCard();
    blueCherryCard(QString name, int width, int height);
    virtual ~blueCherryCard();
  public Q_SLOTS:   
 
    void setVideoSize(int width,int height);
    void setVideoSource(QString name);
    void open_video_dev(QString name,int width,int height);
    void set_osd(char* text);
    void v4l_prepare(void);
    void av_prepare(void);
    void decode_prepare(void);
    v4l2_buffer getNextFrame(void);
    AVPacket get_CompressedFrame(v4l2_buffer *vb);   
    int get_decodedFrame(AVPacket *pkt, AVFrame *frame);
    void start(void);
    void stop(void);
    
    
    
  Q_SIGNALS: 
    void sendVideoFrame(void);
    
  private:
    v4l2_format vfmt;
    v4l2_capability vcap;
    v4l2_streamparm vparm;
    int vfd;
    int got_vop;
    int frameCounter;
    
    AVOutputFormat *fmt_out;
    AVStream *video_st;
    AVFormatContext *oc;
    AVCodecContext *decodeCtx; 
    video_buf p_buf[V_BUFFERS];
    
    QTimer *framesTimer;
    QString videoName;
    int videoWidth;
    int videoHeight;
};
#endif // BLUECHERRYCARDH