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
//#include <cmath>
//#include "cxTrackPadWidget.h"

//#include "boost/bind.hpp"
//#include "libQtSignalAdapters/Qt2Func.h"
//#include "libQtSignalAdapters/ConnectionFactories.h"

namespace cx
{



Vector3DWidget::Vector3DWidget(QWidget* parent, ssc::Vector3DDataAdapterPtr data) :
    QWidget(parent), mData(data)
{
//	connect(mData.get(), SIGNAL(changed()), this, SLOT(dataChangedSlot()));

  QVBoxLayout* aLayout = new QVBoxLayout(this);

  this->addControlsForIndex("x", "X", 0, aLayout);
  this->addControlsForIndex("y", "Y", 1, aLayout);
  this->addControlsForIndex("z", "Z", 2, aLayout);
}

/** Create a single adapter for a component of the Vector3D, along with widgets.
 *
 */
void Vector3DWidget::addControlsForIndex(QString name, QString help, int index, QVBoxLayout* layout)
{
  QHBoxLayout* hLayout = new QHBoxLayout;

  Vector3DComponentDataAdapterPtr component(new Vector3DComponentDataAdapter(mData, index, name, help));

//  ssc::DoubleDataAdapterXmlPtr adapter = ssc::DoubleDataAdapterXml::initialize(uid, name, "", 0, mData->getValueRange(),mData->getValueDecimals());
//  connect(component.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
//  adapter->setInternal2Display(mData->getInternal2Display());
  hLayout->addWidget(new ssc::SpinBoxGroupWidget(this, component));

  layout->addLayout(hLayout);
  mDoubleAdapter[index] = component;
}

///** Called when a single adapter is changed. Update mData.
// *
// */
//void Vector3DWidget::changedSlot()
//{
////  ssc::Vector3D xyz(
////  		mDoubleAdapter[0]->getValue(),
////  		mDoubleAdapter[1]->getValue(),
////  		mDoubleAdapter[2]->getValue());
////  mData->setValue(xyz);
//}
//
///** Called when mData is changed. Update single adapters
// *
// */
//void Vector3DWidget::dataChangedSlot()
//{
////	ssc::Vector3D xyz = mData->getValue();
////	mDoubleAdapter[0]->setValue(xyz[0]);
////	mDoubleAdapter[1]->setValue(xyz[1]);
////	mDoubleAdapter[2]->setValue(xyz[2]);
//}

}
