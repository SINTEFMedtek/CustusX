/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*
 * cxShadingWidget.h
 *
 *  \date Aug 20, 2010
 *      \author christiana
 */

#ifndef CXSHADINGWIDGET_H_
#define CXSHADINGWIDGET_H_

#include "cxGuiExport.h"

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
typedef boost::shared_ptr<class ImageRenderPropertiesWidget> ImageRenderPropertiesWidgetPtr;

class cxGui_EXPORT ImageRenderPropertiesWidget : public BaseWidget
{
	Q_OBJECT

public:
	ImageRenderPropertiesWidget(QWidget* parent);
	void imageChanged(ImagePtr image);
public slots:
private slots:
	void interpolationTypeChanged(int index);

private:
	QComboBox* mInterpolationType;
	ImagePtr mImage;
};

/**
 * \ingroup cx_gui
 */
class cxGui_EXPORT ShadingWidget : public BaseWidget
{
  Q_OBJECT

public:
  ShadingWidget(ActiveDataPtr activeData, QWidget* parent, bool connectToActiveImage = true);
  ~ShadingWidget();

public slots:
  void activeImageChangedSlot(); ///< listens to the contextdockwidget for when the current image is changed
  void imageChangedSlot(ImagePtr image);
  void shadingToggledSlot(bool val);

protected:
  void init(bool connectToActiveImage);///< Initialize TransferFunctionWidget. Create members

  QVBoxLayout* mLayout;
  QCheckBox* mShadingCheckBox;

  bool mInitialized;///< Is TransferFunctionWidget initialized
  ActiveDataPtr mActiveData;
  ActiveImageProxyPtr mActiveImageProxy;
  ImagePtr mImage;
  ImageRenderPropertiesWidgetPtr mImagePropertiesWidget;
};
}

#endif /* CXSHADINGWIDGET_H_ */
