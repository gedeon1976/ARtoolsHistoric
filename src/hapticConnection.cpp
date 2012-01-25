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
	//Vect6 HapticData(6);
	HapticDevice->getPosition(HapticPosition);
	position = HapticPosition.getTranslation();
	orientation = HapticPosition.getRotation();
		
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
// SLOTS

// enable haptic slot for reading data from haptic device
void hapticConnection::enable_haptic_readings()
{
  getHapticPosition();
  emit sendHapticData(HapticPosition);
  // set workspace limits
  
  // emit signal to update data on GUI and other components
  //emit sendHapticData(HapticData0);
  //emit sendHapticData(position);
  
}
// get the current cubic limits for the haptic workspace
void hapticConnection::getWorkSpaceLimits(mt::Vector3 MinCubicLimits, mt::Vector3 MaxCubicLimits)
{
	float Xmin = 0,Xmax = 100;
	float Ymin = 0,Ymax = 100;
	float Zmin = 0,Zmax = 100;
	/*mt::Scalar Force_X(mt::Scalar(0.0));
	mt::Scalar Force_Y(mt::Scalar(0.0));
	mt::Scalar Force_Z(mt::Scalar(0.0));*/
	Vect6 Force(6);
	Vect6 Speed(6);

	// set values to the force to be send
	Force[0] = 0;// X
	Force[1] = 0;//	Y
	Force[2] = 0;// Z 
	Force[3] = 0; 
	Force[4] = 0; 
	Force[5] = 0;

	Xmin = MinCubicLimits[0];Xmax = MaxCubicLimits[0];
	Ymin = MinCubicLimits[1];Ymax = MaxCubicLimits[1];
	Zmin = MinCubicLimits[2];Zmax = MaxCubicLimits[2];

	// get haptic speeds
	HapticDevice->getVelocity(Speed);

	// set forces according to haptic position
	// We want to display haptic walls
	// using F = K*delta_X - B*speed
	// X Forces
	//float Kstiffness = 0.25;
	//float B = -1.0;
	//float Delta_X = 20;
	//if (position[0] <= Xmin){
	//	float pos = abs(position[0]);
	//	Force[0]= Kstiffness*pos;// - B*Speed[2];		
	//}else if(position[0] >= Xmax){
	//	Force[0]= -2.0;;
	//}else{
	//	Force[0]= 0.0;;	}
	//// Y Forces
	//if (position[1] <= Ymin){
	//	Force[1]= 2.0;
	//}else if(position[1] >= Ymax){
	//	Force[1]= -2.0;;
	//}else{
	//	Force[1]= 0.0;;	}
	//// Z Forces
	//if (position[2] <= Zmin){
	//	Force[2]= 2.0;
	//}else if(position[2] >= Zmax){
	//	Force[2]= -2.0;;
	//}else{
	//	Force[2]= 0.0;;	}
 //
	//HapticDevice->setForce(Force);
}


#include "hapticConnection..moc"

