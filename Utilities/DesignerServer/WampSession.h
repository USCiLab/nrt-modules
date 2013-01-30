#ifndef WAMPSERVER_H
#define WAMPSERVER_H

#include <string>
#include "libs/rapidjson/document.h"
#include <map>
#include <functional>
#include <thread>

class libwebsocket_context;
class libwebsocket;

#define WAMP_SERVER_IDENTITY "libwamp 0.1"
#define WAMP_PROTOCOL_VERSION 1

class WampSession
{  
  public:
    WampSession(libwebsocket_context *ctx, libwebsocket *wsi);
    ~WampSession();
    
  public:
    void setCallbackTable(std::map<std::string, std::function<std::string (rapidjson::Document const &)> >*);
        
  public:
    /**
    * Using the message type given in the argument, call the appropriate recv function.
    */
    void routeMsg(rapidjson::Document const & document);
    
    /**
    * Handle events, either recv (from client) or send (to client)
    */
  public:
    void sendWelcome();
  private:
    void recvPrefix(rapidjson::Document const & document);
    void recvCall(rapidjson::Document const & document);
    void sendCallResult();
    void sendCallError();
    void recvSubscribe(rapidjson::Document const & document);
    void recvUnsubscribe(rapidjson::Document const & document);
    void recvPublish(rapidjson::Document const & document);
    void sendEvent();
        
  private:
    libwebsocket_context *wsContext;
    libwebsocket *wsInterface;
    
    std::string sessionID;
    
    std::map<std::string, std::function<std::string (rapidjson::Document const &)> > *callbackTable;
  private:
    
};

#endif // WAMPSERVER_H

