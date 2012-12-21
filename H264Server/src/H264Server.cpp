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
 
 // constructor
 H264Server::H264Server()
 {
	// initialize GUI
	setupUi(this);

	// start variables 
	videoGeneralIndex = 0;
	videoCounter = 0;
	lastPropertiesIndex = -1;
	// connect signals and slots
	// add video sources
	connect(buttAddVideoSource,SIGNAL(clicked()),
			this,SLOT(addVideoInput()));
	timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()),
	    this,SLOT(updatePreview()));
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
		params.rtspPort = 7000;

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
			QString videoIndex,strFps,strBufferSize,strVideoWidth,strVideoHeight,strRtspPort;
			VideoInputParameters OldParams;
			int width,height;

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
			lnRtspHost->setText(OldParams.rtspHost);
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
			 QTabWidget *tab = new QTabWidget;

			 // set labels	
			 QLabel *lbfps = new QLabel(tr("fps"));
			 QLabel *lbKbps = new QLabel(tr("Kbps"));
			 QLabel *lbBufferSize = new QLabel(tr("frames on buffer"));
			 QLabel *lbVideoSize = new QLabel(tr("pixels"));
			 QLabel *lbRTSPAddress = new QLabel(tr("Rtsp address"));
			 QLabel *lbVideoPreview = new QLabel();

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
			 QCheckBox *input = new QCheckBox(tr("Enable Streaming"));
			 QCheckBox *preview = new QCheckBox(tr("Preview"));
			 input->setChecked(false);
			 input->setEnabled(false);
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



			 GridBox->addWidget(input,0,0,1,2);GridBox->addWidget(propertiesButton,0,3,1,2);
			 GridBox->addWidget(preview,0,2,1,1);
			 GridBox->addWidget(fps,1,0,1,1);GridBox->addWidget(lbfps,1,1,1,1);
			 GridBox->addWidget(Kbps,2,0);GridBox->addWidget(lbKbps,2,1);
			 GridBox->addWidget(bufferSize,3,0);GridBox->addWidget(lbBufferSize,3,1);
			 GridBox->addWidget(videoSize,4,0);GridBox->addWidget(lbVideoSize,4,1);
			 GridBox->addWidget(lbRTSPAddress,5,0);GridBox->addWidget(rtspAddress,5,1,1,3);
			 GridBox->addWidget(lbVideoPreview,1,2,4,2);

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
      VideoInputParameters VideoParams;      
      QString videoSource;
      
      int currentIndex = tabIndex-1;
      QList<QCheckBox*> checkPreview = tabVideoList->currentWidget()->findChildren<QCheckBox*>();
      
      // check if preview is enabled
      isValid = checkPreview.at(1)->isChecked();
      if(isValid){
	  // connect video preview signal and slot
	  connect(cameraList.at(currentIndex),SIGNAL(sendVideoPreview(dataFrame)),this,SLOT(getPreview(dataFrame)));
	  cameraList.at(currentIndex)->start();		// start current camera		
	  timer->start(40);
      }else{
	  // disconnect video preview signal and slot
	  disconnect(cameraList.at(currentIndex),SIGNAL(sendVideoPreview(AVFrame*)),this,SLOT(getPreview(AVFrame*)));
	  cameraList.at(currentIndex)->stop();		// stop camera........
	  timer->stop();
      }
    
  }catch(...){
  }
}
// update the preview frames information
void H264Server::updatePreview(void)
{
  try{
      v4l2_buffer videoFrame;
      AVPacket pkt;
      AVFrame *decodedFrame;
      int camNumber = cameraList.size();
      int BytesUsed = 0;
      
      // get the frames from the cameras and set default values
      decodedFrame = avcodec_alloc_frame();
      avcodec_get_frame_defaults(decodedFrame);
            
      for(int i=0;i<camNumber;i++){
	  
	 //cameraList.at(i)->getImage(); 
    
      }      
    
  }catch(...){
  }
}

// get the images to perform a small window video preview
void H264Server::getPreview(dataFrame image)
{
    try{
	SwsContext *pSwSContext;
	AVCodecContext *decCtx;
	AVFrame *tmpFrame = image.frame;
	AVFrame *pframeRGB;
	
	int width = image.frame->width;
	int height = image.frame->height;
	int index = image.cameraID -1;
	
	// set image size and format
 	QImage dataImage(width,height,QImage::Format_RGB888);
	
	// get the decoding and scaling context without filters
	decCtx = cameraList.at(index)->get_DecodingContext();	
	pSwSContext = sws_getContext(decCtx->width,decCtx->height,
		decCtx->pix_fmt,width,height,PIX_FMT_RGB24,SWS_BICUBIC,NULL,NULL,NULL);
	
	// converts to RGB24
	pframeRGB = avcodec_alloc_frame();
	avcodec_get_frame_defaults(pframeRGB);
	sws_scale(pSwSContext,tmpFrame->data,tmpFrame->linesize,0,height,
		  pframeRGB->data,pframeRGB->linesize);
		  
	// load to image
	int dataSize = sizeof(pframeRGB->data[0]);
	dataImage.loadFromData(pframeRGB->data[0],dataSize);
	
    }
    catch(...){
	
    }

}


 
 // include extra qt moc files
 #include "H264Server.moc"