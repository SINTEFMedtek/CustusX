/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
