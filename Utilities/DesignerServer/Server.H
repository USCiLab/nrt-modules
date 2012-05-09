#include <string>
#include "libs/rapidjson/document.h"
#include <map>
#include <functional>

class libwebsocket_context;
class Server
{
  public:
    Server();
    ~Server();
    void start(int port, std::string interface="");
    void stop();

    void broadcastMessage(std::string const & message);
    void receiveMessage(rapidjson::Document const & document);

    void registerCallback(std::string const & messageName, std::function<void(rapidjson::Document const &)> function);

  private:
    libwebsocket_context* itsContext;
    unsigned char *itsStartMsgBuf;
    unsigned char *itsEndMsgBuf;
    std::map<std::string, std::function<void(rapidjson::Document const &)>> itsCallbacks;
};
