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

namespace cx
{

PipelineWidgetFilterLine::PipelineWidgetFilterLine(QWidget* parent, FilterPtr filter, QButtonGroup* buttonGroup) :
    BaseWidget(parent, "PipelineWidgetFilterLine", "PipelineWidgetFilterLine"),
    mFilter(filter)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    mRadioButton = new QRadioButton(this);
    buttonGroup->addButton(mRadioButton);
    connect(mRadioButton, SIGNAL(toggled(bool)), this, SLOT(radioButtonSelectedSlot(bool)));
    layout->addWidget(mRadioButton);

    mAlgoNameLabel = new QLabel(mFilter->getName(), this);
    layout->addWidget(mAlgoNameLabel);

    mAction = this->createAction(this,
                    QIcon(":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png"),
                    "Run Filter", "",
                    SIGNAL(requestRunFilter()),
                    layout);
    mAction->setData(mFilter->getType());

    mTimedAlgorithmProgressBar = new TimedAlgorithmProgressBar;
    layout->addWidget(mTimedAlgorithmProgressBar, 1);
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

QString PipelineWidgetFilterLine::defaultWhatsThis() const
{
    return QString("");
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

PipelineWidget::PipelineWidget(QWidget* parent, PipelinePtr pipeline) :
    BaseWidget(parent, "PipelineWidget", "Pipeline"),
    mPipeline(pipeline),
    mCurrentlyRunningIndex(-1)
{
    FilterGroupPtr filters = mPipeline->getFilters();
    std::vector<SelectDataStringDataAdapterBasePtr> nodes = mPipeline->getNodes();
    if (filters->size()+1 != nodes.size())
        ssc::messageManager()->sendError("Filter/Node mismatch");

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    mButtonGroup = new QButtonGroup(this);

    for (unsigned i=0; i<filters->size(); ++i)
    {
        topLayout->addWidget(ssc::createDataWidget(this, nodes[i]));

        PipelineWidgetFilterLine* algoLine = new PipelineWidgetFilterLine(this, filters->get(i), mButtonGroup);
        connect(algoLine, SIGNAL(requestRunFilter()), this, SLOT(runFilterSlot()));
        connect(algoLine, SIGNAL(filterSelected(QString)), this, SLOT(filterSelectedSlot(QString)));
        mAlgoLines.push_back(algoLine);
        topLayout->addWidget(algoLine);
    }
    topLayout->addWidget(ssc::createDataWidget(this, nodes.back()));

    mSetupWidget = new FilterSetupWidget(this, filters->getOptions(), true);
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

//void PipelineWidget::selectFilter(int index)
//{
//    FilterPtr filter = mPipeline->getFilters()[index];

//    mAlgoLines[index]->mRadioButton->setChecked(true);

//    mSetupWidget->setFilter(filter);
//}

void PipelineWidget::runFilterSlot()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if (!action)
        return;

    int index = action->data().toInt();

    if (index<0 || index>=mPipeline->getFilters()->size())
        return;

    mCurrentlyRunningIndex = index;
    FilterPtr filter = mPipeline->getFilters()->get(index);

    if (mThread)
    {
        ssc::messageManager()->sendWarning(QString("Last operation on %1 is not finished. Could not start filtering")
                                           .arg(mThread->getFilter()->getName()));
        return;
    }

    mThread.reset(new FilterTimedAlgorithm(filter));
    connect(mThread.get(), SIGNAL(finished()), this, SLOT(finishedSlot()));
    mAlgoLines[index]->mTimedAlgorithmProgressBar->attach(mThread);

    mThread->execute();
}

void PipelineWidget::finishedSlot()
{
    mAlgoLines[mCurrentlyRunningIndex]->mTimedAlgorithmProgressBar->detach(mThread);
    disconnect(mThread.get(), SIGNAL(finished()), this, SLOT(finishedSlot()));
    mThread.reset();
}

QString PipelineWidget::defaultWhatsThis() const
{
    return QString("<html>"
                   "<h3>Pipeline Widget.</h3>"
                   "<p>Run a series of filters.</p>"
                   "</html>");
}


} // namespace cx
