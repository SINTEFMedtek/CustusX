/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxClipMeshFilter.h"

#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkProbeFilter.h>
#include <vtkClipPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>

#include "cxMesh.h"
#include "cxImage.h"
#include "cxRegistrationTransform.h"
#include "cxLog.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxSelectDataStringProperty.h"
#include "cxDoubleProperty.h"

namespace cx
{

ClipMeshFilter::ClipMeshFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString ClipMeshFilter::getType() const
{
	return "clip_mesh_filter";
}

QString ClipMeshFilter::getName() const
{
	return "Clip Mesh by Volume";
}

QString ClipMeshFilter::getHelp() const
{
	return "<html>"
		"<h3>Clip Mesh by Volume</h3>"
		"<p>Removes parts of a mesh that lie in regions where the volume value "
		"is at or below a threshold.</p>"
		"<p>Each mesh vertex is probed against the volume. Triangles whose "
		"vertices are in regions above the threshold are kept. "
		"Triangles that straddle the boundary are split along the isosurface.</p>"
		"<p>Typical use: create a binary mask volume (0 outside, 1 inside), "
		"set threshold to 0, and run to keep only the mesh inside the mask.</p>"
		"</html>";
}

DoublePropertyPtr ClipMeshFilter::getThresholdOption(QDomElement root)
{
	return DoubleProperty::initialize("Threshold", "",
		"Keep mesh parts where the volume value is above this threshold. "
		"Use 0 for binary masks (values 0 and 1).",
		0.0, DoubleRange(-1000, 65535, 1), 0, root);
}

void ClipMeshFilter::createOptions()
{
	mOptionsAdapters.push_back(getThresholdOption(mOptions));
}

void ClipMeshFilter::createInputTypes()
{
	StringPropertySelectMeshPtr mesh = StringPropertySelectMesh::New(mServices->patient());
	mesh->setValueName("Mesh to clip");
	mesh->setHelp("Mesh to clip using the volume mask");
	mInputTypes.push_back(mesh);

	SelectDataStringPropertyBasePtr image = StringPropertySelectImage::New(mServices->patient());
	image->setValueName("Mask volume");
	image->setHelp("Volume used as mask. Parts of the mesh where the volume value exceeds the threshold are kept.");
	mInputTypes.push_back(image);
}

void ClipMeshFilter::createOutputTypes()
{
	StringPropertySelectMeshPtr output = StringPropertySelectMesh::New(mServices->patient());
	output->setValueName("Clipped mesh");
	output->setHelp("Mesh clipped to the mask region");
	mOutputTypes.push_back(output);
}

bool ClipMeshFilter::execute()
{
	MeshPtr mesh = boost::dynamic_pointer_cast<Mesh>(mCopiedInput[0]);
	ImagePtr image = boost::dynamic_pointer_cast<Image>(mCopiedInput[1]);

	if (!mesh || !image)
	{
		CX_LOG_ERROR() << "ClipMeshFilter: Missing mesh or image input";
		return false;
	}

	vtkPolyDataPtr polyData = mesh->getVtkPolyData();
	vtkImageDataPtr imageData = image->getBaseVtkImageData();

	if (!polyData || !imageData)
	{
		CX_LOG_ERROR() << "ClipMeshFilter: Mesh or image has no VTK data";
		return false;
	}

	// Transform mesh points from mesh data space to image data space so the
	// probe filter can sample the vtkImageData in its own coordinate system.
	Transform3D iMm = image->get_rMd().inverse() * mesh->get_rMd();

	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(polyData);
	transformFilter->SetTransform(iMm.getVtkTransform().Get());
	transformFilter->Update();

	// Sample image scalar values at each mesh vertex.
	vtkSmartPointer<vtkProbeFilter> probeFilter = vtkSmartPointer<vtkProbeFilter>::New();
	probeFilter->SetInputConnection(transformFilter->GetOutputPort());
	probeFilter->SetSourceData(imageData);
	probeFilter->Update();

	vtkPointData* pd = probeFilter->GetOutput()->GetPointData();
	vtkDataArray* probedScalars = pd->GetScalars();
	vtkDataArray* validMask = pd->GetArray("vtkValidPointMask");

	if (!probedScalars)
	{
		CX_LOG_ERROR() << "ClipMeshFilter: Probe filter produced no scalars — check that mesh and volume overlap";
		return false;
	}

	// Build a named scalar array on the original (mesh-space) polydata.
	// Vertices outside the image extent are forced to 0 so they are removed.
	vtkIdType numPoints = probedScalars->GetNumberOfTuples();
	vtkSmartPointer<vtkDoubleArray> maskValues = vtkSmartPointer<vtkDoubleArray>::New();
	maskValues->SetName("MaskScalars");
	maskValues->SetNumberOfComponents(1);
	maskValues->SetNumberOfTuples(numPoints);

	for (vtkIdType i = 0; i < numPoints; ++i)
	{
		double val = probedScalars->GetComponent(i, 0);
		if (validMask && validMask->GetComponent(i, 0) < 0.5)
			val = 0.0;
		maskValues->SetValue(i, val);
	}

	vtkSmartPointer<vtkPolyData> meshWithMask = vtkSmartPointer<vtkPolyData>::New();
	meshWithMask->ShallowCopy(polyData);
	meshWithMask->GetPointData()->AddArray(maskValues);
	meshWithMask->GetPointData()->SetActiveScalars("MaskScalars");

	// Clip: keep where scalar > threshold (vtkClipPolyData default, InsideOut off).
	double threshold = getThresholdOption(mCopiedOptions)->getValue();

	vtkSmartPointer<vtkClipPolyData> clipFilter = vtkSmartPointer<vtkClipPolyData>::New();
	clipFilter->SetInputData(meshWithMask);
	clipFilter->SetValue(threshold);
	clipFilter->GenerateClippedOutputOff();
	clipFilter->Update();

	vtkSmartPointer<vtkCleanPolyData> cleanFilter = vtkSmartPointer<vtkCleanPolyData>::New();
	cleanFilter->SetInputConnection(clipFilter->GetOutputPort());
	cleanFilter->Update();

	mClippedPolyData = cleanFilter->GetOutput();

	CX_LOG_INFO() << "ClipMeshFilter: Result has "
				  << mClippedPolyData->GetNumberOfPoints() << " points and "
				  << mClippedPolyData->GetNumberOfCells() << " cells";

	return mClippedPolyData->GetNumberOfPoints() > 0;
}

bool ClipMeshFilter::postProcess()
{
	if (!mClippedPolyData)
		return false;

	MeshPtr inputMesh = boost::dynamic_pointer_cast<Mesh>(mInputTypes[0]->getData());
	if (!inputMesh)
		return false;

	QString uid = inputMesh->getUid() + "_clipped_%1";
	QString name = inputMesh->getName() + "_clipped";
	MeshPtr outputMesh = mServices->patient()->createSpecificData<Mesh>(uid, name);

	outputMesh->setVtkPolyData(mClippedPolyData);
	outputMesh->get_rMd_History()->setParentSpace(inputMesh->getParentSpace());
	outputMesh->get_rMd_History()->setRegistration(inputMesh->get_rMd());
	outputMesh->setColor(inputMesh->getColor());

	mServices->patient()->insertData(outputMesh);
	mOutputTypes[0]->setValue(outputMesh->getUid());

	mClippedPolyData = vtkPolyDataPtr();
	return true;
}

} // namespace cx
