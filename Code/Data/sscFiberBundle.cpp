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
#include "sscMesh.h"
#include "sscBoundingBox3D.h"

#include "vtkPolyData.h"

namespace ssc
{

FiberBundlePtr FiberBundle::New(const QString& uid, const QString& name)
{
    FiberBundlePtr retval(new FiberBundle(uid, name));
    return retval;
}

DoubleBoundingBox3D FiberBundle::boundingBox() const
{
    DoubleBoundingBox3D bbox;

    if (mVtkPolyData)
    {
        // Make sure we have updated data first
        mVtkPolyData->UpdateInformation();
        bbox = DoubleBoundingBox3D(mVtkPolyData->GetBounds());
    }

    return bbox;
}

void FiberBundle::printSelf(std::ostream &os, Indent indent)
{
    // Output some numbers and brief info
    os << indent << "uid: " << mUid.toStdString() << std::endl;
    os << indent << "name: " << mName.toStdString() << std::endl;
    os << indent << "fileName: " << mFilePath.toStdString() << std::endl;
    os << indent << "numPoints: " << mVtkPolyData->GetNumberOfPoints() << std::endl;
    os << indent << "numVerts: " << mVtkPolyData->GetNumberOfVerts() << std::endl;
    os << indent << "numPolys: " << mVtkPolyData->GetNumberOfPolys() << std::endl;
}

} // end namespace
