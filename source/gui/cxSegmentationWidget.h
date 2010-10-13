#ifndef CXSEGMENTATIONWIDGET_H_
#define CXSEGMENTATIONWIDGET_H_

#include <vector>
#include <QtGui>
#include "cxWhatsThisWidget.h"
#include <QString>

namespace cx
{

/**
 * \class SegmentationWidget
 *
 * \brief Widget for segmenting out parts of volumes using a threshold.
 *
 * \date 12. okt. 2010
 * \author: Janne Beate Bakeng
 * \author: Christian Askeland, SINTEF
 */
class SegmentationWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  SegmentationWidget(QWidget* parent);
  virtual ~SegmentationWidget();

  virtual QString defaultWhatsThis() const;

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff
private slots:
  void segmentSlot();
  void contourSlot();

private:
  SegmentationWidget();
  void fillImageCombobox();
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
