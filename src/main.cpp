//
// C++ Implementation: main
//
// Description: This program is an example to 
//		add a video node to a inventor scene
//
//
// Author:  Henry Portilla (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "augmentedreality.h"

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCube.h>

int main(int argc, char** argv)
{
	QWidget *mainwin = SoQt::init(argv[0]);
	if (mainwin==NULL) exit(1);
	
	SoSeparator *root = new SoSeparator;
    	root->ref();
	
		//	start connection
	const char *URL = "rtsp://antlia:7070/cam3";
	
	AugmentedReality Video(root, URL);
	//root->addChild(new SoCube);

	SoQtExaminerViewer *eviewer = new SoQtExaminerViewer(mainwin);
   	eviewer->setSceneGraph(root);
	eviewer->setFeedbackVisibility(true);
	eviewer->show();
	// Pop up the main window.
    	SoQt::show(mainwin);
    	// Loop until exit.
    	SoQt::mainLoop();
	delete eviewer;

	// Clean up resources.				  
    	root->unref();
	return 0;

}

/*
#include <QApplication>
#include "HenryViewer.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(qooliv);

    QApplication app(argc, argv);
    HenryViewer mainWin;

    const char *URL = "rtsp://antlia:7070/cam3";
    SoSeparator* root = mainWin.getScene();
    AugmentedReality Video(root, URL);

    mainWin.show();
    return app.exec();
}*/