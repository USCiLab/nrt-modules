#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include "libs/rapidjson/document.h"
extern "C"
{
  #include "libs/libwebsockets/libwebsockets.h"
}
#include <stdexcept>
#include "WampSession.h"
#include "WampServer.h"

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


WampSession::WampSession(WampServer* parent, libwebsocket_context *ctx, libwebsocket *wsi) :
server(nullptr)
{
  this->server = parent;
  this->wsContext = ctx;
  this->wsInterface = wsi;
}

WampSession::~WampSession() 
{
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
}

// Actions
void WampSession::sendWelcome()
{
  std::cout << "Send Welcome Msg" << std::endl;
  std::string msg = "[0, \"v59mbCGDXZ7WTyxB\", 1, \"Autobahn/0.5.1\"]";
  writeText(msg);
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
  
  if(server->getCallbackTable()->count(procURI)) {
    std::cout << "Will make call" << std::endl;
    
    try {
      std::string result = (*server->getCallbackTable())[procURI](document);
      std::cout << "Success" << std::endl;
      sendCallResult(callID, result);
      
    } catch(const WampRPCException &e) {
      std::cerr << "Error " << e.what() << std::endl;
      sendCallError(callID, e.getErrorURI(), e.getErrorDesc(), e.getErrorDetails());
    }
    
  } else {
    std::cerr << "URI is unregistered.\n";
  }
}

void WampSession::sendCallResult(std::string const & callID, std::string const & msg)
{
  std::stringstream ss;
  
  ss << "[" << WAMP_CALLRESULT << ", \"" << callID << "\", " << msg << "]";
  
  writeText(ss.str());
}

void WampSession::sendCallError(std::string const & callID, std::string const & errorURI, std::string const & errorDesc, std::string const & errorDetails)
{
  std::stringstream ss;
  
  ss << "[" << WAMP_CALLERROR << ", \"" << callID << "\", \"" << errorURI << "\", \"" << errorDesc << "\"";
  if(errorDetails == "") {
    ss << "]";
  } else {
    // errorDetails is already escaped json
    ss << ", " << errorDetails << "]";
  }
  
  writeText(ss.str());
}

void WampSession::recvSubscribe(rapidjson::Document const & document)
{
  std::string topicURI = document[1u].GetString();
  std::map<std::string, std::vector<WampSession*>> *table = server->getSubscriptionTable();
  
  // Register ourself
  (*table)[topicURI].push_back(this);
  std::cout << "Registered for topic " << topicURI << std::endl;
}

void WampSession::recvUnsubscribe(rapidjson::Document const & document)
{
  
}

void WampSession::recvPublish(rapidjson::Document const & document)
{
  
}

void WampSession::sendEvent(std::string const & topic, std::string const & msg)
{
  std::stringstream ss;
  ss << "[" << WAMP_EVENT << ", \"" << topic << "\", " << msg << "]";
  
  writeText(ss.str());
}


void WampSession::writeText(std::string const & msg)
{
  libwebsocket_write(wsInterface, (unsigned char*)msg.c_str(), msg.length(), LWS_WRITE_TEXT);
}