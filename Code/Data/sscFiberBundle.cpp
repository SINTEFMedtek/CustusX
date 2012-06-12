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
        polydata->UpdateInformation();
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
    emit bundleChanged();
}

void FiberBundle::transformChangedSlot()
{
    emit transformChanged();
}

} // end namespace
