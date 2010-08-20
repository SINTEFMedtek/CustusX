/*
 * cxShadingWidget.h
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#ifndef CXSHADINGWIDGET_H_
#define CXSHADINGWIDGET_H_

#include <QWidget>
#include <QCheckBox>
#include <QDomElement>
#include <sscImage.h>
#include "cxPresetTransferFunctions3D.h"

class QVBoxLayout;
class QComboBox;
class QStringList;

namespace cx
{

/**
 */
class ShadingWidget : public QWidget
{
  Q_OBJECT

public:
  ShadingWidget(QWidget* parent);
  ~ShadingWidget();

public slots:
  void activeImageChangedSlot(); ///< listens to the contextdockwidget for when the current image is changed
  void shadingToggledSlot(bool val);

protected:
  void init();///< Initialize TransferFunctionWidget. Create members
  //gui
  QVBoxLayout* mLayout;
  QCheckBox* mShadingCheckBox;

  //ssc::ImagePtr mCurrentImage;
  bool mInitialized;///< Is TransferFunctionWidget initialized
};
}

#endif /* CXSHADINGWIDGET_H_ */
