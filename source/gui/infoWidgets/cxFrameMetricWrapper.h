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
  virtual ssc::DataPtr getData() const;
  virtual QString getArguments() const;
  virtual QString getType() const;
private slots:
  void moveToToolPosition();
  void spaceSelected();
  void dataChangedSlot();
  void frameWidgetChangedSlot();
private:
  cx::FrameMetricPtr mData;
  ssc::StringDataAdapterXmlPtr mSpaceSelector;
  bool mInternalUpdate;
  Transform3DWidget* mFrameWidget;
};

} //namespace cx
#endif // CXFRAMEMETRICWRAPPER_H_
