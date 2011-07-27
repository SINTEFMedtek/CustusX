/*
 * cxVector3DWidget.cpp
 *
 *  Created on: Jul 25, 2011
 *      Author: christiana
 */

#include <cxVector3DWidget.h>

#include <QTextEdit>
#include <QLayout>
#include <QLabel>
#include <QFontMetrics>

#include "sscTypeConversions.h"

#include "cxVector3DComponentDataAdapter.h"
#include "sscDoubleWidgets.h"

namespace cx
{

Vector3DWidget::Vector3DWidget(QWidget* parent, ssc::Vector3DDataAdapterPtr data) :
    QWidget(parent), mData(data)
{
//  QVBoxLayout* aLayout = new QVBoxLayout(this);
//
//  this->addSliderControlsForIndex("x", "X", 0, aLayout);
//  this->addSliderControlsForIndex("y", "Y", 1, aLayout);
//  this->addSliderControlsForIndex("z", "Z", 2, aLayout);
}

Vector3DWidget* Vector3DWidget::createSmallHorizontal(QWidget* parent, ssc::Vector3DDataAdapterPtr data)
{
  Vector3DWidget* retval = new Vector3DWidget(parent, data);

  QHBoxLayout* aLayout = new QHBoxLayout(retval);
  aLayout->setMargin(0);
  aLayout->addWidget(new QLabel(data->getValueName(), retval), 0);
  retval->addSmallControlsForIndex("x", "X", 0, aLayout);
  retval->addSmallControlsForIndex("y", "Y", 1, aLayout);
  retval->addSmallControlsForIndex("z", "Z", 2, aLayout);

  return retval;
}

Vector3DWidget* Vector3DWidget::createVerticalWithSliders(QWidget* parent, ssc::Vector3DDataAdapterPtr data)
{
  Vector3DWidget* retval = new Vector3DWidget(parent, data);

  QVBoxLayout* aLayout = new QVBoxLayout(retval);
  aLayout->setMargin(0);
  retval->addSliderControlsForIndex("x", "X", 0, aLayout);
  retval->addSliderControlsForIndex("y", "Y", 1, aLayout);
  retval->addSliderControlsForIndex("z", "Z", 2, aLayout);

  return retval;
}


/** Create a single adapter for a component of the Vector3D, along with widgets.
 *
 */
void Vector3DWidget::addSliderControlsForIndex(QString name, QString help, int index, QBoxLayout* layout)
{
//  QHBoxLayout* hLayout = new QHBoxLayout;

  Vector3DComponentDataAdapterPtr component(new Vector3DComponentDataAdapter(mData, index, name, help));
  layout->addWidget(new ssc::SpinBoxInfiniteSliderGroupWidget(this, component));

//  layout->addLayout(hLayout);
  mDoubleAdapter[index] = component;
}

/** Create a single adapter for a component of the Vector3D, along with widgets.
 *
 */
void Vector3DWidget::addSmallControlsForIndex(QString name, QString help, int index, QBoxLayout* layout)
{
  Vector3DComponentDataAdapterPtr component(new Vector3DComponentDataAdapter(mData, index, name, help));

  ssc::ScalarInteractionWidget* widget = new ssc::ScalarInteractionWidget(this, component);
  widget->enableSpinBox();
  widget->build();
  layout->addWidget(widget, 1);

//  layout->addWidget(new ssc::SpinBoxGroupWidget(this, component));
  mDoubleAdapter[index] = component;
}

}
