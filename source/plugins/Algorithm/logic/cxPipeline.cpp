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

Pipeline::Pipeline(QObject *parent) :
    QObject(parent)
{
}

void Pipeline::initialize(FilterGroupPtr filters)
{
    mFilters = filters;
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

    for (unsigned i=0; i<mFilters->size(); ++i)
        retval.push_back(mFilters->get(i)->getInputTypes()[0]);
    retval.push_back(mFilters->get(mFilters->size()-1)->getOutputTypes()[0]);

    return retval;
}

void Pipeline::execute(int filterIndex)
{

}


} // namespace cx
