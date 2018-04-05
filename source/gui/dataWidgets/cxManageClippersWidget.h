/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMANAGECLIPPERSWIDGET_H
#define CXMANAGECLIPPERSWIDGET_H

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxStringProperty.h"
#include "cxTabbedWidget.h"

namespace cx
{
class ClipperWidget;

/**
 * \brief Widget for displaying and changing clipper properties.
 * \ingroup cx_gui
 *
 *  \date 10 Nov, 2015
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT ClippingPropertiesWidget : public TabbedWidget
{
  Q_OBJECT
public:
	ClippingPropertiesWidget(VisServicesPtr services, QWidget* parent);
  virtual ~ClippingPropertiesWidget() {}
};

//--------------------------------------

/**\brief Widget for managing clippers.
 *
 *  \date Oct, 2015
 *  \author Ole Vegard Solberg, SINTEF
 */
class cxGui_EXPORT ManageClippersWidget: public BaseWidget
{
	Q_OBJECT
public:
	ManageClippersWidget(VisServicesPtr services, QWidget* parent);

protected:
	VisServicesPtr mServices;
	QVBoxLayout* mLayout;
	StringPropertyPtr mClipperSelector;

	InteractiveClipperPtr mCurrentClipper;
	ClipperWidget *mClipperWidget;

	void setupUI();
//	void setupClipperUI();
	void initClipperSelector();
	QString getNameBaseOfCurrentClipper();
	ClippersPtr getClippers();
protected slots:
	void newClipperButtonClicked();
	void clipperChanged();
private slots:
	void clippersChanged();
};
}//cx
#endif // CXMANAGECLIPPERSWIDGET_H
