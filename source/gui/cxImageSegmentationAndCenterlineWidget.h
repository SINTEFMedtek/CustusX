#ifndef CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_
#define CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_

#include "cxWhatsThisWidget.h"

namespace cx
{
class SegmentationWidget;
class CenterlineWidget;

/**
 * \class ImageSegmentationAndCenterlineWidget
 *
 * \brief Widget for extracting a segment from a image and finding a centerline
 * from that segment.
 *
 * \date 12. okt. 2010
 * \author: Janne Beate Bakeng
 */
class ImageSegmentationAndCenterlineWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  ImageSegmentationAndCenterlineWidget(QWidget* parent);
  ~ImageSegmentationAndCenterlineWidget();
  virtual QString defaultWhatsThis() const;

private:
  SegmentationWidget* mSegmentationWidget;
  CenterlineWidget*   mCenterlineWidget;
};

}//namespace cx
#endif /* CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_ */
