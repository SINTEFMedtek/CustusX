// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscFiberBundle.h"
#include "sscVtkFileMesh.h"
#include "sscBoundingBox3D.h"

#include "vtkPolyData.h"
#include "vtkImageData.h"
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkPointData.h>

namespace ssc
{

FiberBundlePtr FiberBundle::New(const QString& uid, const QString& name)
{
	FiberBundlePtr retval(new FiberBundle(uid, name));
	return retval;
}

FiberBundle::FiberBundle(const QString &uid, const QString &name)
	: Data(uid, name)
{
	mMesh.reset(new VtkFileMesh(uid, name));
	mSpacing = Vector3D(1.0, 1.0, 1.0);
	mVtkImageCached = false;

	// Enable shading as default
	setShading(true);
}

void FiberBundle::setMesh(const MeshPtr& mesh)
{
	if (hasMesh(mesh))
		return;

	// Disconnect current mesh signals, if any
	if(mMesh)
	{
		disconnect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
		disconnect(mMesh.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
	}

	// Connect new mesh signals
	mMesh = mesh;

	if (mMesh)
	{
		connect(mMesh.get(), SIGNAL(meshChanged()), this, SLOT(meshChangedSlot()));
		connect(mMesh.get(), SIGNAL(transformChanged()), this, SLOT(transformChangedSlot()));
	}

	// Flag image cache as false, so that we can read new image on request
	mVtkImageCached = false;
	emit bundleChanged();
}

bool FiberBundle::hasMesh(const MeshPtr& mesh) const
{
	return (mMesh == mesh);
}

void FiberBundle::setFilePath(const QString& filePath)
{
	mMesh->setFilePath(filePath);
}

QString FiberBundle::getFilePath() const
{
	return mMesh->getFilePath();
}

DoubleBoundingBox3D FiberBundle::boundingBox() const
{
	DoubleBoundingBox3D bbox;
	vtkPolyDataPtr polydata;

	if (mMesh) polydata = getVtkPolyData();

	if(polydata)
	{
		// Make sure we have updated data first
		polydata->Update();
		bbox = DoubleBoundingBox3D(polydata->GetBounds());
	}

	return bbox;
}

void FiberBundle::setVtkPolyData(const vtkPolyDataPtr& polyData)
{
	MeshPtr mesh(new Mesh(mUid, "FiberBundle", polyData));
	setMesh(mesh);
}

vtkPolyDataPtr FiberBundle::getVtkPolyData() const
{
	return mMesh->getVtkPolyData();
}

/**
 * Try to generate an volume representation of current poly data model,
 * where the foreground voxels are 1 and the background voxels are 0.
 *
 * Volume spacing is presumed to be set.
 */
vtkImageDataPtr FiberBundle::getVtkImageData()
{
	if (!mVtkImageCached)
	{
		vtkPolyDataPtr pd = getVtkPolyData();
		if (!pd) // No poly data to generate image data from
			return vtkImageDataPtr();

		vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New();

		double bounds[6];
		pd->GetBounds(bounds);

		whiteImage->SetSpacing(mSpacing[0], mSpacing[1], mSpacing[2]);

		// Compute dimensions
		int dim[3];
		for (int i = 0; i < 3; i++)
		{
			dim[i] = static_cast<int>(std::ceil((bounds[i * 2 + 1] - bounds[i * 2]) / mSpacing[i]));
		}

		whiteImage->SetDimensions(dim);
		whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

		double origin[3];
		origin[0] = bounds[0] + mSpacing[0] / 2;
		origin[1] = bounds[2] + mSpacing[1] / 2;
		origin[2] = bounds[4] + mSpacing[2] / 2;
		whiteImage->SetOrigin(origin);

		whiteImage->SetScalarTypeToUnsignedChar();
		whiteImage->AllocateScalars();

		// Fill the image with foreground voxels:
		unsigned char inval = 255;
		unsigned char outval = 0;
		vtkIdType count = whiteImage->GetNumberOfPoints();
		for (vtkIdType i = 0; i < count; ++i)
		{
			whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
		}

		// Apply an image stencil to the poly data
		vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
		pol2stenc->SetInput(pd);
		pol2stenc->SetOutputOrigin(origin);
		pol2stenc->SetOutputSpacing(mSpacing[0], mSpacing[1], mSpacing[2]);
		pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
		pol2stenc->Update();

		// Cut the corresponding white image and set the background:
		vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
		imgstenc->SetInput(whiteImage);
		imgstenc->SetStencil(pol2stenc->GetOutput());
		imgstenc->ReverseStencilOff();
		imgstenc->SetBackgroundValue(outval);
		imgstenc->Update();

		// Cache the resulting image
		mVtkImageData = imgstenc->GetOutput();
		mVtkImageCached = true;
	}

	return mVtkImageData;
}

void FiberBundle::setSpacing(double x, double y, double z)
{
	mSpacing = Vector3D(x, y, z);
}

void FiberBundle::setColor(const QColor& color)
{
	mMesh->setColor(color);
}

QColor FiberBundle::getColor() const
{
	return mMesh->getColor();
}

void FiberBundle::printSelf(std::ostream &os, Indent indent)
{
	// Output some numbers and brief info
	os << indent << "uid: " << mUid.toStdString() << std::endl;
	os << indent << "name: " << mName.toStdString() << std::endl;
	os << indent << "fileName: " << mFilePath.toStdString() << std::endl;
	os << indent << "numPoints: " << mMesh->getVtkPolyData()->GetNumberOfPoints() << std::endl;
	os << indent << "numVerts: " << mMesh->getVtkPolyData()->GetNumberOfVerts() << std::endl;
	os << indent << "numPolys: " << mMesh->getVtkPolyData()->GetNumberOfPolys() << std::endl;
}

void FiberBundle::meshChangedSlot()
{
	// Flag image cache as false, so that we can read new image on request
	mVtkImageCached = false;
	emit bundleChanged();
}

void FiberBundle::transformChangedSlot()
{
	emit transformChanged();
}

Transform3D FiberBundle::get_rMd() const
{
	return mMesh->get_rMd();
}
} // end namespace
