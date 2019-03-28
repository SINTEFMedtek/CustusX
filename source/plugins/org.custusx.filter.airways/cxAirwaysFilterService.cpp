/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxAirwaysFilterService.h"

#include <QTimer>

#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <ctkPluginContext.h>
#include <vtkImplicitModeller.h>
#include <vtkContourFilter.h>
#include "cxBranchList.h"
#include "cxBronchoscopyRegistration.h"
#include "cxAirwaysFromCenterline.h"

#include "cxTime.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxDoubleProperty.h"
#include "cxContourFilter.h"
#include "cxImage.h"
#include "cxDataLocations.h"
#include "cxSelectDataStringProperty.h"
#include "vtkForwardDeclarations.h"
#include "cxPatientModelServiceProxy.h"
#include "cxVisServices.h"
#include "cxUtilHelpers.h"
#include "FAST/Algorithms/LungSegmentation/LungSegmentation.hpp"
#include "FAST/Algorithms/AirwaySegmentation/AirwaySegmentation.hpp"
#include "FAST/Algorithms/CenterlineExtraction/CenterlineExtraction.hpp"
#include "FAST/Importers/ImageFileImporter.hpp"
#include "FAST/Exporters/VTKImageExporter.hpp"
#include "FAST/Exporters/VTKMeshExporter.hpp"
#include "FAST/Data/Segmentation.hpp"
#include "FAST/SceneGraph.hpp"
#include "FAST/Exporters/VTKMeshFileExporter.hpp" //debug

namespace cx {

AirwaysFilter::AirwaysFilter(VisServicesPtr services) :
	FilterImpl(services),
	mDefaultStraightCLTubesOption(false)
{
	fast::Reporter::setGlobalReportMethod(fast::Reporter::COUT);
    //Need to create OpenGL context of fast in main thread, this is done in the constructor of DeviceManger
    fast::ImageFileImporter::pointer importer = fast::ImageFileImporter::New();
    Q_UNUSED(importer)
}


AirwaysFilter::~AirwaysFilter() {
}

QString AirwaysFilter::getName() const
{
	return "Airway Segmentation Filter";
}

QString AirwaysFilter::getType() const
{
	return "airways_filter";
}

QString AirwaysFilter::getHelp() const
{
	return "<html>"
	        "<h3>Airway Segmentation.</h3>"
	        "<p><i>Extracts segmentation and centerline from a CT volume. If method fails, try to crop volume. </br>Algorithm written by Erik Smistad.</i></p>"
           "</html>";
}

QString AirwaysFilter::getNameSuffixCenterline()
{
    return "_centerline";
}

QString AirwaysFilter::getNameSuffixStraight()
{
	return "_straight";
}

QString AirwaysFilter::getNameSuffixTubes()
{
	return "_tubes";
}

Vector3D AirwaysFilter::getSeedPointFromTool(SpaceProviderPtr spaceProvider, DataPtr data)
{
	// Retrieve position of tooltip and use it as seed point
	Vector3D point = spaceProvider->getActiveToolTipPoint(
			spaceProvider->getD(data));

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

bool AirwaysFilter::isSeedPointInsideImage(Vector3D seedPoint, DataPtr image)
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

bool AirwaysFilter::preProcess()
{
    DataPtr inputImage = mInputTypes[0].get()->getData();
	if (!inputImage)
	{
		CX_LOG_ERROR() << "No input data selected";
		return false;
	}

	if (inputImage->getType() != Image::getTypeName())
	{
		CX_LOG_ERROR() << "Input data has to be an image";
		return false;
	}

	std::string filename = (patientService()->getActivePatientFolder()
			+ "/" + inputImage->getFilename()).toStdString();

    // only check seed point inside image if use seed point is checked
	bool useManualSeedPoint = getManualSeedPointOption(mOptions)->getValue();
	if(useManualSeedPoint)
	{
		seedPoint = getSeedPointFromTool(mServices->spaceProvider(), inputImage);
		if(!isSeedPointInsideImage(seedPoint, inputImage)) {
			CX_LOG_ERROR() << "Seed point is not inside image. Use cursor to set seed point inside trachea in the CT image.";
			return false;
		}
	}
	mInputImage = patientService()->getData<Image>(inputImage->getUid());

	return true;
}

bool AirwaysFilter::execute()
{

    CX_LOG_INFO() << "EXECUTING AIRWAYS FILTER";
	// Check if pre process went ok:
    if(!mInputImage)
		return false;

    QString q_filename = "";
    QString activePatienFolder = patientService()->getActivePatientFolder();
    QString inputImageFileName = mInputImage->getFilename();
    if(!activePatienFolder.isEmpty())
        q_filename = activePatienFolder+"/"+inputImageFileName;
    else
        q_filename = inputImageFileName;

    std::string filename = q_filename.toStdString();
    bool useManualSeedPoint = getManualSeedPointOption(mOptions)->getValue();
	try {
		fast::Config::getTestDataPath(); // needed for initialization
        QString cacheDir = cx::DataLocations::getCachePath();
        fast::Config::setKernelBinaryPath(cacheDir.toStdString());
		QString kernelDir = cx::DataLocations::findConfigFolder("/FAST", FAST_SOURCE_DIR);
		fast::Config::setKernelSourcePath(kernelDir.toStdString());

        // Import image data from disk
		fast::ImageFileImporter::pointer importer = fast::ImageFileImporter::New();
		importer->setFilename(filename);

	    // Need to know the data type
        //importer->update(0);
        //fast::Image::pointer image = importer->getOutputData<fast::Image>();

        // Do segmentation
       // fast::Segmentation::pointer segmentationData;
		//bool doLungSegmentation = getLungSegmentationOption(mOptions)->getValue();


		fast::AirwaySegmentation::pointer airwaySegmentationPtr = fast::AirwaySegmentation::New();

		airwaySegmentationPtr->setInputConnection(importer->getOutputPort());
		airwaySegmentationPtr->setSmoothing(0.5);
		if(useManualSeedPoint) {
			CX_LOG_INFO() << "Using seed point: " << seedPoint.transpose();
			airwaySegmentationPtr->setSeedPoint(seedPoint(0), seedPoint(1), seedPoint(2));
		}
		auto segPort = airwaySegmentationPtr->getOutputPort();

		// Convert fast segmentation data to VTK data which CX can use
		vtkSmartPointer<fast::VTKImageExporter> vtkExporter = fast::VTKImageExporter::New();
		vtkExporter->setInputConnection(airwaySegmentationPtr->getOutputPort());
		vtkExporter->Update();
		mAirwaySegmentationOutput = vtkExporter->GetOutput();

		auto airwaySegmentationData = segPort->getNextFrame<fast::SpatialDataObject>();

		CX_LOG_SUCCESS() << "FINISHED AIRWAY SEGMENTATION";

	    // Get the transformation of the segmentation
        Eigen::Affine3f T = fast::SceneGraph::getEigenAffineTransformationFromData(airwaySegmentationData);
        mTransformation.matrix() = T.matrix().cast<double>(); // cast to double

        // Extract centerline
        fast::CenterlineExtraction::pointer centerline = fast::CenterlineExtraction::New();
        centerline->setInputData(airwaySegmentationData);
        // Get centerline
	    vtkSmartPointer<fast::VTKMeshExporter> vtkCenterlineExporter = fast::VTKMeshExporter::New();
	    vtkCenterlineExporter->setInputConnection(centerline->getOutputPort());
	    vtkCenterlineExporter->Update();
	    mCenterlineOutput = vtkCenterlineExporter->GetOutput();

	} catch(fast::Exception& e) {
		std::string error = e.what();
		reportError("fast::Exception: "+qstring_cast(error));
		if(!useManualSeedPoint)
			CX_LOG_ERROR() << "Try to set the seed point manually.";

		return false;

	} catch(cl::Error& e) {
		reportError("cl::Error:"+qstring_cast(e.what()));

		return false;

	} catch (std::exception& e){
		reportError("std::exception:"+qstring_cast(e.what()));
		std::cout << "DEBUG std::exception" << std::endl;  //debug
		return false;

	} catch (...){
		reportError("Airway segmentation algorithm threw a unknown exception.");

		return false;
    }

 	return true;
}

bool AirwaysFilter::postProcess()
{
	if(!mAirwaySegmentationOutput)
		return false;

	std::cout << "POST PROCESS" << std::endl;

	// Make contour of segmented volume
	double threshold = 1; /// because the segmented image is 0..1
	vtkPolyDataPtr rawContour = ContourFilter::execute(
			mAirwaySegmentationOutput,
			threshold,
			false, // reduce resolution
			true, // smoothing
			true, // keep topology
			0 // target decimation
	);
	//outputSegmentation->get_rMd_History()->setRegistration(rMd_i);
	//patientService()->insertData(outputSegmentation);

	// Add contour internally to cx
	MeshPtr contour = ContourFilter::postProcess(
			patientService(),
			rawContour,
			mInputImage,
			QColor("green")
	);
	contour->get_rMd_History()->setRegistration(mTransformation);

	// Set output
	mOutputTypes[1]->setValue(contour->getUid());

//	if(getLungSegmentationOption(mOptions)->getValue()) {
//		vtkPolyDataPtr rawContour = ContourFilter::execute(
//				mLungSegmentationOutput,
//				threshold,
//				false, // reduce resolution
//				true, // smoothing
//				true, // keep topology
//				0 // target decimation
//		);
		//outputSegmentation->get_rMd_History()->setRegistration(rMd_i);
		//patientService()->insertData(outputSegmentation);

		// Add contour internally to cx
//		QColor color("red");
//		color.setAlpha(100);
//		MeshPtr contour = ContourFilter::postProcess(
//				patientService(),
//				rawContour,
//				mInputImage,
//                color
//		);
//		contour->get_rMd_History()->setRegistration(mTransformation);
//
//		// Set output
//		mOutputTypes[2]->setValue(contour->getUid());
//	}

    // Centerline
    QString uid = mInputImage->getUid() + AirwaysFilter::getNameSuffixCenterline() + "%1";
    QString name = mInputImage->getName() + AirwaysFilter::getNameSuffixCenterline() + "%1";
	MeshPtr centerline = patientService()->createSpecificData<Mesh>(uid, name);
	centerline->setVtkPolyData(mCenterlineOutput);
	centerline->get_rMd_History()->setParentSpace(mInputImage->getUid());
	centerline->get_rMd_History()->setRegistration(mTransformation);
	patientService()->insertData(centerline);
	mOutputTypes[0]->setValue(centerline->getUid());

    this->createAirwaysFromCenterline();

	return true;
}


void AirwaysFilter::createAirwaysFromCenterline()
{
    AirwaysFromCenterlinePtr airwaysFromCLPtr = AirwaysFromCenterlinePtr(new AirwaysFromCenterline());

    airwaysFromCLPtr->processCenterline(mCenterlineOutput);

    // Create the mesh object from the airway walls
    QString uidMesh = mInputImage->getUid() + AirwaysFilter::getNameSuffixTubes() + "%1";
    QString nameMesh = mInputImage->getName()+ AirwaysFilter::getNameSuffixTubes() + "%1";
    MeshPtr airwayWalls = patientService()->createSpecificData<Mesh>(uidMesh, nameMesh);
    airwayWalls->setVtkPolyData(airwaysFromCLPtr->generateTubes());
    airwayWalls->get_rMd_History()->setParentSpace(mInputImage->getUid());
    airwayWalls->get_rMd_History()->setRegistration(mTransformation);
    airwayWalls->setColor(QColor(253, 173, 136, 255));
    patientService()->insertData(airwayWalls);
    mOutputTypes[4]->setValue(airwayWalls->getUid());


    //insert filtered centerline from airwaysFromCenterline
    QString uidCenterline = mInputImage->getUid() + AirwaysFilter::getNameSuffixTubes() + AirwaysFilter::getNameSuffixCenterline() + "%1";
    QString nameCenterline = mInputImage->getName() + AirwaysFilter::getNameSuffixTubes() + AirwaysFilter::getNameSuffixCenterline() + "%1";
    MeshPtr centerline = patientService()->createSpecificData<Mesh>(uidCenterline, nameCenterline);
    centerline->setVtkPolyData(airwaysFromCLPtr->getVTKPoints());
    centerline->get_rMd_History()->setParentSpace(mInputImage->getUid());
    centerline->get_rMd_History()->setRegistration(mTransformation);
    patientService()->insertData(centerline);
    mOutputTypes[3]->setValue(centerline->getUid());
}

void AirwaysFilter::setDefaultStraightCLTubesOption(bool defaultStraightCLTubesOption)
{
	mDefaultStraightCLTubesOption = defaultStraightCLTubesOption;
}

void AirwaysFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getManualSeedPointOption(mOptions));
	mOptionsAdapters.push_back(this->getLungSegmentationOption(mOptions));
	mOptionsAdapters.push_back(this->getVesselSegmentationOption(mOptions));
}

void AirwaysFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(patientService());
	temp->setValueName("Input");
	temp->setHelp("Select input to run airway segmentation on.");
	mInputTypes.push_back(temp);
}

void AirwaysFilter::createOutputTypes()
{
	StringPropertySelectMeshPtr tempMeshStringAdapter;
	std::vector<std::pair<QString, QString>> valueHelpPairs;
	valueHelpPairs.push_back(std::make_pair(tr("Airway Centerline"), tr("Generated centerline mesh (vtk-format).")));
	valueHelpPairs.push_back(std::make_pair(tr("Airway Segmentation"), tr("Generated surface of the airway segmentation volume.")));
	valueHelpPairs.push_back(std::make_pair(tr("Lung Segmentation"), tr("Generated surface of the lung segmentation volume.")));
	valueHelpPairs.push_back(std::make_pair(tr("Straight Airway Centerline"), tr("A centerline with straight lines between the branch points.")));
	valueHelpPairs.push_back(std::make_pair(tr("Straight Airway Tubes"), tr("Tubes based on the straight centerline")));

	foreach(auto pair, valueHelpPairs)
	{
		tempMeshStringAdapter = StringPropertySelectMesh::New(patientService());
		tempMeshStringAdapter->setValueName(pair.first);
		tempMeshStringAdapter->setHelp(pair.second);
		mOutputTypes.push_back(tempMeshStringAdapter);
	}
}


BoolPropertyPtr AirwaysFilter::getManualSeedPointOption(QDomElement root)
{
	BoolPropertyPtr retval =
			BoolProperty::initialize("Use manual seed point",
					"",
					"If the automatic seed point detection algorithm fails you can use cursor to set the seed point "
                    "inside trachea of the patient. "
                    "Then tick this checkbox to use the manual seed point in the airways filter.",
					false, root);
	return retval;

}

BoolPropertyPtr AirwaysFilter::getLungSegmentationOption(QDomElement root)
{
	BoolPropertyPtr retval =
			BoolProperty::initialize("Lung segmentation",
					"",
					"Selecting this option will also segment the two lung sacs",
					false, root);
	return retval;

}

BoolPropertyPtr AirwaysFilter::getVesselSegmentationOption(QDomElement root)
{
	BoolPropertyPtr retval =
			BoolProperty::initialize("Vessel segmentation",
					"",
					"Selecting this option will also segment the blood vessels in the lungs",
					false, root);
	return retval;

}

} /* namespace cx */

