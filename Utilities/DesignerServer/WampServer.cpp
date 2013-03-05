#include "WampServer.h"
#include <iostream>
#include <algorithm>
extern "C"
{
  #include "libs/libwebsockets/libwebsockets.h"
}

enum wamp_protocols {
  /* always first */
  PROTOCOL_HTTP = 0,

  PROTOCOL_WAMP_WS,

  /* always last */
  DEMO_PROTOCOL_COUNT
};

#define LOCAL_RESOURCE_PATH "/home/chris/nrt-modules/Utilities/DesignerServer/files"

/* this protocol server (always the first one) just knows how to do HTTP */
static int callback_http(struct libwebsocket_context *context,
    struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len)
{
  char client_name[128];
  char client_ip[128];
  char filenamebuffer[1024];
  char *input = (char*)in;

  switch (reason) {
  case LWS_CALLBACK_HTTP:
    if(input[0] == '/' && input[1] == 0)
      sprintf(filenamebuffer, LOCAL_RESOURCE_PATH"/index.html");
    else
      snprintf(filenamebuffer, 1024, LOCAL_RESOURCE_PATH"%s", input);

    if (in && strcmp(input, "/favicon.ico") == 0) {
      if (libwebsockets_serve_http_file(wsi,
           LOCAL_RESOURCE_PATH"/favicon.ico", "image/x-icon"))
        std::cerr << "Failed to send favicon" << std::endl;
      break;
    }

    if (libwebsockets_serve_http_file(wsi, filenamebuffer, ""))
      std::cerr << "Failed to send HTTP file" << std::endl;
    break;

  default: break;
  }

  return 0;
}

struct per_session_data__wamp_server {
  WampSession *ws;
};

static int callback_wamp_ws(struct libwebsocket_context *context,
                            struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason,
                            void *user, void *in, size_t len)
{
  struct per_session_data__wamp_server *pss = (per_session_data__wamp_server*)user;
  WampSession *ws = pss->ws;

  rapidjson::Document document;
  std::string msgtype;

	switch (reason) {
    case LWS_CALLBACK_ESTABLISHED:
      // First time getting called, create the wamp session object
      pss->ws = new WampSession(&WampServer::getInstance(), context, wsi);
      ws = pss->ws;

      ws->sendWelcome();
      break;

    case LWS_CALLBACK_CLOSED:
      // Last time getting called, delete our object and clean up
      WampServer::getInstance().endSession(ws);
      delete ws;
      std::cout << "Disconnect\n\n\n";
      break;

    case LWS_CALLBACK_RECEIVE:
      std::cout << "[WampServer] Got Raw Data: " << (char*)in << std::endl;
      if(document.Parse<0>((char*)in).HasParseError() || !document.IsArray() )
      {
        std::cerr << "[WampServer] ERROR parsing request" << std::endl;
        break;
      }

      ws->routeMsg(document);
      break;

    // case LWS_CALLBACK_BROADCAST:
    //
    //   // n = libwebsocket_write(wsi, (unsigned char*)in, len, LWS_WRITE_TEXT);
    //   // if (n < 0) {
    //   //   std::cerr << "ERROR writing to socket" << std::endl;
    //   //   return 1;
    //   // }
    //   break;

    default:
      break;

  }

  return 0;
}

/* list of supported protocols and callbacks */
static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",	 /* name */
		callback_http, /* callback */
		0			         /* per_session_data_size */
	},
  {
    "wamp",
    callback_wamp_ws,
    sizeof(per_session_data__wamp_server)
  },
	{
		NULL, NULL, 0		/* End of list */
	}
};


WampServer::WampServer() :
itsRunning(false),
itsContext(nullptr)
{

}

WampServer::~WampServer()
{

}

void WampServer::start(int port, std::string interface)
{
  if(itsContext != nullptr) stop();

  itsContext = libwebsocket_create_context(port, nullptr, protocols,
                                           libwebsocket_internal_extensions, nullptr, nullptr,
                                           -1, -1, 0);

  if (itsContext == NULL) throw std::runtime_error("libwebsocket init failed");

  itsRunning = true;
  itsThread = std::thread(std::bind(&WampServer::serveRequests, this));
}

void WampServer::stop()
{
  if(itsContext != nullptr)
    libwebsocket_context_destroy(itsContext);
  itsContext = nullptr;
  itsRunning = false;
  try{ itsThread.join(); } catch(...) {}
}

void WampServer::broadcastEvent(std::string topic, std::string message)
{
  std::cout << "[WampServer] Broadcast on " << topic << std::endl;
  std::vector<WampSession*> interested_parties = itsSubscriptions[topic];

  for(std::vector<WampSession*>::iterator it = interested_parties.begin(); it != interested_parties.end(); it++) {
    std::cout << "\t=> Someone is interested\n";
    WampSession *ws = *it;
    ws->sendEvent(topic, message);
  }
}

void WampServer::registerProcedure(std::string const & messageName, std::function<std::string (rapidjson::Document const&)> callback)
{
  itsCallbacks[messageName] = callback;
}

void WampServer::endSession(WampSession* ws)
{
  // Remove from registered subscriptions
  for(std::map<std::string, std::vector<WampSession*>>::iterator it = itsSubscriptions.begin(); it != itsSubscriptions.end(); it++) {
    std::vector<WampSession*> *sessions = &it->second;

    sessions->erase(std::remove_if(sessions->begin(), sessions->end(), [ws](WampSession* wampsession){
      return wampsession == ws;
    }), sessions->end());
  }

}

void WampServer::serveRequests()
{
  while(itsRunning)
    libwebsocket_service(itsContext, 50);
}
