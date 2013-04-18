/************************************************************************

    ARscene class
    
    Description:	this class allow to show AR over a stereo video
		  
    author:			Henry Portilla (c) 2013
					
    
    This code is freely available and is delivered as is
    without any warranty
    
    Copyright: See COPYING file that comes with this distribution

************************************************************************/
#pragma once

#include <QWidget>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
// include common types
#include "common.h"

class ARScene:public QWidget
{
Q_OBJECT
public:
	ARScene(QWidget* parent=0);
	~ARScene(void);
signals:
public slots:
	void GetCameraRTSPAddresses(rtspAddress address);

private:
	SoQtExaminerViewer *viewer;
	const char* leftRTSPCam;
	const char* rightRTSPCam;
	int videoWidth;
	int videoHeight;
};
