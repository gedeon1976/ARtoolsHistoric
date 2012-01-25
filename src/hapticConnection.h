/*****************************************************************
    HapticConnection class
    
    Description:	this class allows to connect to 
					a haptic server and receive the 
					current haptic position
		  
    author:			Henry Portilla (c) 2011
					this code use IOCCOMM a library 
					done by Alexander Perez
    
    This code is freely available and is delivered as is
    without any warranty
    
    Copyright: See COPYING file that comes with this distribution

*****************************************************************/

#ifndef HAPTICCONNECTION_H
#define HAPTICCONNECTION_H

// include IOCCOMM library
#include "../libcomm/client.h"
#include "../haptic/haptic.h"
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <QObject>

using namespace std;
//using namespace haptic;
class hapticConnection:public QObject{
 
  Q_OBJECT  
  public:
    hapticConnection(void);			// direct access to the haptic (understood as the haptic is connected
									// to the same machine)
    hapticConnection(const std::string& URLserver,const std::string& port);
    ~hapticConnection();    
    void startConnection(void);
    void getHapticPosition(void);
	void closeConnection(void);
  private:
    bool hapticStatus;
	mt::Transform HapticPosition;
	mt::Vector3 position;
	mt::Rotation orientation;
	haptic::Haptic* HapticDevice;	// haptic device
	
    //ioc_comm::Client* hapticClient;
    //ioc_comm::vecData sendingData;
    //ioc_comm::cartesian::force forceOnHaptic;
    //ioc_comm::vecData serverData;
	//ioc_comm::baseData serverData;

  private slots:
    void enable_haptic_readings();
	void getWorkSpaceLimits(mt::Vector3 MinCubicLimits, mt::Vector3 MaxCubicLimits); 
  signals:
	void sendHapticData(mt::Transform HapticPosition);
	//void sendHapticData(mt::Vector3 position);
   
   //void sendHapticData(Vect6 HapticData);
  
};
#endif //  _END _HAPTICCONNECTION_H_
