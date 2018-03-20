/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLOGQDEBUGREDIRECTER_H
#define CXLOGQDEBUGREDIRECTER_H

#include <QtGlobal>

/**
 * \file
 * \addtogroup cx_resource_core_logger
 * @{
 */

namespace cx
{

void convertQtMessagesToCxMessages(QtMsgType type, const QMessageLogContext &, const QString &msg);

} //namespace cx

/**
 * @}
 */

#endif // CXLOGQDEBUGREDIRECTER_H
