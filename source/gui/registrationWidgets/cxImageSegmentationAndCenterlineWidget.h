#ifndef CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_
#define CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_

#include "cxBaseWidget.h"
#include "cxDataInterface.h"
#include "cxSurfaceWidget.h"
#include "cxResampleWidget.h"
#include "cxCenterlineWidget.h"

class QPushButton;
class QVBoxLayout;

namespace cx
{
/**
 * \class ImageSegmentationAndCenterlineWidget
 *
 * \brief Widget for extracting a segment from a image and finding a centerline
 * from that segment.
 *
 * \date 12. okt. 2010
 * \author: Janne Beate Bakeng
 */
class ImageSegmentationAndCenterlineWidget : public BaseWidget
{
  Q_OBJECT

public:
  ImageSegmentationAndCenterlineWidget(QWidget* parent);
  virtual ~ImageSegmentationAndCenterlineWidget();
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


}//namespace cx
#endif /* CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_ */
