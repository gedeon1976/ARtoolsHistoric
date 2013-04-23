/************************************************************************

    GUI class
    
    Description:	this class allow to show a main GUI
		  
    author:			Henry Portilla (c) 2013
					based on code by Carlos vazquez
    
    This code is freely available and is delivered as is
    without any warranty
    
    Copyright: See COPYING file that comes with this distribution

************************************************************************/
#pragma once
#include <QtGui>
#include "ARtools.h"
#include "ARScene.h"


class mainWidget:public QWidget
{
	Q_OBJECT
public:
	mainWidget(void);
	~mainWidget(void);
public slots:
	void createCoinScene(bool status);
private:
	ARtools *userGUI;
	ARScene *ARviewer;
	
};

