/*
 * 
 * 	H264Server bluecherry live555 source class
 * 
 * 	brief@	this class serves as source for feed the
		H.264 stream
 * 
 * 	date		February/15/2013
 * 	author		Henry Portilla
 *
 * 	
 *	Notes:	The code is based on the DeviceSource.hh
		from live555 libraries and is released
 *		under the same Lesser General Public License.
 */

#ifndef BLUECHERRYSOURCE_H
#define BLUECHERRYSOURCE_H

#ifndef _FRAMED_SOURCE_HH
#include "FramedSource.hh"
#include "blueCherryCard.h"
#include <QObject>
#endif

// The following class can be used to define specific encoder parameters
class CamParameters {
  //%%% TO BE WRITTEN %%%
};



class BlueCherrySource: public FramedSource{
  
public:
  static BlueCherrySource* createNew(UsageEnvironment& env,
				 CamParameters params);

public:
  EventTriggerId eventTriggerId;
  EventTriggerId getEventTriggerID(void);
  void setData(H264Frame newData);
  static void signalNewDataFrame(void* clientData);
  // Note that this is defined here to be a static class variable, because this code is intended to illustrate how to
  // encapsulate a *single* device - not a set of devices.
  // You can, however, redefine this to be a non-static member variable.

protected:
  BlueCherrySource(UsageEnvironment& env, CamParameters params);
  // called only by createNew(), or by subclass constructors
  virtual ~BlueCherrySource();

private:
  // redefined virtual functions:
  virtual void doGetNextFrame();
  //virtual void doStopGettingFrames(); // optional

private:
  
  static void deliverFrame0(void* clientData);
  void deliverFrame();
  

private:
  
  static unsigned referenceCount; // used to count how many instances of this class currently exist
  CamParameters fParams;
  TaskScheduler *ourScheduler;
  H264Frame NAL_data;
};

// Auxiliary class to transport data and source object
class dataForRTSP{
  public:
      dataForRTSP(){};
      ~dataForRTSP(){};
      // methods
      void setSource(BlueCherrySource *sourceObject){source = sourceObject;};
      void setData(H264Frame data){NAL_data = data;};
      BlueCherrySource* getSource(void){BlueCherrySource *tmpSource = source;return tmpSource;};
      H264Frame getNALdata(void){H264Frame data = NAL_data; return data;};
  private:
      BlueCherrySource *source;
      H264Frame NAL_data;
};

#endif // BLUECHERRYSOURCE_H
