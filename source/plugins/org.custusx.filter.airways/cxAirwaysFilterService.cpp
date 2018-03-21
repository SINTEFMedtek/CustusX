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

#include "cxTime.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxDataReaderWriter.h"
#include "cxRegistrationTransform.h"
#include "cxDoubleProperty.h"
#include "cxContourFilter.h"
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

QString AirwaysFilter::getNameSuffix()
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

	if (inputImage->getType() != "image")
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
	    importer->update();
	    fast::Image::pointer image = importer->getOutputData<fast::Image>();

        // Do segmentation
        fast::Segmentation::pointer segmentationData;
		bool doLungSegmentation = getLungSegmentationOption(mOptions)->getValue();
		bool useManualSeedPoint = getManualSeedPointOption(mOptions)->getValue();
		try {
			if(doLungSegmentation) {
				fast::LungSegmentation::pointer segmentation = fast::LungSegmentation::New();
				if(useManualSeedPoint) {
					CX_LOG_INFO() << "Using seed point: " << seedPoint.transpose();
					segmentation->setAirwaySeedPoint(seedPoint(0), seedPoint(1), seedPoint(2));
				}
				segmentation->setInputConnection(importer->getOutputPort());
				segmentation->update();
				segmentationData = segmentation->getOutputData<fast::Segmentation>(1);

                // Convert fast segmentation data to VTK data which CX can use (Airways)
                vtkSmartPointer<fast::VTKImageExporter> vtkExporter = fast::VTKImageExporter::New();
                vtkExporter->setInputConnection(segmentation->getOutputPort(1));
                vtkExporter->Update();
                mAirwaySegmentationOutput = vtkExporter->GetOutput();

                // Convert fast segmentation data to VTK data which CX can use (Lungs)
                vtkSmartPointer<fast::VTKImageExporter> vtkExporter2 = fast::VTKImageExporter::New();
                vtkExporter2->setInputConnection(segmentation->getOutputPort(0));
                vtkExporter2->Update();
                mLungSegmentationOutput = vtkExporter2->GetOutput();
			} else {

				fast::AirwaySegmentation::pointer segmentation = fast::AirwaySegmentation::New();
				if(useManualSeedPoint) {
					CX_LOG_INFO() << "Using seed point: " << seedPoint.transpose();
					segmentation->setSeedPoint(seedPoint(0), seedPoint(1), seedPoint(2));
				}
				segmentation->setInputConnection(importer->getOutputPort());
				segmentation->update();
				segmentationData = segmentation->getOutputData<fast::Segmentation>(0);

				// Convert fast segmentation data to VTK data which CX can use
				vtkSmartPointer<fast::VTKImageExporter> vtkExporter = fast::VTKImageExporter::New();
				vtkExporter->setInputConnection(segmentation->getOutputPort());
				vtkExporter->Update();
				mAirwaySegmentationOutput = vtkExporter->GetOutput();
            }
        } catch(fast::Exception & e)
        {
			CX_LOG_ERROR() << "The airways filter failed: \n"
						   << e.what();
            if(!useManualSeedPoint)
                CX_LOG_ERROR() << "Try to set the seed point manually.";
            return false;
        }

		CX_LOG_SUCCESS() << "FINISHED AIRWAY SEGMENTATION";


	    // Get the transformation of the segmentation
		Eigen::Affine3f T = fast::SceneGraph::getEigenAffineTransformationFromData(segmentationData);
	    mTransformation.matrix() = T.matrix().cast<double>(); // cast to double

        // Extract centerline
        fast::CenterlineExtraction::pointer centerline = fast::CenterlineExtraction::New();
        centerline->setInputData(segmentationData);

        // Get centerline
	    vtkSmartPointer<fast::VTKMeshExporter> vtkCenterlineExporter = fast::VTKMeshExporter::New();
	    vtkCenterlineExporter->setInputConnection(centerline->getOutputPort());
	    mCenterlineOutput = vtkCenterlineExporter->GetOutput();
	    vtkCenterlineExporter->Update();

	} catch(fast::Exception& e) {
		std::string error = e.what();
		reportError("fast::Exception: "+qstring_cast(error));

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

	if(getLungSegmentationOption(mOptions)->getValue()) {
		vtkPolyDataPtr rawContour = ContourFilter::execute(
				mLungSegmentationOutput,
				threshold,
				false, // reduce resolution
				true, // smoothing
				true, // keep topology
				0 // target decimation
		);
		//outputSegmentation->get_rMd_History()->setRegistration(rMd_i);
		//patientService()->insertData(outputSegmentation);

		// Add contour internally to cx
		QColor color("red");
		color.setAlpha(100);
		MeshPtr contour = ContourFilter::postProcess(
				patientService(),
				rawContour,
				mInputImage,
                color
		);
		contour->get_rMd_History()->setRegistration(mTransformation);

		// Set output
		mOutputTypes[2]->setValue(contour->getUid());
	}

    // Centerline
    QString uid = mInputImage->getUid() + AirwaysFilter::getNameSuffix() + "%1";
    QString name = mInputImage->getName() + AirwaysFilter::getNameSuffix() + "%1";
	MeshPtr centerline = patientService()->createSpecificData<Mesh>(uid, name);
	centerline->setVtkPolyData(mCenterlineOutput);
	centerline->get_rMd_History()->setParentSpace(mInputImage->getUid());
	centerline->get_rMd_History()->setRegistration(mTransformation);
	patientService()->insertData(centerline);
	mOutputTypes[0]->setValue(centerline->getUid());

	// Straight centerline and tubes
	if(getStraightCLTubesOption(mOptions)->getValue())
	{
		this->createStraightCL();
		this->createTubes();
	}

	return true;
}

/**
 * @brief AirwaysFilter::createTubes
 * This method of drawing tubes is from the Hello vtk example found in the
 * VTK books:
 * https://www.vtk.org/gitweb?p=VTK.git;a=blob;f=Examples/Modelling/Python/hello.py
 * and also from the Blobbylogo example:
 * https://lorensen.github.io/VTKExamples/site/Cxx/Visualization/BlobbyLogo/
 * We found that it was easiest to use implicit modelling to create the tubes around
 * a centerline. However, we have not been able to fully control the radius
 * of the tubes. The current parameters gives the largest radius we have
 * seen, and also the roundest shape.
 * SetMaximumDistance and SetAdjustDistance must be aligned to get larger radius.
 * SetValue may give a square shape.
 * SetSampleDimensions must be large enough to give good resolution,
 * but not so large that the creation takes too long.
 */
void AirwaysFilter::createTubes()
{
	// Get the straight centerline to model the tubes around.
	QString straightCLUid = mOutputTypes[3]->getValue();
	MeshPtr straightCL = boost::dynamic_pointer_cast<Mesh>(patientService()->getData(straightCLUid));
	if(!straightCL)
		return;
	vtkPolyDataPtr clPolyData = straightCL->getVtkPolyData();

	// Create the implicit modeller
	vtkSmartPointer<vtkImplicitModeller> blobbyLogoImp =
			vtkSmartPointer<vtkImplicitModeller>::New();
	blobbyLogoImp->SetInputData(clPolyData);
	blobbyLogoImp->SetMaximumDistance(0.1);
	blobbyLogoImp->SetSampleDimensions(256, 256, 256);
	blobbyLogoImp->SetAdjustDistance(0.1);

	// Extract an iso surface, i.e. the tube shell
	vtkSmartPointer<vtkContourFilter> blobbyLogoIso =
		vtkSmartPointer<vtkContourFilter>::New();
	blobbyLogoIso->SetInputConnection(blobbyLogoImp->GetOutputPort());
	blobbyLogoIso->SetValue(1, 1.5); //orig
	blobbyLogoIso->Update();

	// Create the mesh object from the tube shell
	QString uid = mInputImage->getUid() + AirwaysFilter::getNameSuffix() + AirwaysFilter::getNameSuffixStraight() + AirwaysFilter::getNameSuffixTubes() + "%1";
	QString name = mInputImage->getName() + AirwaysFilter::getNameSuffix() + AirwaysFilter::getNameSuffixStraight() + AirwaysFilter::getNameSuffixTubes() + "%1";
	MeshPtr centerline = patientService()->createSpecificData<Mesh>(uid, name);
	centerline->setVtkPolyData(blobbyLogoIso->GetOutput());
	centerline->get_rMd_History()->setParentSpace(mInputImage->getUid());
	centerline->get_rMd_History()->setRegistration(mTransformation);
	// The color is taken from the new Fraxinus logo. Blue is the common color for lungs/airways. Partly transparent for a nice effect in Fraxinus.
	centerline->setColor(QColor(118, 178, 226, 200));
	patientService()->insertData(centerline);
	mOutputTypes[4]->setValue(centerline->getUid());

}

void AirwaysFilter::setDefaultStraightCLTubesOption(bool defaultStraightCLTubesOption)
{
	mDefaultStraightCLTubesOption = defaultStraightCLTubesOption;
}

void AirwaysFilter::createStraightCL()
{
	QString uid = mInputImage->getUid() + AirwaysFilter::getNameSuffix() + AirwaysFilter::getNameSuffixStraight() + "%1";
	QString name = mInputImage->getName() + AirwaysFilter::getNameSuffix() + AirwaysFilter::getNameSuffixStraight() + "%1";
	MeshPtr centerline = patientService()->createSpecificData<Mesh>(uid, name);

	BranchListPtr bl = BranchListPtr(new BranchList());

	Eigen::MatrixXd CLpoints = makeTransformedMatrix(mCenterlineOutput);
	bl->findBranchesInCenterline(CLpoints);
	vtkPolyDataPtr retval = bl->createVtkPolyDataFromBranches(false, true);

	centerline->setVtkPolyData(retval);
	centerline->get_rMd_History()->setParentSpace(mInputImage->getUid());
	centerline->get_rMd_History()->setRegistration(mTransformation);
	patientService()->insertData(centerline);
	mOutputTypes[3]->setValue(centerline->getUid());
}

void AirwaysFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getManualSeedPointOption(mOptions));
	mOptionsAdapters.push_back(this->getLungSegmentationOption(mOptions));
	mOptionsAdapters.push_back(this->getStraightCLTubesOption(mOptions));
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

BoolPropertyPtr AirwaysFilter::getStraightCLTubesOption(QDomElement root)
{
	BoolPropertyPtr retval =
			BoolProperty::initialize("Straight centerline and tubes",
					"",
					"Use this option to generate a centerline with straight branches between "
					"the branch points. "
					"You also get tubes based on this straight line.",
					mDefaultStraightCLTubesOption, root);
	return retval;
}


} /* namespace cx */

