#include <string>
class libwebsocket_context;
class Server
{
  public:
    Server();
    ~Server();
    void start(int port, std::string interface="");
    void stop();

    void broadcastMessage(std::string const& message);

  private:
    libwebsocket_context* itsContext;
    unsigned char *itsStartMsgBuf;
    unsigned char *itsEndMsgBuf;
    //unsigned char itsStartMsgBuf[LWS_SEND_BUFFER_PRE_PADDING + sizeof("NRT_MESSAGE_BEGIN") + LWS_SEND_BUFFER_POST_PADDING];
    //unsigned char itsEndMsgBuf[LWS_SEND_BUFFER_PRE_PADDING + sizeof("NRT_MESSAGE_END") + LWS_SEND_BUFFER_POST_PADDING];
};
