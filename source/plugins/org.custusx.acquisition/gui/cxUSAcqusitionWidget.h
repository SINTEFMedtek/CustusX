/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXUSACQUSITIONWIDGET_H_
#define CXUSACQUSITIONWIDGET_H_

#include "cxBaseWidget.h"
#include "cxAcquisitionService.h"
#include "org_custusx_acquisition_Export.h"


namespace cx
{
typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;
typedef boost::shared_ptr<class ThreadedTimedReconstructer> ThreadedTimedReconstructerPtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class StringPropertyActiveProbeConfiguration> StringPropertyActiveProbeConfigurationPtr;
class TimedAlgorithmProgressBar;
class DisplayTimerWidget;
class RecordSessionWidget;

/**
* \file
* \addtogroup org_custusx_acquisition
* @{
*/

/**
 * USAcqusitionWidget
 *
 * \brief
 *
 * \date Dec 9, 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_acquisition_EXPORT USAcqusitionWidget : public BaseWidget
{
	Q_OBJECT
public:
	USAcqusitionWidget(AcquisitionServicePtr acquisitionService, VisServicesPtr services, UsReconstructionServicePtr usReconstructionService, QWidget* parent);
	virtual ~USAcqusitionWidget();

private slots:
	void reconstructStartedSlot();
	void reconstructFinishedSlot();
	void toggleDetailsSlot();
	void acquisitionDataReadySlot();
	void reconstructAboutToStartSlot();
	void acquisitionStateChangedSlot();

	void recordStarted();
	void recordStopped();
	void recordCancelled();

private:
	AcquisitionServicePtr mAcquisitionService;
	QVBoxLayout* mLayout;
	RecordSessionWidget* mRecordSessionWidget;

	UsReconstructionServicePtr mUsReconstructionService;
	VisServicesPtr mServices;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
	DisplayTimerWidget* mDisplayTimerWidget;

	QWidget* mOptionsWidget;
	QWidget* createOptionsWidget();
	QWidget* wrapVerticalStretch(QWidget* input);
	QWidget* wrapGroupBox(QWidget* input, QString name, QString tip);
};


/**
* @}
*/
}//namespace cx

#endif /* CXUSACQUSITIONWIDGET_H_ */
