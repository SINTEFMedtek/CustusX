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

#ifndef CXBRONCHOSCOPYREGISTRATIONWIDGET_H
#define CXBRONCHOSCOPYREGISTRATIONWIDGET_H

#include <QPushButton>
#include "cxRegistrationBaseWidget.h"
//#include "sscStringDataAdapter.h"
#include "cxTrackedCenterlineWidget.h"

namespace cx
{

typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;
typedef boost::shared_ptr<class SelectMeshStringDataAdapter> SelectMeshStringDataAdapterPtr;
typedef boost::shared_ptr<class ToolRep3D> ToolRep3DPtr;
typedef boost::shared_ptr<class RecordSessionWidget> RecordSessionWidgetPtr;

/**
 * BronchoscopyRegistrationWidget
 *
 * \brief Register tracked bronchostopy tool path to lung centerline data (from CT)
 *
 * \date Oct 10, 2013
 * \author Ole Vegard Solberg
 * \author Erlend Hofstad
 */
class BronchoscopyRegistrationWidget: public RegistrationBaseWidget
{
	Q_OBJECT
public:
	BronchoscopyRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent);
	virtual ~BronchoscopyRegistrationWidget()
	{
	}
	virtual QString defaultWhatsThis() const;

private slots:
	void registerSlot();
	void acquisitionStarted();
	void acquisitionStopped();

private:
	QVBoxLayout* mVerticalLayout;
	QLabel* mLabel;


	AcquisitionPtr mAquisition;
	RecordSessionWidgetPtr mRecordSessionWidget;
	SelectMeshStringDataAdapterPtr mSelectMeshWidget;
	QPushButton* mRegisterButton;
    ToolPtr mTool;
//    TrackedCenterlineWidget* mTrackedCenterLine;

    ToolRep3DPtr getToolRepIn3DView(ToolPtr tool);

};

} //namespace cx

#endif // CXBRONCHOSCOPYREGISTRATIONWIDGET_H
