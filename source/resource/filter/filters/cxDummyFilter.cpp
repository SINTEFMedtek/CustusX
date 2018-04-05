/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDummyFilter.h"
#include "cxTypeConversions.h"
#include "cxSelectDataStringProperty.h"
#include "cxData.h"
#include "cxDoubleProperty.h"
#include "cxStringProperty.h"
#include "cxBoolProperty.h"
#include "cxVisServices.h"

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

StringPropertyBasePtr DummyFilter::getStringOption(QDomElement root)
{
	QStringList list;
	list << "String0" << "String1";
	return StringProperty::initialize("String", "", "dummy string option",
												 list[0], list, root);
}

DoublePropertyBasePtr DummyFilter::getDoubleOption(QDomElement root)
{
	return DoubleProperty::initialize("Value", "",
	                                             "dummy double value.", 1, DoubleRange(0.1, 10, 0.01), 2,
												 root);
}

BoolPropertyBasePtr DummyFilter::getBoolOption(QDomElement root)
{
	return BoolProperty::initialize("Bool0", "",
	                                           "Dummy bool value.", false, root);
}

void DummyFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getStringOption(mOptions));
	mOptionsAdapters.push_back(this->getDoubleOption(mOptions));
	mOptionsAdapters.push_back(this->getBoolOption(mOptions));
}

void DummyFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Dummy data input for a dummy algorithm");
	mInputTypes.push_back(temp);

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Image");
	temp->setHelp("Dummy image input for a dummy algorithm");
	mInputTypes.push_back(temp);

	temp = StringPropertySelectMesh::New(mServices->patient());
	temp->setValueName("Mesh");
	temp->setHelp("Dummy mesh input for a dummy algorithm");
	mInputTypes.push_back(temp);
}

void DummyFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Dummy output from the dummy algorithm");
	mOutputTypes.push_back(temp);
}

bool DummyFilter::execute()
{
	StringPropertyBasePtr stringOption = this->getStringOption(mCopiedOptions);
	DoublePropertyBasePtr doubleOption = this->getDoubleOption(mCopiedOptions);
	BoolPropertyBasePtr boolOption = this->getBoolOption(mCopiedOptions);

	std::cout << "Running dummy algorithm..." << std::endl;
	std::cout << QString("  String option [%1]: %2").arg(stringOption->getDisplayName()).arg(stringOption->getValue()) << std::endl;
	std::cout << QString("  Double option [%1]: %2").arg(doubleOption->getDisplayName()).arg(doubleOption->getValue()) << std::endl;
	std::cout << QString("  Bool   option [%1]: %2").arg(boolOption->getDisplayName()).arg(boolOption->getValue()) << std::endl;

	for (unsigned i=0; i< mCopiedInput.size(); ++i)
		std::cout << QString("  Input %1: %2").arg(mCopiedInput[i] ? mCopiedInput[i]->getName() : "NULL") << std::endl;

	std::cout << "Returning input as output." << std::endl;

	return true;
}

bool DummyFilter::postProcess()
{
	//TODO: add stuff such as saving to dataManager here.
	std::cout << "DummyFilter::postProcess " << mCopiedInput.size() << std::endl;

	if (mInputTypes.front()->getData())
		mOutputTypes.front()->setValue(mInputTypes.front()->getData()->getUid());

	//    return mInput;
	return true;
}



} // namespace cx
