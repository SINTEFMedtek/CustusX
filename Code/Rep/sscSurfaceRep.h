#ifndef SSCSURFACEREP_H_
#define SSCSURFACEREP_H_

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscRepImpl.h"

namespace ssc
{
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

