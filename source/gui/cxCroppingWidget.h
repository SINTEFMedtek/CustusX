/*
 * cxCroppingWidget.h
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#ifndef CXCROPPINGWIDGET_H_
#define CXCROPPINGWIDGET_H_

#include <QWidget>
#include "sscForwardDeclarations.h"
#include "sscStringDataAdapter.h"
#include "cxViewManager.h"
class QCheckBox;

typedef boost::shared_ptr<class InteractiveCropper> InteractiveCropperPtr;


namespace cx
{

/**
 *
 */
class CroppingWidget : public QWidget
{
  Q_OBJECT
public:
  CroppingWidget(QWidget* parent);
private:
  InteractiveCropperPtr mInteractiveCropper;

  QCheckBox* mUseCropperCheckBox;
  QCheckBox* mShowBoxCheckBox;
private slots:
  void cropperChangedSlot();
};

}//namespace cx


#endif /* CXCROPPINGWIDGET_H_ */
