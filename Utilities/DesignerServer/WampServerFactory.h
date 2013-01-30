#ifndef WAMP_SERVER_FACTORY
#define WAMP_SERVER_FACTORY

#include <vector>
#include <string>
#include "WampServer.h"

class WampServerFactory
{
public:
  WampServerFactory ();
  virtual ~WampServerFactory ();

public:
  void start(int port, std::string interface="");
  void stop();
  void broadcastMessage(std::string msg);
  void registerProcedure(std::string const & messageName, std::function<std::string (rapidjson::Document const &)> callback);
  
private:
  std::vector<WampServer*> serverPool;
  
  bool itsRunning;
  std::thread itsThread;
  void serveRequests(); // Does not return until server is stopped
  
  libwebsocket_context* itsContext;
  
  std::map<std::string, std::function<std::string (rapidjson::Document const &)> > itsCallbacks;
};

#endif