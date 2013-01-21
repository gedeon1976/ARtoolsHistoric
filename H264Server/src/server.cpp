/*
 * 
 * 	Server class
 * 
 * 	brief@	this class contains the basic functionality
 * 		to stream a H.264 encoded by hardware input
 * 		using RTSP and RTP/RTCP protocols
 * 
 * 	date		January/2013
 * 	author		Henry Portilla
 * 
 */

#include "server.h"
// constructor
Server::Server()
{
      // set ports and maximum jumps
      rtpPortNumBase = 18888;
      ttl = 255; 
}

// destructor
Server::~Server()
{

}
// Setup server
void Server::setupServer(int rtspPort)
{
  try{
      // Begin by setting up our usage environment:
      scheduler = BasicTaskScheduler::createNew();
      env = BasicUsageEnvironment::createNew(*scheduler);
      
      // create groupsocks for RTP and RTCP
      destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
      // Note:: this is a multicast address
      
      // create a RTSP server     
      Port outPort(rtspPort);
      rtspServer = RTSPServer::createNew(*env,outPort);
      if (rtspServer == NULL) {
	*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
	exit(1);
      }
      
     
    
  }catch(...){
    
  }
}
// Add a RTP session it manages a camera input
void Server::AddRTPSession(const char* name)
{
  try{
      int i = 0;
      unsigned short rtpPortNum;
      unsigned short rtcpPortNum;
      RTPSink *videoSink;			// RTP sink         
      
      rtpPortNum = rtpPortNumBase + i;
      rtcpPortNum = rtpPortNumBase + 1;
      
      Port rtpPort(rtpPortNum);
      Port rtcpPort(rtcpPortNum);
      
      // sockets groupsock 
      Groupsock rtpGroupsock(*env,destinationAddress,rtpPort,ttl);
      rtpGroupsock.multicastSendOnly(); // we're a SSM source
      Groupsock rtcpGroupsock(*env,destinationAddress,rtcpPort,ttl);
      rtcpGroupsock.multicastSendOnly();// we're a SSM source
      
      // create a video RTP sink from the rtpGroupsock
      OutPacketBuffer::maxSize = 100000;
      videoSink = H264VideoRTPSink::createNew(*env,&rtpGroupsock,96);
      
      // create and start a RTCP instance  for this RTP sink
      unsigned estimatedSessionBandwidth = 500;// in kbps; for RTCP b/w share
      unsigned maxCNAMElen = 100;
      unsigned char CNAME[maxCNAMElen+1];
      gethostname((char*)CNAME,maxCNAMElen);
      CNAME[maxCNAMElen]='\0';			// just in case
      
      RTCPInstance* rtcp = RTCPInstance::createNew(*env,&rtcpGroupsock,
						   estimatedSessionBandwidth,
						   CNAME,videoSink,NULL/*we're a server*/,
						   True /* we're a SSM source */);
      // Note: This starts RTCP running automatically
      
      ServerMediaSession *sms =
      ServerMediaSession::createNew(*env,name,name,
				    "Session streamed by \"H264VideoStreamer\"",
				    True/*SSM*/);
      sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink,rtcp));
      
      // add camera input to the rtsp Server
      rtspServer->addServerMediaSession(sms);
      
      // show access address
      *env<<"play this stream using the URl"<<rtspServer->rtspURL(sms)<<"\"\n";
    
  }catch(...){
  
  }
}

// Get an used RTP session
ServerMediaSession* Server::getRTPSession(int i)
{

}

// start the server
bool Server::play(void )
{

}

// stop the server
bool Server::stop(void )
{

}

#include "server.moc"




