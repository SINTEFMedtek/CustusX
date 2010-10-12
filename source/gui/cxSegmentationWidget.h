#ifndef CXSEGMENTATIONWIDGET_H_
#define CXSEGMENTATIONWIDGET_H_

#include <vector>
#include <QtGui>
#include "sscDoubleWidgets.h"

namespace cx
{

/**
 * \class SegmentationWidget
 *
 *
 * \date 2010.10.12
 * \author: Christian Askeland, SINTEF
 */
class SegmentationWidget : public QWidget
{
  Q_OBJECT

public:
  SegmentationWidget(QWidget* parent);
  virtual ~SegmentationWidget();

signals:

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff
private slots:
  void segmentSlot();
  void contourSlot();

private:
  SegmentationWidget();
};

}//end namespace cx

#endif /* CXSEGMENTATIONWIDGET_H_ */
