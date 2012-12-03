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

#include "cxPipelineWidget.h"
#include "sscHelperWidgets.h"
#include "sscMessageManager.h"
#include "cxTimedAlgorithmProgressBar.h"
#include <QtGui>
#include "sscTypeConversions.h"
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
	//    std::cout << QString("PipelineWidgetFilterLine margin=%1, spacing=%2").arg(layout->margin()).arg(layout->spacing()) << std::endl;
	//    layout->setMargin(layout->margin()/2);
	layout->setMargin(0);
	layout->setSpacing(2);

	mAlgoNameLabel = new QLabel(QString("<b>%1</b>").arg(mFilter->getName()), this);
	mAlgoNameLabel->setToolTip(mFilter->getHelp());
	layout->addWidget(mAlgoNameLabel);

	mTimedAlgorithmProgressBar = new TimedAlgorithmProgressBar;
	mTimedAlgorithmProgressBar->setShowTextLabel(false);
	layout->addWidget(mTimedAlgorithmProgressBar, 1);

	mAction = this->createAction(this,
	                             QIcon(":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png"),
	                             "Run Filter", "",
	                             SIGNAL(requestRunFilter()),
	                             NULL);
	mAction->setData(mFilter->getUid());

	CXSmallToolButton* button = new CXSmallToolButton();
	button->setObjectName("RunFilterButton");
	button->setDefaultAction(mAction);
	layout->addWidget(button);

	mDetailsAction = this->createAction(this,
	                                    QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png"),
	                                    "Run Filter", "",
	                                    SIGNAL(showDetails()),
	                                    NULL);
	mDetailsAction->setData(mFilter->getUid());

	button = new CXSmallToolButton();
	button->setDefaultAction(mDetailsAction);
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

	//    std::cout << "mAlgoNameLabel " << mAlgoNameLabel->width() << " " << mAlgoNameLabel->height() << std::endl;
	//    std::cout << "mRadioButton " << mRadioButton->width() << " " << mRadioButton->height() << std::endl << std::endl;

	emit filterSelected(mFilter->getUid());
}

void PipelineWidgetFilterLine::mousePressEvent(QMouseEvent* event)
{
	QWidget::mousePressEvent(event);

	mRadioButton->setChecked(true);
}

QString PipelineWidgetFilterLine::defaultWhatsThis() const
{
	return QString("");
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

PipelineWidget::PipelineWidget(QWidget* parent, PipelinePtr pipeline) :
    BaseWidget(parent, "PipelineWidget", "Pipeline"),
    mPipeline(pipeline)//,
  //mCurrentlyRunningPipelineWidgetFilterLine(NULL)
{
	FilterGroupPtr filters = mPipeline->getFilters();
	std::vector<SelectDataStringDataAdapterBasePtr> nodes = mPipeline->getNodes();
	if (filters->size()+1 != nodes.size())
		ssc::messageManager()->sendError("Filter/Node mismatch");

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	std::cout << "PipelineWidget spacing " << topLayout->spacing() << std::endl;\
	topLayout->setSpacing(4);
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
		topLayout->addLayout(Inner::addHMargin(new DataSelectWidget(this, nodes[i])));
		//        topLayout->addWidget(new DataSelectWidget(this, nodes[i]));

		PipelineWidgetFilterLine* algoLine = new PipelineWidgetFilterLine(this, filters->get(i), mButtonGroup);
		connect(algoLine, SIGNAL(requestRunFilter()), this, SLOT(runFilterSlot()));
		connect(algoLine, SIGNAL(showDetails()), this, SLOT(toggleDetailsSlot()));
		connect(algoLine, SIGNAL(filterSelected(QString)), this, SLOT(filterSelectedSlot(QString)));
		algoLine->mTimedAlgorithmProgressBar->attach(mPipeline->getTimedAlgorithm(filters->get(i)->getUid()));

		mAlgoLines.push_back(algoLine);
		QFrame* frame = this->wrapInFrame(algoLine);
		//        frame->layout()->setContentsMargins(4,0,4,0); // nice on mac
		frame->layout()->setContentsMargins(4,4,4,4); // nice on linux
		frame->setObjectName("FilterBackground");
		topLayout->addWidget(frame);
		//        topLayout->addWidget(algoLine);
	}
	//    topLayout->addWidget(new DataSelectWidget(this, nodes.back()));
	topLayout->addLayout(Inner::addHMargin(new DataSelectWidget(this, nodes.back())));

	topLayout->addSpacing(12);

	mSetupWidget = new FilterSetupWidget(this, filters->getOptions(), true);
	topLayout->addWidget(mSetupWidget);
	mSetupWidget->setVisible(settings()->value("pipeline/showDetails").toBool());

	topLayout->addStretch();

	this->filterSelectedSlot(filters->get(0)->getUid());
}

void PipelineWidget::toggleDetailsSlot()
{
	mSetupWidget->setVisible(!mSetupWidget->isVisible());
	settings()->setValue("pipeline/showDetails", mSetupWidget->isVisible());
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

QString PipelineWidget::defaultWhatsThis() const
{
	return QString("<html>"
	               "<h3>Pipeline Widget.</h3>"
	               "<p>Run a series of filters.</p>"
	               "</html>");
}


} // namespace cx
