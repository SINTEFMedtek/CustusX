#include "cxGrabber.h"

namespace cx
{

Grabber::Grabber() :
    QObject()
{
  typedef cx::Frame Frame;
  qRegisterMetaType<Frame>("Frame");
}

}//namespace cx
