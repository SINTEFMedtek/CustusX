/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXACTIVETOOLWIDGET_H_
#define CXACTIVETOOLWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class StringPropertyBase> StringPropertyBasePtr;

/**
 * \class ActiveToolWidget
 *
 * \brief Widget that contains a select active tool combo box
 * \ingroup cx_gui
 *
 * \date May 4, 2011
 * \author Christian Askeland, SINTEF
 */
class cxResourceWidgets_EXPORT ActiveToolWidget : public BaseWidget
{
  Q_OBJECT
public:
  ActiveToolWidget(TrackingServicePtr trackingService, QWidget* parent);
  virtual ~ActiveToolWidget() {}
  StringPropertyBasePtr getSelector() { return mSelector; }

private:
  StringPropertyBasePtr mSelector;
};

}

#endif /* CXACTIVETOOLWIDGET_H_ */
