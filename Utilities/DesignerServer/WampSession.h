#ifndef WAMPSERVER_H
#define WAMPSERVER_H

#include <string>
#include "libs/rapidjson/document.h"
#include <map>
#include <functional>
#include <thread>

class libwebsocket_context;
class libwebsocket;
class WampServer;

#define WAMP_SERVER_IDENTITY "libwamp 0.1"
#define WAMP_PROTOCOL_VERSION 1

class WampSession
{  
  public:
    WampSession(WampServer* parent, libwebsocket_context *ctx, libwebsocket *wsi);
    ~WampSession();
    
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
    void sendEvent(std::string const & topic, std::string const & msg);
    
  private:
    void recvPrefix(rapidjson::Document const & document);
    void recvCall(rapidjson::Document const & document);
    void sendCallResult(std::string const & callID, std::string const & msg);
    void sendCallError();
    void recvSubscribe(rapidjson::Document const & document);
    void recvUnsubscribe(rapidjson::Document const & document);
    void recvPublish(rapidjson::Document const & document);
  
  private:
    // These abstract the raw io commands of libwebsocket
    void writeText(std::string const & msg);
    
  private:
    libwebsocket_context *wsContext;
    libwebsocket *wsInterface;
    
    std::string sessionID;
    
    WampServer *server;
  private:
    
};

#endif // WAMPSERVER_H

