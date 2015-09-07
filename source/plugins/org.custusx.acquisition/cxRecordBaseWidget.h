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

#ifndef CXRECORDBASEWIDGET_H_
#define CXRECORDBASEWIDGET_H_

#include "org_custusx_acquisition_Export.h"

#include <QWidget>
#include "cxBaseWidget.h"
#include "cxTool.h"
#include "cxVideoRecorder.h"
//#include "cxRecordSession.h"
#include "cxAcquisitionService.h"

class QLabel;
class QVBoxLayout;
class QDoubleSpinBox;
class QPushButton;

namespace cx
{
//*
//* \file
//* \addtogroup org_custusx_acquisition
//* @{
//	*/


//	typedef boost::shared_ptr<class RecordSession> RecordSessionPtr;
//	class RecordSessionWidget;
//	/**
// * RecordBaseWidget
// *
// * \brief
// *
// * \date Dec 9, 2010
// * \author Janne Beate Bakeng, SINTEF
// */
//	class org_custusx_acquisition_EXPORT  RecordBaseWidget : public BaseWidget
//	{
//		Q_OBJECT

//	public:
//		RecordBaseWidget(AcquisitionServicePtr acquisitionService, QWidget* parent, QString description = "Record Session");
//		virtual ~RecordBaseWidget();

//	protected:
//		AcquisitionServicePtr mAcquisitionService;
//		QVBoxLayout* mLayout;
//		RecordSessionWidget* mRecordSessionWidget;

//	};

//	/**
// * TrackedRecordWidget
// *
// * \brief
// *
// * \date Dec 17, 2010
// * \author Janne Beate Bakeng, SINTEF
// */
//	class org_custusx_acquisition_EXPORT  TrackedRecordWidget : public RecordBaseWidget
//	{
//		Q_OBJECT
//	public:
//		TrackedRecordWidget(AcquisitionServicePtr acquisitionService, QWidget* parent, QString description);
//		virtual ~TrackedRecordWidget();

//	signals:
//		void toolChanged();

//	protected slots:
//		virtual void checkIfReadySlot() = 0;
//		virtual void postProcessingSlot(QString sessionId) = 0;
//		virtual void startedSlot(QString sessionId) = 0;
//		virtual void stoppedSlot(bool) = 0;

//	protected:
//		virtual TimedTransformMap getRecording(RecordSessionPtr session) = 0; ///< gets the tracking data from all relevant tool for the given session
//		void setTool(ToolPtr tool);
//		ToolPtr getTool();

//	private:
//		ToolPtr mTool;
//	};

//	/**
//* @}

}//namespace cx
#endif /* CXRECORDBASEWIDGET_H_ */
