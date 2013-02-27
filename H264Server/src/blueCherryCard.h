/*
 * 
 * 	H264Server bluecherry interface class
 * 
 * 	brief@	this class serves to capture one video and audio flow
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
#include <QMetaType>

// FFMPEG LIBRARIES
extern "C"{
  #include <libswscale/swscale.h>
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

// h264bitstream class
#include "h264_stream.h"// h264bitstream class

// POSIX threads 
#include <pthread.h>                                   
#include <semaphore.h>
#include <deque>
// add this for manage thread cancel exceptions
#include <cxxabi.h>


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

struct dataFrame{
    int cameraID;
    AVFrame *frame;
};

struct H264encodedFrame{
    int camera_ID;
    AVPacket frame;
};

// define the structures like a type to allow to the signal/slot mechanism
// understand the  variables correctly
typedef dataFrame pictureFrame;
typedef H264encodedFrame H264Frame; 
Q_DECLARE_METATYPE(pictureFrame)
Q_DECLARE_METATYPE(H264Frame)

class blueCherryCard:public QObject{
  Q_OBJECT
  public:
    blueCherryCard();
    blueCherryCard(QString name, int width, int height);
    virtual ~blueCherryCard();
  public Q_SLOTS:   
 
    void setInputID(int cameraID);  
    int getInputID(void);
    void setVideoSize(int width,int height);
    void setVideoSource(QString name);
    void setBufferSize(int bufferSize);
    void open_video_dev(QString name,int width,int height);
    void set_osd(char* text);
    void v4l_prepare(void);
    void av_prepare(void);
    void decode_prepare(void);
    v4l2_buffer getNextFrame(void);
    AVPacket get_CompressedFrame(v4l2_buffer *vb);    
    void getData(void);
    void show_NAL_info(AVPacket pkt);
    void getSPS_NAL(AVPacket pkt);
    void getPPS_NAL(AVPacket pkt);
    AVCodecContext* get_DecodingContext(void);
    int get_decodedFrame(AVPacket *pkt, AVFrame *frame);
    void getImage();
    void start(void);
    void stop(void);
    // threads code
    int create_Thread(void);
    int cancel_Thread(void);
    static void* Entry_Point(void *pthis);
    int get_ThreadPriority(void);
    void init_semaphore(int sem, int value);	/// init the semaphore
    void set_semaphore(int sem);		/// set the semaphore signal
    void wait_semaphore(int sem);		/// wait and lock the semaphore
    
            
  Q_SIGNALS: 
    void sendVideoPreview(pictureFrame image);	/// signal used for preview
    void sendEncodedVideo(H264Frame encodedFrame);/// signal that sends encoded frames
    
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
    int ID;				/// video input ID
    
    int cameraID;			/// camera ID for thread
    pthread_t videoInput;		/// thread for this video input
    sem_t Sem1,Sem2;			/// semaphores
    int semaphores_global_flag;		/// flag to control semaphores start
    int BufferMaxSize;			/// maximun size of buffer
    std::deque<AVPacket> InputBuffer;   /// FIFO buffer to save the compressed frames
};
#endif // BLUECHERRYCARDH