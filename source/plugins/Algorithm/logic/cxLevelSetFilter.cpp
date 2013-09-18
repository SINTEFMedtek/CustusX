#include "cxLevelSetFilter.h"

#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "cxSelectDataStringDataAdapter.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscData.h"
#include "sscImage.h"
#include "sscDataManager.h"
#include <vtkImageData.h>

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
	        "<h3>Level Set Segmentation Filter.</h3>"
	        "</html>";
}


bool LevelSetFilter::preProcess() {
    return true;
}

ssc::Vector3D getSeedPointFromTool(ssc::DataPtr image) {
    // Retrieve position of tooltip and use it as seed point
    ssc::Vector3D point = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(
            ssc::CoordinateSystemHelpers::getD(image)
    );
    std::cout << "the selected seed point is: " << point(0) << " " << point(1) << " " << point(2) << "\n";

    return point;
}

int * getImageSize(ssc::DataPtr inputImage) {
    ssc::ImagePtr image = ssc::DataManager::getInstance()->getImage(inputImage->getUid());
    return image->getBaseVtkImageData()->GetDimensions();
}

bool isSeedPointInsideImage(ssc::Vector3D seedPoint, ssc::DataPtr image) {
    int * size = getImageSize(image);
    std::cout << "size of image is: " << size[0] << " " << size[1] << " " << size[2] << "\n";
    int x = (int)seedPoint(0);
    int y = (int)seedPoint(1);
    int z = (int)seedPoint(2);
    bool result = x >= 0 && y >= 0 && z >= 0
            && x < size[0] && y < size[1] && z < size[2];
    return result;
}


bool LevelSetFilter::execute() {
    ssc::DataPtr inputImage = mInputTypes[0].get()->getData();
    if(!inputImage) {
        std::cout << "No input data selected" << std::endl;
        return false;
    }

    if(inputImage->getType() != "image") {
        std::cout << "Input data has to be an image" << std::endl;
        return false;
    }

    ssc::Vector3D seedPoint = getSeedPointFromTool(inputImage);

    if(!isSeedPointInsideImage(seedPoint, inputImage)) {
        std::cout << "Seed point is not inside image!" << std::endl;
        return false;
    }

    float threshold = getThresholdOption(mOptions)->getValue();
    float epsilon = getEpsilonOption(mOptions)->getValue();
    float alpha = getAlphaOption(mOptions)->getValue();

    std::cout << "Parameters are set to: " << threshold << " "  << epsilon << " " << alpha << std::endl;

    // TODO: Run level set segmentation here

    return true;
}

bool LevelSetFilter::postProcess() {
    return true;
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
	                                                                            "Select threshold for the segmentation", 1, ssc::DoubleRange(0, 100, 1), 0,
	                                                                            root);
	retval->setAddSlider(true);
	return retval;

}

ssc::DoubleDataAdapterXmlPtr LevelSetFilter::getEpsilonOption(
        QDomElement root) {
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("Epsilon", "",
	                                                                            "Select epsilon for the segmentation", 1, ssc::DoubleRange(0, 100, 1), 0,
	                                                                            root);
	retval->setAddSlider(true);
	return retval;

}

ssc::DoubleDataAdapterXmlPtr LevelSetFilter::getAlphaOption(
        QDomElement root) {
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("Alpha", "",
	                                                                            "Select alpha for the segmentation", 0.1, ssc::DoubleRange(0, 1, 0.01), 2,
	                                                                            root);
	retval->setAddSlider(true);
	return retval;

}



} // end namespace cx


