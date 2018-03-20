/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPROBECONFIGWIDGET_H_
#define CXPROBECONFIGWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"
#include "cxBoundingBoxWidget.h"
#include "cxVector3DProperty.h"
#include "cxDoubleProperty.h"

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class StringPropertyActiveProbeConfiguration> StringPropertyActiveProbeConfigurationPtr;

/**
 *
 * \brief Widget that displays/edits a probe configuration
 * \ingroup cx_gui
 *
 * \date Mar 16, 2012
 * \author Christian Askeland, SINTEF
 */
class cxResourceWidgets_EXPORT ProbeConfigWidget : public BaseWidget
{
	Q_OBJECT
public:
	ProbeConfigWidget(VisServicesPtr services, QWidget* parent=NULL);
	virtual ~ProbeConfigWidget();
	QWidget* getActiveProbeConfigWidget() { return mActiveProbeConfigWidget; }

private slots:
	void activeProbeConfigurationChangedSlot();
	void savePresetSlot();
	void deletePresetSlot();
	void guiImageSettingsChanged();
	void guiProbeSectorChanged();
	void guiOriginSettingsChanged();
	void syncBoxToSectorChanged();

private:
	VisServicesPtr mServices;
	QWidget* mActiveProbeConfigWidget;
	BoundingBoxWidget* mBBWidget;
	Vector3DPropertyBasePtr mOrigin;
	SliderRangeGroupWidget* mDepthWidget;
	DoublePropertyPtr mWidth;
	QCheckBox* mSyncBoxToSector;

	StringPropertyActiveProbeConfigurationPtr mActiveProbeConfig;
	QString mLastKnownProbeConfigName; ///< used for suggesting save names for new config
	bool mUpdating;
};

}

#endif /* CXPROBECONFIGWIDGET_H_ */
