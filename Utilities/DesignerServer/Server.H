#ifndef NRT_MODULES_UTILITIES_DESIGNERSERVER_SERVER_H
#define NRT_MODULES_UTILITIES_DESIGNERSERVER_SERVER_H

#include <string>
#include "libs/rapidjson/document.h"
#include <map>
#include <functional>
#include <thread>

class libwebsocket_context;
class Server
{
  public:
    Server();
    ~Server();
    void start(int port, std::string interface="");
    void stop();

    void broadcastMessage(std::string const & message);

    void registerCallback(std::string const & messageName, std::function<void(rapidjson::Document const &)> callback);

    void receiveMessage(rapidjson::Document const & document);

  private:
    bool itsRunning;
    void serviceThread();
    std::thread itsServiceThread;

    libwebsocket_context* itsContext;
    unsigned char *itsStartMsgBuf;
    unsigned char *itsEndMsgBuf;
    std::map<std::string, std::function<void(rapidjson::Document const &)>> itsCallbacks;
};

#endif // NRT_MODULES_UTILITIES_DESIGNERSERVER_SERVER_H

