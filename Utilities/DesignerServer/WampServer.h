#ifndef WAMP_SERVER_FACTORY
#define WAMP_SERVER_FACTORY

#include <vector>
#include <string>
#include "WampSession.h"

class WampServer
{
public:
  static WampServer& getInstance()
  {
      static WampServer instance;
      return instance;
  }
private:
  WampServer ();
  WampServer(WampServer const&);              // Don't Implement
  void operator=(WampServer const&); // Don't implement
  
  virtual ~WampServer ();

  /*****************
  * Usable interface
  *****************/
public:
  /**
  * Starts the server on port aPort
  */
  void start(int aPort, std::string interface="");
  
  /**
  * Stop the server
  */
  void stop();
  
  /**
  * Broadcast an event to all interested parties
  */
  void broadcastEvent(std::string topic, std::string msg);
  
  /**
  * Register a procedure for RPC
  */
  void registerProcedure(std::string const & messageName, std::function<std::string (rapidjson::Document const &)> callback);
  
  /*******************
  * Internal interface
  *******************/
public:
  // Removes all traces of ws
  void endSession(WampSession* ws);
  
public:
  std::map<std::string, std::function<std::string (rapidjson::Document const &)>>* getCallbackTable() { return &itsCallbacks; }
  std::map<std::string, std::vector<WampSession*>>* getSubscriptionTable() { return &itsSubscriptions; }
  
private:
  std::vector<WampSession*> sessionPool;
  
  bool itsRunning;
  std::thread itsThread;
  void serveRequests(); // Does not return until server is stopped
  
  libwebsocket_context* itsContext;
  
  std::map<std::string, std::function<std::string (rapidjson::Document const &)>> itsCallbacks;
  std::map<std::string, std::vector<WampSession*>> itsSubscriptions;
};

#endif