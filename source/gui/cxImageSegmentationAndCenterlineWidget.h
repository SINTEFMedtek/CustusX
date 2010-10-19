#ifndef CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_
#define CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "cxDataInterface.h"

class QGroupBox;
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
class ImageSegmentationAndCenterlineWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  ImageSegmentationAndCenterlineWidget(QWidget* parent);
  virtual ~ImageSegmentationAndCenterlineWidget();
  virtual QString defaultWhatsThis() const = 0;

protected:
  QVBoxLayout* mLayout;
  ssc::StringDataAdapterPtr mOutput;

private slots:
  void setImageSlot(QString uid);

private:
  ImageSegmentationAndCenterlineWidget();

  class SegmentationWidget* mSegmentationWidget;
  class SurfaceWidget*      mSurfaceWidget;
  class CenterlineWidget*   mCenterlineWidget;
};

class FixedImage2ImageWidget : public ImageSegmentationAndCenterlineWidget
{
public:
  FixedImage2ImageWidget(QWidget* parent);
  virtual ~FixedImage2ImageWidget();
  virtual QString defaultWhatsThis() const;

private:
  FixedImage2ImageWidget();
};

class MovingImage2ImageWidget : public ImageSegmentationAndCenterlineWidget
{
public:
  MovingImage2ImageWidget(QWidget* parent);
  virtual ~MovingImage2ImageWidget();
  virtual QString defaultWhatsThis() const;

private:
  MovingImage2ImageWidget();
};

}//namespace cx
#endif /* CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_ */
