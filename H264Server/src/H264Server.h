#ifndef H264SERVER_H
#define H264SERVER_H

// GUI
#include <QtGui>
#include <QtGui/QMainWindow>

// include H264Server GUI
#include "ui_H264Server.h"

// bluecherry Class
#include "blueCherryCard.h"

// std headers
#include <iostream>
#include <cstdio>

// Data structures
struct VideoInputParameters{	
	int index;
	int width;
	int height;
	float fps;
	float Kbps;
	float bufferSize;	
	float rtspPort;
	QString sourcePath;
	QString rtspName;
	QString rtspHost;	
};
struct VideoProperties{
	QWidget *Parent;
	VideoInputParameters parameters;
};
struct checkIndex{
	int index;
	int passCounter;
};

// class declaration
class H264Server: public QMainWindow,private Ui::MainWindow
{
	Q_OBJECT
	public:
		H264Server();
		virtual ~H264Server();
	public  Q_SLOTS:
		void addVideoInput(void);
		void checkVideoProperties(int index);
		void saveVideoProperties(QWidget *parameters);
		void closeVideoProperties(int index);
		void startCameraPreview(int tabIndex);
		unsigned char* AVFrame2QImage(AVFrame *frame,QImage image,
					      int width,int  height);
		void updatePreview(void);
		void getPreview(pictureFrame image);
		void init_semaphore(int sem, int value);	
		void set_semaphore(int sem);			
		void wait_semaphore(int sem);			
		void setupServer(void);
		void startVideoServer(void);
		void stopVideoServer(void);

	Q_SIGNALS:
		void saveProperties(VideoProperties properties);
	private:
	int videoGeneralIndex;						// index to keep video inputs
	int videoCounter;
	int convertedCounter;
	int lastPropertiesIndex;					// last and actual properties visited index
	int actualPropertiesIndex;
	typedef std::deque<AVFrame*> frameBuffer;   			/// FIFO buffer to save the compressed frames
	std::deque<frameBuffer> cameraBufferList;			/// contains all buffers from the cameras
	std::deque<bool> cameraStatusList;				/// it contains the status of each camera on or off
	QList<checkIndex> propertiesIndex;				// to keep control of video properties access
	QList<VideoInputParameters> VideoInputPropertiesList;		// video parameters list
	QList<blueCherryCard*> cameraList;
	QList<v4l2_buffer> bufferList;					// buffer for each camera
	QSignalMapper *signalMapper;					// signal mappers for properties
	QSignalMapper *signalVideoPropSavingMapper;
	QSignalMapper *signalClosePropWindows;
	QSignalMapper *signalPreview;
	QSignalMapper *signalTiming;
	QList<QString> pButtonText;
	QList<bool> closedWindow;
	QTimer *timer;							// this timer controls the video
	
	// flow control
	QSemaphore freeBytes;
	QSemaphore usedBytes;
	sem_t Sem1,Sem2;						// flow semaphore control
									// capturing timing
        // live555 variables
        UsageEnvironment *env;						// main environment
        H264VideoStreamDiscreteFramer *videoSource;			// video source 
        RTPSink *videoSink;						// RTP sink
        TaskScheduler *scheduler;					// RTSP, RTP scheduler
        struct in_addr destinationAddress;
	unsigned short rtpPortNum;
	unsigned short rtcpPortNum;
	unsigned char ttl;
	
};

#endif //H264SERVER_H