/*

    HapticConnection class
    
    Description:  this class allows to connect to 
		  a haptic server and receive the 
		  current haptic position
		  
    author:	  Henry Portilla (c) 2011
		  this code use IOCCOMM a library 
		  done by Alexander Perez
    
    This code is freely available and is delivered as is
    without any warranty
    
    Copyright: See COPYING file that comes with this distribution

*/
// include definitions
#include "hapticConnection.h"
//constructor
hapticConnection::hapticConnection(const std::string& URLserver,const std::string& port)
{
  // configure the connection
  hapticClient = new ioc_comm::Client(URLserver,port,ioc_comm::HAPTIC,1.0, 6);
}
// destructor
hapticConnection::~hapticConnection()
{

}
int hapticConnection::startConnection(void)
{
 
  hapticClient->start();  
  // set initial values to the force to be send
  forceOnHaptic.time_stamp.assign(ioc_comm::cal_time_stamp());
  forceOnHaptic._data.at(0) = 0.0;
  forceOnHaptic._data.at(1) = 0.0;
  forceOnHaptic._data.at(2) = 0.0;
  forceOnHaptic._data.at(3) = 0.0;
  forceOnHaptic._data.at(4) = 0.0;
  forceOnHaptic._data.at(5) = 0.0;
  // send initial force values
  sendingData.push_back(forceOnHaptic);
  hapticClient->setSendingData(sendingData);
  
}
void hapticConnection::closeConnection(void )
{
  hapticClient->close();
}
void hapticConnection::getHapticPosition(void )
{
    std::stringstream sstream;
    sstream.precision(3);
    hapticClient->getServerData(serverData);
    if(serverData.size() > 0)
    {
      ioc_comm::baseData& temp = serverData[0];
      sstream<<temp.time_stamp<<"\t";
      for(unsigned int i =0; i < temp._data.size();i++)
      {
	sstream << temp._data[i] <<"\t";
	std::cout<<sstream.str()<<std::endl;
      }
      sstream.clear();
    }

}
// enable haptic slot for reading data from haptic device
void hapticConnection::enable_haptic_readings()
{
  getHapticPosition();
  // emit signal to update data on GUI and other components
  emit sendHapticData(serverData);
}

#include "hapticConnection..moc"

