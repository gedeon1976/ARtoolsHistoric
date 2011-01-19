/*


    ARtools main GUI
    
    this is the main GUI for the results of my PhD thesis
    so this was long hard work (maybe excesive time)
    
    copyright(2010) Henry Portilla
    
		    some parts of this code are based from
		    code by Adolfo Rodríguez
		    and Leopoldo Palomo
    
    This code can be distributed freely without any warranty.


*/
// Qt nokia library
#include <QtGui/QApplication>
#include <QtGui>
#include "ARtools.h"
// Coin3D
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

// Quarter viewer
#include <Quarter/Quarter.h>
#include <Quarter/QuarterWidget.h>

// Stereo camera
#include "SoStereoTexture.h"
// Stereo video
#include "stereovideo.h"

using namespace SIM::Coin3D::Quarter;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    ARtools mainGUI;
    mainGUI.show();
    // initializes Quarter library (also the Qt and Coin3D libraries implicitly)  
    Quarter::init();
    //      initialize the new nodes, required
    SoStereoTexture::initClass();   
    
    // make a simple scene to test the viewer
    SoSeparator *root = new SoSeparator;
    root->ref();
    SoBaseColor *color = new SoBaseColor;
    color->rgb = SbColor(1,1,0.5);
    root->addChild(color);
    
    //      add Stereo node
    
    SoStereoTexture *Stereo = new SoStereoTexture;
    Stereo->width.setValue(720);
    Stereo->heigh.setValue(576);
    Stereo->IOD.setValue(5);//IOD
  
    root->addChild(Stereo);
    StereoVideo video("rtsp://sonar.upc.es:7070/cam0","rtsp://sonar.upc.es:7070/cam1",720,576,Stereo);
    QObject::connect(&video,SIGNAL(updatedone()),&mainGUI,SLOT(show_fps())); 
    
    // create a Quarter widget for displaying a scene graph
    QuarterWidget *viewer = new QuarterWidget(&mainGUI);
    viewer->setSceneGraph(root);
    
    // make the viewer react to inputs events similar to the good old 
    // examinerViewer
    
    viewer->setInteractionModeEnabled(true);
    viewer->setContextMenuEnabled(true);
    viewer->setNavigationModeFile(QUrl("coin:///scxml/navigation/examiner.xml"));
    viewer->move(10,40);
    viewer->resize(800,600);
    viewer->show();      
    return app.exec();
    // Clean up resources.
    root->unref();
    delete viewer;

    Quarter::clean();

}
