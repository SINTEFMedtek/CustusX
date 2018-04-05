/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef SONIXHELPER_H_
#define SONIXHELPER_H_

#include "cxGrabberExport.h"


#include <QObject>
#include "cxFrame.h"

typedef cx::Frame Frame;

/**
* \file
* \addtogroup cx_resource_videoserver
* @{
*/

/**
 * \class SonixHelper
 * \brief Support Qt support for vtkSonixVideoSource
 *
 * \date 21. jun. 2011
 * \\author Ole Vegard Solberg, SINTEF
 */
class cxGrabber_EXPORT SonixHelper : public QObject
{
  Q_OBJECT

public:
  signals:
  void frame(Frame& newFrame); ///< Emitted when a new frame is available

public:
  void emitFrame(Frame& newFrame) {emit frame(newFrame);};
};

/**
* @}
*/

#endif //SONIXHELPER_H_
