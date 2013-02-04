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
#ifndef CXSAMPLERWIDGET_H
#define CXSAMPLERWIDGET_H

#include "cxBaseWidget.h"

#include "sscStringDataAdapterXml.h"
#include "sscVector3DDataAdapterXml.h"
#include "cxDominantToolProxy.h"
#include "sscPointMetric.h"

namespace cx
{

/** 
 * Utility for sampling the current tool point.
 * Displays the current tool tip position in a selected coordinate system.
 *
 * \ingroup cxGUI
 * \date feb 1, 2013, 2013
 * \author christiana
 */
class SamplerWidget : public BaseWidget
{
	Q_OBJECT
public:
	SamplerWidget(QWidget* parent);
	virtual ~SamplerWidget();

	virtual QString defaultWhatsThis() const;
private slots:
	void setModified2();
	void spacesChangedSlot();
	void toggleAdvancedSlot();
protected:
	void prePaintEvent();
private:
	void showAdvanced();
	QHBoxLayout* mLayout;

	ssc::StringDataAdapterXmlPtr mSpaceSelector;
	ssc::CoordinateSystemListenerPtr mListener;
//    ssc::Vector3DDataAdapterXmlPtr mCoordinate;
	DominantToolProxyPtr mActiveTool;
	QLineEdit* mCoordLineEdit;
	QAction* mAdvancedAction;
	QHBoxLayout* mAdvancedLayout;
	QWidget* mAdvancedWidget;
};

} // namespace cx

#endif // CXSAMPLERWIDGET_H
