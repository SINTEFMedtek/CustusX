#include "cxLevelSetFilter.h"

#ifdef CX_USE_LEVEL_SET

#include "cxTime.h"
#include "cxMesh.h"
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxRegistrationTransform.h"
#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxData.h"
#include "cxImage.h"
#include "cxDataManager.h"
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include "cxContourFilter.h"
#include "cxDataLocations.h"
#include "cxPatientService.h"
#include "cxPatientData.h"

#include <itkBinaryMorphologicalClosingImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include "cxAlgorithmHelpers.h"
#include <vtkImageCast.h>

#include "levelSet.hpp"
#include "OpenCLManager.hpp"
#include "HelperFunctions.hpp"
#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"

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

Vector3D LevelSetFilter::getSeedPointFromTool(DataPtr data)
{
	// Retrieve position of tooltip and use it as seed point
	Vector3D point = spaceProvider()->getDominantToolTipPoint(
			spaceProvider()->getD(data));

	// Have to multiply by the inverse of the spacing to get the voxel position
	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	double spacingX, spacingY, spacingZ;
	image->getBaseVtkImageData()->GetSpacing(spacingX, spacingY, spacingZ);
	point(0) = point(0) * (1.0 / spacingX);
	point(1) = point(1) * (1.0 / spacingY);
	point(2) = point(2) * (1.0 / spacingZ);

	std::cout << "the selected seed point is: " << point(0) << " " << point(1)
			<< " " << point(2) << "\n";

	return point;
}

int * getImageSize(DataPtr inputImage)
{
	ImagePtr image = boost::dynamic_pointer_cast<Image>(inputImage);
	return image->getBaseVtkImageData()->GetDimensions();
}

bool LevelSetFilter::isSeedPointInsideImage(Vector3D seedPoint, DataPtr image)
{
	int * size = getImageSize(image);
	std::cout << "size of image is: " << size[0] << " " << size[1] << " "
			<< size[2] << "\n";
	int x = (int) seedPoint(0);
	int y = (int) seedPoint(1);
	int z = (int) seedPoint(2);
	bool result = x >= 0 && y >= 0 && z >= 0 && x < size[0] && y < size[1]
			&& z < size[2];
	return result;
}

bool LevelSetFilter::preProcess()
{
	DataPtr inputImage = mInputTypes[0].get()->getData();
	if (!inputImage)
	{
		std::cout << "No input data selected" << std::endl;
		return false;
	}

	if (inputImage->getType() != "image")
	{
		std::cout << "Input data has to be an image" << std::endl;
		return false;
	}

	filename = (patientService()->getPatientData()->getActivePatientFolder()
			+ "/" + inputImage->getFilename()).toStdString();

	seedPoint = getSeedPointFromTool(inputImage);
	if (!isSeedPointInsideImage(seedPoint, inputImage))
	{
		std::cout << "Seed point is not inside image!" << std::endl;
		return false;
	}
	image = dataManager()->getImage(inputImage->getUid());

	return true;
}

bool LevelSetFilter::execute()
{
	DataPtr inputImage = mInputTypes[0].get()->getData();

	float threshold = getThresholdOption(mOptions)->getValue();
	float epsilon = getEpsilonOption(mOptions)->getValue();
	float alpha = getAlphaOption(mOptions)->getValue();

	std::cout << "Parameters are set to: " << threshold << " " << epsilon << " "
			<< alpha << std::endl;

	// Run level set segmentation
	SIPL::int3 seed(seedPoint(0), seedPoint(1), seedPoint(2));
	try
	{
		SIPL::Volume<char> * result = runLevelSet(filename.c_str(), seed, 10, // seed radius
				1000, // iterations per narrow band
				threshold, epsilon, alpha);
		SIPL::int3 size = result->getSize();
		rawSegmentation = this->convertToVtkImageData(
				(char *) result->getData(), size.x, size.y, size.z, image);
		delete result;

		return true;
	} catch (SIPL::SIPLException &e)
	{
		std::string error = e.what();
		reportError(
				"SIPL::SIPLException: " + qstring_cast(error));

		return false;
	} catch (cl::Error &e)
	{
		if (e.err() == CL_MEM_OBJECT_ALLOCATION_FAILURE
				|| e.err() == CL_OUT_OF_RESOURCES)
		{
			reportError(
					"cl::Error: Not enough memory on the device to process this image. ("
							+ qstring_cast(oul::getCLErrorString(e.err()))
							+ ")");
		}
		else
		{
			reportError(
					"cl::Error: "
							+ qstring_cast(oul::getCLErrorString(e.err())));
		}
		return false;
	} catch (...)
	{
		reportError("Unknown exception occured.");
		return false;
	}
}

bool LevelSetFilter::postProcess()
{
	if(!rawSegmentation)
		return false;

	//add segmentation internally to cx
	QString uidSegmentation = image->getUid() + "_seg%1";
	QString nameSegmentation = image->getName() + "_seg%1";
	ImagePtr outputSegmentation2 = dataManager()->createDerivedImage(
			rawSegmentation, uidSegmentation, nameSegmentation, image);
	ImagePtr outputSegmentation;
	if (!outputSegmentation2)
		return false;

	float radius = getRadiusOption(mOptions)->getValue();
	if (radius > 0)
	{
		std::cout << "Performing morphological closing on segmentation result"
				<< std::endl;

		// Convert radius in mm to radius in voxels for the structuring element
		Eigen::Array3d spacing = image->getSpacing();
		itk::Size<3> radiusInVoxels;
		radiusInVoxels[0] = radius / spacing(0);
		radiusInVoxels[1] = radius / spacing(1);
		radiusInVoxels[2] = radius / spacing(2);

		itkImageType::ConstPointer itkImage =
				AlgorithmHelper::getITKfromSSCImage(outputSegmentation2);

		// Create structuring element
		typedef itk::BinaryBallStructuringElement<unsigned char, 3> StructuringElementType;
		StructuringElementType structuringElement;
		structuringElement.SetRadius(radiusInVoxels);
		structuringElement.CreateStructuringElement();

		// Morphological closing
		typedef itk::BinaryMorphologicalClosingImageFilter<itkImageType,
				itkImageType, StructuringElementType> closingFilterType;
		closingFilterType::Pointer closingFilter = closingFilterType::New();
		closingFilter->SetInput(itkImage);
		closingFilter->SetKernel(structuringElement);
		closingFilter->SetForegroundValue(1);
		closingFilter->Update();
		itkImage = closingFilter->GetOutput();

		//Convert ITK to VTK
		itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
		itkToVtkFilter->SetInput(itkImage);
		itkToVtkFilter->Update();

		vtkImageDataPtr rawResult = vtkImageDataPtr::New();
		rawResult->DeepCopy(itkToVtkFilter->GetOutput());

		vtkImageCastPtr imageCast = vtkImageCastPtr::New();
		imageCast->SetInputData(rawResult);
		imageCast->SetOutputScalarTypeToUnsignedChar();
		rawResult = imageCast->GetOutput();

		outputSegmentation = dataManager()->createDerivedImage(rawResult,
				uidSegmentation, nameSegmentation, image);
		rawSegmentation = rawResult;
	}
	else
	{
		outputSegmentation = outputSegmentation2;
	}

	//make contour of segmented volume
	double threshold = 1; /// because the segmented image is 0..1
	vtkPolyDataPtr rawContour = ContourFilter::execute(rawSegmentation,
			threshold);
	Transform3D rMd_i = image->get_rMd(); //transform from the volumes coordinate system to our reference coordinate system
	outputSegmentation->get_rMd_History()->setRegistration(rMd_i);
	dataManager()->loadData(outputSegmentation);
	dataManager()->saveImage(outputSegmentation,
			patientService()->getPatientData()->getActivePatientFolder());

	//add contour internally to cx
	MeshPtr contour = ContourFilter::postProcess(rawContour, image,
			QColor("blue"));
	contour->get_rMd_History()->setRegistration(rMd_i);

	//set output
	mOutputTypes[0]->setValue(outputSegmentation->getUid());
	mOutputTypes[1]->setValue(contour->getUid());

	return true;
}

void LevelSetFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getThresholdOption(mOptions));
	mOptionsAdapters.push_back(this->getEpsilonOption(mOptions));
	mOptionsAdapters.push_back(this->getAlphaOption(mOptions));
	mOptionsAdapters.push_back(this->getRadiusOption(mOptions));
}

QDomElement LevelSetFilter::getmOptions()
{
	return this->mOptions;
}

void LevelSetFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select image input for thresholding");
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

vtkImageDataPtr LevelSetFilter::convertToVtkImageData(char * data, int size_x,
		int size_y, int size_z, ImagePtr input)
{
	vtkImageDataPtr retval = this->importRawImageData((void*) data, size_x,
			size_y, size_z, input, VTK_UNSIGNED_CHAR);
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
vtkImageDataPtr LevelSetFilter::importRawImageData(void * data, int size_x,
		int size_y, int size_z, ImagePtr input, int type)
{
	vtkImageImportPtr imageImport = vtkImageImportPtr::New();

	imageImport->SetWholeExtent(0, size_x - 1, 0, size_y - 1, 0, size_z - 1);
	imageImport->SetDataExtentToWholeExtent();
	imageImport->SetDataScalarType(type);
	imageImport->SetNumberOfScalarComponents(1);
	imageImport->SetDataSpacing(input->getBaseVtkImageData()->GetSpacing());
	imageImport->SetImportVoidPointer(data);
	imageImport->Update();
	imageImport->Modified();

	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(imageImport->GetOutput());

	return retval;
}

DoubleDataAdapterXmlPtr LevelSetFilter::getThresholdOption(QDomElement root)
{
	DoubleDataAdapterXmlPtr retval = DoubleDataAdapterXml::initialize(
			"Threshold", "", "Select threshold for the segmentation", 1,
			DoubleRange(-5000, 5000, 0.0000001), 0, root);
	return retval;

}

DoubleDataAdapterXmlPtr LevelSetFilter::getEpsilonOption(QDomElement root)
{
	DoubleDataAdapterXmlPtr retval = DoubleDataAdapterXml::initialize("Epsilon",
			"", "Select epsilon for the segmentation", 1,
			DoubleRange(-5000, 5000, 0.0000001), 0, root);
	return retval;

}

DoubleDataAdapterXmlPtr LevelSetFilter::getAlphaOption(QDomElement root)
{
	DoubleDataAdapterXmlPtr retval = DoubleDataAdapterXml::initialize("Alpha",
			"", "Select alpha for the segmentation", 0.1,
			DoubleRange(0, 1, 0.01), 2, root);
	retval->setGuiRepresentation(DoubleDataAdapter::grSLIDER);
	return retval;

}

DoubleDataAdapterXmlPtr LevelSetFilter::getRadiusOption(QDomElement root)
{
	DoubleDataAdapterXmlPtr retval =
			DoubleDataAdapterXml::initialize("Radius for morphological closing",
					"",
					"Select radius (in mm) for the morphological closing of the final result. Radius at 0 will skip this process.",
					0.0, DoubleRange(0, 20, 0.5), 2, root);
	retval->setGuiRepresentation(DoubleDataAdapter::grSLIDER);
	return retval;

}

} // end namespace cx

#endif
