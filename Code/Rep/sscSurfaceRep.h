#ifndef SSCSURFACEREP_H_
#define SSCSURFACEREP_H_

#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;
typedef	vtkSmartPointer<class vtkContourFilter> vtkContourFilterPtr;
typedef vtkSmartPointer<class vtkProperty> vtkPropertyPtr;
typedef	vtkSmartPointer<class vtkActor> vtkActorPtr;

#include "sscRepImpl.h"

namespace ssc
{
// forward declarations
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class Mesh> MeshPtr;

typedef boost::shared_ptr<class SurfaceRep> SurfaceRepPtr;

/**
 * NOT IN USE BY ANYONE! 
 * If you want to use this class, test is thoroughly first!!
 */
class SurfaceRep : public RepImpl
{
  Q_OBJECT
public:
	static SurfaceRepPtr New(const QString& uid);
	virtual QString getType() const { return "ssc::SurfaceRep"; }
	virtual ~SurfaceRep();
	void setMesh(MeshPtr mesh);
	void setImage(ImagePtr image);
	//void removeImage(ImagePtr image);
	bool hasMesh(MeshPtr mesh) const;
protected:
	SurfaceRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	vtkPolyDataMapperPtr mMapper;
	vtkContourFilterPtr mContourFilter;
	vtkPropertyPtr mProperty;
	vtkActorPtr mActor;
	
	ImagePtr mImage;
	MeshPtr mMesh;
};
//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /*SSCSURFACEREP_H_*/

