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
 *	Notes:	The code is based on the DeviceSource.cpp
		from live555 libraries and is released
 *		under the same Lesser General Public License.
 */

#include <GroupsockHelper.hh> // for "gettimeofday()"
#include "blueCherrySource.h"

BlueCherrySource*
BlueCherrySource::createNew(UsageEnvironment& env,
			CamParameters params) {
  return new BlueCherrySource(env,params);
}



unsigned BlueCherrySource::referenceCount = 0;

BlueCherrySource::BlueCherrySource(UsageEnvironment& env,
			   CamParameters params)
  : FramedSource(env), fParams(params) {
  if (referenceCount == 0) {
    // Any global initialization of the device would be done here:
    //%%% TO BE WRITTEN %%%
    ourScheduler = BasicTaskScheduler::createNew();
    isDataAvailable = false;
    
  }
  ++referenceCount;

  // Any instance-specific initialization of the device would be done here:
  //%%% TO BE WRITTEN %%%
  eventTriggerId = 0;

  // We arrange here for our "deliverFrame" member function to be called
  // whenever the next frame of data becomes available from the device.
  //
  // If the device can be accessed as a readable socket, then one easy way to do this is using a call to
  //     envir().taskScheduler().turnOnBackgroundReadHandling( ... )
  // (See examples of this call in the "liveMedia" directory.)
  //
  // If, however, the device *cannot* be accessed as a readable socket, then instead we can implement it using 'event triggers':
  // Create an 'event trigger' for this device (if it hasn't already been done):
  if (eventTriggerId == 0) {
      
    
    eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
    printf("creating event trigger %d\n",eventTriggerId);
  }
}

BlueCherrySource::~BlueCherrySource() {
  // Any instance-specific 'destruction' (i.e., resetting) of the device would be done here:
  //%%% TO BE WRITTEN %%%

  --referenceCount;
  if (referenceCount == 0) {
    // Any global 'destruction' (i.e., resetting) of the device would be done here:
    //%%% TO BE WRITTEN %%%

    // Reclaim our 'event trigger'
    printf("deleting event trigger %d\n",eventTriggerId);
    envir().taskScheduler().deleteEventTrigger(eventTriggerId);
    eventTriggerId = 0;
  }
}

void BlueCherrySource::doGetNextFrame() {
  // This function is called (by our 'downstream' object) when it asks for new data.
   printf("calling doGetNexFrame\n");
   //deliverFrame();
  // Note: If, for some reason, the source device stops being readable (e.g., it gets closed), then you do the following:
  if (!isDataAvailable /* the source stops being readable */ /*%%% TO BE WRITTEN %%%*/) {
    handleClosure(this);
    return;
  }

  // If a new frame of data is immediately available to be delivered, then do this now:
  if (isDataAvailable /* a new frame of data is immediately available to be delivered*/ ) {
      
    deliverFrame();
    isDataAvailable = false;
  }

  // No new data is immediately available to be delivered.  We don't do anything more here.
  // Instead, our event trigger must be called (e.g., from a separate thread) when new data becomes available.
}

void BlueCherrySource::deliverFrame0(void* clientData) {
      ((BlueCherrySource*)clientData)->deliverFrame();
}

void BlueCherrySource::deliverFrame() {
  // This function is called when new frame data is available from the device.
  // We deliver this data by copying it to the 'downstream' object, using the following parameters (class members):
  // 'in' parameters (these should *not* be modified by this function):
  //     fTo: The frame data is copied to this address.
  //         (Note that the variable "fTo" is *not* modified.  Instead,
  //          the frame data is copied to the address pointed to by "fTo".)
  //     fMaxSize: This is the maximum number of bytes that can be copied
  //         (If the actual frame is larger than this, then it should
  //          be truncated, and "fNumTruncatedBytes" set accordingly.)
  // 'out' parameters (these are modified by this function):
  //     fFrameSize: Should be set to the delivered frame size (<= fMaxSize).
  //     fNumTruncatedBytes: Should be set iff the delivered frame would have been
  //         bigger than "fMaxSize", in which case it's set to the number of bytes
  //         that have been omitted.
  //     fPresentationTime: Should be set to the frame's presentation time
  //         (seconds, microseconds).  This time must be aligned with 'wall-clock time' - i.e., the time that you would get
  //         by calling "gettimeofday()".
  //     fDurationInMicroseconds: Should be set to the frame's duration, if known.
  //         If, however, the device is a 'live source' (e.g., encoded from a camera or microphone), then we probably don't need
  //         to set this variable, because - in this case - data will never arrive 'early'.
  // Note the code below.
  
//   if (!isCurrentlyAwaitingData()){ 
//       printf("We are not ready for next data for delivery\n");
//       return; // we're not ready for the data yet
//   }
    
    
  uint8_t* newFrameDataStart  = (uint8_t*)0xD15EA5E; 
  newFrameDataStart = NAL_data.frame.data;
  unsigned newFrameSize = (unsigned)NAL_data.frame.size; 
    printf("new H.264 RTSP %d data is available\n",newFrameSize);
  
  // Deliver the data here:
  if (newFrameSize > fMaxSize) {
    fFrameSize = fMaxSize;
    fNumTruncatedBytes = newFrameSize - fMaxSize;
  } else {
    fFrameSize = newFrameSize;
  }

  gettimeofday(&fPresentationTime, NULL); // If you have a more accurate time - e.g., from an encoder - then use that instead.
  // If the device is *not* a 'live source' (e.g., it comes instead from a file or buffer), then set "fDurationInMicroseconds" here.
  if (newFrameSize <= fMaxSize){
     memmove(fTo, newFrameDataStart+4, newFrameSize -4);
     debug_bytes(fTo,50);
    //isDataAvailable = false;
  }

  // After delivering the data, inform the reader that it is now available:
  FramedSource::afterGetting(this);
}

EventTriggerId BlueCherrySource::getEventTriggerID()
{
  EventTriggerId eventTrigger;
  eventTrigger = eventTriggerId;
  printf("calling event trigger %d\n",eventTrigger);
  return eventTrigger;
}
// add a method to write the data to the class
void BlueCherrySource::setData(H264Frame newH264data)
{
    int maxSize = 100000;
    // copy the data
    NAL_data = newH264data;
   // uint8_t *newdata = (new uint8_t[NAL_data.frame.size - 4]); 
    if((NAL_data.frame.size>0)&(NAL_data.frame.size<maxSize)){
	//memcpy(newdata,newH264data.frame.data+4,newH264data.frame.size);
	//bufferH264.push_back(newdata);
	printf("H264 NAL size: %d\n",NAL_data.frame.size);
	//debug_bytes(newH264data.frame.data,50);
	envir().taskScheduler().triggerEvent(eventTriggerId,this);
	isDataAvailable = true;
	
    }
    //delete [] newdata;
    //newdata = NULL;
}


// The following code would be called to signal that a new frame of data has become available.
// This (unlike other "LIVE555 Streaming Media" library code) may be called from a separate thread.
// (Note, however, that "triggerEvent()" cannot be called with the same 'event trigger id' from different threads.
// Also, if you want to have multiple device threads, each one using a different 'event trigger id', then you will need
// to make "eventTriggerId" a non-static member variable of "DeviceSource".)
void BlueCherrySource::signalNewDataFrame(void* clientData){

  int maxSize = 100000;
  TaskScheduler *ourScheduler = NULL;
  dataForRTSP *myData = (dataForRTSP*)clientData;
  
  // get the data
  BlueCherrySource *source(myData->getSource());
  H264Frame NAL_frames = myData->getNALdata();
  if((NAL_frames.frame.size>0)&(NAL_frames.frame.size<maxSize)){
	printf("sending frame to RTSP server\n");
	source->setData(NAL_frames);
  }
 
  // set the trigger for warn about new data
  ourScheduler = &(source->envir().taskScheduler());
  if ( ourScheduler != NULL) { // sanity check;
    ourScheduler->triggerEvent(source->eventTriggerId, source);
  }
}

// add another statis function but only takes the DeviceSource object
void BlueCherrySource::signalNewDataSource(BlueCherrySource* clientSource, H264Frame clientData)
{
    TaskScheduler *ourScheduler = NULL;
    ourScheduler = &(clientSource->envir().taskScheduler());
    
    // set the trigger for warn about new data
    
    if ( ourScheduler != NULL) { // sanity check;
    //ourScheduler->triggerEvent(clientSource->eventTriggerId, clientSource);
    clientSource->setData(clientData);  
    
  }
}

// set the boolean flag that wait data to true
void BlueCherrySource::setCurrentlyAwaitingData(bool value)
{
    
}



