/*
 * 
 * 	H264Server class
 * 
 * 	brief@	this class serves to stream a video and audio flow
			from a Bluecherry H.264/AVC 16 ports card to the Internet
 * 
 * 	date		November/2012
 * 	author		Henry Portilla
 * 
 */
 
 #include "H264Server.h"
 #include "glwidget.h"
 
 // constructor
 H264Server::H264Server()
 {
	// initialize GUI
	setupUi(this);
	buttStopServer->setDisabled(true);
	// start variables 
	videoGeneralIndex = 0;
	videoCounter = 0;
	convertedCounter = 0;
	lastPropertiesIndex = -1;
	inputFileNameTest = "slamtv60.264";
	// set ports and maximum jumps for RTP protocols
	rtspServerNew = new H264_rtspServer;
	isRTSPServerStarted = false;
        rtpPortNumBase = 18888;
        ttl = 255;
	// register the own structure types to be used in signal/slot mechanism
	qRegisterMetaType<pictureFrame>();
	qRegisterMetaType<H264Frame>();
	// connect signals and slots
	// add video sources
	connect(buttAddVideoSource,SIGNAL(clicked()),
			this,SLOT(addVideoInput()));
	// add preview timer
	timer = new QTimer(this);
	timer->start(40);
	connect(timer,SIGNAL(timeout()),
	    this,SLOT(updatePreview()));
	// add start video server 
	connect(butStartServer,SIGNAL(clicked()),
	  this,SLOT(startVideoServer()));
	// add stop video server
	connect(buttStopServer,SIGNAL(clicked(bool)),
	  this,SLOT(stopVideoServer()));
	// start semaphore for preview control
	init_semaphore(1,1);
	
 }
 
 H264Server::~H264Server()
 {
 }
 
 // Check the  properties for each video input
 void H264Server::checkVideoProperties(int index){
	 try{
		checkIndex propIndex,propIndex2;
		actualPropertiesIndex = index;
		VideoInputParameters params;
		// set some defualt values
		params.bufferSize = 2;
		params.fps = 25;
		params.width = 704;
		params.height = 576;
		params.rtspPort = 8554;
		frameBuffer nullFrame;

		if (lastPropertiesIndex==-1){// first time for properties index
			propIndex.index = index;
			propIndex.passCounter = 1;
			for(int i=0;i<videoCounter;i++){
				propertiesIndex.push_back(propIndex);
				VideoInputPropertiesList.push_back(params);
				
			}
			propertiesIndex.replace(index-1,propIndex);
			// save the values
		
		}
		if(lastPropertiesIndex==actualPropertiesIndex){
			propIndex2.index = propertiesIndex.at(index-1).index;
			propIndex2.passCounter = propertiesIndex.at(index-1).passCounter + 1;
			int pos = index-1;
			propertiesIndex.replace(pos,propIndex2);
			// save index
			lastPropertiesIndex = index;
		}
		if(propertiesIndex.at(index-1).passCounter == videoCounter){
			// reset last properties index
			lastPropertiesIndex = -1;
			// disable corresponding video tab
			int currentIndex = tabVideoList->currentIndex();
			tabVideoList->setCurrentIndex(currentIndex);
			this->tabVideoList->currentWidget()->setDisabled(true);
			
			
		}
		if(propertiesIndex.at(index-1).passCounter<=1){
	

			QDialog *PropertiesWindow = new QDialog();
			PropertiesWindow->setAttribute(Qt::WA_DeleteOnClose);
			//PropertiesWindow->setWindowFlags(Qt::Tool);
			QFormLayout *InputProperties = new QFormLayout;
			QString videoIndex,strFps,strBufferSize,strVideoWidth,strVideoHeight,
			  strRtspPort;
			VideoInputParameters OldParams;
			int width,height;
			unsigned maxHostNamelen = 100;
			char HostNAME[maxHostNamelen+1];
			
			// get host name
			gethostname((char*)HostNAME,maxHostNamelen);
			HostNAME[maxHostNamelen]='\0';// just in case
			
			QString strRtspHost(HostNAME);

			// layout items
			QLabel *indexTab = new QLabel(tr("internal index"));
			QLineEdit *lnIndexTab = new QLineEdit;			
			QLabel *sourcePath = new QLabel(tr("source Path"));
			QLineEdit *lnSourcePath = new QLineEdit;
			QLabel *fps  = new QLabel(tr("desired fps"));;
			QLineEdit *lnFps = new QLineEdit;
			QLabel *bufferSize = new QLabel(tr("bufferSize(frames)"));
			QLineEdit *lnBufferSize = new QLineEdit;
			QLabel *videoHeight = new QLabel(tr("Height"));
			QLineEdit *lnVideoHeight = new QLineEdit;
			QLabel *videoWidth = new QLabel(tr("Width"));
			QLineEdit *lnVideoWidth = new QLineEdit;
			QLabel *rtspHost = new QLabel(tr("rtspHost"));
			QLineEdit *lnRtspHost = new QLineEdit;
			QLabel *rtspPort = new QLabel(tr("rtspPort"));
			QLineEdit *lnRtspPort = new QLineEdit;
			QLabel *rtspName = new QLabel(tr("rtspName"));
			QLineEdit *lnRtspName = new QLineEdit;
			QPushButton *saveButton = new QPushButton(tr("Save"));
			//QPushButton *cancelButton = new QPushButton(tr("Cancel"));

			// show current values
			OldParams = VideoInputPropertiesList.at(index-1);
			strFps.setNum(OldParams.fps);
			strBufferSize.setNum(OldParams.bufferSize);
			strVideoWidth.setNum(OldParams.width);
			strVideoHeight.setNum(OldParams.height);
			strRtspPort.setNum(OldParams.rtspPort);

			lnSourcePath->setText(OldParams.sourcePath);			
			lnFps->setText(strFps);
			lnBufferSize->setText(strBufferSize);
			lnVideoWidth->setText(strVideoWidth);
			lnVideoHeight->setText(strVideoHeight);
			lnRtspHost->setText(strRtspHost);
			lnRtspPort->setText(strRtspPort);
			lnRtspName->setText(OldParams.rtspName);

			// add widgets to for
			videoIndex.setNum(index);
			lnIndexTab->setText(videoIndex);
			lnIndexTab->setDisabled(true);
			InputProperties->addRow(indexTab,lnIndexTab);
			InputProperties->addRow(sourcePath,lnSourcePath);
			InputProperties->addRow(fps,lnFps);
			InputProperties->addRow(bufferSize,lnBufferSize);
			InputProperties->addRow(videoWidth,lnVideoWidth);
			InputProperties->addRow(videoHeight,lnVideoHeight);			
			InputProperties->addRow(rtspHost,lnRtspHost);
			InputProperties->addRow(rtspPort,lnRtspPort);
			InputProperties->addRow(rtspName,lnRtspName);
			InputProperties->addRow(saveButton);

			// show window
			QString videoName(tr("Video   Properties"));			
			videoName.insert(6,videoIndex);
			PropertiesWindow->setLayout(InputProperties);				
			PropertiesWindow->setWindowTitle(videoName);
			PropertiesWindow->show();
			width = PropertiesWindow->geometry().width();
			height = PropertiesWindow->geometry().height();
			PropertiesWindow->setMinimumSize(width,height);
			PropertiesWindow->setMaximumSize(width,height);

			// save last index
		    
			int tabcount = tabVideoList->count();
			if (tabcount>1){
				lastPropertiesIndex = index;
			}
			// enable the save of the values
			signalClosePropWindows = new QSignalMapper(this);
			signalVideoPropSavingMapper = new QSignalMapper(this);
			signalVideoPropSavingMapper->setMapping(saveButton,PropertiesWindow);
			connect(saveButton,SIGNAL(clicked()),
				signalVideoPropSavingMapper,SLOT(map()));
			connect(signalVideoPropSavingMapper,SIGNAL(mapped(QWidget*)),
				this,SLOT(saveVideoProperties(QWidget*)));
			
			// set closing mapping
			
			signalClosePropWindows->setMapping(PropertiesWindow,index);
			connect(PropertiesWindow,SIGNAL(rejected()),
				signalClosePropWindows,SLOT(map()));
			connect(signalClosePropWindows,SIGNAL(mapped(int)),
				this,SLOT(closeVideoProperties(int)));

		}
		
	 }catch(...){
	 }
 }


 // Save the properties for each video source
 void H264Server::saveVideoProperties(QWidget *parameters){
	 try{
		int width=704;
		int height=576;
		int bufferSize=10;
		int cameraID;
		int portRtsp;
		QString name;
	 	// save parameters
		VideoInputParameters currentVideoProperties,OldParams;
		QList<QLineEdit*> inputParameters = parameters->findChildren<QLineEdit*>();

		currentVideoProperties.index = inputParameters.at(0)->text().toInt();
		currentVideoProperties.sourcePath = inputParameters.at(1)->text();
		currentVideoProperties.fps = inputParameters.at(2)->text().toInt();
		currentVideoProperties.bufferSize = inputParameters.at(3)->text().toInt();
		currentVideoProperties.width = inputParameters.at(4)->text().toInt();
		currentVideoProperties.height = inputParameters.at(5)->text().toInt();
		currentVideoProperties.rtspHost = inputParameters.at(6)->text();
		currentVideoProperties.rtspPort = inputParameters.at(7)->text().toInt();
		currentVideoProperties.rtspName = inputParameters.at(8)->text();

		// save properties
		VideoInputPropertiesList.replace(currentVideoProperties.index-1,currentVideoProperties);
		
		// set camera parameters
		cameraID = currentVideoProperties.index;
		width = currentVideoProperties.width;
		height = currentVideoProperties.height;
		name = currentVideoProperties.sourcePath;
		bufferSize = currentVideoProperties.bufferSize;
		rtspPort = currentVideoProperties.rtspPort;
				
		blueCherryCard *tmpCam;	
		tmpCam = cameraList.at(currentVideoProperties.index-1);
		tmpCam->setInputID(cameraID);
		tmpCam->setVideoSource(name);
		tmpCam->setVideoSize(width,height);
		tmpCam->setBufferSize(bufferSize);	
			
		cameraList.replace(currentVideoProperties.index-1,tmpCam);
		
		// enable current tab
		int currentIndex = tabVideoList->currentIndex();
		tabVideoList->setCurrentIndex(currentIndex);
		tabVideoList->currentWidget()->setEnabled(true);

		// show data on main window
		QList<QLineEdit*> showData = tabVideoList->currentWidget()->findChildren<QLineEdit*>();
		QList<QCheckBox*> checkings = tabVideoList->currentWidget()->findChildren<QCheckBox*>();
		QList<QLCDNumber*> numbers = tabVideoList->currentWidget()->findChildren<QLCDNumber*>();
		
		int length ;
		QString size,address;
		
		// enable checkings
		checkings.at(0)->setEnabled(true);
		checkings.at(1)->setEnabled(true);

		// show fps
		numbers.at(0)->display(inputParameters.at(2)->text().toInt());
		
		// show size
		size.append(inputParameters.at(4)->text());
		size.append("x");
		length = size.length();
		size.insert(length,inputParameters.at(5)->text());
		showData.at(0)->setText(size);

		// show address
		address.append("rtsp://");
		address.append(inputParameters.at(6)->text());
		address.append(":");
		address.append(inputParameters.at(7)->text());
		address.append("/");
		address.append(inputParameters.at(8)->text());
		showData.at(1)->setText(address);

		// close widget
		parameters->close();
		
		

	 }catch(...){
	 }
 }
// Close the properties windows and enable properties buttons
 void H264Server::closeVideoProperties(int index){
	 try{
		//int ind = index;
		int currentIndex = tabVideoList->currentIndex();
		tabVideoList->setCurrentIndex(currentIndex);
		tabVideoList->currentWidget()->setEnabled(true);

	 }catch(...){
	 }
 }

 // Add video source from text input
 void H264Server::addVideoInput(void){

	 try{
	 // read input source and add entry to groupBox
		 if(lnVideoInput->text().isEmpty()){
			 // do nothing
		 }else{
			 int tabIndex = 0;	
			 int videoIndex = 0;
			 int widthPreview = 160;
			 int heighPreview = 120;
			 QTabWidget *tab = new QTabWidget;

			 // set labels	
			 QLabel *lbfps = new QLabel(tr("fps"));
			 QLabel *lbKbps = new QLabel(tr("Kbps"));
			 QLabel *lbBufferSize = new QLabel(tr("frames on buffer"));
			 QLabel *lbVideoSize = new QLabel(tr("pixels"));
			 QLabel *lbRTSPAddress = new QLabel(tr("Rtsp address"));
			 QLabel *lbVideoPreview = new QLabel;
			 GLwidget *glPreviewWidget = new GLwidget;
			 
			 // set preview area
			 QScrollArea *glPreviewArea = new QScrollArea;
			 glPreviewArea->setWidget(glPreviewWidget);
			 glPreviewArea->setWidgetResizable(true);
			 glPreviewArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			 glPreviewArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			 glPreviewArea->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
			 glPreviewArea->setMinimumSize(widthPreview,heighPreview);
			 glPreviewArea->setEnabled(true);

			 QLCDNumber *fps = new QLCDNumber;
			 QLCDNumber *Kbps = new QLCDNumber;
			 QLCDNumber *bufferSize = new QLCDNumber;
			 QLineEdit *videoSize = new QLineEdit;
			 QLineEdit *rtspAddress = new QLineEdit;

			 // set some properties
			 QColor LCDColor(255,0,0,255);
			 QPalette LCDpalette;
			 int widthLabels = 90;
			 						 
			 LCDpalette.setColor(QPalette::WindowText,LCDColor);
			 
			 rtspAddress->setMinimumWidth(50);
			 videoSize->setMaximumWidth(widthLabels);

			 fps->setSegmentStyle(QLCDNumber::Flat);//fps->setSmallDecimalPoint(true);
			 Kbps->setSegmentStyle(QLCDNumber::Flat);
			 bufferSize->setSegmentStyle(QLCDNumber::Flat);
			 
			 fps->setFrameShape(QFrame::NoFrame);
			 Kbps->setFrameShape(QFrame::NoFrame);
			 bufferSize->setFrameShape(QFrame::NoFrame);

			 fps->setFixedSize(widthLabels,20);fps->setPalette(LCDpalette);
			 Kbps->setFixedSize(widthLabels,20);Kbps->setPalette(LCDpalette);
			 bufferSize->setFixedSize(widthLabels,20);bufferSize->setPalette(LCDpalette);

			 videoSize->setDisabled(true);
			 rtspAddress->setDisabled(true);
			 
			 lbVideoPreview->geometry().size().setWidth(160);
			 lbVideoPreview->geometry().size().setHeight(120);
			 lbVideoPreview->setFrameStyle(QFrame::Box);

			 const QRect TabLayout = QRect(0,0,480,180);
			 const QString nameSource = lnVideoInput->text();
			 QGridLayout *GridBox = new QGridLayout;
			 QCheckBox *enableInputStream = new QCheckBox(tr("Enable Streaming"));
			 QCheckBox *preview = new QCheckBox(tr("Preview"));
			 enableInputStream->setChecked(false);
			 enableInputStream->setEnabled(false);
			 preview->setChecked(false);
			 preview->setEnabled(false);

			 QPushButton *propertiesButton = new QPushButton;
			 propertiesButton->size().setHeight(25);
			 propertiesButton->size().setWidth(40);
			 propertiesButton->setText("Properties");
			 QString pButtonText;
			
			 // set layout
			 
			 GridBox->setGeometry(TabLayout);
			 GridBox->setColumnMinimumWidth(0,40);
			 GridBox->setColumnMinimumWidth(1,40);
			 GridBox->setColumnStretch(1,10);
			 GridBox->setColumnStretch(2,10);


			 GridBox->addWidget(enableInputStream,0,0,1,2);GridBox->addWidget(propertiesButton,0,3,1,2);
			 GridBox->addWidget(preview,0,2,1,1);
			 GridBox->addWidget(fps,1,0,1,1);GridBox->addWidget(lbfps,1,1,1,1);
			 GridBox->addWidget(Kbps,2,0);GridBox->addWidget(lbKbps,2,1);
			 GridBox->addWidget(bufferSize,3,0);GridBox->addWidget(lbBufferSize,3,1);
			 GridBox->addWidget(videoSize,4,0);GridBox->addWidget(lbVideoSize,4,1);
			 GridBox->addWidget(lbRTSPAddress,5,0);GridBox->addWidget(rtspAddress,5,1,1,3);
			 GridBox->addWidget(glPreviewArea,1,2,4,2);

			 // create signal mappers
			 if (videoGeneralIndex==0){
				videoGeneralIndex = videoGeneralIndex + 1;
				signalMapper = new QSignalMapper(this);
			 }
				   
			 tabVideoList->addTab(tab,nameSource);
			 tabIndex = tabVideoList->count();
			 tabVideoList->setCurrentIndex(tabIndex);
			 tabVideoList->setCurrentWidget(tab);

			 tabVideoList->currentWidget()->setLayout(GridBox);

			 // add signal connections for button properties
			 signalMapper->setMapping(propertiesButton,tabIndex);
			 connect(propertiesButton,SIGNAL(clicked()),
					signalMapper,SLOT(map()));
			 
			 connect(signalMapper,SIGNAL(mapped(int)),
				 this,SLOT(checkVideoProperties(int)));
			 
			 // add signal connections for preview check
			 signalPreview = new QSignalMapper(this);
			 signalPreview->setMapping(preview,tabIndex);
			 connect(preview,SIGNAL(stateChanged(int)),
			   signalPreview,SLOT(map()));
			 
			 connect(signalPreview,SIGNAL(mapped(int)),
			   this,SLOT(startCameraPreview(int)));			 

			 // video input counter
			 videoCounter = videoCounter + 1;
			 
			 // create camera objects
			 
			 blueCherryCard *Cam = new blueCherryCard;
			 cameraList.push_back(Cam);
			 
			 frameBuffer tmpFrameBuffer;
			 cameraBufferList.push_back(tmpFrameBuffer);
			 			 
			 bool status = false;
			 cameraStatusList.push_back(status);
			 
	 
		 }
	 }catch(...){

	 }


 }
 
void H264Server::startCameraPreview(int tabIndex)
{
  try{
      bool isValid;
      int width=704;
      int height=576;
      bool currentStatus = false;
      VideoInputParameters VideoParams;      
      QString videoSource;
      
      int currentIndex = tabIndex-1;
      QList<QCheckBox*> checkPreview = tabVideoList->currentWidget()->findChildren<QCheckBox*>();
      
      // check if preview is enabled
      isValid = checkPreview.at(1)->isChecked();
      if(isValid){
	  // connect video preview signal and slot
	  connect(cameraList.at(currentIndex),SIGNAL(sendVideoPreview(pictureFrame)),this,SLOT(getPreview(pictureFrame)));
	  cameraList.at(currentIndex)->start();		// start current camera		
	  
	  // set flag of camera started
	  currentStatus = true;
	  cameraStatusList.at(currentIndex) = currentStatus;
	  
      }else{
	  // disconnect video preview signal and slot
	  disconnect(cameraList.at(currentIndex),SIGNAL(sendVideoPreview(pictureFrame)),this,SLOT(getPreview(pictureFrame)));
	  cameraList.at(currentIndex)->stop();		// stop camera........
	  
	  // unset flag for cameras state
	  currentStatus = false;
	  cameraStatusList.at(currentIndex) = currentStatus;
      }
    
  }catch(...){
  }
}
// convert an AVframe to a QImage to be used on the preview
unsigned char* H264Server::AVFrame2QImage(AVFrame* frame,QImage image,int width, int height)
{
    try{
	// load to image
	unsigned char *src = (unsigned char *)frame->data[0];
	for (int y = 0; y < height; y++)
	{	
	    QRgb *scanLine = (QRgb*)image.scanLine(y);
	    for (int x = 0; x < width; x++)
	    {
		scanLine[x] = qRgb(src[3*x], src[3*x+1], src[3*x+2]);
	    }
	src += frame->linesize[0];
	}
	return src;
	
    }
    catch(...){
    }

}


// update the preview frames information
void H264Server::updatePreview(void)
{
  try{
	SwsContext *pSwSContext;
	AVCodecContext *decCtx;
	AVFrame *pframeRGB;	
	AVFrame *decodedFrame;
	unsigned char *src;
	int BytesUsed = 0;
	int numBytes = 0;
	
	int currentTabIndex;
	int currentInternalTabIndex;
	int widthPreview,heighPreview;	
	int index = 0;
	bool loaded = false; 
	
	// check correct video index for correct display on 
	// the corresponding tab and also ask if the camera is ON
	
	currentTabIndex = tabVideoList->currentIndex();	
	
	if((currentTabIndex==videoGeneralIndex)&(cameraStatusList.at(videoGeneralIndex))){
	
	    // check the internal index to test correct video index
	    // currentInternalTabIndex = VideoInputPropertiesList.at(videoGeneralIndex).index;
	    // get the decoded frame
	    decodedFrame = cameraBufferList.at(videoGeneralIndex).front();
		    
	    // set image size and format
	    int width = decodedFrame->width;
	    int height = decodedFrame->height;
	    
	    // get the frames from the cameras and set default values
	
	    QImage dataImage(width,height,QImage::Format_ARGB32_Premultiplied);
	    QImage noVideo;
	    QImage noVideoScaled,VideoScaled;
	    
	    // determine required buffer size for allocate buffer
	    pframeRGB = avcodec_alloc_frame();
	    if (pframeRGB==NULL){
		std::printf("cannot allocate frame\n");	    
		throw;
	    }
	    numBytes = avpicture_get_size(PIX_FMT_BGRA,width,height);
	    uint8_t* buffer = new uint8_t[numBytes];
	    
	    avpicture_fill((AVPicture*)pframeRGB,buffer,PIX_FMT_BGRA,width,height);
			    
	    loaded=noVideo.load("/home/users/henry.portilla/projects/H264Server/H264Server/src/images/SMPTE_ColorBars.jpeg");
	    
	    widthPreview = 160;
	    heighPreview = 120;	
	    
	    // get the decoding and scaling context without filters
	    decCtx = cameraList.at(index)->get_DecodingContext();	
	    pSwSContext = sws_getContext(decCtx->width,decCtx->height,
		    decCtx->pix_fmt,decCtx->width,decCtx->height,PIX_FMT_BGRA,SWS_BICUBIC,NULL,NULL,NULL);
		    
	    //avcodec_get_frame_defaults(pframeRGB);
	    
	    // converts to RGB32
	    sws_scale(pSwSContext,decodedFrame->data,decodedFrame->linesize,0,decCtx->height,
		    pframeRGB->data,pframeRGB->linesize);
		    
	    // load data to image
	    src = AVFrame2QImage(pframeRGB,dataImage,width,height);
	    // 	convertedCounter = convertedCounter + 1;
	    // 	std::printf("converting frame %d\n",convertedCounter);
	    int dataSize= sizeof(pframeRGB->data);
	    dataImage.loadFromData(src,dataSize);
	    
	    // get widgets for the current tab
	    QList<QScrollArea*> drawSurface = tabVideoList->currentWidget()->findChildren<QScrollArea*>();
	    
	    QList<GLwidget*> renderArea = drawSurface.at(0)->findChildren<GLwidget*>();
	    
	    if(dataImage.isNull()){
		noVideo.convertToFormat(QImage::Format_ARGB32_Premultiplied);
		noVideoScaled = noVideo.scaled(widthPreview,heighPreview);	  
		renderArea.at(0)->setImage(noVideoScaled);
		renderArea.at(0)->repaint();	  
	    }else{
		VideoScaled = dataImage.scaled(widthPreview,heighPreview);	
		renderArea.at(0)->setImage(VideoScaled);
		renderArea.at(0)->repaint();	  
	    }
	
	// free memory
	delete [] buffer;
	av_free(pframeRGB);
	}
	
          
  }catch(...){
  }
}

// get the images to perform a small window video preview
void H264Server::getPreview(pictureFrame image)
{
    try{
	int maxSize = 10;
	int camID = image.cameraID;
	videoGeneralIndex = camID - 1;
	AVFrame *tmpFrame = image.frame;
	
	wait_semaphore(1);		
	// save the frame to the corresponding camera buffer
	cameraBufferList.at(videoGeneralIndex).push_back(tmpFrame);
	if (cameraBufferList.at(videoGeneralIndex).size()> maxSize){
	    cameraBufferList.at(videoGeneralIndex).pop_front();	// delete the first received frame
	    std::printf("camera %d display preview Buffer size is %d\n",camID,cameraBufferList.at(videoGeneralIndex).size());
	}
	set_semaphore(1);
    }
    catch(...){
	   std::printf("there was an error with the buffer cam %d index %d\n",videoGeneralIndex + 1,videoGeneralIndex);
	
    }

}
// get the encoded frames from different cameras
void H264Server::getEncodedFrames(H264Frame encodedImage)
{
    try{
	int maxSize = 10;
	int camID = encodedImage.camera_ID;
	videoEncodedGeneralIndex = camID - 1;
	AVPacket tmpPkt = encodedImage.frame;
	
	wait_semaphore(2);
	// save the frame to the corresponding camera buffer
	cameraCodedBufferList.at(videoEncodedGeneralIndex).push_back(tmpPkt);
	if (cameraCodedBufferList.at(videoEncodedGeneralIndex).size()> maxSize){
	    cameraCodedBufferList.at(videoEncodedGeneralIndex).pop_front();	// delete the first received frame
	    std::printf("camera %d Coded frame Buffer size is %d\n",camID,cameraCodedBufferList.at(videoEncodedGeneralIndex).size());
	}	
	set_semaphore(2);	
	
    }catch(...){
    }

}

// init the semaphore
void H264Server::init_semaphore(int sem, int value)
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
void H264Server::set_semaphore(int sem)
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
void H264Server::wait_semaphore(int sem)
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
// setup server
void H264Server::setupServer(int rtspPort)
{
  try{
      // set ports and maximum jumps
      rtpPortNumBase = 18888;
      ttl = 255; 
      
      // Begin by setting up our usage environment:
      scheduler = BasicTaskScheduler::createNew();
      env= BasicUsageEnvironment::createNew(*scheduler);
      
      // create groupsocks for RTP and RTCP
      destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
      // Note:: this is a multicast address
      
      // create a RTSP server     
      Port outPort(rtspPort);
      RTSPServer *rtspServer = RTSPServer::createNew(*env,outPort);
      if (rtspServer == NULL) {
	*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
	exit(1);
      }
    
  }catch(...){
  }

}
// add a RTP session to manage each camera input
void H264Server::AddRTSPSession(const char* nombreVideo, int i)
{
  try{     
     
      if (!isRTSPServerStarted){
	// Begin by setting up our usage environment:
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
	isRTSPServerStarted = true;
      }
      
      unsigned short rtpPortNum;
      unsigned short rtcpPortNum;
      RTPSink *videoSink;			// RTP sink         
      
      rtpPortNum = rtpPortNumBase + 2*i;
      rtcpPortNum = rtpPortNum + 1;
      
      Port rtpPort(rtpPortNum);
      Port rtcpPort(rtcpPortNum);
      
      // sockets groupsock 
      Groupsock rtpGroupsock(*env,destinationAddress,rtpPort,ttl);
      rtpGroupsock.multicastSendOnly(); // we're a SSM source
      Groupsock rtcpGroupsock(*env,destinationAddress,rtcpPort,ttl);
      rtcpGroupsock.multicastSendOnly();// we're a SSM source
      
      // create a video RTP sink from the rtpGroupsock
      OutPacketBuffer::maxSize = 100000;
      videoSink = H264VideoRTPSink::createNew(*env,&rtpGroupsock,96);
      
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
      inputFileName = "stream.264";// used for SDP
      ServerMediaSession *sms 
      = ServerMediaSession::createNew(*env,nombreVideo,inputFileName,
				    "Session streamed by \"H264VideoStreamer\"",
				    True/*SSM*/);
      sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink,rtcp));
      
      // add camera input to the rtsp Server
      rtspServer->addServerMediaSession(sms);
      
      // show access address
      *env<<"play this stream using the URl: "<<rtspServer->rtspURL(sms)<<"\"\n";
      
      // Start the streaming:
      *env << "Beginning streaming...\n";
      
      play(i);
      
      //env->taskScheduler().doEventLoop();
    
    
  }catch(...){
    
  }

}
// get a RTP session from RTSP server
ServerMediaSession* H264Server::getRTPSession(int i)
{
  try{
    
    
  }catch(...){
    
  }

}



// start video server
void H264Server::startVideoServer(void)
{
  try{
      bool isStreamEnabled = false;
      bool currentCameraStatus = false;
      
      QList<QCheckBox*> streamCheckBox;
      QList<QPushButton*> propertiesButton;
      
       // set port to rtsp Server
      int port = 8554;
      rtspServerNew->setPort(port);
                
      // start available cameras
      for(int i=0;i<videoCounter;i++){
	
	// check if input was enabled to be streamed
	tabVideoList->setCurrentIndex(i);	
	streamCheckBox = tabVideoList->currentWidget()->findChildren<QCheckBox*>();
	
	// get properties button also
	propertiesButton = tabVideoList->currentWidget()->findChildren<QPushButton*>();
	isStreamEnabled = streamCheckBox.at(0)->isChecked();
	
	// connect the camera input to the stream source
	connect(cameraList.at(i),SIGNAL(sendEncodedVideo(H264Frame)),
	  rtspServerNew,SLOT(getEncodedFrames(H264Frame)));
	
	if(isStreamEnabled==true){
	  
	  // check if camera is ON or OFF (preview mode enabled)
	  currentCameraStatus = cameraStatusList.at(i);
	  if(currentCameraStatus){
	    
	    // stop the preview timer
	    timer->stop();
	    cameraStatusList.at(i) = true;
	    streamCheckBox.at(1)->setDisabled(true);
	    propertiesButton.at(0)->setDisabled(true);
	    
	    // add the RTP session for this camera
	    QString streamName(VideoInputPropertiesList.at(i).rtspName);
	    const char *name = streamName.toStdString().c_str(); 
	    rtspServerNew->AddRTSPSession(name,i);
	    
	  	  
	  }else{
	      
	    // start cameras without preview	        
	    cameraList.at(i)->start();	
	    cameraStatusList.at(i) = true;
	    
	     // add the RTP session for this camera
	    QString streamName(VideoInputPropertiesList.at(i).rtspName);
	    const char *name = streamName.toStdString().c_str(); 
	    rtspServerNew->AddRTSPSession(name,i);	    
	    
	    // disable preview
	    streamCheckBox.at(1)->setDisabled(true);
	    propertiesButton.at(0)->setDisabled(true);
	    
	    // stop main timer used for video preview
	    timer->stop();
	    
	  }	  
	}      
      }
     
      // disable all RTP sessions
     
      
      // disable this button
      butStartServer->setDisabled(true);
      // enable stop server
      buttStopServer->setEnabled(true);
      
    
  }catch(...){
  }
}

// stop video server
void H264Server::stopVideoServer(void)
{
  try{
      // stop the video server
      bool currentCameraStatus = false;
      QList<QCheckBox*> streamCheckBox;
      QList<QPushButton*> propertiesButton;
           
      for(int i=0;i<videoCounter;i++){
	
	    tabVideoList->setCurrentIndex(i);
	    // get preview checkbox to enable again
	    streamCheckBox = tabVideoList->currentWidget()->findChildren<QCheckBox*>();
	    // get properties button
	    propertiesButton = tabVideoList->currentWidget()->findChildren<QPushButton*>();
	    currentCameraStatus = cameraStatusList.at(i);
	    
	    // disconnect the camera input to the stream source
	    disconnect(cameraList.at(i),SIGNAL(sendEncodedVideo(H264Frame)),
	      rtspServerNew,SLOT(getEncodedFrames(H264Frame)));
	      
	    if (currentCameraStatus){
		cameraList.at(i)->stop();		
		cameraStatusList.at(i) = false;
	    }
	    streamCheckBox.at(1)->setChecked(false);
	    streamCheckBox.at(1)->setEnabled(true);
	    propertiesButton.at(0)->setEnabled(true);	    
 	  
	}      
	// start main timer for video preview
	timer->start(40);
	
	// enable start server button again
	butStartServer->setEnabled(true);
	
	// disable stop button
	buttStopServer->setDisabled(true);
      
    
    } catch (...) {
      // do something
      std::printf("thread has been canceled\n");
    }
}
// test live555 classes Server 
void H264Server::testServer(void )
{
  try{
    // Begin by setting up our usage environment:
      
//       const unsigned short rtpPortNum = 18888;
//       const unsigned short rtcpPortNum = rtpPortNum+1;
//       const unsigned char ttl = 255;
// 
//       const Port rtpPort(rtpPortNum);
//       const Port rtcpPort(rtcpPortNum);
// 
//       Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
//       rtpGroupsock.multicastSendOnly(); // we're a SSM source
//       Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
//       rtcpGroupsock.multicastSendOnly(); // we're a SSM source
// 
//       // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
//       OutPacketBuffer::maxSize = 100000;
//       videoSink2 = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);
// 
//       // Create (and start) a 'RTCP instance' for this RTP sink:
//       const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
//       const unsigned maxCNAMElen = 100;
//       unsigned char CNAME[maxCNAMElen+1];
//       gethostname((char*)CNAME, maxCNAMElen);
//       CNAME[maxCNAMElen] = '\0'; // just in case
//       RTCPInstance* rtcp
//       = RTCPInstance::createNew(*env, &rtcpGroupsock,
// 				estimatedSessionBandwidth, CNAME,
// 				videoSink2, NULL /* we're a server */,
// 				True /* we're a SSM source */);
//       // Note: This starts RTCP running automatically
// 
//   
//       ServerMediaSession* sms
// 	= ServerMediaSession::createNew(*env, "testStream", inputFileNameTest,
// 		      "Session streamed by \"testH264VideoStreamer\"",
// 					      True /*SSM*/);
//       sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink2, rtcp));
//       rtspServer->addServerMediaSession(sms);
// 
//       char* url = rtspServer->rtspURL(sms);
//       *env << "Play this stream using the URL \"" << url << "\"\n";
//       delete[] url;
// 
//       // Start the streaming:
//       *env << "Beginning streaming...\n";
//       //play();
// 
//       //env->taskScheduler().doEventLoop(); // does not return

    
  }catch(...){
    
  }

}
// test afterPlaying
void H264Server::afterPlaying(void*)
{
  try{
      *env << "...done reading from buffer\n";
      videoSink2->stopPlaying();
      Medium::close(videoSource2);
      // Note that this also closes the input file that this source read from.

      // Start playing once again:
      play(0);
    
  }catch(...){

  }

}

// test play
void H264Server::play(int i)
{
  try{
      // Open the device source in this case are encoded frames
     
      unsigned char* NALbuffer;
      int bufferSize;
      ByteStreamMemoryBufferSource* NAL_Source
	= ByteStreamMemoryBufferSource::createNew(*env, NALbuffer,bufferSize);
      if (NAL_Source == NULL) {
	*env << "Unable to open NAL buffer \"" << "\" as a device source\n";
	exit(1);
      }
      
//       //copy the encoded frame to NAL_Source
//       AVPacket currentEncodedFrame;
//       frameBuffer NAL_list;
//       
//       NAL_list = cameraBufferList.at(i);
//       currentEncodedFrame = NAL_list.front();
//       unsigned char *NAL_data;
//       int NAL_size = currentEncodedFrame.size;
//       memmove(currentEncodedFrame.data,NAL_data,NAL_size);
      
      
      FramedSource* videoES = NAL_Source;

      // Create a framer for the Video Elementary Stream:
      videoSource2 = H264VideoStreamDiscreteFramer::createNew(*env, videoES);

      // Finally, start playing:
      *env << "Beginning to read from camera...\n";
      
     //videoSink2->startPlaying(*videoSource2, afterPlaying, videoSink2);
    
  }catch(...){
    
  }

}


 
 // include extra qt moc files
 #include "H264Server.moc"