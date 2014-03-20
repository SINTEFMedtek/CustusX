// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

/*
 * cxShadingWidget.h
 *
 *  \date Aug 20, 2010
 *      \author christiana
 */

#ifndef CXSHADINGWIDGET_H_
#define CXSHADINGWIDGET_H_

#include <QWidget>
#include <QCheckBox>
#include <QDomElement>
#include "cxImage.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxActiveImageProxy.h"
#include "cxBaseWidget.h"

class QVBoxLayout;
class QComboBox;
class QStringList;

namespace cx
{

/**
 * \ingroup cx_gui
 */
class ShadingWidget : public BaseWidget
{
  Q_OBJECT

public:
  ShadingWidget(QWidget* parent);
  ~ShadingWidget();

public slots:
  void activeImageChangedSlot(); ///< listens to the contextdockwidget for when the current image is changed
  void shadingToggledSlot(bool val);
  virtual QString defaultWhatsThis() const;

protected:
  void init();///< Initialize TransferFunctionWidget. Create members
  //gui
  QVBoxLayout* mLayout;
  QCheckBox* mShadingCheckBox;

  //ImagePtr mCurrentImage;
  bool mInitialized;///< Is TransferFunctionWidget initialized
  ActiveImageProxyPtr mActiveImageProxy;
};
}

#endif /* CXSHADINGWIDGET_H_ */
