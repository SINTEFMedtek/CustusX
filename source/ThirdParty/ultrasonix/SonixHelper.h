


#include <QObject>
#include "cxGrabber.h"

class SonixHelper : public QObject
{
  //Q_OBJECT

public:
  signals:
  void newFrame(cx::Frame newFrame); ///< Emitted when a new frame is available

public:
  void emitFrame(cx::Frame newFrame) {emit newFrame;}
}
