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

#include "cxFilterWidget.h"

#include "sscStringDataAdapter.h"
#include "cxDummyFilter.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxDataLocations.h"
#include "sscHelperWidgets.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxDataInterface.h"

namespace cx
{

OptionsWidget::OptionsWidget(QWidget* parent)
{
    this->setSizePolicy(this->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    mStackedLayout = new QStackedLayout(this);
    mStackedLayout->setMargin(0);
}

void OptionsWidget::setOptions(QString uid, std::vector<DataAdapterPtr> options)
{
    // return if already on uid
    if (mStackedLayout->currentWidget() && (uid == mStackedLayout->currentWidget()->objectName()))
        return;

    // set current to uid, if already created
    for (int i=0; i<mStackedLayout->count(); ++i)
    {
        if (uid==mStackedLayout->widget(i)->objectName())
        {
            mStackedLayout->setCurrentIndex(i);
            return;
        }
    }

    // No existing found,
    // create a new stack element for this uid:
    QWidget* widget = new QWidget(this);
    widget->setObjectName(uid);
    mStackedLayout->addWidget(widget);
    QGridLayout* layout = new QGridLayout(widget);
//    layout->setMargin(0);

    for (unsigned i = 0; i < options.size(); ++i)
    {
        ssc::createDataWidget(widget, options[i], layout, i);
    }

    mStackedLayout->setCurrentWidget(widget);
    mOptions[uid] = options;
}

QString OptionsWidget::getCurrentUid()
{
    return mStackedLayout->currentWidget()->objectName();
}

std::vector<DataAdapterPtr> OptionsWidget::getCurrentOptions()
{
    return this->getOptions(this->getCurrentUid());
}

std::vector<DataAdapterPtr> OptionsWidget::getOptions(QString uid)
{
    if (!mOptions.count(uid))
        return std::vector<DataAdapterPtr>();
    return mOptions[uid];
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

FilterWidget::FilterWidget(QWidget* parent) :
    BaseWidget(parent, "FilterWidget", "Configurable Filter")
{
    mOptions = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("filterwidget");

    mAvailableFilters.push_back(FilterPtr(new DummyFilter()));
    QStringList availableFilters;
    for (unsigned i=0; i<mAvailableFilters.size(); ++i)
        availableFilters << mAvailableFilters[i]->getType();

    mFilterSelector = ssc::StringDataAdapterXml::initialize("filter",
                                                            "Filter",
                                                            "Select which filter to use.",
                                                            availableFilters[0],
                                                            availableFilters,
                                                            mOptions.getElement());
    connect(mFilterSelector.get(), SIGNAL(valueWasSet()), this, SLOT(filterChangedSlot()));

    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QHBoxLayout* filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mFilterSelector));
    topLayout->addLayout(filterLayout);

    this->createAction(this,
                    QIcon(":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png"),
                    "Run Filter", "",
                    SLOT(runFilterSlot()),
                    filterLayout);

    mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
    topLayout->addWidget(mTimedAlgorithmProgressBar);

    mInputsWidget = new OptionsWidget(this);
    topLayout->addWidget(this->wrapInGroupBox(mInputsWidget, "Input"));
    mOutputsWidget = new OptionsWidget(this);
    topLayout->addWidget(this->wrapInGroupBox(mOutputsWidget, "Output"));

    mOptionsWidget = new OptionsWidget(this);
    topLayout->addWidget(this->wrapInGroupBox(mOptionsWidget, "Options"));

    topLayout->addStretch();

    this->filterChangedSlot();
}

QGroupBox* FilterWidget::wrapInGroupBox(QWidget* base, QString name)
{
    QGroupBox* groupBox = new QGroupBox(name);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setMargin(0);
    layout->addWidget(base);
    return groupBox;
}

QString FilterWidget::defaultWhatsThis() const
{
    QString name("None");
    QString help("");
    if (mCurrentFilter)
    {
        name = mCurrentFilter->getName();
        help = mCurrentFilter->getHelp();
    }
    return QString("<html>"
                   "<h3>Filter Widget.</h3>"
                   "<p>Select one type of filter.</p>"
                   "<p><i>Currently selected filter:</ithis></p>"
                   "<h4>%1</h4>"
                   "<p>%2</p>"
                   "</html>").arg(name).arg(help);
}

void FilterWidget::filterChangedSlot()
{
//    std::cout << "FilterWidget::filterChangedSlot()" << std::endl;

    for (unsigned i=0; i<mAvailableFilters.size(); ++i)
    {
        if (mAvailableFilters[i]->getType() == mFilterSelector->getValue())
        {
            mCurrentFilter = mAvailableFilters[i];
        }
    }

    mFilterSelector->setHelp(this->defaultWhatsThis());

    if (mCurrentFilter)
    {
        std::vector<Filter::ArgumentType> inputTypes = mCurrentFilter->getInputTypes();
        std::vector<Filter::ArgumentType> outputTypes = mCurrentFilter->getOutputTypes();


        std::vector<DataAdapterPtr> inputs;
        for (unsigned i=0; i<inputTypes.size(); ++i)
            inputs.push_back(this->createDataAdapter(inputTypes[i]));
        mInputsWidget->setOptions(mCurrentFilter->getType(), inputs);

        std::vector<DataAdapterPtr> outputs;
        for (unsigned i=0; i<outputTypes.size(); ++i)
            outputs.push_back(this->createDataAdapter(outputTypes[i]));
        mOutputsWidget->setOptions(mCurrentFilter->getType(), outputs);

        ssc::XmlOptionFile node = mOptions.descend(mCurrentFilter->getType());
        std::vector<DataAdapterPtr> options = mCurrentFilter->getOptions(node.getElement());
        mOptionsWidget->setOptions(mCurrentFilter->getType(), options);
    }
    else
        mOptionsWidget->setOptions("", std::vector<DataAdapterPtr>());
}

DataAdapterPtr FilterWidget::createDataAdapter(Filter::ArgumentType type)
{
    SelectDataStringDataAdapterBasePtr retval;

    if (type.mDataType == "data")
    {
        retval = SelectDataStringDataAdapter::New();
    }
    else if (type.mDataType == "mesh")
    {
        retval = SelectMeshStringDataAdapter::New();
    }
    else if (type.mDataType == "image")
    {
        retval = SelectImageStringDataAdapter::New();
    }

    if (retval)
    {
        retval->setHelp(type.mHelp);
        retval->setValueName(type.mName);
    }

    return retval;
}

void FilterWidget::runFilterSlot()
{
    if (!mCurrentFilter)
        return;
    if (mThread)
    {
        ssc::messageManager()->sendWarning(QString("Last operation on %1 is not finished. Could not start filtering")
                                           .arg(mThread->getFilter()->getName()));
        return;
    }

    mThread.reset(new FilterTimedAlgorithm(mCurrentFilter));
    connect(mThread.get(), SIGNAL(finished()), this, SLOT(finishedSlot()));
    mTimedAlgorithmProgressBar->attach(mThread);

    std::vector<ssc::DataPtr> input;
    std::vector<DataAdapterPtr> inputAdapters = mInputsWidget->getOptions(mCurrentFilter->getType());

    for (unsigned i=0; i<inputAdapters.size(); ++i)
    {
        SelectDataStringDataAdapterBasePtr base = boost::shared_dynamic_cast<SelectDataStringDataAdapterBase>(inputAdapters[i]);
        if (!base)
            continue;
        input.push_back(base->getData());
    }

    mThread->setInput(input,
                     patientService()->getPatientData()->getActivePatientFolder(),
                     mOptions.getElement(mCurrentFilter->getType()));

    mThread->execute();
}

void FilterWidget::finishedSlot()
{
    mTimedAlgorithmProgressBar->detach(mThread);
    disconnect(mThread.get(), SIGNAL(finished()), this, SLOT(finishedSlot()));

    std::vector<ssc::DataPtr> result = mThread->getOutput();
    std::vector<DataAdapterPtr> outputAdapters = mOutputsWidget->getOptions(mCurrentFilter->getType());

    for (unsigned i=0; i<outputAdapters.size(); ++i)
    {
        SelectDataStringDataAdapterBasePtr base = boost::shared_dynamic_cast<SelectDataStringDataAdapterBase>(outputAdapters[i]);
        if (!base)
            continue;
        if (i < result.size())
            base->setValue(result[i]->getUid());
    }

    mThread.reset();
}

} // namespace cx
