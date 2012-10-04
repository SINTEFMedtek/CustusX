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
#include <QColor>

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
	FiberBundleROI() : mPoint(Vector3D()), mRadius(0) {}
	FiberBundleROI(Vector3D pt, float radius) : mPoint(pt), mRadius(radius) {}
	FiberBundleROI(const FiberBundleROI& other) : mPoint(other.getPoint()), mRadius(other.getRadius()) {}

	FiberBundleROI &operator=(const FiberBundleROI& other)
	{
		if (this != &other) // protect agains self assignment
		{
			// Copy what is needed
			mPoint = other.getPoint();
			mRadius = other.getRadius();
		}

		// return this by convention
		return *this;
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

	/** Assign mesh to bundle */
	virtual void setMesh(const MeshPtr& mesh);
	/** Return bundle as mesh */
	virtual MeshPtr getMesh() { return mMesh; }
	virtual bool hasMesh(const MeshPtr& mesh) const;

	/** Assign bundle file path */
	virtual void setFilePath(const QString& filePath);
	/** Return current bundle file path */
	virtual QString getFilePath() const;

	/** Return model's bounding box */
	virtual DoubleBoundingBox3D boundingBox() const;

	/** Set bundle color */
	virtual void setColor(const QColor& color);
	/** Return current bundle color */
	virtual QColor getColor() const;

	/** Enable/disable shading */
	virtual void setShading(bool shading) { mShading = shading; }
	/** Return current shading policy */
	virtual bool getShading() const { return mShading; }

	/** Assign desired volume spacing */
	virtual void setSpacing(double x, double y, double z);
	/** Return currently assigned volume spacing */
	virtual Vector3D getSpacing() { return mSpacing; }

	vtkLookupTablePtr getLut() { return mLut; }
	vtkImageDataPtr getVtkImageData();
	ssc::ImagePtr getImage() { return ssc::ImagePtr(); } // NOT YET IMPLEMENTED

	void setROI(const FiberBundleROI& roi) { mROI = roi; }
	FiberBundleROI* getROI() { return &mROI; }

	/** Debug output */
	virtual void printSelf(std::ostream &os, Indent indent);

protected:
	FiberBundle(const QString& uid, const QString& name = "");

private:
	vtkLookupTablePtr mLut;
	bool mVtkImageCached;
	vtkImageDataPtr mVtkImageData;
	MeshPtr mMesh;
	FiberBundleROI mROI;
	Vector3D mSpacing;
	bool mShading;

signals:
	void bundleChanged();

protected slots:
	void meshChangedSlot();
	void transformChangedSlot();

};

typedef boost::shared_ptr<FiberBundle> FiberBundlePtr;

} // namespace ssc

#endif /*SSCFIBERBUNDLE_H_*/
