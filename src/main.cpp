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


//using namespace SIM::Coin3D::Quarter;

int main(int argc, char** argv)
{
	
	try{

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
    color->rgb = SbColor(1.0,1.0,0.0);
    root->addChild(color);
    ////selection->addChild(color);
    
    //      add Stereo node
    
    SoStereoTexture *Stereo = new SoStereoTexture;
    Stereo->width.setValue(640);
    Stereo->heigh.setValue(480);
    Stereo->IOD.setValue(7.0);//IOD
  ////camera->viewAll(selection,viewer->getSoRenderManager()->getViewportRegion());
    root->addChild(Stereo);
    root->addChild(new SoCone);    
    //rtsp://sonar.upc.es:7070/cam1
	StereoVideo video("rtsp://147.83.37.135:5544/CameraStreamL","rtsp://147.83.37.135:5545/CameraStreamR",640,480,Stereo);
    QObject::connect(&video,SIGNAL(updatedone()),&mainGUI,SLOT(show_fps())); 
    // timer for update the image every 40 ms = 25fps	
    QTimer* timer = new QTimer;
    timer->setInterval(40);
    timer->start();  
    QObject::connect(timer,SIGNAL(timeout()),&mainGUI,SLOT(show_fps())); 
    
    // add haptic connection
    const char* URLserver = "localhost";
    const char* port = "5000";
    //hapticConnection hapticDevice(URLserver,port);    
    //hapticDevice.startConnection();
	hapticConnection hapticDevice0;   
	hapticDevice0.startConnection();
    
    QObject::connect(timer,SIGNAL(timeout()),&hapticDevice0,SLOT(enable_haptic_readings()));
	QObject::connect(&hapticDevice0,SIGNAL(sendHapticData(mt::Transform)),
		     &mainGUI,SLOT(show_haptic_data(mt::Transform)));
    // send the haptic values to the virtual pointer
    QObject::connect(&hapticDevice0,SIGNAL(sendHapticData(mt::Transform)),
  		     &video,SLOT(set_haptic_data(mt::Transform)));
    //// send image projected points to main GUI
    QObject::connect(&video,SIGNAL(sendimagepoints(imagePoints)),
		     &mainGUI,SLOT(get_image_points(imagePoints)));
	// send left and right captured images to openCV processing
	QObject::connect(&video,SIGNAL(sendIplImageStereo(IplImage*,IplImage*)),
			&mainGUI,SLOT(get_IplImageStereo(IplImage*,IplImage*)));
	
   
    SoQtExaminerViewer *viewer = new SoQtExaminerViewer(mainwin);
    viewer->setSceneGraph(root);
    
    mainGUI.setCentralWidget(viewer->getWidget());    
	// add extra widgets
	
	


    
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
	/*#if defined(WIN32) && defined(_DEBUG)
		_CrtDumpMemoryLeaks();
		setFilterDebugHook();
	#endif*/
	}
	catch(cv::Exception){

		int err = cvGetErrStatus();
		const char* description = cvErrorStr(err);
		int i=143;
		cvGuiBoxReport(err,"ShowLeftVideo",description,"ARtools.cpp",i,"2");

	}

}

// #include <../libcomm/client.h>
// #include <boost/asio.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>
//         
// boost::asio::deadline_timer* t;
// ioc_comm::Client* _client;
// void print(const boost::system::error_code& /*e*/){
//       ioc_comm::vecData serverData;
//                         std::stringstream sstream;
//       sstream.precision(3);
//                   _client->getServerData(serverData);
//                   if(serverData.size() > 0 ){
//                           ioc_comm::baseData& tmp = serverData[0];
//                           sstream << tmp.time_stamp << "\t";
//                           for(unsigned int i = 0; i < tmp._data.size(); i++)
//                                   sstream << tmp._data[i] << "\t";
// 
//                           std::cout << sstream.str() << std::endl;
//                           std::cout.flush();
//         sstream.clear();
//       }
//       t->expires_at(t->expires_at() + boost::posix_time::seconds(2));
// 
//       t->async_wait(print);
// }
// 
// 
// int main(int argc, char *argv[]){
// 
// try{
//     // Check command line arguments.
// //     if (argc != 3){
// //       std::cerr << "Usage: haptic_client <server> <port>" << std::endl;
// //       return 1;
// //     }
// 
//     boost::asio::io_service io;
//      const char* URLserver = "sirio.upc.es";
//      const char* port = "5000";
// 
//     t = new boost::asio::deadline_timer(io, boost::posix_time::seconds(2));
//     t->async_wait(print);
// 
//     _client = new ioc_comm::Client(URLserver, port, ioc_comm::HAPTIC, 1.0, 6);
//     _client->start();
// 
//     ioc_comm::vecData sendingData;
// 
//     ioc_comm::cartesian::force force;
// 
//     // Set initial values to the force to be send
//     force.time_stamp.assign(ioc_comm::cal_time_stamp());
//     force._data.at(0) = 2.0;  force._data.at(1) = 2.0;
//     force._data.at(2) = 2.0;  force._data.at(3) = 0.0;
//     force._data.at(4) = 0.0;  force._data.at(5) = 0.0;
// 
//     sendingData.push_back(force);
// 
//     _client->setSendingData(sendingData);
// 
//     boost::thread th(boost::bind(&boost::asio::io_service::run, &io));
// 
//     for(int i = 0; i < 10; i++){
//                           std::cout << i ;
// 
//   #if defined(WIN32)
//                   Sleep(2000);
//   #else
//                   usleep(2000*1000);
//   #endif
//   //            
//           }
//         
//           _client->close();
//     th.interrupt();
//     t->cancel();
//     th.join();
//     delete t;
//           return 0;
// 
//   }catch (std::exception& e) {
//     std::cerr << e.what() << std::endl;
// 
//   }
//   return 1;
// }
