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

#ifndef CXFRAMEMETRICWRAPPER_H_
#define CXFRAMEMETRICWRAPPER_H_

#include "cxFrameMetric.h"
#include "cxDataMetricWrappers.h"
#include "cxTransform3DWidget.h"

namespace cx {

/*
 * cxFrameMetricWrapper.h
 *
 * \date Aug 23, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class FrameMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit FrameMetricWrapper(cx::FrameMetricPtr data);
  virtual ~FrameMetricWrapper() {}
  virtual QWidget* createWidget();
  virtual QString getValue() const;
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
private slots:
  void moveToToolPosition();
  void spaceSelected();
  void dataChangedSlot();
  void frameWidgetChangedSlot();
private:
  cx::FrameMetricPtr mData;
  StringDataAdapterXmlPtr mSpaceSelector;
  bool mInternalUpdate;
  Transform3DWidget* mFrameWidget;
};

} //namespace cx
#endif // CXFRAMEMETRICWRAPPER_H_
