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
#include "cxPipeline.h"

namespace cx
{


FusedInputOutputSelectDataStringDataAdapterPtr FusedInputOutputSelectDataStringDataAdapter::create(SelectDataStringDataAdapterBasePtr base, SelectDataStringDataAdapterBasePtr input)
{
    FusedInputOutputSelectDataStringDataAdapterPtr retval(new FusedInputOutputSelectDataStringDataAdapter(base, input));
    return retval;
}

FusedInputOutputSelectDataStringDataAdapter::FusedInputOutputSelectDataStringDataAdapter(SelectDataStringDataAdapterBasePtr base, SelectDataStringDataAdapterBasePtr input)
{
    mBase = base;
    mInput = input;
    connect(mInput.get(), SIGNAL(changed()), this, SLOT(inputDataChangedSlot()));
//    connect(mInput.get(), SIGNAL(dataChanged(QString)), this, SLOT(inputDataChangedSlot()));
//    connect(mBase.get(), SIGNAL(dataChanged(QString)), this, SIGNAL(dataChanged(QString)));
    connect(mBase.get(), SIGNAL(changed()), this, SIGNAL(changed()));
}


bool FusedInputOutputSelectDataStringDataAdapter::setValue(const QString& value)
{
    return mBase->setValue(value);
}

QString FusedInputOutputSelectDataStringDataAdapter::getValue() const
{
    return mBase->getValue();
}

QString FusedInputOutputSelectDataStringDataAdapter::getValueName() const
{
    return mBase->getValueName();
}

QStringList FusedInputOutputSelectDataStringDataAdapter::getValueRange() const
{
    return mBase->getValueRange();
}

QString FusedInputOutputSelectDataStringDataAdapter::convertInternal2Display(QString internal)
{
    return mBase->convertInternal2Display(internal);
}

QString FusedInputOutputSelectDataStringDataAdapter::getHelp() const
{
    return mBase->getHelp();
}

ssc::DataPtr FusedInputOutputSelectDataStringDataAdapter::getData() const
{
    return mBase->getData();
}

void FusedInputOutputSelectDataStringDataAdapter::setValueName(const QString name)
{
    mBase->setValueName(name);
}

void FusedInputOutputSelectDataStringDataAdapter::setHelp(QString text)
{
    mBase->setHelp(text);
}

void FusedInputOutputSelectDataStringDataAdapter::inputDataChangedSlot()
{
    // the entire point of the class: update mBase when mInput is changed:
    mBase->setValue(mInput->getValue());
}



///--------------------------------------------------------inputDataChangedSlot
///--------------------------------------------------------
///--------------------------------------------------------


Pipeline::Pipeline(QObject *parent) :
    QObject(parent)
{
}

void Pipeline::initialize(FilterGroupPtr filters)
{
    mFilters = filters;

    for (unsigned i=0; i<mFilters->size(); ++i)
    {
        FilterPtr filter = mFilters->get(i);
        filter->getInputTypes();
        filter->getOutputTypes();
        filter->getOptions(mFilters->getOptions().descend(filter->getUid()).getElement());
    }
}
FilterGroupPtr Pipeline::getFilters() const
{
    return mFilters;
}

std::vector<SelectDataStringDataAdapterBasePtr> Pipeline::getNodes()
{
    // TODO: create fused nodes: input+output
    // TODO: create getMainXXType() in filters instead of using zero.

    std::vector<SelectDataStringDataAdapterBasePtr> retval;

    if (mFilters->empty())
        return retval;

    // first node is the input of the first algo
    retval.push_back(mFilters->get(0)->getInputTypes()[0]);

    // intermediate nodes are fusions between output and input
    for (unsigned i=1; i<mFilters->size(); ++i)
    {
        SelectDataStringDataAdapterBasePtr output = mFilters->get(i-1)->getOutputTypes()[0];
        SelectDataStringDataAdapterBasePtr base  = mFilters->get(i)->getInputTypes()[0];
        FusedInputOutputSelectDataStringDataAdapterPtr node;
        node = FusedInputOutputSelectDataStringDataAdapter::create(base, output);
        retval.push_back(node);
    }

    // last node is the output of the last algo
    retval.push_back(mFilters->get(mFilters->size()-1)->getOutputTypes()[0]);

    return retval;
}

void Pipeline::execute(int filterIndex)
{

}


} // namespace cx
