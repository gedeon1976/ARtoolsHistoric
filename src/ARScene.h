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

class ARScene:public QWidget
{
public:
	ARScene(QWidget* parent=0);
	~ARScene(void);
private:
	SoQtExaminerViewer *viewer;
};
