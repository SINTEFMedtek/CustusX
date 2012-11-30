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
#include "cxDataInterface.h"
#include "cxResampleWidget.h"
#include "cxCenterlineWidget.h"

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
  void setImageSlot(QString uid);
  void resampleOutputArrived(QString uid);
  void segmentationOutputArrived(QString uid);
  void centerlineOutputArrived(QString uid);

  void setColorSlot(QColor color);
  void toMovingSlot();
  void toFixedSlot();

protected:
  class ResampleWidget*     mResampleWidget;
  class BinaryThresholdImageFilterWidget* mSegmentationWidget;
  class CenterlineWidget*   mCenterlineWidget;
  SelectImageStringDataAdapterPtr mResampleOutput;
  SelectImageStringDataAdapterPtr mSegmentationOutput;
  SelectDataStringDataAdapterPtr mCenterlineOutput;
};


/**
 * @}
 */
}//namespace cx

#endif // CXPREPAREVESSELSWIDGET_H
