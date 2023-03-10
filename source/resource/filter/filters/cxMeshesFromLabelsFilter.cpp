/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMeshesFromLabelsFilter.h"

#include <vtkImageShrink3D.h>
//#include <vtkMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataNormals.h>
#include <vtkImageData.h>

//#include <vtkDiscreteFlyingEdges3D.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkNew.h>
#include <vtkImageAccumulate.h>
#include <vtkThreshold.h>
#include <vtkDataSetAttributes.h>
#include <vtkPointData.h>
#include <vtkMaskFields.h>
#include <vtkGeometryFilter.h>
//#include <vtkXMLPolyDataWriter.h>//Test

#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxColorProperty.h"
#include "cxSelectDataStringProperty.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVisServices.h"
#include "cxLogger.h"

namespace cx
{

MeshesFromLabelsFilter::MeshesFromLabelsFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString MeshesFromLabelsFilter::getName() const
{
	return "MeshesFromLabels";
}

QString MeshesFromLabelsFilter::getType() const
{
	return "MeshesFromLabels_filter";
}

QString MeshesFromLabelsFilter::getHelp() const
{
	return	"<html>"
					"<h3>Surfacing.</h3>"
					"<p><i>Create surfaces from a label volume using marching cubes.</i></p>"
					"<p>- Optional factor 2 reduction</p>"
					"<p>- Marching Cubes contouring</p>"
					"<p>- Optional Windowed Sinc smoothing</p>"
					"<p>- Decimation of triangles</p>"
				"</html>";
}

QString MeshesFromLabelsFilter::getNameSuffix()
{
	return "_ge";
}

BoolPropertyPtr MeshesFromLabelsFilter::getReduceResolutionOption(QDomElement root)
{
	return BoolProperty::initialize("Reduce input", "",
																	"Reduce input volumes resolution by a factor of 2 in all directions.",
																	 false, root);
}

BoolPropertyPtr MeshesFromLabelsFilter::getSmoothingOption(QDomElement root)
{
	return BoolProperty::initialize("Smoothing", "",
																	"Smooth the output contour", true, root);
}

BoolPropertyPtr MeshesFromLabelsFilter::getPreserveTopologyOption(QDomElement root)
{
	return BoolProperty::initialize("Preserve mesh topology", "",
																	"Preserve mesh topology during reduction", true, root);
}

DoublePropertyPtr MeshesFromLabelsFilter::getSurfaceThresholdOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Threshold", "",
																												"Values from this threshold and above will be included",
																												100.0, DoubleRange(-1000, 1000, 1), 0, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	return retval;
}

DoublePropertyPtr MeshesFromLabelsFilter::getDecimationOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Decimation %", "",
																												"Reduce number of triangles in output surface",
																												0.2, DoubleRange(0, 1, 0.01), 0, root);
	retval->setInternal2Display(100);
	return retval;
}

ColorPropertyPtr MeshesFromLabelsFilter::getColorOption(QDomElement root)
{
	return ColorProperty::initialize( "Color", "",
																		"Color of output model.",
																		QColor("green"), root);
}

DoublePropertyPtr MeshesFromLabelsFilter::getNumberOfIterationsOption(QDomElement root)
{
	return DoubleProperty::initialize("Number of iterations (smoothing)", "",
																		"Number of iterations in smoothing filter. Higher number = more smoothing",
																		15, DoubleRange(1, 50, 1), 0, root);
}

DoublePropertyPtr MeshesFromLabelsFilter::getPassBandOption(QDomElement root)
{
	return DoubleProperty::initialize("Band pass smoothing", "",
																		"Band pass width in smoothing filter. Smaller number = more smoothing",
																		0.30, DoubleRange(0.01, 0.95, 0.01), 2, root);
}
DoublePropertyPtr MeshesFromLabelsFilter::getStartLabelOption(QDomElement root)
{
	return DoubleProperty::initialize("Start label", "",
									  "Start creating meshes from labeled volume from this label",
									  0, DoubleRange(-1000, 1000, 1), 0, root);
}

DoublePropertyPtr MeshesFromLabelsFilter::getEndLabelOption(QDomElement root)
{
	return DoubleProperty::initialize("End label", "",
									  "Stop creating meshes from labeled volume at this label",
									  0, DoubleRange(-1000, 1000, 1), 0, root);
}
DoublePropertyPtr MeshesFromLabelsFilter::getCombineLabelsOption(QDomElement root)
{
	return DoubleProperty::initialize("Combine labels", "",
									  "Combine labels into buckets",
									  1, DoubleRange(1, 100, 1), 0, root);
}

void MeshesFromLabelsFilter::createOptions()
{
	mReduceResolutionOption = this->getReduceResolutionOption(mOptions);
	mOptionsAdapters.push_back(mReduceResolutionOption);

	mSurfaceThresholdOption = this->getSurfaceThresholdOption(mOptions);
	connect(mSurfaceThresholdOption.get(), SIGNAL(changed()), this, SLOT(thresholdSlot()));
	mOptionsAdapters.push_back(mSurfaceThresholdOption);

	mOptionsAdapters.push_back(this->getSmoothingOption(mOptions));
	mOptionsAdapters.push_back(this->getNumberOfIterationsOption(mOptions));
	mOptionsAdapters.push_back(this->getPassBandOption(mOptions));
	mOptionsAdapters.push_back(this->getDecimationOption(mOptions));
	mOptionsAdapters.push_back(this->getPreserveTopologyOption(mOptions));

	mOptionsAdapters.push_back(this->getColorOption(mOptions));

	//TODO: Update label values from image. Use FilterImpl::updateThresholdPairFromImageChange()?
	mOptionsAdapters.push_back(this->getStartLabelOption(mOptions));
	mOptionsAdapters.push_back(this->getEndLabelOption(mOptions));
	mOptionsAdapters.push_back(this->getCombineLabelsOption(mOptions));
}

void MeshesFromLabelsFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select label image for contouring");
	connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void MeshesFromLabelsFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectMesh::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output meshes");
	mOutputTypes.push_back(temp);
}

void MeshesFromLabelsFilter::setActive(bool on)
{
	FilterImpl::setActive(on);

	if (!mActive)
		this->stopPreview();
}

void MeshesFromLabelsFilter::stopPreview()
{
	if(mPreviewImage)
		mPreviewImage->stopThresholdPreview();
	mPreviewImage.reset();
}

void MeshesFromLabelsFilter::imageChangedSlot(QString uid)
{
	ImagePtr image = mServices->patient()->getData<Image>(uid);
	if(!image)
		return;

	this->updateThresholdFromImageChange(uid, mSurfaceThresholdOption);
	this->stopPreview();

	int extent[6];
	image->getBaseVtkImageData()->GetExtent(extent);
	mReduceResolutionOption->setHelp( "Current input resolution: " + qstring_cast(extent[1])
																		+ " " + qstring_cast(extent[3]) + " " + qstring_cast(extent[5])
																		+ " (If checked: " + qstring_cast(extent[1]/2)+ " " + qstring_cast(extent[3]/2) + " "
																		+ qstring_cast(extent[5]/2) + ")");
}

void MeshesFromLabelsFilter::thresholdSlot()
{
	if (mActive)
	{
		mPreviewImage = boost::dynamic_pointer_cast<Image>(mInputTypes[0]->getData());
		if(mPreviewImage)
		{
			Eigen::Vector2d threshold = Eigen::Vector2d(mSurfaceThresholdOption->getValue(),  mPreviewImage->getMax());
			mPreviewImage->startThresholdPreview(threshold);
		}
	}
}

bool MeshesFromLabelsFilter::preProcess()
{
	this->stopPreview();
	return FilterImpl::preProcess();
}

bool MeshesFromLabelsFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;


//	int startLabel = 0;
//	int endLabel = input->getRange();

	//std::cout << "MeshesFromLabelsFilter::execute : " << mCopiedOptions.ownerDocument().toString() << std::endl;

	BoolPropertyPtr reduceResolutionOption = this->getReduceResolutionOption(mCopiedOptions);
	BoolPropertyPtr smoothingOption = this->getSmoothingOption(mCopiedOptions);
    DoublePropertyPtr numberOfIterationsOption = this->getNumberOfIterationsOption(mCopiedOptions);
    DoublePropertyPtr passBandOption = this->getPassBandOption(mCopiedOptions);
	BoolPropertyPtr preserveTopologyOption = this->getPreserveTopologyOption(mCopiedOptions);
	DoublePropertyPtr surfaceThresholdOption = this->getSurfaceThresholdOption(mCopiedOptions);
	DoublePropertyPtr decimationOption = this->getDecimationOption(mCopiedOptions);
	DoublePropertyPtr startLabelOption = this->getStartLabelOption(mCopiedOptions);
	DoublePropertyPtr endLabelOption = this->getEndLabelOption(mCopiedOptions);
	DoublePropertyPtr combineLabelsOption = this->getCombineLabelsOption(mCopiedOptions);


	//report(QString("Creating contour from \"%1\"...").arg(input->getName()));

	mRawResult = this->execute( input->getBaseVtkImageData(),
//								startLabel,
//								endLabel,
								startLabelOption->getValue(),
								endLabelOption->getValue(),
								combineLabelsOption->getValue(),
								surfaceThresholdOption->getValue(),
								reduceResolutionOption->getValue(),
								smoothingOption->getValue(),
								preserveTopologyOption->getValue(),
								decimationOption->getValue(),
								numberOfIterationsOption->getValue(),
								passBandOption->getValue());
	return true;
}

//Example code for vtkDiscreteMarchingCubes and vtkDiscreteFlyingEdges3D
//https://kitware.github.io/vtk-examples/site/Cxx/Medical/GenerateModelsFromLabels/

std::vector<vtkPolyDataPtr> MeshesFromLabelsFilter::execute(vtkImageDataPtr input,
											   int startLabel,
											   int endLabel,
											   int combineLabels,
											   double threshold,
											   bool reduceResolution,
											   bool smoothing,
											   bool preserveTopology,
											   double decimation,
											   double numberOfIterations,
											   double passBand)
{
	if (!input)
		return std::vector<vtkPolyDataPtr>();

	//Shrink input volume
	vtkImageShrink3DPtr shrinker = vtkImageShrink3DPtr::New();
	if(reduceResolution)
	{
//		std::cout << "smooth" << std::endl;
		shrinker->SetInputData(input);
		shrinker->SetShrinkFactors(2,2,2);
		shrinker->Update();
	}

//	vtkNew<vtkDiscreteFlyingEdges3D> discreteCubes;//Possible with new VTK
	vtkNew<vtkDiscreteMarchingCubes> discreteCubes;

	vtkNew<vtkImageAccumulate> histogram;

	// Find countour
	if(reduceResolution)
	{
		discreteCubes->SetInputConnection(shrinker->GetOutputPort());
		histogram->SetInputConnection(shrinker->GetOutputPort());
	}
	else
	{
		discreteCubes->SetInputData(input);
		histogram->SetInputData(input);
	}

	discreteCubes->GenerateValues(endLabel - startLabel + 1, startLabel, endLabel);
	discreteCubes->Update();


	// Smooth surface model
	vtkWindowedSincPolyDataFilterPtr smoother = vtkWindowedSincPolyDataFilterPtr::New();

	//Placeholder for the last outputPort from various optional filters
	vtkAlgorithmOutput* outputPort = discreteCubes->GetOutputPort();

	if(smoothing)
	{
		smoother->SetInputConnection(outputPort);
		outputPort = smoother->GetOutputPort();

		smoother->SetNumberOfIterations(numberOfIterations);// Higher number = more smoothing  -  default 15
		smoother->SetBoundarySmoothing(false);
		smoother->SetFeatureEdgeSmoothing(false);
		smoother->SetNormalizeCoordinates(true);
		smoother->SetFeatureAngle(120);
		smoother->SetPassBand(passBand);//Lower number = more smoothing  -  default 0.3
		smoother->Update();
	}

	//Create a surface of triangles

	//Not working here. Move down as the normals prosessing
//	//Decimate surface model (remove a percentage of the polygons)
//	vtkTriangleFilterPtr trifilt = vtkTriangleFilterPtr::New();
//	vtkDecimateProPtr deci = vtkDecimateProPtr::New();
//	if (decimation > 0.000001)
//	{
//		trifilt->SetInputConnection(outputPort);
//		outputPort = trifilt->GetOutputPort();
//		trifilt->Update();

//		deci->SetInputConnection(outputPort);
//		outputPort = deci->GetOutputPort();

//		deci->SetTargetReduction(decimation);
//		deci->SetPreserveTopology(preserveTopology);
//		deci->Update();
//	}

	vtkNew<vtkThreshold> selector;
	selector->SetInputConnection(outputPort);

//  #ifdef USE_FLYING_EDGES
//	selector->SetInputArrayToProcess(0, 0, 0,
//									 vtkDataObject::FIELD_ASSOCIATION_POINTS,
//									 vtkDataSetAttributes::SCALARS);
//  #else
	selector->SetInputArrayToProcess(0, 0, 0,
									 vtkDataObject::FIELD_ASSOCIATION_CELLS,
									 vtkDataSetAttributes::SCALARS);
//  #endif


	// Strip the scalars from the output
	vtkNew<vtkMaskFields> scalarsOff;
	scalarsOff->SetInputConnection(selector->GetOutputPort());
	scalarsOff->CopyAttributeOff(vtkMaskFields::POINT_DATA, vtkDataSetAttributes::SCALARS);
	scalarsOff->CopyAttributeOff(vtkMaskFields::CELL_DATA, vtkDataSetAttributes::SCALARS);


//	selector->SetInputConnection(smoother->GetOutputPort());
	selector->SetInputConnection(outputPort);
	selector->SetInputArrayToProcess(0, 0, 0,
									 vtkDataObject::FIELD_ASSOCIATION_CELLS,
									 vtkDataSetAttributes::SCALARS);

	vtkNew<vtkGeometryFilter> geometry;
	geometry->SetInputConnection(scalarsOff->GetOutputPort());
	outputPort = geometry->GetOutputPort();

	//Need do decimation and normals at the end of the pipeline
	//Decimate surface model (remove a percentage of the polygons)
	vtkTriangleFilterPtr trifilt = vtkTriangleFilterPtr::New();
	vtkDecimateProPtr deci = vtkDecimateProPtr::New();
	if (decimation > 0.000001)
	{
		trifilt->SetInputConnection(outputPort);
		outputPort = trifilt->GetOutputPort();
		trifilt->Update();
		deci->SetInputConnection(outputPort);
		outputPort = deci->GetOutputPort();
		deci->SetTargetReduction(decimation);
		deci->SetPreserveTopology(preserveTopology);
		deci->Update();
	}

	vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
//	normals->SetInputConnection(geometry->GetOutputPort());
	normals->SetInputConnection(outputPort);
	normals->SetComputeCellNormals(true);
	normals->AutoOrientNormalsOn();

	histogram->SetComponentExtent(0, endLabel, 0, 0, 0, 0);
	histogram->SetComponentOrigin(0, 0, 0);
	histogram->SetComponentSpacing(1, 1, 1);
	histogram->Update();

	std::vector<vtkPolyDataPtr> retval;

	//Experimenting with combining labels. Allows dividing regular volume as well, not just label volume
	//It really don't work that well, the combined mesh looks like something that is knitted
//	for (unsigned int i = startLabel; i <= endLabel; i++)
	for (unsigned int i = startLabel; i <= endLabel; i += combineLabels)
	{
		// see if the label exists, if not skip it
		double frequency = histogram->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i);
		CX_LOG_DEBUG() << "Label " << i << " frequency: " << frequency;
		if (frequency == 0.0)
			continue;

		// select the cells for a given label
		//selector->SetLowerThreshold(i);
		//selector->SetUpperThreshold(i);
		selector->ThresholdBetween(i,i+combineLabels);//VTK 7.1. To be replaced with the above lines
		selector->Update();


		//Test: Save polydata to file
//		vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
//		writer->SetInputConnection(geometry->GetOutputPort());
//		std::stringstream ss;
//		std::string filePrefix = "Label";
//		ss << filePrefix << i << ".vtp";
//		cout << " writing " << ss.str() << endl;
//		writer->SetFileName(ss.str().c_str());
//		writer->Write();



//		geometry->Update();
		normals->Update();

//		vtkPolyDataPtr cubesPolyData = normals->GetOutput();
		//Need to do a copy, to break pipeline. If not all meshes will show the last label.
		//It seems a shallow copy may be enough
		vtkPolyDataPtr cubesPolyData = vtkPolyDataPtr::New();
		cubesPolyData->ShallowCopy(normals->GetOutput());
//		cubesPolyData->DeepCopy(normals->GetOutput());
//		CX_LOG_DEBUG() << "Num points: " << cubesPolyData->GetNumberOfPoints();
		retval.push_back(cubesPolyData);
	}
	
	return retval;
}

bool MeshesFromLabelsFilter::postProcess()
{
	if (mRawResult.empty())
		return false;

	ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return false;

	ColorPropertyPtr colorOption = this->getColorOption(mOptions);
	std::vector<MeshPtr> output = this->postProcess(mServices->patient(), mRawResult, input, colorOption->getValue());
	mRawResult.clear();

	if (!output.empty())
		mOutputTypes.front()->setValue(output[0]->getUid());//Only add first mesh to selector

	return true;
}

std::vector<MeshPtr> MeshesFromLabelsFilter::postProcess(PatientModelServicePtr patient, std::vector<vtkPolyDataPtr> contours, ImagePtr base, QColor color)
{
	if (contours.empty() || !base)
		return std::vector<MeshPtr>();

	std::vector<MeshPtr> retval;

	for(int i = 0; i < contours.size(); ++i)
	{
		QString uid = base->getUid() + MeshesFromLabelsFilter::getNameSuffix() + "%1";
		QString name = base->getName()+ MeshesFromLabelsFilter::getNameSuffix() + "%1";
		MeshPtr output = patient->createSpecificData<Mesh>(uid, name);
		output->setVtkPolyData(contours[i]);

		output->get_rMd_History()->setRegistration(base->get_rMd());
		output->get_rMd_History()->setParentSpace(base->getUid());

		output->setColor(color);

		patient->insertData(output);
		retval.push_back(output);
	}

	return retval;
}

} // namespace cx
