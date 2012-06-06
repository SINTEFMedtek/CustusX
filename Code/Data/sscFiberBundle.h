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

#ifndef SSCFIBERBUNDLE_H_
#define SSCFIBERBUNDLE_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"
#include "sscIndent.h"
#include "sscData.h"

namespace ssc
{

class FiberBundleROI : QObject
{
    Q_OBJECT

public:
    FiberBundleROI(){}
    FiberBundleROI(Vector3D pt, float radius) : mPoint(pt), mRadius(radius) {}
    FiberBundleROI(const FiberBundleROI& other) : mPoint(other.getPoint()), mRadius(other.getRadius()) {}

    FiberBundleROI operator=(const FiberBundleROI& other)
    {
        return FiberBundleROI(other.getPoint(), other.getRadius());
    }

    virtual ~FiberBundleROI(){}

    void setRadius(float radius) { mRadius = radius; emit changed(); }
    float getRadius() const{ return mRadius; }

    void setPoint(Vector3D point) { mPoint = point; emit changed(); }
    Vector3D getPoint() const { return mPoint; }

private:
    Vector3D mPoint;
    float mRadius;

signals:
    void changed();
};

/**
  * \brief Interface to a DTI fiber bundle.
  *
  * A fiber bundle is polydata consisting of
  * lines/line segments only.
  *
  * Yields both Mesh interface and native vtkPolyData
  * interface on request.
  *
  * \ingroup sscData
  */
class FiberBundle : public Data
{
    Q_OBJECT

public:
    static FiberBundlePtr New(const QString& uid, const QString& name = "");
    virtual ~FiberBundle() {}

    /** Assign polydata model */
    virtual void setVtkPolyData(const vtkPolyDataPtr& polyData);
    virtual vtkPolyDataPtr getVtkPolyData() const;

    virtual void setMesh(const MeshPtr& mesh);
    virtual MeshPtr getMesh() { return mMesh; }
    virtual bool hasMesh(const MeshPtr& mesh) const;

    virtual void setFilePath(const QString& filePath);
    virtual QString getFilePath() const;

    /** Return model's bounding box */
    virtual DoubleBoundingBox3D boundingBox() const;

    vtkLookupTablePtr getLut() { return mLut; }
    vtkImageDataPtr getVtkImageData() { return mImageData; }    // NOT YET SUPPORTED
    ssc::ImagePtr getImage() { return mImage; }

    void setROI(const FiberBundleROI& roi) { mROI = roi; }
    FiberBundleROI* getROI() { return &mROI; }

    /** Debug output */
    virtual void printSelf(std::ostream &os, Indent indent);

protected:
    FiberBundle(const QString& uid, const QString& name = "");

private:
    vtkLookupTablePtr mLut;
    vtkImageDataPtr mImageData;
    MeshPtr mMesh;
    FiberBundleROI mROI;
    ssc::ImagePtr mImage;

signals:
    void bundleChanged();

protected slots:
    void meshChangedSlot();
    void transformChangedSlot();

};

typedef boost::shared_ptr<FiberBundle> FiberBundlePtr;

} // namespace ssc

#endif /*SSCFIBERBUNDLE_H_*/
