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

void DummyFilter::createOptions(QDomElement root)
{
	mOptionsAdapters.push_back(this->getStringOption(root));
	mOptionsAdapters.push_back(this->getDoubleOption(root));
	mOptionsAdapters.push_back(this->getBoolOption(root));
}

void DummyFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Dummy data input for a dummy algorithm");
	mInputTypes.push_back(temp);

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Image");
	temp->setHelp("Dummy image input for a dummy algorithm");
	mInputTypes.push_back(temp);

	temp = SelectMeshStringDataAdapter::New();
	temp->setValueName("Mesh");
	temp->setHelp("Dummy mesh input for a dummy algorithm");
	mInputTypes.push_back(temp);
}

void DummyFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Dummy output from the dummy algorithm");
	mOutputTypes.push_back(temp);
}

bool DummyFilter::execute()
{
	ssc::StringDataAdapterPtr stringOption = this->getStringOption(mCopiedOptions);
	ssc::DoubleDataAdapterPtr doubleOption = this->getDoubleOption(mCopiedOptions);
	ssc::BoolDataAdapterPtr boolOption = this->getBoolOption(mCopiedOptions);

	std::cout << "Running dummy algorithm..." << std::endl;
	std::cout << QString("  String option [%1]: %2").arg(stringOption->getValueName()).arg(stringOption->getValue()) << std::endl;
	std::cout << QString("  Double option [%1]: %2").arg(doubleOption->getValueName()).arg(doubleOption->getValue()) << std::endl;
	std::cout << QString("  Bool   option [%1]: %2").arg(boolOption->getValueName()).arg(boolOption->getValue()) << std::endl;

	for (unsigned i=0; i< mCopiedInput.size(); ++i)
		std::cout << QString("  Input %1: %2").arg(mCopiedInput[i] ? mCopiedInput[i]->getName() : "NULL") << std::endl;

	std::cout << "Returning input as output." << std::endl;

	return true;
}

void DummyFilter::postProcess()
{
	//TODO: add stuff such as saving to dataManager here.
	std::cout << "DummyFilter::postProcess " << mCopiedInput.size() << std::endl;

	if (mInputTypes.front()->getData())
		mOutputTypes.front()->setValue(mInputTypes.front()->getData()->getUid());

	//    return mInput;
}



} // namespace cx
