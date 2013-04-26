
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HELPER CLASSES
//
//	these are helper classes for the rtsp client from live555 examples
//	date:		April/2013
//	author:		Henry Portilla
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ourRTSPClient.h"

// Implementation of "ourRTSPClient":

ourRTSPClient* ourRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
					int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) {
  return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

ourRTSPClient::ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
			     int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
  : RTSPClient(env,rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum) {
}

ourRTSPClient::~ourRTSPClient() {
}