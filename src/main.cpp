/*/*


    ARtools main GUI
    
    this is the main GUI for the results of my PhD thesis
    so this was long hard work (maybe extra time)
    
    copyright(2010) Henry Portilla
    
		    some parts of this code are based from
		    code by Adolfo Rodríguez
		    and Leopoldo Palomo
    
    This code can be distributed freely without any warranty.

*/
//#if _WIN32
//	// Visual Studio Debug
//	#include "reportingHook.h"
//	#include "setDebugNew.h"
//#endif
	



// Qt nokia library
#include <QtGui/QApplication>
#include <QtGui>
#include "ARtools.h"
// Coin3D
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/SoRenderManager.h>
#include <Inventor/actions/SoGLRenderAction.h>

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

// Quarter viewer
//#include <Quarter/Quarter.h>
//#include <Quarter/QuarterWidget.h>

// Stereo camera
#include "SoStereoTexture.h"
// Stereo video
#include "stereovideo.h"
// haptic connection
#include "hapticConnection.h"
#include <QTimer>
#include "common.h"

// load GUI
#include "mainWidget.h"

//using namespace SIM::Coin3D::Quarter;

int main(int argc, char** argv)
{
	// start application
	Q_INIT_RESOURCE(ARtools);
	QApplication app(argc, argv);
	mainWidget mainGUI;
	// don't show yet here
	// because we use a Coin3D based widget
	return app.exec(); 
}