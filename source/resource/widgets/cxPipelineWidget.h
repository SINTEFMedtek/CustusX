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
#ifndef CXPIPELINEWIDGET_H
#define CXPIPELINEWIDGET_H

#include "cxResourceWidgetsExport.h"

#include "cxBaseWidget.h"
#include "cxPipeline.h"
class QButtonGroup;
class QRadioButton;
class QAction;
//#include "cxFilterWidget.h"
#include "cxCompactFilterSetupWidget.h"

namespace cx
{
class TimedAlgorithmProgressBar;


class cxResourceWidgets_EXPORT PipelineWidgetFilterLine : public BaseWidget
{
	Q_OBJECT
public:
	PipelineWidgetFilterLine(QWidget* parent, FilterPtr filter, QButtonGroup *buttonGroup);

	QRadioButton* mRadioButton;
	QLabel* mAlgoNameLabel;
	QAction* mAction;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
	FilterPtr mFilter;

signals:
	void requestRunFilter();
	void filterSelected(QString uid);

private slots:
	void radioButtonSelectedSlot(bool on);
	void requestRunFilterSlot();
protected:
	virtual void mousePressEvent(QMouseEvent* event);
};

/** GUI for sequential execution of Filters.
 *
 * \ingroup cxPluginAlgorithms
 * \date Nov 22, 2012
 * \author christiana
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResourceWidgets_EXPORT PipelineWidget : public BaseWidget
{
	Q_OBJECT
public:
	PipelineWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent, PipelinePtr pipeline);
private slots:
	void runFilterSlot();
	void filterSelectedSlot(QString uid);
private:
	void selectFilter(int index);
	PipelinePtr mPipeline;
	QButtonGroup* mButtonGroup;
	std::vector<PipelineWidgetFilterLine*> mAlgoLines;
	CompactFilterSetupWidget* mSetupWidget;
};


} // namespace cx

#endif // CXPIPELINEWIDGET_H
