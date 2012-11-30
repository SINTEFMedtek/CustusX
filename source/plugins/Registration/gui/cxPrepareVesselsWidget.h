// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXPREPAREVESSELSWIDGET_H
#define CXPREPAREVESSELSWIDGET_H

#include "cxRegistrationBaseWidget.h"
#include "sscColorDataAdapter.h"
#include "cxPipeline.h"
#include "cxFilter.h"

class QPushButton;
class QVBoxLayout;

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginRegistration
 * @{
 */

/**
 * \brief Widget for extracting a segment from a image and finding a centerline
 * from that segment.
 *
 * \date 12. okt. 2010
 * \date 29. nov  2012
 * \author Janne Beate Bakeng
 * \author Christian Askeland
 */
class PrepareVesselsWidget : public RegistrationBaseWidget
{
  Q_OBJECT

public:
  PrepareVesselsWidget(RegistrationManagerPtr regManager, QWidget* parent);
  virtual ~PrepareVesselsWidget();
  virtual QString defaultWhatsThis() const;

protected:
  QVBoxLayout* mLayout;

private slots:
  void setColorSlot();
  void toMovingSlot();
  void toFixedSlot();

protected:
  class PipelineWidget* mPipelineWidget;
  PipelinePtr mPipeline;
  ssc::ColorDataAdapterPtr mColorDataAdapter;
};


/**
 * @}
 */
}//namespace cx

#endif // CXPREPAREVESSELSWIDGET_H
