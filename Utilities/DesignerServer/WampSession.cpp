#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include "libs/rapidjson/document.h"
extern "C"
{
  #include "libs/libwebsockets/libwebsockets.h"
}
#include <stdexcept>
#include "WampSession.H"

enum wamp_message_types {
  /* Auxillary */
  WAMP_WELCOME, // server-to-client
  WAMP_PREFIX, // client-to-server
  /* RPC */
  WAMP_CALL, // client-to-server
  WAMP_CALLRESULT, // server-to-client
  WAMP_CALLERROR, // server-to-client
  /* PubSub */
  WAMP_SUBSCRIBE, // client-to-server
  WAMP_UNSUBSCRIBE, // client-to-server
  WAMP_PUBLISH, // client-to-server
  WAMP_EVENT // server-to-client
};


WampSession::WampSession(libwebsocket_context *ctx, libwebsocket *wsi) :
  callbackTable(NULL)
{
  this->wsContext = ctx;
  this->wsInterface = wsi;
}

WampSession::~WampSession() 
{
}

void WampSession::setCallbackTable(std::map<std::string, std::function<std::string (rapidjson::Document const &)> > * aCallbackTable)
{
  callbackTable = aCallbackTable;
}

void WampSession::routeMsg(rapidjson::Document const & document)
{
  int wamp_msg_type = document[0u].GetInt();
  
  switch(wamp_msg_type) {
    case WAMP_PREFIX:
      recvPrefix(document);
      break;
    
    case WAMP_CALL:
      recvCall(document);
      break;
    
    case WAMP_SUBSCRIBE:
      recvSubscribe(document);
      break;
    
    case WAMP_UNSUBSCRIBE:
      recvUnsubscribe(document);
      break;
    
    case WAMP_PUBLISH:
      recvPublish(document);
      break;
  }
  // if(wamp_msg_type == WAMP_PREFIX) {
  //   std::cerr << "Prefix not supported yet." << std::endl;
  //   throw std::runtime_error("Prefix not supported");
  // } else if(wamp_msg_type == WAMP_CALL) {
  //   std::cout << "WAMP_CALL" << std::endl;
  //   
  //   std::cout << "Call id?" << std::endl;
  //   std::string callID = document[1u].GetString();
  //   std::cout << callID << " now procURI? " << std::endl;
  //   std::string procURI = document[2u].GetString();
  //   
  //   std::cout << callID << " " << procURI << std::endl;
  //   std::cout << &itsCallbacks << std::endl;
  //   assert(&itsCallbacks != NULL);
  //   
  //   if(itsCallbacks.count(procURI)) {
  //     std::cout << "Do callback\n";
  //     // std::cout << "Calling function: " << itsCallbacks[procURI].target<void>() << std::endl;
  //     // itsCallbacks[procURI](document);
  //     // std::stringstream ss;
  //     // ss << "[3, \"" << callID << "\", " << result << "]";
  //     // libwebsocket_write(wsi, (unsigned char*)ss.c_str(), ss.length(), LWS_WRITE_TEXT);
  //   } else {
  //     std::cerr << "No callback registered for [" << procURI << "]" << std::endl;
  //   }
  // } else {
  //   std::cerr << "Type " << wamp_msg_type << " not yet supported" << std::endl;
  // }
}

// Actions
void WampSession::sendWelcome()
{
  std::cout << "Send Welcome Msg" << std::endl;
  std::string msg = "[0, \"v59mbCGDXZ7WTyxB\", 1, \"Autobahn/0.5.1\"]";
  libwebsocket_write(wsInterface, (unsigned char*)msg.c_str(), msg.length(), LWS_WRITE_TEXT);
}

void WampSession::recvPrefix(rapidjson::Document const & document)
{
  throw std::runtime_error("Prefix not supported");
}
void WampSession::recvCall(rapidjson::Document const & document)
{
  std::cout << "RPC Call\n";
  // Only supports one argument for now
  std::string callID = document[1u].GetString();
  std::string procURI = document[2u].GetString();
  
  // if(itsCallbacks.count(procURI)) {
  //   std::cout << "Will make call" << std::endl;
  //   
  // } else {
  //   std::cerr << "URI is unregistered.\n";
  // }
}
void WampSession::sendCallResult()
{
  
}
void WampSession::sendCallError()
{
  
}
void WampSession::recvSubscribe(rapidjson::Document const & document)
{
  
}
void WampSession::recvUnsubscribe(rapidjson::Document const & document)
{
  
}
void WampSession::recvPublish(rapidjson::Document const & document)
{
  
}
void WampSession::sendEvent()
{
  
}
