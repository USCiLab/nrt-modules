#ifndef VISO_MODULE_H
#define VISO_MODULE_H

#include <nrt/Core/Blackboard/Module.H>
#include <nrt/Robotics/Messages/TransformMessage.H>
#include <Messages/ImageProc/StereoPairMessage.H>
#include "libviso2/viso_stereo.h"

namespace viso 
{
  NRT_DECLARE_MESSAGESUBSCRIBER_PORT(StereoPair, StereoPairMessage, void, "Incoming pair of images");
  NRT_DECLARE_MESSAGEPOSTER_PORT(Transform, nrt::TransformMessage, void, "The transform port");
}

class VisoModule : public nrt::Module,
                   public nrt::MessageSubscriber<viso::StereoPair>,
                   public nrt::MessagePoster<viso::Transform>
{
  public:
    VisoModule(std::string const& instanceName="");

    void onMessage(viso::StereoPair msg);

    void run();

  private:
    nrt::Transform3D itsTransform;
    std::shared_ptr<StereoPairMessage const> itsCurrMessage;
    std::mutex itsMtx;

    Matrix itsPose;
};

#endif // VISO_MODULE_H
