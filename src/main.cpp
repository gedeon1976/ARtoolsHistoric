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
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/SoRenderManager.h>
#include <Inventor/actions/SoGLRenderAction.h>

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
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
    ////Quarter::init();
    //      initialize the new nodes, required
    
    QWidget * mainwin = SoQt::init(argc, argv, argv[0]);

    if (mainwin==NULL) exit(1);   
    
    SoStereoTexture::initClass();   
    // create a Quarter widget for displaying a scene graph
    ////QuarterWidget *viewer = new QuarterWidget(&mainGUI);
    
    
    // make a simple scene to test the viewer
    SoSeparator *root = new SoSeparator;
    root->ref();
    SoSelection *selection= new SoSelection;
    selection->ref();
    SoOrthographicCamera *camera = new SoOrthographicCamera;
    selection->addChild(camera);
    
    SoBaseColor *color = new SoBaseColor;
    color->rgb = SbColor(1,1,0.5);
    root->addChild(color);
    ////selection->addChild(color);
    
    //      add Stereo node
    
    SoStereoTexture *Stereo = new SoStereoTexture;
    Stereo->width.setValue(720);
    Stereo->heigh.setValue(576);
    Stereo->IOD.setValue(5);//IOD
  ////camera->viewAll(selection,viewer->getSoRenderManager()->getViewportRegion());
    root->addChild(Stereo);
    //root->addChild(new SoCone);    
  
    StereoVideo video("rtsp://sonar.upc.es:7070/cam0","rtsp://sonar.upc.es:7070/cam1",720,576,Stereo);
    QObject::connect(&video,SIGNAL(updatedone()),&mainGUI,SLOT(show_fps())); 
    
    SoQtExaminerViewer *viewer = new SoQtExaminerViewer(mainwin);
    viewer->setSceneGraph(root);
    mainGUI.setCentralWidget(viewer->getWidget());    
    
    // make the viewer react to inputs events similar to the good old 
    // examinerViewer
    
    //viewer->setInteractionModeEnabled(true);
    //viewer->setContextMenuEnabled(true);
//    /* viewer->setNavigationModeFile(QUrl("coin:///scxml/navigation/examiner.xml"));
//     viewer->move(10,40);
//     viewer->resize(800,600);*/
    //viewer->show();
    //SoQt::show(mainwin);
    //SoQt::mainLoop();
//     QDockWidget* stereo_dock = new QDockWidget(viewer->getWidget(),0);
//     stereo_dock->setAttribute(Qt::WA_DeleteOnClose);// important
//     stereo_dock->setAllowedAreas(Qt::LeftDockWidgetArea);
//     stereo_dock->setWidget(viewer->getWidget());
//     mainGUI.addDockWidget(Qt::LeftDockWidgetArea,stereo_dock);

      
      


    // get Coin's render manager (code from virtualtech at google code)
   /* 
    SoRenderManager *renderManager = new SoRenderManager;
    renderManager = viewer->getSoRenderManager();
    root->GLRender(renderManager->getGLRenderAction());
   */
    return app.exec();
    // Clean up resources.
    root->unref();
    delete viewer;

   // Quarter::clean();



}
