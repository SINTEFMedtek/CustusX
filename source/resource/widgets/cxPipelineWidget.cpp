/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPipelineWidget.h"
#include "cxHelperWidgets.h"

#include "cxTimedAlgorithmProgressBar.h"
#include <QtWidgets>
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxDataSelectWidget.h"
#include "cxSettings.h"

namespace cx
{

PipelineWidgetFilterLine::PipelineWidgetFilterLine(QWidget* parent, FilterPtr filter, QButtonGroup* buttonGroup) :
    BaseWidget(parent, "PipelineWidgetFilterLine", "PipelineWidgetFilterLine"),
    mFilter(filter)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	connect(this, SIGNAL(requestRunFilter()), this, SLOT(requestRunFilterSlot()));

	mRadioButton = new QRadioButton(this);
	buttonGroup->addButton(mRadioButton);
	connect(mRadioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonSelectedSlot(bool)));
	layout->addWidget(mRadioButton);
	layout->setMargin(0);
	layout->setSpacing(2);

	mAlgoNameLabel = new QLabel(QString("<b>%1</b>").arg(mFilter->getName()), this);
	mAlgoNameLabel->setToolTip(mFilter->getHelp());
	layout->addWidget(mAlgoNameLabel);

	mTimedAlgorithmProgressBar = new TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->setShowTextLabel(false);
	layout->addWidget(mTimedAlgorithmProgressBar, 1);

	mAction = this->createAction(this,
	                             QIcon(":/icons/open_icon_library/arrow-right-3.png"),
	                             "Run Filter", "",
	                             SIGNAL(requestRunFilter()),
	                             NULL);
	mAction->setData(mFilter->getUid());

	CXSmallToolButton* button = new CXSmallToolButton();
	button->setObjectName("RunFilterButton");
	button->setDefaultAction(mAction);
	layout->addWidget(button);

}

void PipelineWidgetFilterLine::requestRunFilterSlot()
{
	mRadioButton->setChecked(true);
}

void PipelineWidgetFilterLine::radioButtonSelectedSlot(bool on)
{
	if (!on)
		return;

	emit filterSelected(mFilter->getUid());
}

void PipelineWidgetFilterLine::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);

	mRadioButton->setChecked(true);
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

PipelineWidget::PipelineWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent, PipelinePtr pipeline) :
    BaseWidget(parent, "PipelineWidget", "Pipeline"),
    mPipeline(pipeline)
{
	this->setToolTip("Run a series of filters");
	FilterGroupPtr filters = mPipeline->getFilters();
	std::vector<SelectDataStringPropertyBasePtr> nodes = mPipeline->getNodes();
	if (filters->size()+1 != nodes.size())
		reportError("Filter/Node mismatch");

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	mButtonGroup = new QButtonGroup(this);

	struct Inner
	{
		static QHBoxLayout* addHMargin(QWidget* base)
		{
			QHBoxLayout* layout = new QHBoxLayout;
			layout->addWidget(base);
			layout->setContentsMargins(4,0,4,0);
			return layout;
		}
	};

	for (unsigned i=0; i<filters->size(); ++i)
	{
		topLayout->addLayout(Inner::addHMargin(new DataSelectWidget(viewService, patientModelService, this, nodes[i])));

		PipelineWidgetFilterLine* algoLine = new PipelineWidgetFilterLine(this, filters->get(i), mButtonGroup);
		connect(algoLine, SIGNAL(requestRunFilter()), this, SLOT(runFilterSlot()));
		connect(algoLine, SIGNAL(filterSelected(QString)), this, SLOT(filterSelectedSlot(QString)));
		algoLine->mTimedAlgorithmProgressBar->attach(mPipeline->getTimedAlgorithm(filters->get(i)->getUid()));

		mAlgoLines.push_back(algoLine);
		QFrame* frame = this->wrapInFrame(algoLine);
		frame->layout()->setContentsMargins(4,4,4,4); // nice on linux
		frame->setObjectName("FilterBackground");
		topLayout->addWidget(frame);
	}
	topLayout->addLayout(Inner::addHMargin(new DataSelectWidget(viewService, patientModelService, this, nodes.back())));

	topLayout->addSpacing(12);

	mSetupWidget = new CompactFilterSetupWidget(viewService, patientModelService, this, filters->getOptions(), true);
	topLayout->addWidget(mSetupWidget);

	topLayout->addStretch();

	this->filterSelectedSlot(filters->get(0)->getUid());
}


void PipelineWidget::filterSelectedSlot(QString uid)
{
	FilterPtr filter = mPipeline->getFilters()->get(uid);
	mSetupWidget->setFilter(filter);

	for (unsigned i=0; i<mAlgoLines.size(); ++i)
		if (mAlgoLines[i]->mFilter->getUid()==uid)
			mAlgoLines[i]->mRadioButton->setChecked(true);
}

void PipelineWidget::runFilterSlot()
{
	PipelineWidgetFilterLine* line = dynamic_cast<PipelineWidgetFilterLine*>(sender());

	if (!line)
		return;

	mPipeline->execute(line->mFilter->getUid());
}


} // namespace cx
