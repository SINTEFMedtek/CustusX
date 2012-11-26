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

#ifndef CXDUMMYFILTER_H
#define CXDUMMYFILTER_H

#include "cxFilterImpl.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"

namespace cx
{

/** Dummy implementation of Filter
 *
 * \ingroup cxResourceAlgorithms
 * \date Nov 17, 2012
 * \author christiana
 */
class DummyFilter : public FilterImpl
{
    Q_OBJECT

public:
    virtual ~DummyFilter() {}

    virtual QString getType() const;
    virtual QString getName() const;
    virtual QString getHelp() const;

//    virtual bool preProcess(QString outputPath);
    virtual bool execute();
    virtual void postProcess();

protected:
    virtual void createOptions(QDomElement root);
    virtual void createInputTypes();
    virtual void createOutputTypes();

private:
    ssc::StringDataAdapterPtr getStringOption(QDomElement root);
    ssc::DoubleDataAdapterPtr getDoubleOption(QDomElement root);
    ssc::BoolDataAdapterPtr getBoolOption(QDomElement root);
};

} // namespace cx


#endif // CXDUMMYFILTER_H
