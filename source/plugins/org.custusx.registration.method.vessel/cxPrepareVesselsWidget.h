/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXPREPAREVESSELSWIDGET_H
#define CXPREPAREVESSELSWIDGET_H

#include "cxRegistrationBaseWidget.h"
#include "cxColorPropertyBase.h"
#include "cxPipeline.h"
#include "cxFilter.h"

class QPushButton;
class QVBoxLayout;

namespace cx
{

/**
 * \brief Widget for extracting a segment from a image and finding a centerline
 * from that segment.
 *
 * \ingroup org_custusx_registration_method_vessel
 * \date 12. okt. 2010
 * \date 29. nov  2012
 * \author Janne Beate Bakeng
 * \author Christian Askeland
 */
class PrepareVesselsWidget : public RegistrationBaseWidget
{
  Q_OBJECT

public:
	PrepareVesselsWidget(RegServicesPtr services, QWidget* parent);
  virtual ~PrepareVesselsWidget();

protected:
  QVBoxLayout* mLayout;

private slots:
  void setColorSlot();
  void toMovingSlot();
  void toFixedSlot();

protected:
  class PipelineWidget* mPipelineWidget;
  PipelinePtr mPipeline;
  ColorPropertyBasePtr mColorProperty;
};

}//namespace cx

#endif // CXPREPAREVESSELSWIDGET_H
