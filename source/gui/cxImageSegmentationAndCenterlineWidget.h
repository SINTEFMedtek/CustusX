#ifndef CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_
#define CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_

#include "cxWhatsThisWidget.h"

class QGroupBox;
class QPushButton;

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
  ImageSegmentationAndCenterlineWidget();

  SegmentationWidget* mSegmentationWidget;
  CenterlineWidget*   mCenterlineWidget;
};

/**
 * \class RegisterWidget
 *
 * \brief Widget for performing the registration between two vessel segments.
 *
 * \date 13. okt. 2010
 * \author: Janne Beate Bakeng
 */
class RegisterWidget : public WhatsThisWidget
{
public:
  RegisterWidget(QWidget* parent);
  ~RegisterWidget();
  virtual QString defaultWhatsThis() const;

private:
  RegisterWidget();

  QPushButton* mRegisterButton;
};

}//namespace cx
#endif /* CXIMAGESEGMENTATIONANDCENTERLINEWIDGET_H_ */
