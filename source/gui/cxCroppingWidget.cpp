/*
 * cxCroppingWidget.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#include "cxCroppingWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include "sscStringDataAdapter.h"
#include "sscStringWidgets.h"

namespace cx
{

CroppingWidget::CroppingWidget(QWidget* parent) : QWidget(parent)
{
  this->setObjectName("CroppingWidget");
  this->setWindowTitle("Crop");

  QVBoxLayout* layout = new QVBoxLayout(this);

  QGroupBox* activeClipGroupBox = new QGroupBox("Clip Active Image");
  layout->addWidget(activeClipGroupBox);
  QVBoxLayout* activeClipLayout = new QVBoxLayout(activeClipGroupBox);

  ssc::ComboGroupWidget* combo = new ssc::ComboGroupWidget(this, ssc::StringDataAdapterPtr(new ssc::StringDataAdapterNull()));
  activeClipLayout->addWidget(new QCheckBox("Interactive clipping"));
  activeClipLayout->addWidget(combo);
  activeClipLayout->addWidget(new QCheckBox("Invert plane"));
  activeClipLayout->addWidget(new QPushButton("Save clip plane"));
  activeClipLayout->addWidget(new QPushButton("Clear saved planes"));
}

}
