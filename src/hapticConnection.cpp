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

//constructors
hapticConnection::hapticConnection(void)
{
	// Create haptic device
	hapticStatus = false;
	HapticDevice = new haptic::Haptic(hapticStatus);
	//ForceonHaptic = new Vect6(6);
	//HapticData = new Vect6(6);
	/*ForceonHaptic[0] = 0; HapticData[0] = 0;
	ForceonHaptic[1] = 0; HapticData[1] = 0;
	ForceonHaptic[2] = 0; HapticData[2] = 0;
	ForceonHaptic[3] = 0; HapticData[3] = 0;
	ForceonHaptic[4] = 0; HapticData[4] = 0;
	ForceonHaptic[5] = 0; HapticData[5] = 0;*/
	
}

hapticConnection::hapticConnection(const std::string& URLserver,const std::string& port)
{
  // configure the connection
  //hapticClient = new ioc_comm::Client(URLserver,port,ioc_comm::HAPTIC,1.0, 6);
}
// destructor
hapticConnection::~hapticConnection()
{
	// close the haptic connection
	HapticDevice->stop();
}
void hapticConnection::startConnection(void)
{
	Vect6 ForceonHaptic(6);
	HapticDevice->calibrate();
	HapticDevice->start();

  // set initial values to the force to be send
	ForceonHaptic[0] = 0; 
	ForceonHaptic[1] = 0;
	ForceonHaptic[2] = 0; 
	ForceonHaptic[3] = 0; 
	ForceonHaptic[4] = 0; 
	ForceonHaptic[5] = 0; 
	HapticDevice->setForce(ForceonHaptic);
 
    
}
void hapticConnection::closeConnection(void )
{
  //hapticClient->close();
}
void hapticConnection::getHapticPosition(void )
{
	Vect6 HapticData(6);
	HapticDevice->getPosition(HapticPosition);
	position = HapticPosition.getTranslation();
	

	/*   std::stringstream sstream;
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
    }*/

}
// enable haptic slot for reading data from haptic device
void hapticConnection::enable_haptic_readings()
{
  getHapticPosition();
  // emit signal to update data on GUI and other components
  //emit sendHapticData(HapticData0);
  emit sendHapticData(position);
}

#include "hapticConnection..moc"

