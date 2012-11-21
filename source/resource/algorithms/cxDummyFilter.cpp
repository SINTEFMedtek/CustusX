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

#include "cxDummyFilter.h"
#include "sscTypeConversions.h"

namespace cx
{

QString DummyFilter::getName() const
{
    return "Dummy Filter";
}

QString DummyFilter::getType() const
{
    return "DummyFilter";
}

QString DummyFilter::getHelp() const
{
    return "A dummy implementation of the cx::Filter class,\n"
           "intended for concept testing.\n"
           "Can also be used as a template for new filters.";
}

std::vector<DataAdapterPtr> DummyFilter::getOptions(QDomElement root)
{
    std::vector<DataAdapterPtr> retval;
    retval.push_back(this->getStringOption(root));
    retval.push_back(this->getDoubleOption(root));
    retval.push_back(this->getBoolOption(root));
    return retval;
}

ssc::StringDataAdapterPtr DummyFilter::getStringOption(QDomElement root)
{
    QStringList list;
    list << "String0" << "String1";
    return ssc::StringDataAdapterXml::initialize("String", "", "dummy string option",
        list[0], list, root);
}

ssc::DoubleDataAdapterPtr DummyFilter::getDoubleOption(QDomElement root)
{
    return ssc::DoubleDataAdapterXml::initialize("Value", "",
            "dummy double value.", 1, ssc::DoubleRange(0.1, 10, 0.01), 2,
            root);
}

ssc::BoolDataAdapterPtr DummyFilter::getBoolOption(QDomElement root)
{
    return ssc::BoolDataAdapterXml::initialize("Bool0", "",
        "Dummy bool value.", false, root);
}


std::vector<Filter::ArgumentType> DummyFilter::getInputTypes()
{
    std::vector<Filter::ArgumentType> retval;

    Filter::ArgumentType arg0("data");
    arg0.mName = "Dummy Input";
    arg0.mHelp = "Dummy input for a dummy algorithm";
    retval.push_back(arg0);

    Filter::ArgumentType arg1("mesh");
    arg1.mName = "Dummy Mesh Input";
    arg1.mHelp = "Dummy mesh input for a dummy algorithm";
    retval.push_back(arg1);

    Filter::ArgumentType arg2("image");
    arg2.mName = "Dummy Image Input";
    arg2.mHelp = "Dummy image input for a dummy algorithm";
    retval.push_back(arg2);

    return retval;
}

std::vector<Filter::ArgumentType> DummyFilter::getOutputTypes()
{
    std::vector<Filter::ArgumentType> retval;

    Filter::ArgumentType arg0("data");
    arg0.mName = "Dummy Output";
    arg0.mHelp = "Dummy output from the dummy algorithm.\n"
            "Output equals input.";
    retval.push_back(arg0);

    return retval;
}


bool DummyFilter::preProcess(std::vector<ssc::DataPtr> input, QDomElement options, QString outputPath)
{
    std::cout << "DummyFilter::preProcess " << input.size() << std::endl;
    mInput = input;
    mOptions = options;
    mOutputPath = outputPath;
    return true;
}

bool DummyFilter::execute()
{
    ssc::StringDataAdapterPtr stringOption = this->getStringOption(mOptions);
    ssc::DoubleDataAdapterPtr doubleOption = this->getDoubleOption(mOptions);
    ssc::BoolDataAdapterPtr boolOption = this->getBoolOption(mOptions);

    std::cout << "Running dummy algorithm..." << std::endl;
    std::cout << QString("  String option [%1]: %2").arg(stringOption->getValueName()).arg(stringOption->getValue()) << std::endl;
    std::cout << QString("  Double option [%1]: %2").arg(doubleOption->getValueName()).arg(doubleOption->getValue()) << std::endl;
    std::cout << QString("  Bool   option [%1]: %2").arg(boolOption->getValueName()).arg(boolOption->getValue()) << std::endl;

    for (unsigned i=0; i< mInput.size(); ++i)
        std::cout << QString("  Input %1: %2").arg(mInput[i] ? mInput[i]->getName() : "NULL") << std::endl;

    std::cout << "Returning input as output." << std::endl;

    return true;
}

std::vector<ssc::DataPtr> DummyFilter::postProcess()
{
    //TODO: add stuff such as saving to dataManager here.
    std::cout << "DummyFilter::postProcess " << mInput.size() << std::endl;

    return mInput;
}



} // namespace cx
