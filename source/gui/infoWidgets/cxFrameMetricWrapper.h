/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFRAMEMETRICWRAPPER_H_
#define CXFRAMEMETRICWRAPPER_H_

#include "cxGuiExport.h"

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
class cxGui_EXPORT FrameMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
	explicit FrameMetricWrapper(VisServicesPtr services, FrameMetricPtr data);
  virtual ~FrameMetricWrapper();
  virtual QWidget* createWidget();
  virtual QString getValue() const;
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void moveToToolPosition();
  void spaceSelected();
  void frameWidgetChangedSlot();
private:
  cx::FrameMetricPtr mData;
  SpacePropertyPtr mSpaceSelector;
  bool mInternalUpdate;
  Transform3DWidget* mFrameWidget;
};

} //namespace cx
#endif // CXFRAMEMETRICWRAPPER_H_
