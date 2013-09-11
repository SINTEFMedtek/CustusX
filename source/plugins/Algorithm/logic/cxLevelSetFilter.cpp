#include "cxLevelSetFilter.h"

#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "cxSelectDataStringDataAdapter.h"

namespace cx
{

QString LevelSetFilter::getName() const
{
	return "Level Set Segmentation";
}

QString LevelSetFilter::getType() const
{
	return "LevelSetFilter";
}

QString LevelSetFilter::getHelp() const
{
	return "<html>"
	        "<h3>Binary Threshold Image Filter.</h3>"
	        "<p><i>Segment out areas from the selected image using a threshold.</i></p>"
	        "<p>This filter produces an output image whose pixels are either one of two values"
	        "( OutsideValue or InsideValue ), depending on whether the corresponding input"
	        "image pixels lie between the two thresholds ( LowerThreshold and UpperThreshold )."
	        "Values equal to either threshold is considered to be between the thresholds.<p>"
	        "</html>";
}


bool LevelSetFilter::preProcess() {
}

bool LevelSetFilter::execute() {
    // TODO: Retrieve position of tooltip and use it as seed point
}

bool LevelSetFilter::postProcess() {
}

void LevelSetFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getThresholdOption(mOptions));
	mOptionsAdapters.push_back(this->getEpsilonOption(mOptions));
	mOptionsAdapters.push_back(this->getAlphaOption(mOptions));
}

void LevelSetFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select image input for thresholding");
	connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void LevelSetFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Output thresholded binary image");
	mOutputTypes.push_back(temp);

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Contour");
	temp->setHelp("Output contour generated from thresholded binary image.");
	mOutputTypes.push_back(temp);
}

void LevelSetFilter::setActive(bool on)
{
	FilterImpl::setActive(on);

	/*
	if (!mActive)
		RepManager::getInstance()->getThresholdPreview()->removePreview();
		*/
}

ssc::DoubleDataAdapterXmlPtr LevelSetFilter::getThresholdOption(
        QDomElement root) {
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("Threshold", "",
	                                                                            "Select lower threshold for the segmentation", 1, ssc::DoubleRange(0, 100, 1), 0,
	                                                                            root);
	retval->setAddSlider(true);
	return retval;

}

ssc::DoubleDataAdapterXmlPtr LevelSetFilter::getEpsilonOption(
        QDomElement root) {
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("Epsilon", "",
	                                                                            "Select lower threshold for the segmentation", 1, ssc::DoubleRange(0, 100, 1), 0,
	                                                                            root);
	retval->setAddSlider(true);
	return retval;

}

ssc::DoubleDataAdapterXmlPtr LevelSetFilter::getAlphaOption(
        QDomElement root) {
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("Alpha", "",
	                                                                            "Select lower threshold for the segmentation", 0.1, ssc::DoubleRange(0, 1, 0.01), 2,
	                                                                            root);
	retval->setAddSlider(true);
	return retval;

}



} // end namespace cx


