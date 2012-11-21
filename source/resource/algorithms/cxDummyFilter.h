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

#include "cxFilter.h"
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
class DummyFilter : public Filter
{
    Q_OBJECT

public:
    virtual ~DummyFilter() {}

    virtual QString getType() const;
    virtual QString getName() const;
    virtual QString getHelp() const;
    virtual std::vector<DataAdapterPtr> getOptions(QDomElement root);
    virtual std::vector<Filter::ArgumentType> getInputTypes();
    virtual std::vector<Filter::ArgumentType> getOutputTypes();
    virtual bool preProcess(std::vector<ssc::DataPtr> input, QDomElement options, QString outputPath);
    virtual bool execute();
    virtual std::vector<ssc::DataPtr> postProcess();


    // extensions:
    ssc::StringDataAdapterPtr getStringOption(QDomElement root);
    ssc::DoubleDataAdapterPtr getDoubleOption(QDomElement root);
    ssc::BoolDataAdapterPtr getBoolOption(QDomElement root);

private:
    std::vector<ssc::DataPtr> mInput;
    QDomElement mOptions;
    QString mOutputPath;
};

} // namespace cx


#endif // CXDUMMYFILTER_H
