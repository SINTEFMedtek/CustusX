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

#ifndef CXWIREPHANTOMWIDGET_H_
#define CXWIREPHANTOMWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "cxDataInterface.h"
#include "cxResampleWidget.h"
#include "cxCenterlineWidget.h"

class QPushButton;
class QVBoxLayout;

namespace cx
{

/**
 * \brief Probe accuracy measurements using the Wire Phantom.
 * \ingroup cxPluginRegistration
 *
 *  \date Jun 21, 2012
 *  \author christiana
 */
class WirePhantomWidget : public RegistrationBaseWidget
{
  Q_OBJECT

public:
  WirePhantomWidget(RegistrationManagerPtr regManager, QWidget* parent);
  virtual ~WirePhantomWidget();
  virtual QString defaultWhatsThis() const;

protected:
  QVBoxLayout* mLayout;

private slots:
  void setImageSlot(QString uid);
  void segmentationOutputArrived(QString uid);
  void centerlineOutputArrived(QString uid);

  void setColorSlot(QColor color);
  void measureSlot();

protected:
  class BinaryThresholdImageFilterWidget* mSegmentationWidget;
  class CenterlineWidget*   mCenterlineWidget;
  SelectImageStringDataAdapterPtr mUSImageInput;
  SelectImageStringDataAdapterPtr mSegmentationOutput;
  SelectDataStringDataAdapterPtr mCenterlineOutput;
  QPushButton* mMeasureButton;
};


} /* namespace cx */
#endif /* CXWIREPHANTOMWIDGET_H_ */
