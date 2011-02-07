/*
 * sscCheckBoxWidget.h
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#ifndef SSCCHECKBOXWIDGET_H_
#define SSCCHECKBOXWIDGET_H_

#include "sscBoolDataAdapter.h"

#include <QWidget>
#include <QCheckBox>
#include <QGridLayout>

namespace ssc
{

class CheckBoxWidget : public QWidget
{
  Q_OBJECT
public:
  CheckBoxWidget(QWidget* parent, BoolDataAdapterPtr data, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void valueChanged(bool val);

private:
  QCheckBox* mCheckBox;
  BoolDataAdapterPtr mData;
};

}

#endif /* SSCCHECKBOXWIDGET_H_ */
