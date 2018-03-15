/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLOGIOSTREAMREDIRECTER_H
#define CXLOGIOSTREAMREDIRECTER_H

#include "boost/shared_ptr.hpp"
#include "cxDefinitions.h"

/**
 * \file
 * \addtogroup cx_resource_core_logger
 * @{
 */

namespace cx
{

/**
 * Redirect text from std::cout and std::cerr to cx::Reporter
 */
class SingleStreamerImpl
{
private:
  boost::shared_ptr<class MyStreamBuf> StreamBuf;
  std::streambuf *OrigBuf;
  std::ostream& mStream;
public:
  SingleStreamerImpl(std::ostream& str, MESSAGE_LEVEL level);
  ~SingleStreamerImpl();
  void sendUnredirected(const QString& sequence);
};

} //namespace cx

/**
 * @}
 */

#endif // CXLOGIOSTREAMREDIRECTER_H
