#ifndef SONIXHELPER_H_
#define SONIXHELPER_H_


#include <QObject>
#include "cxGrabber.h"

/**
 * \class SonixHelper
 * \brief Support Qt support for vtkSonixVideoSource
 *
 * \date 21. jun. 2011
 * \author: Ole Vegard Solberg, SINTEF
 */
class SonixHelper : public QObject
{
  Q_OBJECT

public:
  signals:
  void frame(cx::Frame newFrame); ///< Emitted when a new frame is available

public:
  void emitFrame(cx::Frame newFrame) {emit frame;};
};

#endif //SONIXHELPER_H_
