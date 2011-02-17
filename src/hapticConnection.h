/*

    HapticConnection class
    
    Description:  this class allows to connect to 
		  a haptic server and receive the 
		  current haptic position
		  
    author:	  Henry Portilla (c) 2011
		  this code use IOCCOMM a library 
		  done by Alexander Perez
    
    This code is freely available and is delivered as is
    without warranty
    
    Copyright: See COPYING file that comes with this distribution

*/

#ifndef HAPTICCONNECTION_H
#define HAPTICCONNECTION_H

// include IOCCOMM library
#include "../libcomm/client.h"
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <QObject>

using namespace std;
class hapticConnection:public QObject{
 
  Q_OBJECT  
  public:
    //hapticConnection();
    hapticConnection(const std::string& URLserver,const std::string& port);
    ~hapticConnection();    
    int startConnection(void);
    void getHapticPosition(void);
    void closeConnection(void);
  private:
    ioc_comm::Client* hapticClient;
    ioc_comm::vecData sendingData;
    ioc_comm::cartesian::force forceOnHaptic;
    ioc_comm::vecData serverData;

  private slots:
    void enable_haptic_readings();
  signals:
   void sendHapticData(ioc_comm::vecData serverData);
  
};
#endif //  _END _HAPTICCONNECTION_H_
