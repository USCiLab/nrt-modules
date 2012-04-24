#include "iNRTJoystick.H"
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/Text.H>
#include <nrt/Core/Util/StringUtil.H>
#include <nrt/Core/Util/MathUtils.H>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"

using namespace nrt;
using namespace inrtjoystick;

// ######################################################################
iNRTJoystickModule::iNRTJoystickModule(std::string const & instanceName) :
  Module(instanceName),
  itsDisplaySink(new DisplayImageSink),
  itsPort(PortParam, this, &iNRTJoystickModule::PortChangeCallback),
  itsMaxLinearVelParam(MaxLinearVel, this),
  itsMaxAngularVelParam(MaxAngularVel, this),
  itsWebviewURL(WebviewURLParam, this)
{ 
  addSubComponent(itsDisplaySink);
}

// ######################################################################
iNRTJoystickModule::~iNRTJoystickModule()
{
  itsRunning = false;
  close(itsSocket);
}

// ######################################################################
void iNRTJoystickModule::PortChangeCallback(int const & port)
{
  std::lock_guard<std::mutex> lock(itsMtx);
  NRT_INFO("Setting port to " << port);

  if (port == 0)
    return;

  // avoid sigpipe when sending data
  signal(SIGPIPE, SIG_IGN);

  itsSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (itsSocket < 0)
    NRT_FATAL("Cannot create socket");

  int yes = 1;
  if ( setsockopt(itsSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) == -1 )
    NRT_FATAL("Error setting socket options to reusable");

  struct sockaddr_in servaddr;
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  // non-blocking mode
  //if ( fcntl(itsSocket, F_SETFL, O_NONBLOCK) < 0 )
  //  NRT_FATAL("Error setting iNRTJoystick server to non-blocking mode");

  if ( bind(itsSocket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0 )
    throw exception::BadParameter(nrt::sformat("Cannot bind to port: %d", port)); 

  itsRunning = true;
  NRT_INFO("Listening...");
}

// ######################################################################
void iNRTJoystickModule::run()
{
  struct sockaddr_in other;
  int olen = sizeof(other);

  #define BUFLEN 512 
  unsigned char json[BUFLEN];

  fd_set rfds;
  struct timeval tv;
  int retval;
  rapidjson::Document doc;

  while(running())
  {
    double linear, angular;
    {
      std::lock_guard<std::mutex> lock(itsMtx);
      
      FD_ZERO(&rfds);
      FD_SET(itsSocket, &rfds);
  
      tv.tv_sec = 5;
      tv.tv_usec = 0;

      NRT_INFO("Selecting...");
      retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);

      if (retval == -1)
        NRT_FATAL("Error setting up select(): errno=" << errno);

      else if (retval)
      {
        memset(json, 0, BUFLEN);
        if ( recvfrom(itsSocket, json, BUFLEN, 0, (struct sockaddr*)&other, (unsigned int*)&olen) < 0 && !(errno == EWOULDBLOCK || errno == EAGAIN) )
          NRT_FATAL("Cannot receive on socket: " << errno);

        //NRT_INFO("Read JSON string: " << json);

        if (doc.Parse<0>((const char*)json).HasParseError())
          NRT_FATAL("JSON parsing error (JSON data follows): " << json);

        if ( doc.IsArray() )
        {
          for (rapidjson::SizeType i = 0; i < doc.Size(); i++)
          {
            if ( doc[i].IsObject() )
            {
              // (joystick position varies from -100.0 to 100.0)

              // Left joystick (linear velocity)
              if ( doc[i]["joystick"].GetInt() == 0 )
                linear = doc[i]["y"].GetDouble() * (itsMaxLinearVelParam.getVal()/100.0);

              // Right joystick (angular velocity)
              if ( doc[i]["joystick"].GetInt() == 1)
                angular = doc[i]["x"].GetDouble() * (itsMaxAngularVelParam.getVal()/100.0);
            }
            else if ( doc[i].IsString() )
            {
              if ( doc[i].GetString() == "refresh" )
              {
                std::string urljson("[{\"url\": \"" + itsWebviewURL.getVal() + "\"}]");
                if ( sendto(itsSocket, urljson.c_str(), urljson.length(), 0, (struct sockaddr*)&other, olen) == -1 )
                  NRT_FATAL("Error sending URL back to iNRTJoystick: " << errno);
              }
            }
            else
              NRT_INFO("Unexpected JSON data (data wasn't a map!)");
          }
        }
        else
          NRT_INFO("Unexpected JSON data (top level object must be an array!)");
      }
      else // select timed out
      {
        linear = 0.0;
        angular = 0.0;
      }
    }
  
    Image<PixRGB<byte>> image(290, 60, ImageInitPolicy::Zeros);
    nrt::drawText(image, Point2D<int32>(10, 10), nrt::sformat("Translational Vel: %f", linear));
    nrt::drawText(image, Point2D<int32>(10, 30), nrt::sformat("Rotational Vel   : %f", angular));
    itsDisplaySink->out(GenericImage(image), "Velocity Commander");

    //if ( !(itsLinearVel == linear && itsAngularVel == angular) )
    {
      VelocityMessage::unique_ptr msg(new VelocityMessage);
      msg->linear.x()  = linear;
      msg->angular.z() = angular;
      post<VelocityCommand>(msg);
      itsLinearVel = linear;
      itsAngularVel = angular;
    }
  }
}

NRT_REGISTER_MODULE(iNRTJoystickModule);
