#include "cxTubeSegmentationFilter.h"

#ifdef CX_USE_TSF
#include "tube-segmentation.hpp"

namespace cx {

QString TubeSegmentationFilter::getName() const
{
	return "Tube-Segmentation Filter";
}

QString TubeSegmentationFilter::getType() const
{
	return "TubeSegmentationFilter";
}

QString TubeSegmentationFilter::getHelp() const
{
	return "<html>"
	        "<h3>Tube-Segmentation.</h3>"
	        "<p><i>Extracts the centerline and creates a segementation. </br>GPU-base algorithm wrtiten by Erik Smistad (NTNU).</i></p>"
	        "</html>";
}

//ssc::StringDataAdapterPtr TubeSegmentationFilter::getStringOption(QDomElement root)
//{
//	QStringList list;
//	list << "String0" << "String1";
//	return ssc::StringDataAdapterXml::initialize("String", "", "dummy string option",
//	                                             list[0], list, root);
//}
//
//ssc::DoubleDataAdapterPtr TubeSegmentationFilter::getDoubleOption(QDomElement root)
//{
//	return ssc::DoubleDataAdapterXml::initialize("Value", "",
//	                                             "dummy double value.", 1, ssc::DoubleRange(0.1, 10, 0.01), 2,
//	                                             root);
//}
//
//ssc::BoolDataAdapterPtr TubeSegmentationFilter::getBoolOption(QDomElement root)
//{
//	return ssc::BoolDataAdapterXml::initialize("Bool0", "",
//	                                           "Dummy bool value.", false, root);
//}

void TubeSegmentationFilter::createOptions(QDomElement root)
{
//	mOptionsAdapters.push_back(this->getStringOption(root));
//	mOptionsAdapters.push_back(this->getDoubleOption(root));
//	mOptionsAdapters.push_back(this->getBoolOption(root));
}

void TubeSegmentationFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select input to run Tube segmentation on.");
	mInputTypes.push_back(temp);
}

void TubeSegmentationFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Generated centerline.");
	mOutputTypes.push_back(temp);
}

bool TubeSegmentationFilter::execute()
{
	ssc::ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	//======================================================

	//======================================================

	std::cout << "NOT IMPLEMENTED: TubeSegmentationFilter::execute()" << std::endl;
	return false;
}

void TubeSegmentationFilter::postProcess()
{
	/*
	//TODO: add stuff such as saving to dataManager here.
	std::cout << "TubeSegmentationFilter::postProcess " << mCopiedInput.size() << std::endl;

	if (mInputTypes.front()->getData())
		mOutputTypes.front()->setValue(mInputTypes.front()->getData()->getUid());

	//    return mInput;
	 */
	std::cout << "NOT IMPLEMENTED: TubeSegmentationFilter::postProcess()" << std::endl;
}

} /* namespace cx */
#endif //CX_USE_TSF

