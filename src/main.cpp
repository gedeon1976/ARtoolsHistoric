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

int main(int argc, char** argv)
{
	QWidget * mainwin = SoQt::init(argv[0]);
	if (mainwin==NULL) exit(1);

	SoSeparator *root = new SoSeparator;
    	root->ref();

		//	start connection
	const char *URL = "rtsp://antlia:7070/cam3";
	AugmentedReality::init();
	AugmentedReality Video(root, URL);

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