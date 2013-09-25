#include "cxLevelSetFilter.h"

#ifdef CX_USE_LEVEL_SET

#include "sscTime.h"
#include "sscMesh.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscRegistrationTransform.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "cxSelectDataStringDataAdapter.h"
#include "sscCoordinateSystemHelpers.h"
#include "sscData.h"
#include "sscImage.h"
#include "sscDataManager.h"
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include "cxContourFilter.h"
#include "cxDataLocations.h"
#include "cxPatientService.h"
#include "cxPatientData.h"


#include "levelSet.hpp"

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

    // Run level set segmentation 
	std::string filename = (patientService()->getPatientData()->getActivePatientFolder()+"/"+inputImage->getFilePath()).toStdString();
    SIPL::int3 seed(seedPoint(0), seedPoint(1), seedPoint(2));
    try {
        SIPL::Volume<char> * result = runLevelSet(
                filename.c_str(),
                seed,
                10, // seed radius
                1000, // iterations per narrow band
                threshold,
                epsilon,
                alpha
        );
        SIPL::int3 size = result->getSize();
        ssc::ImagePtr image = ssc::DataManager::getInstance()->getImage(inputImage->getUid());
        vtkImageDataPtr rawSegmentation = this->convertToVtkImageData((char *)result->getData(), size.x, size.y, size.z, image);

        //make contour of segmented volume
        double threshold = 1;/// because the segmented image is 0..1
        vtkPolyDataPtr rawContour = ContourFilter::execute(rawSegmentation, threshold);
        //add segmentation internally to cx
        QString uidSegmentation = image->getUid() + "_seg%1";
        QString nameSegmentation = image->getName()+"_seg%1";
        ssc::ImagePtr outputSegmentation = ssc::dataManager()->createDerivedImage(rawSegmentation,uidSegmentation, nameSegmentation, image);
        if (!outputSegmentation)
            return false;

        ssc::Transform3D rMd_i = image->get_rMd(); //transform from the volumes coordinate system to our reference coordinate system
        outputSegmentation->get_rMd_History()->setRegistration(rMd_i);
        ssc::dataManager()->loadData(outputSegmentation);
        ssc::dataManager()->saveImage(outputSegmentation, patientService()->getPatientData()->getActivePatientFolder());

        //add contour internally to cx
        ssc::MeshPtr contour = ContourFilter::postProcess(rawContour, image, QColor("blue"));
        contour->get_rMd_History()->setRegistration(rMd_i);

        //set output
        mOutputTypes[2]->setValue(outputSegmentation->getUid());
        mOutputTypes[3]->setValue(contour->getUid());

        return true;
    } catch(SIPL::SIPLException &e) {
        return false;
    } catch(cl::Error &e) {
        return false;
    } catch(...) {
        return false;
    }
}


bool LevelSetFilter::postProcess() {

    // TODO: create contour of segmentation

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
	temp->setHelp("Output segmented binary image");
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

vtkImageDataPtr LevelSetFilter::convertToVtkImageData(char * data, int size_x, int size_y, int size_z, ssc::ImagePtr input)
{
	vtkImageDataPtr retval = this->importRawImageData((void*) data, size_x, size_y, size_z, input, VTK_UNSIGNED_CHAR);
	return retval;
}

//From vtkType.h (on Ubuntu 12.04)
//#define VTK_VOID            0
//#define VTK_BIT             1
//#define VTK_CHAR            2
//#define VTK_SIGNED_CHAR    15
//#define VTK_UNSIGNED_CHAR   3
//#define VTK_SHORT           4
//#define VTK_UNSIGNED_SHORT  5
//#define VTK_INT             6
//#define VTK_UNSIGNED_INT    7
//#define VTK_LONG            8
//#define VTK_UNSIGNED_LONG   9
//#define VTK_FLOAT          10
//#define VTK_DOUBLE         11
//#define VTK_ID_TYPE        12
vtkImageDataPtr LevelSetFilter::importRawImageData(void * data, int size_x, int size_y, int size_z, ssc::ImagePtr input, int type)
{
	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	imageImport->SetWholeExtent(0, size_x - 1, 0, size_y - 1, 0, size_z - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetDataScalarType(type);
	imageImport->SetNumberOfScalarComponents(1);
	imageImport->SetDataSpacing(input->getBaseVtkImageData()->GetSpacing());
	imageImport->SetImportVoidPointer(data);
	imageImport->GetOutput()->Update();
	imageImport->Modified();

	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(imageImport->GetOutput());

	return retval;
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

#endif
