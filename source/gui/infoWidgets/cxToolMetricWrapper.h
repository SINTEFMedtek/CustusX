/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTOOLMETRICWRAPPER_H
#define CXTOOLMETRICWRAPPER_H

#include "cxGuiExport.h"

#include "cxToolMetric.h"
#include "cxDataMetricWrappers.h"
#include "cxTransform3DWidget.h"

namespace cx {

/**
 *
 * \date Aug 30, 2013
 * \author Ole Vegard Solberg, SINTEF
 * \author Christian Askeland, SINTEF
 */
class cxGui_EXPORT ToolMetricWrapper : public MetricBase
{
  Q_OBJECT
public:
  explicit ToolMetricWrapper(VisServicesPtr services, cx::ToolMetricPtr data);
  virtual ~ToolMetricWrapper();
  virtual QWidget* createWidget();
  virtual QString getValue() const;
  virtual DataMetricPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
	virtual void update();

private slots:
  void resampleMetric();
  void spaceSelected();
  void toolNameSet();
  void toolOffsetSet();
//  void dataChangedSlot();
  void frameWidgetChangedSlot();
private:
  cx::ToolMetricPtr mData;
  SpacePropertyPtr mSpaceSelector;
  StringPropertyPtr mToolNameSelector;
  DoublePropertyPtr mToolOffsetSelector;
  bool mInternalUpdate;
  Transform3DWidget* mFrameWidget;
  void initializeProperties();
};

} //namespace cx

#endif // CXTOOLMETRICWRAPPER_H
