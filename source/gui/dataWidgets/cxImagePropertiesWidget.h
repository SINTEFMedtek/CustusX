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

#ifndef CXIMAGEPROPERTIESWIDGET_H
#define CXIMAGEPROPERTIESWIDGET_H

#include <QtGui>
#include "cxBaseWidget.h"
#include "cxActiveImageProxy.h"

class QComboBox;

namespace cx
{

class ImagePropertiesWidget : public BaseWidget
{
	Q_OBJECT

public:
	ImagePropertiesWidget(QWidget* parent);

public slots:
	virtual QString defaultWhatsThis() const;

private slots:
	void interpolationTypeChanged(int index);
	void activeImageChangedSlot();

private:
	QComboBox* mInterpolationType;
	ActiveImageProxyPtr mActiveImageProxy;
};

}//end namespace cx

#endif // CXIMAGEPROPERTIESWIDGET_H
