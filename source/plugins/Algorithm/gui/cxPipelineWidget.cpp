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
    std::cout << QString("PipelineWidgetFilterLine margin=%1, spacing=%2").arg(layout->margin()).arg(layout->spacing()) << std::endl;
//    layout->setMargin(layout->margin()/2);
    layout->setMargin(0);

    mAlgoNameLabel = new QLabel(mFilter->getName(), this);
    mAlgoNameLabel->setToolTip(mFilter->getHelp());
    layout->addWidget(mAlgoNameLabel);    

//    // debug
//    if (mFilter->getName()=="Segmentation")
//    {

    this->setStyleSheet("CXSmallToolButton#RunFilterButton { width : 36px; height : 12px; }");

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

//    }
//    else if (mFilter->getName()=="Centerline")
//    {
//        mAction = this->createAction(this,
//                        QIcon(":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png"),
//                        "Run Filter", "",
//                        SIGNAL(requestRunFilter()),
//                        layout);
//        mAction->setData(mFilter->getUid());
//    }
//    else
//    {

//    }

    mTimedAlgorithmProgressBar = new TimedAlgorithmProgressBar;
    mTimedAlgorithmProgressBar->setShowTextLabel(false);
    layout->addWidget(mTimedAlgorithmProgressBar, 1);
}

void PipelineWidgetFilterLine::radioButtonSelectedSlot(bool on)
{
    if (!on)
        return;

    std::cout << "mAlgoNameLabel " << mAlgoNameLabel->width() << " " << mAlgoNameLabel->height() << std::endl;
    std::cout << "mRadioButton " << mRadioButton->width() << " " << mRadioButton->height() << std::endl << std::endl;

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
    mCurrentlyRunningPipelineWidgetFilterLine(NULL)
{
    FilterGroupPtr filters = mPipeline->getFilters();
    std::vector<SelectDataStringDataAdapterBasePtr> nodes = mPipeline->getNodes();
    if (filters->size()+1 != nodes.size())
        ssc::messageManager()->sendError("Filter/Node mismatch");

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    std::cout << "PipelineWidget spacing " << topLayout->spacing() << std::endl;\
    topLayout->setSpacing(4);
    mButtonGroup = new QButtonGroup(this);

    for (unsigned i=0; i<filters->size(); ++i)
    {
        topLayout->addWidget(new DataSelectWidget(this, nodes[i]));

        PipelineWidgetFilterLine* algoLine = new PipelineWidgetFilterLine(this, filters->get(i), mButtonGroup);
        connect(algoLine, SIGNAL(requestRunFilter()), this, SLOT(runFilterSlot()));
        connect(algoLine, SIGNAL(filterSelected(QString)), this, SLOT(filterSelectedSlot(QString)));
        mAlgoLines.push_back(algoLine);
        topLayout->addWidget(this->wrapInFrame(algoLine));
    }
    topLayout->addWidget(new DataSelectWidget(this, nodes.back()));

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

void PipelineWidget::runFilterSlot()
{
    if (mThread)
    {
        ssc::messageManager()->sendWarning(QString("Last operation on %1 is not finished. Could not start filtering")
                                           .arg(mThread->getFilter()->getName()));
        return;
    }

    mCurrentlyRunningPipelineWidgetFilterLine = dynamic_cast<PipelineWidgetFilterLine*>(sender());

    if (!mCurrentlyRunningPipelineWidgetFilterLine)
        return;

    FilterPtr filter = mCurrentlyRunningPipelineWidgetFilterLine->mFilter;

    mThread.reset(new FilterTimedAlgorithm(filter));
    connect(mThread.get(), SIGNAL(finished()), this, SLOT(finishedSlot()));
    mCurrentlyRunningPipelineWidgetFilterLine->mTimedAlgorithmProgressBar->attach(mThread);

    mThread->execute();
}

void PipelineWidget::finishedSlot()
{
    mCurrentlyRunningPipelineWidgetFilterLine->mTimedAlgorithmProgressBar->detach(mThread);
    mCurrentlyRunningPipelineWidgetFilterLine = NULL;
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
