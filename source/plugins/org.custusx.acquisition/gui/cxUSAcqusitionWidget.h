/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXUSACQUSITIONWIDGET_H_
#define CXUSACQUSITIONWIDGET_H_

#include "org_custusx_acquisition_Export.h"

#include "cxRecordBaseWidget.h"

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
