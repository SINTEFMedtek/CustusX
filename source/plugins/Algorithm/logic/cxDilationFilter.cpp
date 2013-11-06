#include "cxDilationFilter.h"

namespace cx {

QString DilationFilter::getName() const
{
	return "Dilation";
}

QString DilationFilter::getType() const
{
	return "DilationFilter";
}

QString DilationFilter::getHelp() const
{
	return "<html>"
	        "<h3>Dilation Filter.</h3>"
	        "</html>";
}

DoubleDataAdapterXmlPtr DilationFilter::getDilationAmountOption(QDomElement root)
{
	DoubleDataAdapterXmlPtr retval = DoubleDataAdapterXml::initialize("Amount", "",
    "Select amount of dilation in number of voxels", 1, DoubleRange(1, 20, 1), 0,
                    root);
	retval->setAddSlider(true);
	return retval;
}

void DilationFilter::createOptions()
{
	mDilationAmountOption = this->getDilationAmountOption(mOptions);
	mOptionsAdapters.push_back(mDilationAmountOption);
}

void DilationFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select segmentation input for dilation");
	mInputTypes.push_back(temp);
}

void DilationFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Dilated segmentation image");
	mOutputTypes.push_back(temp);

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Contour");
	temp->setHelp("Output contour generated from dilated segmentation image.");
	mOutputTypes.push_back(temp);
}

bool DilationFilter::preProcess() {

    return true;
}

bool DilationFilter::execute() {

    return true;
}

bool DilationFilter::postProcess() {

    return true;
}



} // namespace cx
