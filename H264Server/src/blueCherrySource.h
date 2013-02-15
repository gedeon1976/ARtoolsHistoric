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
#endif

// The following class can be used to define specific encoder parameters
class DeviceParameters {
  //%%% TO BE WRITTEN %%%
};

class BlueCherrySource: public FramedSource {
public:
  static BlueCherrySource* createNew(UsageEnvironment& env,
				 DeviceParameters params);

public:
  static EventTriggerId eventTriggerId;
  // Note that this is defined here to be a static class variable, because this code is intended to illustrate how to
  // encapsulate a *single* device - not a set of devices.
  // You can, however, redefine this to be a non-static member variable.

protected:
  BlueCherrySource(UsageEnvironment& env, DeviceParameters params);
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
  DeviceParameters fParams;
};


#endif // BLUECHERRYSOURCE_H
