#include <iostream>

// Qt nokia library
#include <QtGui/QApplication>
#include <QtGui>

// include main GUI
#include "H264Server.h"

int main(int argc, char **argv) {
  
    // Start application and GUI mainWindow 
    QApplication app(argc, argv);
    H264Server mainGUI;
    mainGUI.show();
    return app.exec();
}