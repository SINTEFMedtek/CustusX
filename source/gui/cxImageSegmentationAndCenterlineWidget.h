#ifndef CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_
#define CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "cxDataInterface.h"

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
  void resampleOutputArrived(QString uid);
  void segmentationOutputArrived(QString uid);
  void surfaceOutputArrived(QString uid);
  void centerlineOutputArrived(QString uid);

protected:
  class ResampleWidget*     mResampleWidget;
  class SegmentationWidget* mSegmentationWidget;
  class SurfaceWidget*      mSurfaceWidget;
  class CenterlineWidget*   mCenterlineWidget;

  SelectImageStringDataAdapterPtr mResampleOutput;
  SelectImageStringDataAdapterPtr mSegmentationOutput;
  SelectMeshStringDataAdapterPtr mSurfaceOutput;
  SelectImageStringDataAdapterPtr mCenterlineOutput;
};

class FixedImage2ImageWidget : public ImageSegmentationAndCenterlineWidget
{
public:
  FixedImage2ImageWidget(QWidget* parent);
  virtual ~FixedImage2ImageWidget();
  virtual QString defaultWhatsThis() const;

private:
};

class MovingImage2ImageWidget : public ImageSegmentationAndCenterlineWidget
{
public:
  MovingImage2ImageWidget(QWidget* parent);
  virtual ~MovingImage2ImageWidget();
  virtual QString defaultWhatsThis() const;

private:
};

}//namespace cx
#endif /* CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_ */
