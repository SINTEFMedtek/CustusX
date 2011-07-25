/*
 * cxVector3DWidget.h
 *
 *  Created on: Jul 25, 2011
 *      Author: christiana
 */

#ifndef CXVECTOR3DWIDGET_H_
#define CXVECTOR3DWIDGET_H_

#include "sscVector3D.h"
#include <QWidget>
class QVBoxLayout;
#include "sscVector3DDataAdapter.h"
#include "sscDoubleDataAdapterXml.h"

namespace cx
{


/**
 * \brief Widget for displaying a ssc::Vector3D
 *
 */
class Vector3DWidget : public QWidget
{
  Q_OBJECT
public:
  Vector3DWidget(QWidget* parent, ssc::Vector3DDataAdapterPtr data);
  virtual ~Vector3DWidget() {}

private slots:
//  void changedSlot();
//  void dataChangedSlot();

private:
  void addControlsForIndex(QString name, QString help, int index, QVBoxLayout* layout);

  ssc::Vector3DDataAdapterPtr mData;
  boost::array<ssc::DoubleDataAdapterPtr, 3> mDoubleAdapter;
};

}

#endif /* CXVECTOR3DWIDGET_H_ */
