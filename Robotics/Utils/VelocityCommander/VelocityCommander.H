#ifndef VELOCITYCOMMANDER_H
#define VELOCITYCOMMANDER_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/VelocityMessage.H>

namespace nrt { class DisplayImageSink; }

namespace velocitycommander
{
  NRT_DECLARE_MESSAGEPOSTER_PORT( VelocityCommand, nrt::VelocityMessage, void, "A velocity to send" );
}

//! A simple module to allow keyboard control of robots
/*! This module simply opens a new GTK window, and allows users to use the arrow keys to send out
  velocity command messages. */
class VelocityCommanderModule : public nrt::Module,
  public nrt::MessagePoster<velocitycommander::VelocityCommand>
{
  public:
    VelocityCommanderModule(std::string const& instanceName="");

    void run();

    void keyCallback(nrt::int32 key);

  private:
    std::mutex itsMtx;
    std::shared_ptr<nrt::DisplayImageSink> itsDisplaySink;
    double itsAngularVel;
    double itsLinearVel;
};

#endif // VELOCITYCOMMANDER_H 
