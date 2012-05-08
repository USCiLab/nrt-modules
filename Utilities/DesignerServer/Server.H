#include <string>
class libwebsocket_context;
class Server
{
  public:
    Server();
    ~Server();
    void start(int port, std::string interface="");
    void stop();

  private:
    libwebsocket_context* itsContext;
};
