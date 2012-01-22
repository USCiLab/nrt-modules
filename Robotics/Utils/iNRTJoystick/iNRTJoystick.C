#include "iNRTJoystick.H"
#include <nrt/ImageProc/IO/ImageSink/DisplayImageSink.H>
#include <nrt/ImageProc/Drawing/Text.H>
#include <nrt/Core/Util/StringUtil.H>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

using namespace nrt;
using namespace inrtjoystick;

// ######################################################################
iNRTJoystickModule::iNRTJoystickModule(std::string const & instanceName) :
  Module(instanceName),
  itsDisplaySink(new DisplayImageSink),
  itsPort(PortParam, this, &iNRTJoystickModule::PortChangeCallback),
  itsWebviewURL(WebviewURLParam, this, &iNRTJoystickModule::WebviewURLChangeCallback)
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
void iNRTJoystickModule::WebviewURLChangeCallback(std::string const & webviewurl)
{
}

// ######################################################################
void iNRTJoystickModule::PortChangeCallback(int const & port)
{
  std::lock_guard<std::mutex> lock(itsMtx);

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
}

// ######################################################################
void iNRTJoystickModule::run()
{
  struct sockaddr_in other;
  int olen = sizeof(other);
  unsigned char axis[6];

  fd_set rfds;
  struct timeval tv;
  int retval;

  while(running())
  {
    // wait for messages from the app
    // convert them to velocity commands
    // repeat
    
    double linear, angular;
    {
      std::lock_guard<std::mutex> lock(itsMtx);
      
      FD_ZERO(&rfds);
      FD_SET(itsSocket, &rfds);
  
      tv.tv_sec = 5;
      tv.tv_usec = 0;

      retval = select(itsSocket+1, &rfds, NULL, NULL, &tv);

      if (retval == -1)
        NRT_FATAL("Error setting up select(): errno=" << errno);

      else if (retval)
      {
        if ( recvfrom(itsSocket, axis, 6, 0, (struct sockaddr*)&other, (unsigned int*)&olen) < 0 && !(errno == EWOULDBLOCK || errno == EAGAIN) )
          NRT_FATAL("Cannot receive on socket: " << errno);

        if (axis[0] == 's' && axis[5] == 'e')
        {
          // (x - 128) / 12 ==> map the x from (-128, 128) to (-10, 10)
          // (y - 128) / 6  ==> map the y from (-128, 128) to (-20, 20)
          linear = (axis[2] - 128)/12;
          angular = (axis[1] - 128)/6;
        }
      }
    }
  
    Image<PixRGB<byte>> image(640, 480, ImageInitPolicy::Zeros);
    nrt::drawText(image, Point2D<int32>(10, 10), nrt::sformat("Translational Vel: %f (raw: %d)", linear, axis[2]));
    nrt::drawText(image, Point2D<int32>(10, 30), nrt::sformat("Rotational Vel   : %f (raw: %d)", angular, axis[1]));
    itsDisplaySink->out(GenericImage(image), "Velocity Commander");

    if ( !(itsLinearVel == linear && itsAngularVel == angular) )
    {
      VelocityMessage::unique_ptr msg(new VelocityMessage);
      msg->linear.x()  = linear;
      msg->angular.z() = angular;
      post<VelocityCommand>(msg);
      itsLinearVel = linear;
      itsAngularVel = angular;
    }
    //usleep(100000);
  }
}

NRT_REGISTER_MODULE(iNRTJoystickModule);
