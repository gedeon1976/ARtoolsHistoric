#ifndef H264SERVER_H
#define H264SERVER_H

// GUI
#include <QtGui>
#include <QtGui/QMainWindow>
// include H264Server GUI
#include "ui_H264Server.h"
// bluecherry Class
#include "blueCherryCard.h"
// h264bitstream class
#include "h264_stream.h"
#include <iostream>

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


// LIVE555 libraries

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
		void updatePreview(void);
		void getPreview(pictureFrame image);

	Q_SIGNALS:
		void saveProperties(VideoProperties properties);
	private:
    int videoGeneralIndex;						// index to keep video inputs
	int videoCounter;
	int lastPropertiesIndex;					// last and actual properties visited index
	int actualPropertiesIndex;
	std::deque<AVFrame*> frameBuffer;   				/// FIFO buffer to save the compressed frames
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
									// capturing timing

};

#endif //H264SERVER_H