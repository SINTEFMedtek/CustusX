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
#include "cxColorVariationFilter.h"
#include "cxVolumeHelpers.h"

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

namespace cx {

AirwaysFilter::AirwaysFilter(VisServicesPtr services) :
	FilterImpl(services)
{
	fast::Reporter::setGlobalReportMethod(fast::Reporter::COUT);
	//Need to create OpenGL context of fast in main thread, this is done in the constructor of DeviceManger
	//This hack sometimes cause CTK plugin start/stop to fail - if needed it must be solved in another way
	//fast::ImageFileImporter::pointer importer = fast::ImageFileImporter::New();
	//Q_UNUSED(importer)
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
	return  "<html>"
			"<h3>Airway Segmentation.</h3>"
			"<p><i>Extracts airways and blood vessels, including centerlines, and lungs from a CT volume. If method fails, try to crop volume. </br>Algorithm written by Erik Smistad.</i></p>"
			"</html>";
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
	bool useManualSeedPoint = mManualSeedPointOption->getValue();
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

	try {
		fast::Config::getTestDataPath(); // needed for initialization
		QString cacheDir = cx::DataLocations::getCachePath();
		fast::Config::setKernelBinaryPath(cacheDir.toStdString());
		QString kernelDir = cx::DataLocations::findConfigFolder("/FAST", FAST_SOURCE_DIR);
		fast::Config::setKernelSourcePath(kernelDir.toStdString());


	} catch(fast::Exception& e) {
		std::string error = e.what();
		reportError("fast::Exception: "+qstring_cast(error));

		return false;

	} catch (...){
		reportError("Airway segmentation algorithm threw a unknown exception.");

		return false;
	}

	bool doAirwaySegmentation = mAirwaySegmentationOption->getValue();
	bool doLungSegmentation = mLungSegmentationOption->getValue();
	bool doVesselSegmentation = mVesselSegmentationOption->getValue();

	if (doAirwaySegmentation)
	{
		std::string volumeFilname = q_filename.toStdString();
		// Import image data from disk
		fast::ImageFileImporter::pointer importerPtr = fast::ImageFileImporter::New();//TODO: This line cause an exception
		importerPtr->setFilename(volumeFilname);

		segmentAirways(importerPtr);
	}

	if (doLungSegmentation)
	{
		std::string volumeFilname = q_filename.toStdString();
		// Import image data from disk
		fast::ImageFileImporter::pointer importerPtr = fast::ImageFileImporter::New();
		importerPtr->setFilename(volumeFilname);

		segmentLungs(importerPtr);
	}

	if (doVesselSegmentation)
	{
		std::string volumeFilname = q_filename.toStdString();
		// Import image data from disk
		fast::ImageFileImporter::pointer importerPtr = fast::ImageFileImporter::New();
		importerPtr->setFilename(volumeFilname);

		segmentVessels(importerPtr);
	}

	return true;
}

void AirwaysFilter::segmentAirways(fast::ImageFileImporter::pointer importerPtr)
{

	bool useManualSeedPoint = mManualSeedPointOption->getValue();


	// Do segmentation
	fast::AirwaySegmentation::pointer airwaySegmentationPtr = fast::AirwaySegmentation::New();

	airwaySegmentationPtr->setInputConnection(importerPtr->getOutputPort());

	if(useManualSeedPoint) {
		CX_LOG_INFO() << "Using seed point: " << seedPoint.transpose();
		airwaySegmentationPtr->setSeedPoint(seedPoint(0), seedPoint(1), seedPoint(2));
	}

	extractAirways(airwaySegmentationPtr);
}

bool AirwaysFilter::extractAirways(fast::AirwaySegmentation::pointer airwaySegmentationPtr)
{

	try{
		auto segPort = airwaySegmentationPtr->getOutputPort();

		// Convert fast segmentation data to VTK data which CX can use
		vtkSmartPointer<fast::VTKImageExporter> vtkExporter = fast::VTKImageExporter::New();
		vtkExporter->setInputConnection(airwaySegmentationPtr->getOutputPort());
		vtkExporter->Update();
		mAirwaySegmentationOutput = vtkExporter->GetOutput();
		vtkExporter->Delete();

		auto airwaySegmentationData = segPort->getNextFrame<fast::SpatialDataObject>();

		// Extract centerline
		fast::CenterlineExtraction::pointer centerline = fast::CenterlineExtraction::New();
		centerline->setInputData(airwaySegmentationData);
		// Get centerline
		vtkSmartPointer<fast::VTKMeshExporter> vtkCenterlineExporter = fast::VTKMeshExporter::New();
		vtkCenterlineExporter->setInputConnection(centerline->getOutputPort());
		vtkCenterlineExporter->Update();
		mAirwayCenterlineOutput = vtkCenterlineExporter->GetOutput();
		vtkCenterlineExporter->Delete();

} catch(fast::Exception& e) {
	std::string error = e.what();
	reportError("fast::Exception: "+qstring_cast(error));
	if(!mManualSeedPointOption->getValue())
		CX_LOG_ERROR() << "Try to set the seed point manually.";

	return false;

} catch(cl::Error& e) {
	reportError("cl::Error:"+qstring_cast(e.what()));

	return false;

} catch (std::exception& e){
	reportError("std::exception:"+qstring_cast(e.what()));
	return false;

} catch (...){
	reportError("Airway segmentation algorithm threw a unknown exception.");

	return false;
}

	return true;

}

void AirwaysFilter::segmentLungs(fast::ImageFileImporter::pointer importerPtr)
{

	bool useManualSeedPoint = mManualSeedPointOption->getValue();

	// Do segmentation
	fast::LungSegmentation::pointer lungSegmentationPtr = fast::LungSegmentation::New();
	lungSegmentationPtr->setInputConnection(importerPtr->getOutputPort());

	if(useManualSeedPoint) {
		CX_LOG_INFO() << "Using seed point: " << seedPoint.transpose();
		lungSegmentationPtr->setAirwaySeedPoint(seedPoint(0), seedPoint(1), seedPoint(2));
	}

	extractLungs(lungSegmentationPtr);
}

void AirwaysFilter::segmentVessels(fast::ImageFileImporter::pointer importerPtr)
{

	bool useManualSeedPoint = mManualSeedPointOption->getValue();

	// Do segmentation
	fast::LungSegmentation::pointer lungSegmentationPtr = fast::LungSegmentation::New();
	lungSegmentationPtr->setInputConnection(importerPtr->getOutputPort());

	if(useManualSeedPoint) {
		CX_LOG_INFO() << "Using seed point: " << seedPoint.transpose();
		lungSegmentationPtr->setAirwaySeedPoint(seedPoint(0), seedPoint(1), seedPoint(2));
	}

	extractBloodVessels(lungSegmentationPtr);
}

bool AirwaysFilter::extractBloodVessels(fast::LungSegmentation::pointer lungSegmentationPtr)
{
	try{
		auto segPortBloodVessels = lungSegmentationPtr->getBloodVesselOutputPort();

		// Convert fast segmentation data to VTK data which CX can use
		vtkSmartPointer<fast::VTKImageExporter> vtkBloodVesselExporter = fast::VTKImageExporter::New();
		vtkBloodVesselExporter->setInputConnection(lungSegmentationPtr->getBloodVesselOutputPort());
		vtkBloodVesselExporter->Update();
		mBloodVesselSegmentationOutput = vtkBloodVesselExporter->GetOutput();
		vtkBloodVesselExporter->Delete();

		bool generateVesselCenterlines = mVesselCenterlineOption->getValue();
		if (generateVesselCenterlines)
		{
			auto bloodVesselSegmentationData = segPortBloodVessels->getNextFrame<fast::SpatialDataObject>();
			// Extract centerline
			fast::CenterlineExtraction::pointer bloodVesselCenterline = fast::CenterlineExtraction::New();
			bloodVesselCenterline->setInputData(bloodVesselSegmentationData);

			// Get centerline
			vtkSmartPointer<fast::VTKMeshExporter> vtkBloodVesselCenterlineExporter = fast::VTKMeshExporter::New();
			vtkBloodVesselCenterlineExporter->setInputConnection(bloodVesselCenterline->getOutputPort());
			vtkBloodVesselCenterlineExporter->Update();
			mBloodVesselCenterlineOutput = vtkBloodVesselCenterlineExporter->GetOutput();
			vtkBloodVesselCenterlineExporter->Delete();
		}

} catch(fast::Exception& e) {
	std::string error = e.what();
	reportError("In vessel segmentation fast::Exception: "+qstring_cast(error));
	if(!mManualSeedPointOption->getValue())
		CX_LOG_ERROR() << "Try to set the seed point manually.";

	return false;

} catch(cl::Error& e) {
	reportError("In vessel segmentation cl::Error:"+qstring_cast(e.what()));

	return false;

} catch (std::exception& e){
	reportError("In vessel segmentation std::exception:"+qstring_cast(e.what()));

	return false;

} catch (...){
	reportError("Vessel segmentation algorithm threw a unknown exception.");

	return false;
}

	return true;
}

bool AirwaysFilter::extractLungs(fast::LungSegmentation::pointer lungSegmentationPtr)
{
	try{
		// Convert fast segmentation data to VTK data which CX can use
		vtkSmartPointer<fast::VTKImageExporter> vtkLungExporter = fast::VTKImageExporter::New();
		vtkLungExporter->setInputConnection(lungSegmentationPtr->getOutputPort(0));
		vtkLungExporter->Update();
		mLungSegmentationOutput = vtkLungExporter->GetOutput();
		vtkLungExporter->Delete();

} catch(fast::Exception& e) {
	std::string error = e.what();
	reportError("In lung segmentation fast::Exception: "+qstring_cast(error));
	if(!mManualSeedPointOption->getValue())
		CX_LOG_ERROR() << "Try to set the seed point manually.";

	return false;

} catch(cl::Error& e) {
	reportError("In lung segmentation cl::Error:"+qstring_cast(e.what()));

	return false;

} catch (std::exception& e){
	reportError("In lung segmentation std::exception:"+qstring_cast(e.what()));

	return false;

} catch (...){
	reportError("Lung segmentation algorithm threw a unknown exception.");

	return false;
}

	return true;
}

bool AirwaysFilter::postProcess()
{
	std::cout << "POST PROCESS" << std::endl;

	if(mAirwaySegmentationOption->getValue())
	{
		postProcessAirways();
		mAirwaySegmentationOutput = NULL; //To avoid publishing old results if next segmentation fails
		mAirwayCenterlineOutput = NULL;
	}

	if(mLungSegmentationOption->getValue()) {
		postProcessLungs();
		mLungSegmentationOutput = NULL; //To avoid publishing old results if next segmentation fails
	}

	if(mVesselSegmentationOption->getValue())
	{
		postProcessVessels();
		mBloodVesselSegmentationOutput = NULL; //To avoid publishing old results if next segmentation fails
		mBloodVesselCenterlineOutput = NULL;
	}

	return true;
}

bool AirwaysFilter::postProcessAirways()
{
	if(!mAirwaySegmentationOutput)
		return false;

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

	//Create temporary ImagePtr for correct output name from contour filter
	QString uidOutput = mInputImage->getUid() + airwaysFilterGetNameSuffixAirways() + "%1";
	QString nameOutput = mInputImage->getName() + airwaysFilterGetNameSuffixAirways() + "%1";
	ImagePtr outputImage = patientService()->createSpecificData<Image>(uidOutput, nameOutput);
	// Add contour internally to cx
	MeshPtr contour = ContourFilter::postProcess(
			patientService(),
			rawContour,
			outputImage,
			QColor("green")
	);
	contour->get_rMd_History()->setRegistration(mInputImage->get_rMd());

	// Set output
	mOutputTypes[1]->setValue(contour->getUid());

	// Centerline
	QString uid = mInputImage->getUid() + airwaysFilterGetNameSuffixAirways() + airwaysFilterGetNameSuffixCenterline() + "%1";
	QString name = mInputImage->getName() + airwaysFilterGetNameSuffixAirways() + airwaysFilterGetNameSuffixCenterline() + "%1";
	MeshPtr airwaysCenterline = patientService()->createSpecificData<Mesh>(uid, name);
	airwaysCenterline->setVtkPolyData(mAirwayCenterlineOutput);
	airwaysCenterline->get_rMd_History()->setParentSpace(mInputImage->getUid());
	airwaysCenterline->get_rMd_History()->setRegistration(mInputImage->get_rMd());
	patientService()->insertData(airwaysCenterline);
	mOutputTypes[0]->setValue(airwaysCenterline->getUid());

	if(mAirwayTubesGenerationOption->getValue())
		this->createAirwaysFromCenterline();

	return true;
}

bool AirwaysFilter::postProcessLungs()
{
	if(!mLungSegmentationOutput)
		return false;

	double threshold = 1; /// because the segmented image is 0..1
	vtkPolyDataPtr rawContour = ContourFilter::execute(
			mLungSegmentationOutput,
			threshold,
			false, // reduce resolution
			true, // smoothing
			true, // keep topology
			0 // target decimation
	);

	//Create temporary ImagePtr for correct output name from contour filter
	QString uidOutput = mInputImage->getUid() + airwaysFilterGetNameSuffixLungs() + "%1";
	QString nameOutput = mInputImage->getName() + airwaysFilterGetNameSuffixLungs() + "%1";
	ImagePtr outputImage = patientService()->createSpecificData<Image>(uidOutput, nameOutput);

	 //Add contour internally to cx
	QColor color("red");
	color.setAlpha(100);
	MeshPtr contour = ContourFilter::postProcess(
			patientService(),
			rawContour,
			outputImage,
			color
	);

	contour->get_rMd_History()->setRegistration(mInputImage->get_rMd());

	// Set output
	mOutputTypes[4]->setValue(contour->getUid());

	return true;
}

bool AirwaysFilter::postProcessVessels()
{
	if(!mBloodVesselSegmentationOutput)
	{
		CX_LOG_WARNING() << "In AirwaysFilter::postProcessVessels(): No BloodVessel segmentation output created.";
		return false;
	}

	// Make contour of segmented volume
	double threshold = 1; /// because the segmented image is 0..1
	vtkPolyDataPtr rawContour = ContourFilter::execute(
			mBloodVesselSegmentationOutput,
			threshold,
			false, // reduce resolution
			true, // smoothing
			true, // keep topology
			0 // target decimation
	);

	//Create temporary ImagePtr for correct output name from contour filter
	QString uidOutput = mInputImage->getUid() + airwaysFilterGetNameSuffixLungVessels() + "%1";
	QString nameOutput = mInputImage->getName() + airwaysFilterGetNameSuffixLungVessels() + "%1";
	ImagePtr outputImage = patientService()->createSpecificData<Image>(uidOutput, nameOutput);

	// Add contour internally to cx(
	QColor color = QColor("blue");
	color.setAlpha(255);

	MeshPtr contour = ContourFilter::postProcess(
			patientService(),
			rawContour,
			outputImage,
				color
	);
	contour->get_rMd_History()->setRegistration(mInputImage->get_rMd());

	// Set output
	mOutputTypes[6]->setValue(contour->getUid());

	bool generateVesselCenterlines = mVesselCenterlineOption->getValue();
	if (generateVesselCenterlines)
	{
		// Centerline
		QString uid = mInputImage->getUid() + airwaysFilterGetNameSuffixLungVessels() + airwaysFilterGetNameSuffixCenterline() + "%1";
		QString name = mInputImage->getName() + airwaysFilterGetNameSuffixLungVessels() + airwaysFilterGetNameSuffixCenterline() + "%1";
		MeshPtr bloodVesselsCenterline = patientService()->createSpecificData<Mesh>(uid, name);
		bloodVesselsCenterline->setVtkPolyData(mBloodVesselCenterlineOutput);
		bloodVesselsCenterline->get_rMd_History()->setParentSpace(mInputImage->getUid());
		bloodVesselsCenterline->get_rMd_History()->setRegistration(mInputImage->get_rMd());
		bloodVesselsCenterline->setColor("blue");
		patientService()->insertData(bloodVesselsCenterline);
		mOutputTypes[5]->setValue(bloodVesselsCenterline->getUid());
	}

	bool generateVesselVolume = mVesselVolumeOption->getValue();
	if (generateVesselVolume)
	{
		//Create segmented volume output
		QString uidVolume = mInputImage->getUid() + airwaysFilterGetNameSuffixLungVessels() + airwaysFilterGetNameSuffixVolume() + "%1";
		QString nameVolume = mInputImage->getName() + airwaysFilterGetNameSuffixLungVessels() + airwaysFilterGetNameSuffixVolume() + "%1";
		ImagePtr outputVolume = createDerivedImage( mServices->patient(),
													uidVolume, nameVolume,
													mBloodVesselSegmentationOutput, mInputImage);
		outputVolume->mergevtkSettingsIntosscTransform();
		patientService()->insertData(outputVolume);
		mOutputTypes[7]->setValue(outputVolume->getUid());
	}

	return true;
}

void AirwaysFilter::createAirwaysFromCenterline()
{
	AirwaysFromCenterlinePtr airwaysFromCLPtr = AirwaysFromCenterlinePtr(new AirwaysFromCenterline());

	airwaysFromCLPtr->processCenterline(mAirwayCenterlineOutput);
	airwaysFromCLPtr->setSegmentedVolume(mAirwaySegmentationOutput);

	// Create the mesh object from the airway walls
	QString uidMesh = mInputImage->getUid() + airwaysFilterGetNameSuffixAirways() + airwaysFilterGetNameSuffixTubes() + "%1";
	QString nameMesh = mInputImage->getName() + airwaysFilterGetNameSuffixAirways() + airwaysFilterGetNameSuffixTubes() + "%1";
	MeshPtr airwayWalls = patientService()->createSpecificData<Mesh>(uidMesh, nameMesh);
	airwayWalls->setVtkPolyData(airwaysFromCLPtr->generateTubes(0, true));
	airwayWalls->get_rMd_History()->setParentSpace(mInputImage->getUid());
	airwayWalls->get_rMd_History()->setRegistration(mInputImage->get_rMd());
	airwayWalls->setColor(QColor(253, 173, 136, 255));
	patientService()->insertData(airwayWalls);
	mOutputTypes[3]->setValue(airwayWalls->getUid());
		
		if(mColoredAirwaysOption->getValue())
			this->createColoredAirways();

	//insert filtered centerline from airwaysFromCenterline
	QString uidCenterline = mInputImage->getUid() + airwaysFilterGetNameSuffixAirways() + airwaysFilterGetNameSuffixTubes() + airwaysFilterGetNameSuffixCenterline() + "%1";
	QString nameCenterline = mInputImage->getName() + airwaysFilterGetNameSuffixAirways() + airwaysFilterGetNameSuffixTubes() + airwaysFilterGetNameSuffixCenterline() + "%1";
	MeshPtr centerline = patientService()->createSpecificData<Mesh>(uidCenterline, nameCenterline);
	centerline->setVtkPolyData(airwaysFromCLPtr->getVTKPoints());
	centerline->get_rMd_History()->setParentSpace(mInputImage->getUid());
	centerline->get_rMd_History()->setRegistration(mInputImage->get_rMd());
	patientService()->insertData(centerline);
	mOutputTypes[2]->setValue(centerline->getUid());
}

void AirwaysFilter::createColoredAirways()
{
	if(mOutputTypes[3]->getValue().isEmpty())
		return;
	
	ColorVariationFilterPtr coloringFilter = ColorVariationFilterPtr(new ColorVariationFilter(mServices));
	MeshPtr mesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mOutputTypes[3])->getMesh();
	double globaleVariance = 50.0;
	double localeVariance = 5.0;
	int smoothingIterations = 5;
	
	MeshPtr coloredMesh = coloringFilter->execute(mesh, globaleVariance, localeVariance, smoothingIterations);
	if(coloredMesh)
		mOutputTypes[3]->setValue(coloredMesh->getUid());
}

void AirwaysFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getManualSeedPointOption(mOptions));
	mOptionsAdapters.push_back(this->getAirwaySegmentationOption(mOptions));
	mOptionsAdapters.push_back(this->getAirwayTubesGenerationOption(mOptions));
	mOptionsAdapters.push_back(this->getColoredAirwaysOption(mOptions));
	mOptionsAdapters.push_back(this->getLungSegmentationOption(mOptions));
	mOptionsAdapters.push_back(this->getVesselSegmentationOption(mOptions));
	mOptionsAdapters.push_back(this->getVesselCenterlineOption(mOptions));
	mOptionsAdapters.push_back(this->getVesselVolumeOption(mOptions));
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
	valueHelpPairs.push_back(std::make_pair(tr("Airway Segmentation Mesh"), tr("Generated surface of the airway segmentation volume.")));
	valueHelpPairs.push_back(std::make_pair(tr("Tube Airway Centerline"), tr("Smoothed centerline.")));
	valueHelpPairs.push_back(std::make_pair(tr("Tube Airway Tubes Mesh"), tr("Tubes based on the smoothed centerline")));
	valueHelpPairs.push_back(std::make_pair(tr("Lung Segmentation Mesh"), tr("Generated surface of the lung segmentation volume.")));
	valueHelpPairs.push_back(std::make_pair(tr("Blood Vessel Centerlines"), tr("Segmented blood vessel centerlines.")));
	valueHelpPairs.push_back(std::make_pair(tr("Blood Vessels Mesh"), tr("Segmented blood vessels in the lungs.")));

	foreach(auto pair, valueHelpPairs)
	{
		tempMeshStringAdapter = StringPropertySelectMesh::New(patientService());
		tempMeshStringAdapter->setValueName(pair.first);
		tempMeshStringAdapter->setHelp(pair.second);
		mOutputTypes.push_back(tempMeshStringAdapter);
	}

	StringPropertySelectImagePtr tempVolumeStringAdapter = StringPropertySelectImage::New(patientService());
	tempVolumeStringAdapter->setValueName("Blood Vessels Volume");
	tempVolumeStringAdapter->setHelp("Volume of segmented blood vessels in the lungs.");
	mOutputTypes.push_back(tempVolumeStringAdapter);
}

void AirwaysFilter::setAirwaySegmentation(bool airwaySegmentation)
{
	mAirwaySegmentationOption->setValue(airwaySegmentation);
}

void AirwaysFilter::setColoringAirways(bool coloringAirways)
{
	mColoredAirwaysOption->setValue(coloringAirways);
}

void AirwaysFilter::setVesselSegmentation(bool vesselSegmentation)
{
	mVesselSegmentationOption->setValue(vesselSegmentation);
}

BoolPropertyPtr AirwaysFilter::getManualSeedPointOption(QDomElement root)
{
	mManualSeedPointOption = BoolProperty::initialize(
				"Use manual seed point",
				"",
				"If the automatic seed point detection algorithm fails you can use cursor to set the seed point "
				"inside trachea of the patient. "
				"Then tick this checkbox to use the manual seed point in the airways filter.",
				false, root);
	return mManualSeedPointOption;

}

BoolPropertyPtr AirwaysFilter::getAirwaySegmentationOption(QDomElement root)
{
	mAirwaySegmentationOption = BoolProperty::initialize(
				"Airway segmentation",
				"",
				"Selecting this option will segment airways",
				true, root);
	return mAirwaySegmentationOption;
}

BoolPropertyPtr AirwaysFilter::getAirwayTubesGenerationOption(QDomElement root)
{
	mAirwayTubesGenerationOption = BoolProperty::initialize(
				"Airway tubes generation",
				"",
				"Selecting this option will generate artificial airway tubes for virtual bronchoscopy",
				true, root);
	return mAirwayTubesGenerationOption;

}

BoolPropertyPtr AirwaysFilter::getColoredAirwaysOption(QDomElement root)
{
	mColoredAirwaysOption = BoolProperty::initialize(
				"Add color specter to airway tubes",
				"",
				"Selecting this option will add a random color specter to the airway tubes using the filter Color Variation",
				true, root);
	return mColoredAirwaysOption;

}

BoolPropertyPtr AirwaysFilter::getLungSegmentationOption(QDomElement root)
{
	mLungSegmentationOption = BoolProperty::initialize(
				"Lung segmentation",
				"",
				"Selecting this option will segment the two lung sacs",
				false, root);
	return mLungSegmentationOption;
}

BoolPropertyPtr AirwaysFilter::getVesselSegmentationOption(QDomElement root)
{
	mVesselSegmentationOption = BoolProperty::initialize(
				"Vessel segmentation",
				"",
				"Selecting this option will segment the blood vessels in the lungs",
				false, root);
	return mVesselSegmentationOption;
}

BoolPropertyPtr AirwaysFilter::getVesselCenterlineOption(QDomElement root)
{
	mVesselCenterlineOption = BoolProperty::initialize(
				"Vessel centerline",
				"",
				"Selecting this option will generate centerlines of the seegmented blood vessels",
				false, root);
	return mVesselCenterlineOption;

}

BoolPropertyPtr AirwaysFilter::getVesselVolumeOption(QDomElement root)
{
	mVesselVolumeOption = BoolProperty::initialize(
				"Vessel volume",
				"",
				"Selecting this option will generate binary volume of the seegmented blood vessels",
				false, root);
	return mVesselVolumeOption;
}

} /* namespace cx */

