#ifndef CXSEGMENTATIONWIDGET_H_
#define CXSEGMENTATIONWIDGET_H_

#include "cxWhatsThisWidget.h"

namespace cx
{
/**
 * \class cxSegmentationWidget.h
 *
 * \brief Widget for segmenting out parts of volumes using a threshold.
 *
 * \date 12. okt. 2010
 * \author: Janne Beate Bakeng
 */
class SegmentationWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  SegmentationWidget(QWidget* parent);
  ~SegmentationWidget();
  virtual QString defaultWhatsThis() const;
};

class CenterlineWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  CenterlineWidget(QWidget* parent);
  ~CenterlineWidget();
  virtual QString defaultWhatsThis() const;
};
}//namespace cx
#endif /* CXSEGMENTATIONWIDGET_H_ */
