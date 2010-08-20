/*
 * cxCroppingWidget.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#include "cxCroppingWidget.h"
#include <QVBoxLayout>

namespace cx
{

CroppingWidget::CroppingWidget(QWidget* parent) : QWidget(parent)
{
  this->setObjectName("CroppingWidget");
  this->setWindowTitle("Crop");

  QVBoxLayout* layout = new QVBoxLayout(this);
}

}
