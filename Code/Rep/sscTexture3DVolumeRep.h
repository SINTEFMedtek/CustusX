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

#ifndef SSCTEXTURE3DVOLUME_H_
#define SSCTEXTURE3DVOLUME_H_

#include "sscRepImpl.h"
#include <vector>
#include "sscTransform3D.h"
#include "sscBoundingBox3D.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

#define MAX_CONCURRENT_VOLUMES 4
//---------------------------------------------------------
typedef vtkSmartPointer<class vtkCubeSource> vtkCubeSourcePtr;
namespace ssc
{
typedef vtkSmartPointer<class TextureVolumePainter> TextureVolumePainterPtr;

class Texture3DVolumeRep: public ssc::RepImpl
{
Q_OBJECT
public:
	static Texture3DVolumeRepPtr New(const QString& uid);
	virtual ~Texture3DVolumeRep();
	virtual QString getType() const
	{
		return "ssc::Texture3DVolumeRep";
	}
	void setShaderFiles(QString vertexShaderFile, QString fragmentShaderFiles);
	virtual void printSelf(std::ostream & os, ssc::Indent indent);
	void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp); // DEPRECATED: use zoomfactor in View and the object will auto-update
	void setImages(std::vector<ssc::ImagePtr> images);
	std::vector<ssc::ImagePtr> getImages();
	void update();

protected:
	Texture3DVolumeRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

private slots:
	void viewChanged();
	void updateColorAttributeSlot();
	void transformChangedSlot();
	QString getTCoordName(int index);
	private:
	DoubleBoundingBox3D mBB_s;
	std::vector<ssc::ImagePtr> mImages;
	View* mView;

	TextureVolumePainterPtr mPainter;
	vtkActorPtr mActor;
	boost::array<vtkTransformPolyDataFilterPtr, MAX_CONCURRENT_VOLUMES> mTransformPolyData;
	vtkAppendPolyDataPtr mMerger;
	vtkPainterPolyDataMapperPtr mPainterPolyDatamapper;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* SSCTEXTURE3DVOLUME_H_ */

